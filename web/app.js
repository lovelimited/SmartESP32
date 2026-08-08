// Import Firebase modules via CDN ES modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-app.js";
import { 
    getDatabase, 
    ref, 
    onValue, 
    set, 
    get, 
    child 
} from "https://www.gstatic.com/firebasejs/10.8.0/firebase-database.js";
import { 
    getAuth, 
    signInAnonymously, 
    onAuthStateChanged 
} from "https://www.gstatic.com/firebasejs/10.8.0/firebase-auth.js";

// Firebase Configuration from firebase_config.h
const firebaseConfig = {
    apiKey: "AIzaSyA0tqHqwo-4Yy3jiyOOKmOltF7veTgIGH0",
    authDomain: "smartesp32-9615f.firebaseapp.com",
    databaseURL: "https://smartesp32-9615f-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "smartesp32-9615f",
    storageBucket: "smartesp32-9615f.firebasestorage.app",
    messagingSenderId: "32177383970",
    appId: "1:32177383970:web:993fb46b4f67f3f0f9387b"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const auth = getAuth(app);

// State Data
let currentSchedule = {
    relay1: [],
    relay2: []
};

let currentSettings = {
    relay1_mode: "AUTO",
    relay2_mode: "AUTO"
};

let esp32LastSeen = 0;
let isFirebaseConnected = false;

// DOM Elements
const connectionStatusEl = document.getElementById("connectionStatus");
const currentTimeEl = document.getElementById("currentTime");

// Clock update and ESP32 Status check
setInterval(() => {
    const now = new Date();
    currentTimeEl.textContent = now.toLocaleTimeString("th-TH", { hour12: false });

    // Check ESP32 Status if Firebase is connected
    if (isFirebaseConnected) {
        const timeDiff = Date.now() - esp32LastSeen;
        if (esp32LastSeen === 0 || timeDiff > 60000) { // 60 seconds timeout
            updateConnectionStatus(false, "ESP32 ขาดการเชื่อมต่อ");
        } else {
            updateConnectionStatus(true, "เชื่อมต่อระบบแล้ว");
        }
    }
}, 1000);

// Anonymous Auth
signInAnonymously(auth).catch((error) => {
    console.error("Auth error:", error);
    showToast("เข้าสู่ระบบ Firebase ไม่สำเร็จ: " + error.message);
});

onAuthStateChanged(auth, (user) => {
    if (user) {
        isFirebaseConnected = true;
        updateConnectionStatus(false, "กำลังค้นหา ESP32...");
        listenToDatabase();
    } else {
        isFirebaseConnected = false;
        updateConnectionStatus(false, "Web ขาดการเชื่อมต่อ Firebase");
    }
});

function updateConnectionStatus(isOnline, message) {
    if (isOnline) {
        connectionStatusEl.innerHTML = `
            <span class="status-dot online"></span>
            <span class="status-text">${message}</span>
        `;
    } else {
        connectionStatusEl.innerHTML = `
            <span class="status-dot offline"></span>
            <span class="status-text">${message}</span>
        `;
    }
}

// Real-time Database Listeners
function listenToDatabase() {
    // 1. Settings Mode listener
    const settingsRef = ref(db, "smartgarden/settings");
    onValue(settingsRef, (snapshot) => {
        const val = snapshot.val();
        if (val) {
            currentSettings.relay1_mode = val.relay1_mode || "AUTO";
            currentSettings.relay2_mode = val.relay2_mode || "AUTO";
            renderModes();
        }
    });

    // 2. Schedule listener
    const scheduleRef = ref(db, "smartgarden/schedule");
    onValue(scheduleRef, (snapshot) => {
        const val = snapshot.val();
        if (val) {
            currentSchedule.relay1 = Array.isArray(val.relay1) ? val.relay1 : [];
            currentSchedule.relay2 = Array.isArray(val.relay2) ? val.relay2 : [];
            renderSchedules();
        } else {
            currentSchedule = { relay1: [], relay2: [] };
            renderSchedules();
        }
    });

    // 3. Heartbeat listener
    const statusRef = ref(db, "smartgarden/settings/last_update");
    onValue(statusRef, (snapshot) => {
        if (snapshot.exists()) {
            esp32LastSeen = snapshot.val();
        }
    });
}

// Render Mode Buttons and Badges
function renderModes() {
    // Relay 1 (Fountain)
    const r1Badge = document.getElementById("relay1ModeBadge");
    r1Badge.textContent = getModeLabel(currentSettings.relay1_mode);
    r1Badge.className = `mode-badge ${currentSettings.relay1_mode.toLowerCase()}`;

    document.querySelectorAll('[data-relay="relay1"]').forEach(btn => {
        if (btn.dataset.mode === currentSettings.relay1_mode) {
            btn.classList.add("active");
        } else {
            btn.classList.remove("active");
        }
    });

    // Relay 2 (Garden)
    const r2Badge = document.getElementById("relay2ModeBadge");
    r2Badge.textContent = getModeLabel(currentSettings.relay2_mode);
    r2Badge.className = `mode-badge ${currentSettings.relay2_mode.toLowerCase()}`;

    document.querySelectorAll('[data-relay="relay2"]').forEach(btn => {
        if (btn.dataset.mode === currentSettings.relay2_mode) {
            btn.classList.add("active");
        } else {
            btn.classList.remove("active");
        }
    });
}

function getModeLabel(mode) {
    switch (mode) {
        case "AUTO": return "AUTO";
        case "MANUAL_ON": return "เปิดอยู่";
        case "MANUAL_OFF": return "ปิดอยู่";
        default: return mode;
    }
}

// Set Relay Mode
window.setRelayMode = function(relayKey, mode) {
    const settingPath = `smartgarden/settings/${relayKey}_mode`;
    set(ref(db, settingPath), mode)
        .then(() => showToast(`เปลี่ยนโหมดเป็น ${getModeLabel(mode)} สำเร็จ`))
        .catch(err => showToast("เกิดข้อผิดพลาด: " + err.message));
};

// Event Listeners for Mode Buttons
document.addEventListener("click", (e) => {
    const btn = e.target.closest(".btn-mode");
    if (btn) {
        const relay = btn.dataset.relay;
        const mode = btn.dataset.mode;
        if (relay && mode) {
            window.setRelayMode(relay, mode);
        }
    }
});

// Render Schedules
function renderSchedules() {
    // Render Relay 1 (Fountain)
    renderRelayScheduleList("relay1", currentSchedule.relay1, "relay1ScheduleList");

    // Render Relay 2 (Garden)
    renderRelayScheduleList("relay2", currentSchedule.relay2, "relay2ScheduleList");
}

function renderRelayScheduleList(relayKey, slots, containerId) {
    const container = document.getElementById(containerId);
    if (!slots || slots.length === 0) {
        container.innerHTML = `<div class="empty-schedule">ไม่มีตารางเวลาที่ตั้งไว้</div>`;
        return;
    }

    container.innerHTML = slots.map((slot, index) => {
        const startTimeStr = minuteToTimeString(slot.start);
        let detailStr = "";
        
        if (slot.duration !== undefined) {
            detailStr = `ทำงาน ${slot.duration} นาที`;
        } else if (slot.stop !== undefined) {
            detailStr = `ถึง ${minuteToTimeString(slot.stop)}`;
        }

        const isChecked = slot.enable ? "checked" : "";

        return `
            <div class="schedule-item">
                <div class="schedule-info">
                    <div>
                        <div class="slot-time">${startTimeStr}</div>
                        <div class="slot-detail">${detailStr}</div>
                    </div>
                </div>
                <div class="schedule-actions">
                    <label class="toggle-switch">
                        <input type="checkbox" ${isChecked} onchange="toggleSlotEnable('${relayKey}', ${index}, this.checked)">
                        <span class="slider"></span>
                    </label>
                    <button class="btn-icon-action" onclick="openEditModal('${relayKey}', ${index})">
                        <i class="fa-solid fa-pen"></i>
                    </button>
                    <button class="btn-icon-action" onclick="deleteScheduleSlot('${relayKey}', ${index})">
                        <i class="fa-solid fa-trash"></i>
                    </button>
                </div>
            </div>
        `;
    }).join("");
}

// Convert minutes from midnight (e.g. 480) to HH:MM (e.g. "08:00")
function minuteToTimeString(minutes) {
    const hrs = Math.floor(minutes / 60).toString().padStart(2, '0');
    const mins = (minutes % 60).toString().padStart(2, '0');
    return `${hrs}:${mins}`;
}

// Convert HH:MM to minutes from midnight
function timeStringToMinutes(timeStr) {
    const [hrs, mins] = timeStr.split(':').map(Number);
    return (hrs * 60) + mins;
}

// Toggle Slot Enable
window.toggleSlotEnable = function(relayKey, index, enabled) {
    currentSchedule[relayKey][index].enable = enabled;
    saveScheduleToFirebase();
};

// Delete Slot
window.deleteScheduleSlot = function(relayKey, index) {
    if (confirm("คุณต้องการลบตารางเวลานี้ใช่หรือไม่?")) {
        currentSchedule[relayKey].splice(index, 1);
        saveScheduleToFirebase();
    }
};

// Save Full Schedule to Firebase
function saveScheduleToFirebase() {
    // Make sure we pass at least an empty array or object so Firebase doesn't completely lose the child structure unexpectedly
    const dataToSave = {
        relay1: currentSchedule.relay1.length > 0 ? currentSchedule.relay1 : [],
        relay2: currentSchedule.relay2.length > 0 ? currentSchedule.relay2 : []
    };
    
    set(ref(db, "smartgarden/schedule"), dataToSave)
        .then(() => showToast("อัปเดตตารางเวลาเรียบร้อย"))
        .catch(err => showToast("บันทึกไม่สำเร็จ: " + err.message));
}

// Modal Handlers
function populateTimeSelects() {
    const hours = Array.from({length: 24}, (_, i) => i.toString().padStart(2, '0'));
    const minutes = Array.from({length: 60}, (_, i) => i.toString().padStart(2, '0'));
    
    const hOptions = hours.map(h => `<option value="${h}">${h}</option>`).join('');
    const mOptions = minutes.map(m => `<option value="${m}">${m}</option>`).join('');
    
    document.getElementById('startHour').innerHTML = hOptions;
    document.getElementById('stopHour').innerHTML = hOptions;
    document.getElementById('startMinute').innerHTML = mOptions;
    document.getElementById('stopMinute').innerHTML = mOptions;
}

// Call once
populateTimeSelects();

window.openAddModal = function(relayKey) {
    document.getElementById("modalTitle").textContent = "เพิ่มตารางเวลาใหม่";
    document.getElementById("modalTargetRelay").value = relayKey;
    document.getElementById("modalEditIndex").value = "-1";
    
    document.getElementById("startHour").value = "08";
    document.getElementById("startMinute").value = "00";
    document.getElementById("slotEnable").checked = true;

    // Toggle fields based on relay type
    if (relayKey === "relay2") { // Garden watering uses duration
        document.getElementById("durationGroup").style.display = "block";
        document.getElementById("stopTimeGroup").style.display = "none";
        document.getElementById("durationMin").value = 10;
    } else { // Fountain uses stop time
        document.getElementById("durationGroup").style.display = "none";
        document.getElementById("stopTimeGroup").style.display = "block";
        document.getElementById("stopHour").value = "08";
        document.getElementById("stopMinute").value = "30";
    }

    document.getElementById("scheduleModal").classList.add("active");
};

window.openEditModal = function(relayKey, index) {
    const slot = currentSchedule[relayKey][index];
    if (!slot) return;

    document.getElementById("modalTitle").textContent = "แก้ไขตารางเวลา";
    document.getElementById("modalTargetRelay").value = relayKey;
    document.getElementById("modalEditIndex").value = index;
    
    const startStr = minuteToTimeString(slot.start);
    document.getElementById("startHour").value = startStr.split(':')[0];
    document.getElementById("startMinute").value = startStr.split(':')[1];
    
    document.getElementById("slotEnable").checked = slot.enable !== false;

    if (relayKey === "relay2" || slot.duration !== undefined) {
        document.getElementById("durationGroup").style.display = "block";
        document.getElementById("stopTimeGroup").style.display = "none";
        document.getElementById("durationMin").value = slot.duration || 10;
    } else {
        document.getElementById("durationGroup").style.display = "none";
        document.getElementById("stopTimeGroup").style.display = "block";
        
        const stopStr = minuteToTimeString(slot.stop || (slot.start + 30));
        document.getElementById("stopHour").value = stopStr.split(':')[0];
        document.getElementById("stopMinute").value = stopStr.split(':')[1];
    }

    document.getElementById("scheduleModal").classList.add("active");
};

window.closeModal = function() {
    document.getElementById("scheduleModal").classList.remove("active");
};

window.saveScheduleSlot = function() {
    const relayKey = document.getElementById("modalTargetRelay").value;
    const index = parseInt(document.getElementById("modalEditIndex").value, 10);
    
    const startH = document.getElementById("startHour").value;
    const startM = document.getElementById("startMinute").value;
    const startTimeStr = `${startH}:${startM}`;
    
    const isEnable = document.getElementById("slotEnable").checked;

    const startMin = timeStringToMinutes(startTimeStr);

    let newSlot = {
        enable: isEnable,
        start: startMin
    };

    if (relayKey === "relay2") {
        const duration = parseInt(document.getElementById("durationMin").value, 10) || 10;
        newSlot.duration = duration;
    } else {
        const stopH = document.getElementById("stopHour").value;
        const stopM = document.getElementById("stopMinute").value;
        const stopTimeStr = `${stopH}:${stopM}`;
        
        const stopMin = timeStringToMinutes(stopTimeStr);
        if (stopMin <= startMin) {
            alert("เวลาสิ้นสุดต้องมากกว่าเวลาเริ่มต้น");
            return;
        }
        newSlot.stop = stopMin;
    }

    if (index === -1) {
        // Add new
        currentSchedule[relayKey].push(newSlot);
    } else {
        // Edit existing
        currentSchedule[relayKey][index] = newSlot;
    }

    saveScheduleToFirebase();
    closeModal();
};

// Toast notification helper
function showToast(msg) {
    const toast = document.getElementById("toast");
    toast.textContent = msg;
    toast.classList.add("show");
    setTimeout(() => {
        toast.classList.remove("show");
    }, 3000);
}

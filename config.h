#ifndef CONFIG_H
#define CONFIG_H

//==============================
// Firmware
//==============================

#define FW_NAME        "Smart Garden V2"
#define FW_VERSION     "1.0.0"

//==============================
// Relay Pin
//==============================

#define RELAY1_PIN     16      // น้ำพุ
#define RELAY2_PIN     17      // รดน้ำ

//==============================
// Relay Logic
//==============================
// Active HIGH Relay Module
#define RELAY_ON       HIGH
#define RELAY_OFF      LOW

//==============================
// Time
//==============================

#define GMT_OFFSET_SEC     (7 * 3600)
#define DAYLIGHT_OFFSET    0

#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.google.com"

//==============================
// Heartbeat
//==============================

#define HEARTBEAT_INTERVAL      30000UL

//==============================
// WiFi
//==============================

#define WIFI_TIMEOUT            30000UL
#define WIFI_RETRY_INTERVAL      500UL

//==============================
// Scheduler
//==============================

#define MAX_RELAY1_SCHEDULE     20
#define MAX_RELAY2_SCHEDULE     20

#endif
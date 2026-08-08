#include "firebase_manager.h"

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Firebase_ESP_Client.h>

#include "firebase_config.h"
#include "scheduler.h"


// =====================================================
// Firebase Objects
// =====================================================

FirebaseData fbdo;

FirebaseAuth auth;

FirebaseConfig config;


// =====================================================
// Status
// =====================================================

bool firebaseStarted = false;

bool firebaseAuthenticated = false;


// =====================================================
// Sync Control
// =====================================================

unsigned long lastFirebaseSync = 0;

const unsigned long FIREBASE_SYNC_INTERVAL = 5000;


// =====================================================
// Last Data
// =====================================================

String lastScheduleJSON = "";

String lastRelay1Mode = "";

String lastRelay2Mode = "";


// =====================================================
// WiFi Reconnect
// =====================================================

unsigned long lastWiFiCheck = 0;

const unsigned long WIFI_CHECK_INTERVAL = 5000;

bool wifiWasDisconnected = false;

unsigned long wifiReconnectStart = 0;

const unsigned long WIFI_RECONNECT_TIMEOUT = 30000;


// =====================================================
// Firebase Begin
// =====================================================

void firebaseBegin()
{
    Serial.println();
    Serial.println("======================");
    Serial.println("Firebase");
    Serial.println("======================");


    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(
            "[Firebase] WiFi not connected"
        );

        return;
    }


    // -----------------------------------------
    // Firebase Config
    // -----------------------------------------

    config.api_key =
        FIREBASE_API_KEY;

    config.database_url =
        FIREBASE_DATABASE_URL;


    // -----------------------------------------
    // Anonymous Authentication
    // -----------------------------------------

    Serial.println(
        "[Firebase] Anonymous login..."
    );


    if (
        Firebase.signUp(
            &config,
            &auth,
            "",
            ""
        )
    )
    {
        Serial.println(
            "[Firebase] Anonymous login OK"
        );

        firebaseAuthenticated = true;
    }
    else
    {
        Serial.print(
            "[Firebase] Anonymous login FAILED : "
        );

        Serial.println(
            config.signer.signupError.message.c_str()
        );

        firebaseAuthenticated = false;

        return;
    }


    // -----------------------------------------
    // Start Firebase
    // -----------------------------------------

    Firebase.begin(
        &config,
        &auth
    );

    Firebase.reconnectWiFi(true);

    firebaseStarted = true;

    Serial.println(
        "[Firebase] Started"
    );
}


// =====================================================
// WiFi Reconnect
// =====================================================

void firebaseCheckWiFi()
{
    if (
        millis() - lastWiFiCheck <
        WIFI_CHECK_INTERVAL
    )
    {
        return;
    }

    lastWiFiCheck = millis();


    // -----------------------------------------
    // WiFi connected
    // -----------------------------------------

    if (
        WiFi.status() == WL_CONNECTED
    )
    {
        // -----------------------------------------
        // Was disconnected → just reconnected
        // -----------------------------------------

        if (wifiWasDisconnected)
        {
            Serial.println(
                "[WiFi] Reconnected"
            );

            Serial.print(
                "[WiFi] IP : "
            );

            Serial.println(
                WiFi.localIP()
            );


            wifiWasDisconnected = false;


            // -------------------------------------
            // Reset Firebase (fix stale SSL)
            // -------------------------------------

            Serial.println(
                "[Firebase] Resetting after WiFi reconnect..."
            );


            fbdo.clear();

            firebaseStarted = false;

            firebaseAuthenticated = false;

            lastRelay1Mode = "";

            lastRelay2Mode = "";

            lastScheduleJSON = "";

            lastFirebaseSync = 0;


            delay(1000);


            firebaseBegin();
        }

        return;
    }


    // -----------------------------------------
    // WiFi disconnected
    // -----------------------------------------

    if (!wifiWasDisconnected)
    {
        Serial.println();
        Serial.println(
            "[WiFi] Disconnected"
        );

        Serial.println(
            "[WiFi] Waiting for reconnect..."
        );

        wifiWasDisconnected = true;

        wifiReconnectStart = millis();
    }


    // -----------------------------------------
    // Try reconnect
    // -----------------------------------------

    if (
        millis() - wifiReconnectStart <
        WIFI_RECONNECT_TIMEOUT
    )
    {
        Serial.println(
            "[WiFi] Reconnecting..."
        );

        WiFi.reconnect();
    }
    else
    {
        Serial.println(
            "[WiFi] Reconnect timeout, restarting WiFi..."
        );

        WiFi.disconnect(true);

        delay(1000);

        WiFi.begin();

        wifiReconnectStart = millis();
    }
}


// =====================================================
// Firebase Loop
// =====================================================

void firebaseLoop()
{
    // -----------------------------------------
    // Always check WiFi
    // -----------------------------------------

    firebaseCheckWiFi();


    // -----------------------------------------
    // Offline
    // -----------------------------------------

    if (
        WiFi.status() != WL_CONNECTED
    )
    {
        return;
    }


    // -----------------------------------------
    // Firebase not ready
    // -----------------------------------------

    if (
        !firebaseStarted ||
        !firebaseAuthenticated
    )
    {
        return;
    }


    // -----------------------------------------
    // Sync interval
    // -----------------------------------------

    if (
        millis() - lastFirebaseSync <
        FIREBASE_SYNC_INTERVAL
    )
    {
        return;
    }

    lastFirebaseSync = millis();


    firebaseSync();
}


// =====================================================
// Firebase Sync
// =====================================================

void firebaseSync()
{
    if (
        !Firebase.ready()
    )
    {
        Serial.println(
            "[Firebase] Not ready"
        );

        return;
    }


    Serial.println();
    Serial.println(
        "[Firebase] Sync"
    );


    bool changed = false;


    // =================================================
    // Relay 1 Mode
    // =================================================

    if (
        Firebase.RTDB.getString(
            &fbdo,
            "/smartgarden/settings/relay1_mode"
        )
    )
    {
        String mode =
            fbdo.stringData();


        if (
            mode !=
            lastRelay1Mode
        )
        {
            Serial.print(
                "[Firebase] Relay1 Mode changed: "
            );

            Serial.println(mode);


            lastRelay1Mode =
                mode;

            changed = true;
        }
    }
    else
    {
        Serial.print(
            "[Firebase] Relay1 ERROR : "
        );

        Serial.println(
            fbdo.errorReason()
        );
    }


    // =================================================
    // Relay 2 Mode
    // =================================================

    if (
        Firebase.RTDB.getString(
            &fbdo,
            "/smartgarden/settings/relay2_mode"
        )
    )
    {
        String mode =
            fbdo.stringData();


        if (
            mode !=
            lastRelay2Mode
        )
        {
            Serial.print(
                "[Firebase] Relay2 Mode changed: "
            );

            Serial.println(mode);


            lastRelay2Mode =
                mode;

            changed = true;
        }
    }
    else
    {
        Serial.print(
            "[Firebase] Relay2 ERROR : "
        );

        Serial.println(
            fbdo.errorReason()
        );
    }


    // =================================================
    // Schedule
    // =================================================

    if (
        Firebase.RTDB.getJSON(
            &fbdo,
            "/smartgarden/schedule"
        )
    )
    {
        String scheduleJSON;


        FirebaseJson *json =
            fbdo.to<FirebaseJson *>();


        if (
            json != nullptr
        )
        {
            json->toString(
                scheduleJSON,
                false
            );


            // -----------------------------------------
            // Schedule changed
            // -----------------------------------------

            if (
                scheduleJSON !=
                lastScheduleJSON
            )
            {
                Serial.println(
                    "[Firebase] Schedule changed"
                );

                Serial.println(
                    scheduleJSON
                );


                // -------------------------------------
                // Save LittleFS
                // -------------------------------------

                File file =
                    LittleFS.open(
                        "/schedule.json",
                        "w"
                    );


                if (file)
                {
                    file.print(
                        scheduleJSON
                    );

                    file.close();


                    Serial.println(
                        "[Firebase] schedule.json saved"
                    );


                    lastScheduleJSON =
                        scheduleJSON;


                    changed = true;
                }
                else
                {
                    Serial.println(
                        "[Firebase] Cannot open schedule.json"
                    );
                }
            }
        }
    }
    else
    {
        Serial.print(
            "[Firebase] Schedule ERROR : "
        );

        Serial.println(
            fbdo.errorReason()
        );
    }


    // =================================================
    // Reload Scheduler
    // =================================================

    if (changed)
    {
        Serial.println(
            "[Firebase] Updating Scheduler"
        );


        if (
            schedulerReload()
        )
        {
            Serial.println(
                "[Firebase] Scheduler updated"
            );
        }
        else
        {
            Serial.println(
                "[Firebase] Scheduler reload FAILED"
            );
        }
    }
}
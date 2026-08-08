#include <WiFi.h>
#include "config.h"
#include "secrets.h"
#include "relay.h"
#include "fs_manager.h"
#include "ntp_manager.h"
#include "scheduler.h"

#include <Arduino.h>
#include <LittleFS.h>
#include "relay_control.h"
#include "settings_manager.h"
#include "scheduler.h"
#include "firebase_manager.h"


void connectWiFi()
{
  Serial.println();
  Serial.println("======================");
  Serial.println("WiFi");
  Serial.println("======================");

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.print("Connecting WiFi : ");
  Serial.println(WIFI_SSID);

  int count = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    count < 30
  )
  {
    delay(500);
    Serial.print(".");
    count++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected");

    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

    Serial.print("RSSI : ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  else
  {
    Serial.println("WiFi Connection Failed");
    Serial.println("Running OFFLINE");
  }
}



void setup()
{

  Serial.begin(115200);


  delay(1000);


  Serial.println(
    "=========================="
  );

  Serial.println(
    "SMART GARDEN V2"
  );

  

  // Relay
  relayBegin();

  connectWiFi();

  // NTP
  ntpBegin();

  // LittleFS
  if(!LittleFS.begin())
  {

    Serial.println(
      "LittleFS Mount Failed"
    );

    return;

  }


  Serial.println(
    "LittleFS Mounted"
  );



  // Settings
  settingsBegin();



  // Relay Mode
  relayControlBegin();



  // Scheduler
  schedulerBegin();


    // ==========================================
    // Firebase
    // ==========================================

    firebaseBegin();

    Serial.println(
        "System Ready"
    );


}


unsigned long lastPrint = 0;
unsigned long lastScheduler = 0;

void loop()
{
    schedulerLoop();

    firebaseLoop();

    delay(100);
}
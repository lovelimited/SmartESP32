#include "ntp_manager.h"
#include "config.h"

#include <WiFi.h>
#include <time.h>

bool ntpBegin()
{
    Serial.println();
    Serial.println("======================");
    Serial.println("NTP");
    Serial.println("======================");

    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET,
        NTP_SERVER_1,
        NTP_SERVER_2);

    struct tm timeinfo;

    Serial.print("Sync");

    int retry = 0;

    while (!getLocalTime(&timeinfo))
    {
        delay(500);
        Serial.print(".");

        retry++;

        if (retry > 20)
        {
            Serial.println();
            Serial.println("NTP Failed");
            return false;
        }
    }

    Serial.println();
    Serial.println("NTP OK");

    Serial.println(getDateTime());

    return true;
}

bool ntpUpdate()
{
    struct tm timeinfo;

    return getLocalTime(&timeinfo);
}

String getDateTime()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return "Unknown";

    char buffer[30];

    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

    return String(buffer);
}

String getTimeString()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return "--:--:--";

    char buffer[10];

    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

    return String(buffer);
}

int getHour()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return 0;

    return timeinfo.tm_hour;
}

int getMinute()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return 0;

    return timeinfo.tm_min;
}

int getSecond()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return 0;

    return timeinfo.tm_sec;
}
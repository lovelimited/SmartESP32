#include "fs_manager.h"
#include <LittleFS.h>

bool fsBegin()
{

    Serial.println();

    Serial.println("======================");
    Serial.println("LittleFS");
    Serial.println("======================");

    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    Serial.println("LittleFS Mounted");

    createDefaultFiles();

    return true;
}

//----------------------------

bool createDefaultFiles()
{

    if (!LittleFS.exists("/schedule.json"))
    {

        File file = LittleFS.open("/schedule.json", "w");

        file.print("{}");

        file.close();

        Serial.println("Create schedule.json");

    }

    if (!LittleFS.exists("/settings.json"))
    {

        File file = LittleFS.open("/settings.json", "w");

        file.print("{}");

        file.close();

        Serial.println("Create settings.json");

    }

    return true;

}

//----------------------------

bool loadSchedule()
{

    File file = LittleFS.open("/schedule.json", "r");

    if (!file)
    {
        Serial.println("Open schedule fail");
        return false;
    }

    Serial.println();

    Serial.println("Schedule File");

    while (file.available())
    {
        Serial.write(file.read());
    }

    Serial.println();

    file.close();

    return true;

}

//----------------------------

bool saveSchedule()
{
    return true;
}

//----------------------------

void listFiles()
{

    File root = LittleFS.open("/");

    File file = root.openNextFile();

    while (file)
    {

        Serial.print(file.name());

        Serial.print("   ");

        Serial.println(file.size());

        file = root.openNextFile();

    }

}
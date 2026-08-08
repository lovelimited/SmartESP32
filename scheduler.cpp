#include "scheduler.h"

#include "relay.h"
#include "ntp_manager.h"
#include "relay_control.h"

#include <ArduinoJson.h>
#include <LittleFS.h>


ScheduleSlot fountainSchedule[MAX_SLOT];

ScheduleSlot wateringSchedule[MAX_SLOT];


bool schedulerFountainState = false;

bool schedulerWateringState = false;



//================================
// Create Default Schedule
//================================

void createScheduleFile()
{

    File file =
        LittleFS.open(
            "/schedule.json",
            "w"
        );


    if(!file)
    {
        Serial.println(
            "[FS] Create Fail"
        );
        return;
    }



    StaticJsonDocument<512> doc;



    JsonArray relay1 =
        doc.createNestedArray(
            "relay1"
        );


    JsonObject r1 =
        relay1.createNestedObject();


    r1["enable"] = true;
    r1["start"] = 480;
    r1["stop"] = 510;



    JsonArray relay2 =
        doc.createNestedArray(
            "relay2"
        );


    JsonObject r2 =
        relay2.createNestedObject();


    r2["enable"] = true;
    r2["start"] = 360;
    r2["duration"] = 10;



    serializeJson(
        doc,
        file
    );


    file.close();


    Serial.println(
        "[FS] schedule.json created"
    );

}



//================================
// Begin
//================================

void schedulerBegin()
{

    Serial.println();

    Serial.println(
        "======================"
    );

    Serial.println(
        "Scheduler V2.3"
    );

    Serial.println(
        "======================"
    );


    schedulerClear();



    if(!schedulerLoad())
    {

        Serial.println(
            "[Scheduler] Create Default"
        );


        createScheduleFile();


        schedulerLoad();

    }



    schedulerPrint();


    Serial.println(
        "[Scheduler] Ready"
    );

}



//================================
// Clear
//================================

void schedulerClear()
{

    for(int i=0;i<MAX_SLOT;i++)
    {

        fountainSchedule[i].enable=false;
        fountainSchedule[i].start=0;
        fountainSchedule[i].stop=0;
        fountainSchedule[i].duration=0;



        wateringSchedule[i].enable=false;
        wateringSchedule[i].start=0;
        wateringSchedule[i].stop=0;
        wateringSchedule[i].duration=0;

    }

}



//================================
// Load JSON
//================================

bool schedulerLoad()
{

    if(!LittleFS.exists(
        "/schedule.json"
    ))
    {
        return false;
    }



    File file =
        LittleFS.open(
            "/schedule.json",
            "r"
        );


    if(!file)
        return false;



    StaticJsonDocument<4096> doc;



    DeserializationError error =
        deserializeJson(
            doc,
            file
        );


    file.close();



    if(error)
    {

        Serial.println(
            "[JSON] Error"
        );

        return false;

    }



    if(!doc.containsKey("relay1") &&
       !doc.containsKey("relay2"))
    {

        Serial.println(
            "[Scheduler] Empty JSON"
        );

        return false;

    }



    schedulerClear();



    //========================
    // Relay1
    //========================

    JsonArray r1 =
        doc["relay1"];


    int index=0;


    for(JsonObject item:r1)
    {

        if(index>=MAX_SLOT)
            break;


        fountainSchedule[index].enable =
            item["enable"] | false;


        fountainSchedule[index].start =
            item["start"] | 0;


        fountainSchedule[index].stop =
            item["stop"] | 0;


        index++;

    }



    //========================
    // Relay2 Duration
    //========================

    JsonArray r2 =
        doc["relay2"];


    index=0;


    for(JsonObject item:r2)
    {

        if(index>=MAX_SLOT)
            break;



        wateringSchedule[index].enable =
            item["enable"] | false;


        wateringSchedule[index].start =
            item["start"] | 0;



        if(item.containsKey("duration"))
        {

            wateringSchedule[index].duration =
                item["duration"];

        }
        else
        {

            uint16_t stop =
                item["stop"] | 0;


            wateringSchedule[index].duration =
                stop -
                wateringSchedule[index].start;

        }


        index++;

    }



    Serial.println(
        "[Scheduler] JSON Loaded"
    );


    return true;

}



//================================
// Reload
//================================

bool schedulerReload()
{

    Serial.println(
        "[Scheduler] Reload"
    );


    if(schedulerLoad())
    {

        schedulerPrint();


        Serial.println(
            "[Scheduler] Reload OK"
        );


        return true;

    }


    return false;

}



//================================
// Current Minute
//================================

uint16_t currentMinute()
{

    return
    getHour()*60 +
    getMinute();

}



//================================
// Check Time
//================================

bool checkTime(
    ScheduleSlot s,
    uint16_t now
)
{

    if(!s.enable)
        return false;


    return
    (
        now >= s.start &&
        now < s.stop
    );

}



//================================
// Check Duration
//================================

bool checkDuration(
    ScheduleSlot s,
    uint16_t now
)
{

    if(!s.enable)
        return false;


    return
    (
        now >= s.start &&
        now < (s.start+s.duration)
    );

}



//================================
// Scheduler Loop
//================================

void schedulerLoop()
{

    uint16_t now =
        currentMinute();



    bool fountain=false;

    bool watering=false;



    for(int i=0;i<MAX_SLOT;i++)
    {

        if(checkTime(
            fountainSchedule[i],
            now))
        {
            fountain=true;
        }



        if(checkDuration(
            wateringSchedule[i],
            now))
        {
            watering=true;
        }

    }



    //========================
    // Relay1
    //========================

    if(relay1Mode == MODE_AUTO)
    {

        if(fountain != schedulerFountainState)
        {

            schedulerFountainState=fountain;


            if(fountain)
            {

                relay1On();

                Serial.println(
                    "[Scheduler] Fountain ON"
                );

            }
            else
            {

                relay1Off();

                Serial.println(
                    "[Scheduler] Fountain OFF"
                );

            }

        }

    }



    //========================
    // Relay2
    //========================

    if(relay2Mode == MODE_AUTO)
    {

        if(watering != schedulerWateringState)
        {

            schedulerWateringState=watering;


            if(watering)
            {

                relay2On();

                Serial.println(
                    "[Scheduler] Water ON"
                );

            }
            else
            {

                relay2Off();

                Serial.println(
                    "[Scheduler] Water OFF"
                );

            }

        }

    }

}



//================================
// Print
//================================

void schedulerPrint()
{

    Serial.println();

    Serial.println(
        "------ Schedule ------"
    );


    Serial.println(
        "Relay1 Fountain"
    );


    for(int i=0;i<MAX_SLOT;i++)
    {

        if(fountainSchedule[i].enable)
        {

            Serial.printf(
                "Slot %d : %s - %s\n",
                i,
                minuteToTime(
                    fountainSchedule[i].start
                ).c_str(),

                minuteToTime(
                    fountainSchedule[i].stop
                ).c_str()
            );

        }

    }



    Serial.println(
        "Relay2 Water"
    );


    for(int i=0;i<MAX_SLOT;i++)
    {

        if(wateringSchedule[i].enable)
        {

            Serial.printf(
                "Slot %d : %s + %d min\n",
                i,

                minuteToTime(
                    wateringSchedule[i].start
                ).c_str(),

                wateringSchedule[i].duration
            );

        }

    }


    Serial.println(
        "----------------------"
    );

}



//================================
// Convert Minute
//================================

String minuteToTime(
    uint16_t value
)
{

    char buf[10];


    sprintf(
        buf,
        "%02d:%02d",
        value/60,
        value%60
    );


    return String(buf);

}
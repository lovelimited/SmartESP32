#include "settings_manager.h"

#include "relay_control.h"

#include <ArduinoJson.h>
#include <LittleFS.h>



#define SETTINGS_FILE "/settings.json"



//================================
// Create Default
//================================

void createSettingsFile()
{

    File file =
        LittleFS.open(
            SETTINGS_FILE,
            "w"
        );


    if(!file)
    {
        Serial.println(
            "[Settings] Create Fail"
        );

        return;
    }



    StaticJsonDocument<512> doc;



    doc["relay1_mode"] =
        "AUTO";


    doc["relay2_mode"] =
        "AUTO";


    doc["relay1_manual"] =
        false;


    doc["relay2_manual"] =
        false;



    serializeJson(
        doc,
        file
    );


    file.close();


    Serial.println(
        "[Settings] Created"
    );

}



//================================
// Begin
//================================

void settingsBegin()
{

    Serial.println();

    Serial.println(
        "======================"
    );

    Serial.println(
        "Settings"
    );

    Serial.println(
        "======================"
    );


    if(!settingsLoad())
    {

        createSettingsFile();

        settingsLoad();

    }


}



//================================
// Load
//================================

bool settingsLoad()
{

    if(!LittleFS.exists(
        SETTINGS_FILE
    ))
    {
        return false;
    }



    File file =
        LittleFS.open(
            SETTINGS_FILE,
            "r"
        );


    if(!file)
        return false;



    StaticJsonDocument<512> doc;



    DeserializationError error =
        deserializeJson(
            doc,
            file
        );


    file.close();



    if(error)
    {
        Serial.println(
            "[Settings] JSON Error"
        );

        return false;
    }




    String mode1 =
        doc["relay1_mode"] |
        "AUTO";


    String mode2 =
        doc["relay2_mode"] |
        "AUTO";



    setRelay1Mode(
        stringToMode(mode1)
    );


    setRelay2Mode(
        stringToMode(mode2)
    );



    Serial.println(
        "[Settings] Loaded"
    );


    Serial.print(
        "Relay1 Mode : "
    );

    Serial.println(
        modeToString(
            relay1Mode
        )
    );


    Serial.print(
        "Relay2 Mode : "
    );

    Serial.println(
        modeToString(
            relay2Mode
        )
    );



    return true;

}



//================================
// Save
//================================

bool settingsSave()
{

    File file =
        LittleFS.open(
            SETTINGS_FILE,
            "w"
        );


    if(!file)
        return false;



    StaticJsonDocument<512> doc;



    doc["relay1_mode"] =
        modeToString(
            relay1Mode
        );


    doc["relay2_mode"] =
        modeToString(
            relay2Mode
        );



    serializeJson(
        doc,
        file
    );


    file.close();


    Serial.println(
        "[Settings] Saved"
    );


    return true;

}
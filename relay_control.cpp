#include "relay_control.h"

#include "relay.h"



RelayMode relay1Mode =
    MODE_AUTO;


RelayMode relay2Mode =
    MODE_AUTO;



bool relay1ManualState=false;

bool relay2ManualState=false;



void relayControlBegin()
{

    Serial.println(
        "[Relay Control] Ready"
    );

}



//===========================
// Relay1 Mode
//===========================

void setRelay1Mode(
    RelayMode mode
)
{

    relay1Mode = mode;


    if(mode == MODE_OFF)
    {
        relay1Off();
    }


    Serial.print(
        "[Relay1 Mode] "
    );


    Serial.println(
        modeToString(mode)
    );

}



//===========================
// Relay2 Mode
//===========================

void setRelay2Mode(
    RelayMode mode
)
{

    relay2Mode = mode;


    if(mode == MODE_OFF)
    {
        relay2Off();
    }


    Serial.print(
        "[Relay2 Mode] "
    );


    Serial.println(
        modeToString(mode)
    );

}



//===========================
// String -> Mode
//===========================

RelayMode stringToMode(
    String mode
)
{

    mode.toUpperCase();


    if(mode=="MANUAL")
        return MODE_MANUAL;


    if(mode=="OFF")
        return MODE_OFF;


    return MODE_AUTO;

}



//===========================
// Mode -> String
//===========================

String modeToString(
    RelayMode mode
)
{

    switch(mode)
    {

        case MODE_MANUAL:
            return "MANUAL";


        case MODE_OFF:
            return "OFF";


        default:
            return "AUTO";

    }

}
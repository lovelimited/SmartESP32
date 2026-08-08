#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H


#include <Arduino.h>


enum RelayMode
{
    MODE_AUTO,
    MODE_MANUAL,
    MODE_OFF
};


extern RelayMode relay1Mode;

extern RelayMode relay2Mode;



void relayControlBegin();


void setRelay1Mode(RelayMode mode);

void setRelay2Mode(RelayMode mode);



RelayMode stringToMode(String mode);

String modeToString(RelayMode mode);



#endif
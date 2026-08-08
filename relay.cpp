#include "relay.h"
#include "config.h"

bool relay1_status = false;
bool relay2_status = false;

void relayBegin()
{
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);

    relayAllOff();

    Serial.println("[Relay] Ready");
}

//=====================
// Relay1
//=====================

void relay1On()
{
    digitalWrite(RELAY1_PIN, RELAY_ON);
    relay1_status = true;

    Serial.println("[Relay1] ON");
}

void relay1Off()
{
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    relay1_status = false;

    Serial.println("[Relay1] OFF");
}

bool relay1State()
{
    return relay1_status;
}

//=====================
// Relay2
//=====================

void relay2On()
{
    digitalWrite(RELAY2_PIN, RELAY_ON);
    relay2_status = true;

    Serial.println("[Relay2] ON");
}

void relay2Off()
{
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    relay2_status = false;

    Serial.println("[Relay2] OFF");
}

bool relay2State()
{
    return relay2_status;
}

//=====================

void relayAllOff()
{
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    digitalWrite(RELAY2_PIN, RELAY_OFF);

    relay1_status = false;
    relay2_status = false;

    Serial.println("[Relay] All OFF");
}
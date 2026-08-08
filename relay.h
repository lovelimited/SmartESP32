#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

// เริ่มต้น Relay
void relayBegin();

// Relay 1 (น้ำพุ)
void relay1On();
void relay1Off();
bool relay1State();

// Relay 2 (รดน้ำ)
void relay2On();
void relay2Off();
bool relay2State();

// ปิดทุก Relay
void relayAllOff();

#endif
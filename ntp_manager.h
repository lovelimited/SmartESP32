#ifndef NTP_MANAGER_H
#define NTP_MANAGER_H

#include <Arduino.h>

bool ntpBegin();

bool ntpUpdate();

String getDateTime();

String getTimeString();

int getHour();

int getMinute();

int getSecond();

#endif
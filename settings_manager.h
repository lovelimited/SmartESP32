#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>


void settingsBegin();

bool settingsLoad();

bool settingsSave();


#endif
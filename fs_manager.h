#ifndef FS_MANAGER_H
#define FS_MANAGER_H

#include <Arduino.h>

bool fsBegin();

bool loadSchedule();

bool saveSchedule();

bool createDefaultFiles();

void listFiles();

#endif
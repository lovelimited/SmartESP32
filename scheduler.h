#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

#define MAX_SLOT 10


struct ScheduleSlot
{
    bool enable;

    uint16_t start;

    uint16_t stop;

    uint16_t duration;
};



extern ScheduleSlot fountainSchedule[MAX_SLOT];

extern ScheduleSlot wateringSchedule[MAX_SLOT];


extern bool schedulerFountainState;

extern bool schedulerWateringState;



void schedulerBegin();

void schedulerLoop();

void schedulerPrint();

void schedulerClear();

bool schedulerLoad();

bool schedulerReload();



uint16_t currentMinute();

String minuteToTime(uint16_t value);


#endif
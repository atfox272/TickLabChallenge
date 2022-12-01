#ifndef _adjustdata_h
#define _adjustdata_h

#include "display.h"
#include "macroPin.h"
#include "macroConst.h"
#include "globalVar.h"

long overflow_sta(volatile long num, char sta_type) {
  switch (sta_type) {
     case 't':     //overflow TIME
      num = (num >= CTIME_AMOUNT) ? num % CTIME_AMOUNT : (num < START_VALUE) ? num + CTIME_AMOUNT : num;
      break;
    case 'm':     //overflow MODE
      num = (num >= MODE_AMOUNT) ? num % MODE_AMOUNT : (num < START_VALUE) ? num + MODE_AMOUNT : num;
      break;
    case 's':     // overflow SET
      num = (num >= SET_AMOUNT) ? num % SET_AMOUNT : (num < START_VALUE) ? num + SET_AMOUNT : num;
      break;
    case 'S':     // overflow SET (special overflow for Mode 1)
      num = (num >= SET_AMOUNT) ? num % SET_AMOUNT + 1 : (num < START_VALUE) ? num + SET_AMOUNT : num;
      break;
    case 'c':     // count down min:sec
      num = (num >= COUNTD_AMOUNT) ? num % COUNTD_AMOUNT : (num < START_VALUE) ? num + COUNTD_AMOUNT : num;
      break;
    case 'C':     // count up sec:msec
      num = (num >= COUNTU_AMOUNT) ? num % COUNTU_AMOUNT : (num < START_VALUE) ? num + COUNTU_AMOUNT : num;
      break;
  }
  return num;
}



void adjust_enable(bool inc_flag = false, bool dec_flag = false) {
  if(inc_flag) {
      TCNT1 = 0;  
      switch (mode) {
        case 1:                         // Set time mode
          (select == 1) ? count_per_025sec += (CTIME_AMOUNT / 24) : count_per_025sec += (CTIME_AMOUNT / 24 / 60);
          count_per_025sec = overflow_sta(count_per_025sec, 't');
          // Check logic
          Serial.println(count_per_025sec);
          break;
        case 2:                         // Set alarm time mode
          (select == 1) ? alarm_time += (CTIME_AMOUNT / 24) : alarm_time += (CTIME_AMOUNT / 24 / 60);
          alarm_time = overflow_sta(alarm_time, 't');
          // Check logic
          Serial.print("Alarm time:\t");
          Serial.println(alarm_time);
          break;
        case 4:                           // Set countdown time mode
          // Set countdown time mode???? //////////////////////////////
          (select == 1) ? countDown_time += (CTIME_AMOUNT / 24 / 60) : countDown_time += (CTIME_AMOUNT / 24 / 60 / 60);
          countDown_time = overflow_sta(countDown_time, 'c');
          break;
      }
    }
    else if(dec_flag) {
      TCNT1 = 0;
      switch (mode) {
        case 1:                         // Set time mode
          (select == 1) ? count_per_025sec -= (CTIME_AMOUNT / 24) : count_per_025sec -= (CTIME_AMOUNT / 24 / 60);
          count_per_025sec = overflow_sta(count_per_025sec, 't');
          // Check logic
          Serial.println(count_per_025sec);
          break;
        case 2:                         // Set alarm time mode
          (select == 1) ? alarm_time -= (CTIME_AMOUNT / 24) : alarm_time -= (CTIME_AMOUNT / 24 / 60);
          alarm_time = overflow_sta(alarm_time, 't');
          // Check logic
          Serial.print("Alarm time:\t");
          Serial.println(alarm_time);
          break;
        case 4:                           // Set countdown time mode
          // Set countdown time mode???? //////////////////////////////
          (select == 1) ? countDown_time -= (CTIME_AMOUNT / 24 / 60) : countDown_time -= (CTIME_AMOUNT / 24 / 60 / 60);
          countDown_time = overflow_sta(countDown_time, 'c');
          break;
      }
    }
}

#endif

#ifndef _global_variable_h
#define _global_variable_h

#include "macroPin.h"
#include "macroConst.h"

// External Interrupt type
bool exINT_mode = (MODE_PIN == 2) ? 0 : 1; // Select INT 0
bool exINT_set = (SET_PIN == 3) ? 1 : 0;   // Select INT 1

// Time var
volatile long count_per_025sec = 0;       // 0 -> 8639999  (00:00:00:00 -> 23:59:59:99)
volatile long alarm_time = 0;             // 0 -> 8639999  (00:00:00:00 -> 23:59:59:99)
volatile long countUp_time = 0;           // 0 -> 9999     (00:00:00:00 -> 00:00:99:99)
volatile long countDown_time = 0;         // 0 -> 599999   (00:00:00:00 -> 00:99:59:99)
// State var
volatile byte mode = 0;                   // 0 -> 5
volatile byte select = 0;                 // 0 -> 2

// Adjust flag
volatile bool inc_enable = false;         // increase enable flag
volatile bool dec_enable = false;         // decrease enable flag

volatile bool countUp_enable = false;     // 
volatile bool countDown_enable = false;   // 
volatile bool isCounting = false;         // isCounting = countUp_enable || countDown_enable
volatile bool isAlarming = false;
volatile bool turnOn_alarmMode = false;
volatile bool hold_PLUSbutton = false;

// Numbers of led
volatile byte led_num = 1;                // 1 -> 4
volatile bool colon   = false;
// Temp
volatile unsigned int temp_count = 0;
volatile unsigned int temp_count2 = 0;    // Use for turning ON / OFF alarm mode

#endif

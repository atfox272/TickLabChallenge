#ifndef _global_variable_h
#define _global_variable_h

#include "macroPin.h"
#include "macroConst.h"

// External Interrupt type
bool exINT_type = (INT_PIN == 2) ? 0 : 1; // Select INT 0

// Time var
volatile long count_per_025sec = 0;       // 0 -> 8639999  (00:00:00:00 -> 23:59:59:99)
volatile long alarm_time = 0;             // 0 -> 8639999  (00:00:00:00 -> 23:59:59:99)
volatile long countUp_time = 0;           // 0 -> 9999     (00:00:00:00 -> 00:00:99:99)
volatile long countDown_time = 0;         // 0 -> 599999   (00:00:00:00 -> 00:99:59:99)
// State var
volatile int mode = 0;                    // 0 -> 5
volatile int select = 0;                  // 0 -> 2

// Adjust flag
volatile bool inc_enable = false;         // increase enable flag
volatile bool dec_enable = false;         // decrease enable flag
volatile bool countUp_enable = false;     // 
volatile bool countDown_enable = false;   // 

// Temp
volatile unsigned int temp_count = 0;

#endif

#ifndef _macro_const_h
#define _macro_const_h

#define FREQUENCY       100
#define FREQUENCY_LED   2500      // Led sweeping (Hz)
#define START_VALUE     0     
#define CTIME_AMOUNT    8640000   // 0        ->  8639999 = 24*60*60*FREQUENCY
#define COUNTD_AMOUNT   600000    // 0        ->  599999  = 100*60*FREQUENCY
#define COUNTU_AMOUNT   10000     // 0        ->  9999    = 100*FREQUENCY
#define HOLDING_ADJ     75        // (x10ms/times)
#define MODE_AMOUNT     5         // (mode 0) ->  (mode 4)
#define SET_AMOUNT      3         // (set 0)  ->  (set 2)
#define LED_AMOUNT      4         // 4 digit 7 segment led
#define ALARM_DUR       1         // (minutes) Alarm duration 

#endif

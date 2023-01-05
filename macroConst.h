#ifndef _macro_const_h
#define _macro_const_h

#define FREQUENCY           500       
#define FREQUENCY_LED       2500      // Led sweeping 
#define START_VALUE         0     
#define CTIME_AMOUNT        43200000  // 0        ->  43199999 = 24*60*60*FREQUENCY
#define COUNTD_AMOUNT       3000000   // 0        ->  599999  = 100*60*FREQUENCY
#define COUNTU_AMOUNT       350000    // 0        ->  9999    = 100*FREQUENCY
#define HOLDING_ADJ         600       // (x2ms/times)
#define MODE_AMOUNT         5         // (mode 0) ->  (mode 4)
#define SET_AMOUNT          3         // (set 0)  ->  (set 2)
#define LED_AMOUNT          4         // 4 digit 7 segment led
#define ALARM_DUR           1         // (minutes) Alarm duration 
#define TOGGLE_ALARM_HOLD   1500      // x2ms
#define DEBOUNCE_TIME       100       // TIME =  DEBOUNCE_TIME / FREQUENCY (second)
#define DEBOUNCE_HOLD_TIME  20        // TIME =  DEBOUNCE_TIME / FREQUENCY (second)

#endif

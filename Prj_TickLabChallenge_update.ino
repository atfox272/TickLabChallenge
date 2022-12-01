#include "display.h"
#include "AdjustData.h"
#include "macroPin.h"
#include "macroConst.h"
#include "globalVar.h"


void setup() {
  Serial.begin(9600);
  pinMode(INT_PIN, INPUT_PULLUP);
  pinMode(MODE_ENCODE_PIN, INPUT_PULLUP);
  pinMode(SET_ENCODE_PIN, INPUT_PULLUP);
  pinMode(INC_PIN, INPUT_PULLUP);
  pinMode(DEC_PIN, INPUT_PULLUP);
  pinMode(BUZZ_PIN, OUTPUT);

  // Display pin
  pinMode(FIRST_DISP_A, OUTPUT);
  pinMode(FIRST_DISP_B, OUTPUT);
  pinMode(SECOND_DISP_A, OUTPUT);
  pinMode(SECOND_DISP_B, OUTPUT);
  pinMode(SECOND_DISP_C, OUTPUT);
  pinMode(THIRD_DISP_A, OUTPUT);
  pinMode(THIRD_DISP_B, OUTPUT);
  pinMode(THIRD_DISP_C, OUTPUT);
  pinMode(FOURTH_DISP_A, OUTPUT);
  pinMode(FOURTH_DISP_B, OUTPUT);
  pinMode(FOURTH_DISP_C, OUTPUT);
  pinMode(FOURTH_DISP_D, OUTPUT);
  
//  Set up Timer 1  //////////////////////////////////////////
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  TCNT1 = 0;
  TCCR1B = B1011;
  OCR1A = 2499;
  TIMSK1 |= (1 << OCIE1A);
  
//  Set up External Interrupt /////////////////////////////////
  attachInterrupt(exINT_type, ISR_external_INT, FALLING);
  
////Set up PinChange Interrupt/////////////////////////////////
  PCICR = B10;
  PCMSK1 = B1100;
  
}


//  Interrupt service routine  ////////////////////////////////////

//  External Interrput  || "MODE" "SET" signal ////////////////////
void ISR_external_INT() {                                     
  switch(!digitalRead(SET_ENCODE_PIN)) {
    case 1:                                                             // SET signal
      (mode == 1 || mode == 2 || mode == 4) ? select++ :                // Mode 1 / 2/ 4 / 5 absord SET signal
                                              select = select;
      select = (mode != 1) ? int(overflow_sta(select, 's')) :           // Mode 1 DONT have state: (select = 0)
                             int(overflow_sta(select, 'S'));            // => 1 -> 2 -> 1 -> 2 ......
      break;
    case 0:                                                             // MODE signal
      (select != 0 & mode != 1) ? select = 0 : mode++;                  // Mode 1 have only 2 state of select (select = 0 or select = 1)
      mode = int(overflow_sta(mode, 'm'));
      (mode == 1) ? select = 1 : select = 0;                            // Mode 1 - select hour (default)
      break;
  }
  // Check
  Serial.print("MODE");
  Serial.print("\t");
  Serial.println("SET");
  Serial.print(mode);
  Serial.print("\t");
  Serial.println(select);
}

//  TC1 COMPA Interrput  || Normal mode ///////////////////////
ISR (TIMER1_COMPA_vect) {               // Timer Compare Match Interrupt
    (mode != 1) ? count_per_025sec++ : count_per_025sec = count_per_025sec;               // Mode 1 - stop counting
    count_per_025sec = overflow_sta(count_per_025sec, 't');
    
    // Match Alarm time ///////////////////////////////////////
    if(count_per_025sec == alarm_time && mode == 0) digitalWrite(BUZZ_PIN, HIGH);         // Just alarm in Mode 0
    else if(count_per_025sec == overflow_sta(alarm_time, 't') + ALARM_DUR * 60 * FREQUENCY || !digitalRead(INC_PIN)) { 
    // After 2 minutes or "+" signal -> turn off alarm////////
      digitalWrite(BUZZ_PIN, LOW);
    }

    // Adjust in <HOLDING_ADJ (x10ms)> (holding button)/////////
    temp_count++;
    if(temp_count == HOLDING_ADJ - 1) {
      adjust_enable(inc_enable, dec_enable);
      temp_count = 0;
    }
    
    // Count up mode /////////////////////////////
    countUp_time += (countUp_enable) ? 1 : 0;
    countUp_time = overflow_sta(countUp_time, 'C');
    if(mode != 3) {
      countUp_enable = false;
      countUp_time = 0;
    }
    // Count down mode ///////////////////////////
    countDown_time -= (countDown_enable) ? 1 : 0;
    if(countDown_time == 0 & countDown_enable == true) {
      digitalWrite(BUZZ_PIN, HIGH);
      countDown_enable = false;
    }
    // Note: "+" signal -> turn off alarm (use function of alarm mode)
    if(mode != 4) {
      countDown_enable = false;
      countDown_time = 0;
    }
}

/// Pin Change INT || + - signal ////////////////////////////
ISR (PCINT1_vect) {
  if(select != 0) {                           // Adjust mode
    inc_enable = !digitalRead(INC_PIN);
    dec_enable = !digitalRead(DEC_PIN);
    
    adjust_enable(inc_enable, dec_enable);    // Increase / Decrease immediately (press button)

    // Check
    Serial.print("INC");
    Serial.print("\t");
    Serial.println("DEC");
    Serial.print(inc_enable);
    Serial.print("\t");
    Serial.println(dec_enable);
  }
  else if(select == 0 & (mode == 4 || mode == 3)) { // Count Up/Down mode
    switch(mode) {                                  // Mode 4 and Mode 3 have counting mode 
      case 3:
        if(!digitalRead(DEC_PIN)) {
          countUp_time = 0;                         // RESET count up value -> 0
          countUp_enable = false;                   // Stop couting
        }
        else {
          // Turn "PinChange INT" to "External INT" (mode: RISING)
          // RISING EDGE -> Toggle countUp_enable 
          countUp_enable = (!digitalRead(INC_PIN)) ? !countUp_enable : countUp_enable;
        }
        break;
      case 4:
          // Count down
        if(!digitalRead(DEC_PIN)) {
          countDown_time = 0;                         // RESET count up value -> 0
          countDown_enable = false;                   // Stop couting
        }
        else {
          // Turn "PinChange INT" to "External INT" (mode: RISING)
          // RISING EDGE -> Toggle countUp_enable 
          countDown_enable = (countDown_time == 0) ? false :         // Cant count down without setting up countdown time
                             (!digitalRead(INC_PIN)) ? !countDown_enable : countDown_enable;
        }
        break;
    }
    // Check case 3
    Serial.print("ENABLE");
    Serial.print("\t");
    Serial.println("COUNT");
    Serial.print(countUp_enable);
    Serial.print("\t");
    Serial.println(countUp_time);
  }
  else if(select != 0 & mode == 4) {
    inc_enable = !digitalRead(INC_PIN);
    dec_enable = !digitalRead(DEC_PIN);
    
    adjust_enable(inc_enable, dec_enable);    // Increase / Decrease immediately (press button)

  }
}

void loop() {
  switch (mode) {
    case 0:
      disp(count_per_025sec, select, true, false, false);
      break;
    case 1:
      disp(count_per_025sec, select, true, false, false);
      break;
    case 2:
      disp(alarm_time, select, true, false, false);
      break;
    case 3:
      disp(countUp_time, select, false, true, false);
      break;
    case 4:
      disp(countDown_time, select, false, false, true);
      break;
  }
  
}

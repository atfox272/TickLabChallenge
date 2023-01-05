#include "display.h"
#include "AdjustData.h"
#include "macroPin.h"
#include "macroConst.h"
#include "globalVar.h"


void setup() {
  Serial.begin(9600);
  pinMode(COM_INT, INPUT_PULLUP);
  pinMode(INC_ENCODER, INPUT_PULLUP);
  pinMode(DEC_ENCODER, INPUT_PULLUP);
  pinMode(MODE_ENCODER, INPUT_PULLUP);
  pinMode(SET_ENCODER, INPUT_PULLUP);
  
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(ALARM_STATE_PIN, OUTPUT);

  // Display pin
  pinMode(BCD_A, OUTPUT);
  pinMode(BCD_B, OUTPUT);
  pinMode(BCD_C, OUTPUT);
  pinMode(BCD_D, OUTPUT);
  pinMode(COLON_CLK, OUTPUT);
  
  
  pinMode(FIRST_NUM, OUTPUT);
  pinMode(SECOND_NUM, OUTPUT);
  pinMode(THIRD_NUM, OUTPUT);
  pinMode(FOURTH_NUM, OUTPUT);
  
//  Set up Timer 2  //////////////////////////////////////////
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;
  TCNT2 = 0;

  TCCR2A = B10;
  TCCR2B = B101;
  OCR2A = 249;            // 500 interrupt time <=> 1 sec
  TIMSK2 |= (1 << OCIE2A);

//  Set up Timer 1  //////////////////////////////////////////
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  TCNT1 = 0;

  TCCR1B |= (1 << ICNC1) | (0 << ICES1) | (1 << WGM12) | (1 << CS12) | (1 << CS11) | (0 << CS10);
  OCR1A = 0;
  // ICES1 (0): falling edge
  // ICNI1 (1): noise cancler

  TIMSK1 |= (1 << ICIE1) | (1 << OCIE1A);

//  Set up Timer 0  //////////////////////////////////////////
  TCCR0A = 0;
  TCCR0B = 0;
  TIMSK0 = 0;
  TCNT0 = 0;
  
  TCCR0A = B10; // Enable CTC
  TCCR0B = B100;
  OCR0A = 16000000 / 256 / FREQUENCY_LED;
  TIMSK0 = B10;
  
}

ISR (TIMER1_CAPT_vect) {
  if(!digitalRead(MODE_ENCODER) && debounce_count >= DEBOUNCE_TIME) {
    debounce_count = 0;                                                             // Debounce
    (!isAlarming) ? (select != 0 & mode != 1) ? select = 0 : mode++ : mode = mode;  // Alarm mode (turn on) cancle all signal (exp: "+" signal)
    mode = int(overflow_sta(mode, 'm'));                                            // Mode 1 have only 2 state of select (select = 0 or select = 1)
    (mode == 1) ? select = 1 : select = 0;                                          // Mode 1 - select hour (default)
  
    // Check
    Serial.print("MODE");
    Serial.print("\t");
    Serial.println("SET");
    Serial.print(mode);
    Serial.print("\t");
    Serial.println(select);
  }
  else if(!digitalRead(SET_ENCODER) && debounce_count >= DEBOUNCE_TIME) {
    debounce_count = 0;                         // Debounce
    isCounting = countUp_enable || countDown_enable;                                         // NOT receive "SET" signal in Couting mode (mode 3 & 4)
    
    (mode == 1 || mode == 2 || (mode == 4 && !isCounting && !isAlarming)) ? select++ :       // Mode 1 / 2/ 4 / 5 absord SET signal
                                                                            select = select;
    select = (mode != 1) ? int(overflow_sta(select, 's')) :                                  // Mode 1 DONT have state: (select = 0)
                           int(overflow_sta(select, 'S'));                                   // => 1 -> 2 -> 1 -> 2 ......
    // Check
    Serial.print("MODE");
    Serial.print("\t");
    Serial.println("SET");
    Serial.print(mode);
    Serial.print("\t");
    Serial.println(select);
  }
  else if(!digitalRead(INC_ENCODER) && debounce_count >= DEBOUNCE_HOLD_TIME) {
    
    debounce_count = 0;     
    if(select != 0) {                           // Adjust mode
      inc_enable = !inc_enable;
      TCCR1B ^= 0b01000000;                     // Catch on FALLING / RISING edge
      
      (!digitalRead(INC_ENCODER)) ? adjust_enable(inc_enable, false) : adjust_enable(false, false);    // Increase / Decrease immediately (press button)
//  
      // Check
      Serial.print("INCC");
      Serial.print("\t");
      Serial.println("DECC");
      Serial.print(inc_enable);
      Serial.print("\t");
      Serial.println(dec_enable);

//      Serial.print("INC_ENCODER: \t");
//      Serial.println(!digitalRead(INC_ENCODER));
    }
    else if(select == 0 & (mode == 4 || mode == 3)) { // Count Up/Down mode
      switch(mode) {                                  // Mode 4 and Mode 3 have counting mode 
        case 3:
          // Turn "PinChange INT" to "External INT" (mode: RISING)
          // RISING EDGE -> Toggle countUp_enable 
          countUp_enable = (!digitalRead(INC_ENCODER)) ? !countUp_enable : countUp_enable;
          break;
        case 4:
          // Turn "PinChange INT" to "External INT" (mode: RISING)
          // RISING EDGE -> Toggle countUp_enable 
          countDown_enable = (countDown_time == 0) ? false :         // Cant count down without setting up countdown time
                             (!digitalRead(INC_ENCODER)) ? !countDown_enable : countDown_enable;
          break;
      }
      // Check case 3
//      Serial.print("ENABLE");
//      Serial.print("\t");
//      Serial.println("COUNT");
//      Serial.print(countUp_enable);
//      Serial.print("\t");
//      Serial.println(countUp_time);
    }
    else if(select == 0 & mode == 2) {              // EASTER EGG: holding "+" button for 3 seconds in Alarm mode -> Toggle (ON / OFF) Alarm mode
      hold_PLUSbutton = !hold_PLUSbutton;           // Press (true) -> Release (false)
    }
  }
  else if(!digitalRead(DEC_ENCODER) && debounce_count >= DEBOUNCE_HOLD_TIME) {

    debounce_count = 0;     
    if(select != 0) {                           // Adjust mode
      
      TCCR1B ^= 0b01000000;                     // Catch on FALLING / RISING edge
      dec_enable = !dec_enable;
      
      (!digitalRead(DEC_ENCODER)) ? adjust_enable(false, dec_enable) : adjust_enable(false, false);    // Increase / Decrease immediately (press button)
   
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
          countUp_time = 0;                         // RESET count up value -> 0
          countUp_enable = false;                   // Stop couting
          break;
        case 4:
          // Count down
          countDown_time = 0;                         // RESET count up value -> 0
          countDown_enable = false;                   // Stop couting
          break;
      }
      // Check case 3
//      Serial.print("ENABLE");
//      Serial.print("\t");
//      Serial.println("COUNT");
//      Serial.print(countUp_enable);
//      Serial.print("\t");
//      Serial.println(countUp_time);
    }
    else if(select == 0 & mode == 2) {              // EASTER EGG: holding "+" button for 3 seconds in Alarm mode -> Toggle (ON / OFF) Alarm mode
      hold_PLUSbutton = !hold_PLUSbutton;           // Press (true) -> Release (false)
    }
  }

}



//  TC1 COMPA Interrput  || Normal mode ///////////////////////
ISR (TIMER2_COMPA_vect) {               // Timer Compare Match Interrupt
    (mode != 1) ? count_per_025sec++ : count_per_025sec = count_per_025sec;         // Mode 1 - stop counting
    count_per_025sec = overflow_sta(count_per_025sec, 't');

    // Debounce button
    debounce_count += (debounce_count >= DEBOUNCE_TIME) ? 0 : 1;

    // Colon in digital clock (mode 0)
    if(mode == 0) {
      colon = (count_per_025sec % (FREQUENCY) == 0) ? !colon : colon;
      digitalWrite(COLON_CLK, colon);
    }
    else digitalWrite(COLON_CLK, LOW);
    
    // Match Alarm time ///////////////////////////////////////
    if(count_per_025sec == alarm_time && mode == 0 && turnOn_alarmMode) {
      digitalWrite(BUZZ_PIN, HIGH);                               // Just alarm in Mode 0
      isAlarming = true;
    }
    else if(count_per_025sec == overflow_sta(alarm_time, 't') + ALARM_DUR * 60 * FREQUENCY || !digitalRead(INC_ENCODER)) { 
    // After n minutes or "+" signal -> turn off alarm////////
      digitalWrite(BUZZ_PIN, LOW);
      isAlarming = false;
    }
    // Holding + button in Alarm mode ////////////////////////
    // -> ON / OFF Alarm mode         ///////////////////////
    (hold_PLUSbutton) ? temp_count2++ : temp_count2 = 0;
    if(temp_count2 == TOGGLE_ALARM_HOLD) {
      turnOn_alarmMode = !turnOn_alarmMode;
      digitalWrite(ALARM_STATE_PIN, turnOn_alarmMode);
      //Check
      if(turnOn_alarmMode) Serial.println("Alarm mode is ON");
      else Serial.println("Alarm mode is ON");
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
      isAlarming = true;
    }
    // Note: "+" signal -> turn off alarm (use function of alarm mode)
    if(mode != 4) {
      countDown_enable = false;
      countDown_time = 0;
    }

}

//  TC0 COMPA Interrput  || Led sweeping ///////////////////////
ISR (TIMER0_COMPA_vect) {
    led_num++;
    led_num = overflow_sta(led_num, 'l');
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

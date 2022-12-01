// 
#define INT_PIN         2                 // MODE & SET signal
#define MODE_ENCODE_PIN A0
#define SET_ENCODE_PIN  A1
#define INC_PIN         A2
#define DEC_PIN         A3
#define BUZZ_PIN        0 

// Display pin
#define FIRST_DISP_A  12
#define FIRST_DISP_B  13
#define SECOND_DISP_A 8
#define SECOND_DISP_B 9
#define SECOND_DISP_C 10
#define SECOND_DISP_D 11
#define THIRD_DISP_A  7
#define THIRD_DISP_B  6
#define THIRD_DISP_C  5
#define FOURTH_DISP_A A5
#define FOURTH_DISP_B A4
#define FOURTH_DISP_C 3
#define FOURTH_DISP_D 4

#define START_VALUE     0     
#define CTIME_AMOUNT    345600            // 0        ->  345599
#define COUNT_AMOUNT    24000             // 0        ->  23999
#define MODE_AMOUNT     5                 // (mode 0) ->  (mode 4)
#define SET_AMOUNT      3                 // (set 0)  ->  (set 2)
#define ALARM_DUR       1                 // Alarm duration (minutes)

bool exINT_type = (INT_PIN == 2) ? 0 : 1; // Select INT 0

// const

bool avaiTable[4][6] =  {{1, 1, 1, 1, 1, 1},
                         {0, 1, 1, 1, 1, 1},
                         {0, 0, 1, 1, 1, 1},
                         {0, 0, 0, 0, 1, 1}};

// Time
volatile long count_per_025sec = 0;       // 0 -> 345600  (00:00:00 -> 23:59:59)
volatile long alarm_time = 0;             // 0 -> 345600  (00:00:00 -> 23:59:59)
volatile int countUp_time = 0;            // 0 -> 23999   (00:00:00 -> 00:99:59)
volatile int countDown_time = 0;          // 0 -> 23999   (00:00:00 -> 00:99:59)
// State
volatile int mode = 0;                    // 0 -> 5
volatile int select = 0;                  // 0 -> 2
// Adjust flag
volatile bool inc_enable = false;         // increase enable flag
volatile bool dec_enable = false;         // decrease enable flag
volatile bool countUp_enable = false;     // 
volatile bool countDown_enable = false;     // 
// Trigger flag
volatile bool trigC_enable = false;


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
    case 'c':     // count min:sec
      num = (num >= COUNT_AMOUNT) ? num % COUNT_AMOUNT : (num < START_VALUE) ? num + COUNT_AMOUNT : num;
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

// Display
void dec_2_bin (int decimal, int *temp_arr) {  //bxxxx: (0 -> 9)
  temp_arr[3] = (decimal / 1) % 2;    // LSB
  temp_arr[2] = (decimal / 2) % 2;
  temp_arr[1] = (decimal / 4) % 2;
  temp_arr[0] = (decimal / 8) % 2;    // MSB
}
void turn_to_ClkDisp(long dis_time, int *arr) {   // arr[0]: hr - arr[1]: min - arr[2]: sec               
  dis_time /= 4;                        // 4 times <-> 1 sec
  arr[2] = dis_time % 60;
  arr[1] = (dis_time / 60) % 60;
  arr[0] = dis_time / (60 * 60);
}
void disp(long dis_time, int dis_select, bool countUD_mode) {
  int dis_clk[3];
  int first_num, second_num, third_num, fourth_num;
  int first_num_bin[2], second_num_bin[2], third_num_bin[2], fourth_num_bin[2];
  turn_to_ClkDisp(dis_time, dis_clk);
  if(countUD_mode) {                                // Display min:sec
    first_num = dis_clk[1] / 10;
    second_num = dis_clk[1] % 10;
    third_num = dis_clk[2] / 10;
    fourth_num = dis_clk[2] % 10;
  }
  else {
    first_num = dis_clk[0] / 10;
    second_num = dis_clk[0] % 10;
    third_num = dis_clk[1] / 10;
    fourth_num = dis_clk[1] % 10;
  }
  
  dec_2_bin(first_num, first_num_bin);
  // Error XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  second_num_bin[3] = (second_num / 1) % 2;    // LSB
  second_num_bin[2] = (second_num / 2) % 2;
  second_num_bin[1] = (second_num / 4) % 2;
  second_num_bin[0] = (second_num / 8) % 2;    // MSB
//  dec_2_bin(second_num, second_num_bin);     // Error
  
  third_num_bin[3] = (third_num / 1) % 2;    // LSB
  third_num_bin[2] = (third_num / 2) % 2;    
  third_num_bin[1] = (third_num / 4) % 2;
  third_num_bin[0] = (third_num / 8) % 2;    // MSB
//  dec_2_bin(third_num, third_num_bin);     // Error

  fourth_num_bin[3] = (fourth_num / 1) % 2;    // LSB
  fourth_num_bin[2] = (fourth_num / 2) % 2;
  fourth_num_bin[1] = (fourth_num / 4) % 2;
  fourth_num_bin[0] = (fourth_num / 8) % 2;    // MSB
//  dec_2_bin(fourth_num, fourth_num_bin);     // Error
  // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

  // U1 //////////////////////////////////////////
  digitalWrite(FIRST_DISP_A, first_num_bin[3]);
  digitalWrite(FIRST_DISP_B, first_num_bin[2]);
  // U2 //////////////////////////////////////////
  digitalWrite(SECOND_DISP_A, second_num_bin[3]);
  digitalWrite(SECOND_DISP_B, second_num_bin[2]);
  digitalWrite(SECOND_DISP_C, second_num_bin[1]);
  digitalWrite(SECOND_DISP_D, second_num_bin[0]);
  // U3 //////////////////////////////////////////
  digitalWrite(THIRD_DISP_A, third_num_bin[3]);
  digitalWrite(THIRD_DISP_B, third_num_bin[2]);
  digitalWrite(THIRD_DISP_C, third_num_bin[1]);
  // U4 //////////////////////////////////////////
  digitalWrite(FOURTH_DISP_A, fourth_num_bin[3]);
  digitalWrite(FOURTH_DISP_B, fourth_num_bin[2]);
  digitalWrite(FOURTH_DISP_C, fourth_num_bin[1]);
  digitalWrite(FOURTH_DISP_D, fourth_num_bin[0]);
   
}

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
  
//  Set up Timer 1  //////////////////////////
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  TCNT1 = 0;
  TCCR1B = B1011;
  OCR1A = 62499;
  TIMSK1 |= (1 << OCIE1A);
  
//  Set up External Interrupt ////////////////////////////////////
  attachInterrupt(exINT_type, ISR_external_INT, FALLING);
  
////Set up PinChange Interrupt////////////////////////////////////////////
  PCICR = B10;
  PCMSK1 = B1100;
  
}
//  Interrupt service routine  ///////////////////////////////////////

//  External Interrput  || "MODE" "SET" signal ///////////////////////
void ISR_external_INT() {                                     
  switch(!digitalRead(SET_ENCODE_PIN)) {
    case 1:                                                             // SET signal
      (mode == 1 || mode == 2 || mode == 4) ? select++ :   // Mode 1 / 2/ 4 / 5 absord SET signal
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
    if(count_per_025sec == alarm_time && mode == 0) digitalWrite(BUZZ_PIN, HIGH);
    else if(count_per_025sec == overflow_sta(alarm_time, 't') + ALARM_DUR * 60 * 4 || !digitalRead(INC_PIN)) { 
    // After 2 minutes or "+" signal -> turn off alarm////////
      digitalWrite(BUZZ_PIN, LOW);
    }
    // Increase / Decrease in 0.25s (holding button)///////////
    adjust_enable(inc_enable, dec_enable);

    // Count up mode /////////////////////////////
    countUp_time += (countUp_enable) ? 1 : 0;
    countUp_time = overflow_sta(countUp_time, 'c');
    if(mode != 3) {
      countUp_enable = false;
      countUp_time = 0;
    }
    // Count down mode ///////////////////////////
    countDown_time -= (countDown_enable) ? 1 : 0;
    if(countDown_time == 0) {
      trigC_enable = true;
      countDown_enable = false;
    }
    if(mode != 4) {
      countDown_enable = false;
      countDown_time = 0;
      trigC_enable = false;
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
          // NOT YET ??????????????????????????????????????????????????????????????????????//
          // Count down
        if(!digitalRead(DEC_PIN)) {
          countDown_time = 0;                         // RESET count up value -> 0
          countDown_enable = false;                   // Stop couting
          trigC_enable = false;                       // Disable trigger
        }
        else {
          // Turn "PinChange INT" to "External INT" (mode: RISING)
          // RISING EDGE -> Toggle countUp_enable 
          countDown_enable = (!digitalRead(INC_PIN)) ? !countDown_enable : countDown_enable;
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
      disp(count_per_025sec, select, false);
      break;
    case 1:
      disp(count_per_025sec, select, false);
      break;
    case 2:
      disp(alarm_time, select, false);
      break;
    case 3:
      disp(countUp_time, select, true);
      break;
    case 4:
      disp(countDown_time, select, true);
      break;
  }
  
}

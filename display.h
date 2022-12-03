#ifndef _display_function_h
#define _display_function_h

#include "AdjustData.h"
#include "macroPin.h"
#include "macroConst.h"
#include "globalVar.h"

void dec_2_bin (int decimal, bool *temp_arr) {  //bxxxx: (0 -> 9)
  temp_arr[3] = (decimal / 1) % 2;    // LSB
  temp_arr[2] = (decimal / 2) % 2;
  temp_arr[1] = (decimal / 4) % 2;
  temp_arr[0] = (decimal / 8) % 2;    // MSB
}
void turn_to_ClkDisp(long dis_time, int *arr) {   // arr[0]: hr - arr[1]: min - arr[2]: sec - arr[3]: (x10)msec  
  arr[3] = dis_time % FREQUENCY;
  arr[2] = (dis_time / FREQUENCY) % 60;
  arr[1] = (dis_time / FREQUENCY / 60) % 60;
  arr[0] = dis_time / FREQUENCY / (60 * 60);
}

void BCD_to_led(bool *arr) {
   digitalWrite(BCD_A, arr[3]);
   digitalWrite(BCD_B, arr[2]);
   digitalWrite(BCD_C, arr[1]);
   digitalWrite(BCD_D, arr[0]);
}

// Display
void disp(long dis_time, byte dis_select, bool normal_mode = false, bool countUp_mode = false , bool countDown_mode = false) {
  int dis_clk[4];
  int first_num, second_num, third_num, fourth_num;
  bool first_num_bin[4], second_num_bin[4], third_num_bin[4], fourth_num_bin[4];
  turn_to_ClkDisp(dis_time, dis_clk);
  // Convert dis_clk to BCD num
  first_num   = (normal_mode) ? dis_clk[0] / 10 : (countUp_mode) ? dis_clk[2] / 10 : (countDown_mode) ? dis_clk[1] / 10 : first_num;
  second_num  = (normal_mode) ? dis_clk[0] % 10 : (countUp_mode) ? dis_clk[2] % 10 : (countDown_mode) ? dis_clk[1] % 10 : second_num;
  third_num   = (normal_mode) ? dis_clk[1] / 10 : (countUp_mode) ? dis_clk[3] / 10 : (countDown_mode) ? dis_clk[2] / 10 : third_num;
  fourth_num  = (normal_mode) ? dis_clk[1] % 10 : (countUp_mode) ? dis_clk[3] % 10 : (countDown_mode) ? dis_clk[2] % 10 : third_num;
  
  Serial.print("Time:\t");
  Serial.print(first_num);
  Serial.print(second_num);
  Serial.print("\t : \t");
  Serial.print(third_num);
  Serial.print(fourth_num);
  Serial.print("\t\t");
  Serial.print(mode);
  Serial.print("\t");
  Serial.println(select);
  
  dec_2_bin(first_num, first_num_bin);
  dec_2_bin(second_num, second_num_bin);
  dec_2_bin(third_num, third_num_bin);     
  dec_2_bin(fourth_num, fourth_num_bin);    


  switch (led_num) {
    case 1:                                    // X- : --
      BCD_to_led(first_num_bin);
      digitalWrite(FIRST_NUM, HIGH);
      digitalWrite(SECOND_NUM, LOW);
      digitalWrite(THIRD_NUM, LOW);
      digitalWrite(FOURTH_NUM, LOW);
      break;
    case 2: 
      BCD_to_led(second_num_bin);
      digitalWrite(FIRST_NUM, LOW);
      digitalWrite(SECOND_NUM, HIGH);
      digitalWrite(THIRD_NUM, LOW);
      digitalWrite(FOURTH_NUM, LOW);
      break;
    case 3:
      BCD_to_led(third_num_bin);
      digitalWrite(FIRST_NUM, LOW);
      digitalWrite(SECOND_NUM, LOW);
      digitalWrite(THIRD_NUM, HIGH);
      digitalWrite(FOURTH_NUM, LOW);
      break;
    case 4:
      BCD_to_led(fourth_num_bin);
      digitalWrite(FIRST_NUM, LOW);
      digitalWrite(SECOND_NUM, LOW);
      digitalWrite(THIRD_NUM, LOW);
      digitalWrite(FOURTH_NUM, HIGH);
      break;
  }
//
//  // U1 //////////////////////////////////////////
//  digitalWrite(FIRST_DISP_A, first_num_bin[3]);
//  digitalWrite(FIRST_DISP_B, first_num_bin[2]);
//  // U2 //////////////////////////////////////////
//  digitalWrite(SECOND_DISP_A, second_num_bin[3]);
//  digitalWrite(SECOND_DISP_B, second_num_bin[2]);
//  digitalWrite(SECOND_DISP_C, second_num_bin[1]);
//  digitalWrite(SECOND_DISP_D, second_num_bin[0]);
//  // U3 //////////////////////////////////////////
//  digitalWrite(THIRD_DISP_A, third_num_bin[3]);
//  digitalWrite(THIRD_DISP_B, third_num_bin[2]);
//  digitalWrite(THIRD_DISP_C, third_num_bin[1]);
//  // U4 //////////////////////////////////////////
//  digitalWrite(FOURTH_DISP_A, fourth_num_bin[3]);
//  digitalWrite(FOURTH_DISP_B, fourth_num_bin[2]);
//  digitalWrite(FOURTH_DISP_C, fourth_num_bin[1]);
//  digitalWrite(FOURTH_DISP_D, fourth_num_bin[0]);
   
}

#endif

#include "led_driver.h"
#include "Arduino.h"
#include "helpers.h"
// LED DRIVER private definitions

LedDriver::LedDriver(int* v_led, int* red_pins, int* blue_pins) {
  char _led_col = 0;
  bool _led_flag = false;
  // Store PINS internally
  for(char i=0; i<8; i++) {
    V_LED[i]      = v_led[i];
    RED_PINS[i]   = red_pins[i];
    BLUE_PINS[i]  = blue_pins[i];
  }
  // Select Mode
  for(char i=0; i<8; i++){
    pinMode(RED_PINS[i], OUTPUT);
    pinMode(BLUE_PINS[i], OUTPUT);
    pinMode(V_LED[i], OUTPUT);
  }
  // Initialize the lights off
  for(char r=0; r<8; r++) {
    for(char c=0; c<8; c++) {
      blue[r][c] = false;
      red[r][c] = false;
    }
  }
}

void LedDriver::isr(void) {
  _led_flag = true;
}

void LedDriver::set_square(Light l, char r, char c, bool val){
  // if val is true, it will turn the light on
  if (check_tuple(r,c)) { 
    switch (l) {
      case RED:
        red[r][c] = val;
        break;
      case BLUE:
        blue[r][c] = val;
        break;
    }
  }
}

void LedDriver::grid_on(Light l, BoolBoard bd) {
// Takes a grid and turns on where bd is true.
// No sanitiy checks since BoolBoard is correct type.
  for(char r=0; r<8; ++r) {
    for(char c=0; c<8; ++c) {
      set_square(l,r,c,bd.get(r,c));
    }
  }
}

void LedDriver::all_off(void) {
  grid_off(RED);
  grid_off(BLUE);
}
void LedDriver::grid_off(Light l){
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      set_square(l,r,c,false);
    }
  }
}

void LedDriver::shine(void) {
  // The function that takes red and blue and turns on 
  // their true values.
  // The order of these lines is critical
  // There should be no overlapping HIGH states.
  if (_led_flag) {      // Is it time to do it?
    _led_flag = false;
    // I will loop through columns, one column per call to shine
    // I don't want to waste time with a column
    // that has no squares on
    bool any_row_on = false;  // This check if any square is on 
    bool cycle_done = false;  // This checks if I already checked them all
    while (!any_row_on && !cycle_done) {
      digitalWrite(V_LED[_led_col], LOW); // Turn the column on
      _led_col = (_led_col + 1) % 8;      // Increment the column
      for(char r=0; r<8; r++) {
        // Turn those rows that are on from the column
        any_row_on |= (blue[r][_led_col] || red[r][_led_col]);
        digitalWrite(RED_PINS[r], (red[r][_led_col] ? HIGH : LOW));
        digitalWrite(BLUE_PINS[r], (blue[r][_led_col] ? HIGH : LOW));
      }
      // If there was nothing on, turn the column off
      digitalWrite(V_LED[_led_col], (any_row_on ? HIGH : LOW));
      cycle_done = (_led_col == 7);
    }
  }
}


void LedDriver::test_setup(void) {
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      // blue_on(r,c,true);
      LedDriver::set_square(BLUE,r,c,true);
    }
  }
}

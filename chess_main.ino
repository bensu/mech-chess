#include <chess_game.h>
#include <TimerOne.h>

#ifndef LCD_
#define LCD_
#include <LiquidCrystal.h>
#endif


// PIN MAP
// Led Driver Pins
int v_led[8]      = {45,35,25,44,34,24,17,23};  // Columns are backwards
int red_pins[8]   = {15,20,30,40,50,31,41,51};  // Rows
int blue_pins[8]  = {14,19,28,38,48,29,39,49};  // Rows
// Hall Driver Pins
int row_in[8] = {16,21,32,42,52,33,43,53};
int col_on[8] = {47,37,27,46,36,26,18,22};  // Columns are backwards
// LCD Driver Pins, follow LiquidCrystal library convention
int lcd_pins[6] = {2,3,4,5,6,7}; // {RS,E,D4,D5,D6,D7}
// Button Pins
int power_button = 9;     // INPUT Reset interruptor
int activate_button = 8;  // OUTPUT Activates signal for buttons
int buttons[4] = {10,11,12,13}; // INPUT, piece buttons

// Initialize the main object that holds all of the state and logic
// Once it's memory is allocated, reseting the device resets it's
// variables, but the object g remains.
ChessGame g(v_led,red_pins,blue_pins,row_in,col_on,lcd_pins,
                activate_button,power_button,buttons);

// Test Driven Development
// Tests 1-16 are for functionality, both hardware and software
// Test 17 is the full game.
// Check ChessGame::test_suite_setup() in chess_game_tests.cpp
// for test lists
int program_num = 17;


void setup(){
  Timer1.initialize(50);
  Timer1.attachInterrupt(isr);
  g.test_suite_setup(program_num);
}

void loop(){
  g.led_driver->shine();
  g.hall_driver->attempt_scan();
  g.test_suite_loop(program_num);
}

// The three main drivers need interruptions
// Buttons don't
void isr(void) {
  g.led_driver->isr();
  g.hall_driver->isr();
  g.display_isr();
}






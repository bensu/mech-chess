/* led_driver.h
Sets the appropiate definitios for led_driver.cpp
The main functionality is to turn on and off values
in blue_led[8][8] & red_led[8][8] (both hidden)
through functions red_on() & blue_on.
*/

#include "Arduino.h"
#include "helpers.h"

#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

enum Light {
	RED, BLUE
};

class LedDriver {
public:
	LedDriver(int v_led[], int red_pins[], int blue_pins[]);
	void isr(void);
	void set_square(Light l, char row, char col, bool val);
	void shine(void);
	void test_setup(void);
	void all_off(void);
	void grid_on(Light l, BoolBoard bd);
	void grid_off(Light l);
private:
	int V_LED[8];		// = {23,17,24,34,44,25,35,45};
	int RED_PINS[8];	// = {15,20,30,40,50,31,41,51};
	int BLUE_PINS[8];	// = {14,19,28,38,48,29,39,49};
	bool blue[8][8];
	bool red[8][8];
	char _led_col = 0;
	bool _led_flag;
};

#endif
// CLOCK DRIVER

#include "clock.h"
#include "Arduino.h"

// #define MAX_TIME 60;

Clock::Clock(void){
	// should implement a selector for max_time
	_flag = false;
	_counter = 0;
	_game_duration = 60*60*1000;	// in milliseconds
	_turn = White;	// Deprecated
	rewind();
	_first_update = false;
	_no_clock = false;			// Is the Inf mode selected?
}


void Clock::print(char clock_string[8], Color turn) {
	// Stores in clock_string the format H::MM::SS for the turn clock.
  // char clock_string[7]; // H:MM:SS
  int big_seconds, seconds, hours, minutes;
  big_seconds = this->get_clock(turn);
  // Prepare the data (clock_string)
  hours   = big_seconds/3600;   big_seconds %= 3600;
  minutes = big_seconds/60;     seconds = big_seconds % 60;
  // for hours the '/10' should not be necessary yet it is.
  // I don't get it.
  clock_string[0] = int_to_char(hours%10);  
  clock_string[1] = ':';
  clock_string[2] = int_to_char(minutes/10);
  clock_string[3] = int_to_char(minutes%10);
  clock_string[4] = ':';
  clock_string[5] = int_to_char(seconds/10);
  clock_string[6] = int_to_char(seconds%10);
  clock_string[7] = '\0';	// If not added it keeps printing
}

bool Clock::done(Color turn) {
	// Is the players clock 0?
	// Should return false when the Inf mode was selected
	return (!_no_clock && _game_duration < (turn==White ? _clock_white : _clock_black)); 
}

void Clock::set_duration(unsigned long duration) {
	// Duration is in minutes
	_no_clock = (duration == 0);
	_game_duration = duration*60*1000;
}

void Clock::rewind(void) {
	_clock_white = 0;		// in milliseconds
	_clock_black = 0;		// in milliseconds
	_epoch = millis();
}

void Clock::toggle_turn(void){
	// Deprecated
	_turn = toggle_color(_turn);
}

unsigned long Clock::get_clock(Color color) {
	if (_no_clock) {
		switch (color) {
			case White: return (_clock_white)/1000; break;
			case Black: return (_clock_black)/1000; break;
		}
	} else {
		switch (color) {
			case White: return (_game_duration - _clock_white)/1000; break;
			case Black: return (_game_duration - _clock_black)/1000; break;
		}
	}
}

bool Clock::is_player_over(Color turn) {
	if (_no_clock) {
		return false;
	} else {
		switch (turn) {
			case White: return (_game_duration < _clock_white); break;
			case Black: return (_game_duration < _clock_black); break;
		}
	}
}

void Clock::update_players_clock(Color turn) {
	switch (turn) {
		case White: 
			_clock_white = (millis() - _epoch - _clock_black);
			break;
		case Black:
			_clock_black = (millis() - _epoch - _clock_white);
			break;
	}
}

unsigned long Clock::get_turns_clock(void) {
	// Deprecated
	return get_clock(_turn);
}

void Clock::isr(void) {
	_flag = true;
}
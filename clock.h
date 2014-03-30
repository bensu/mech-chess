#include "Arduino.h"
#include <helpers.h>
#include <chess.h>


#ifndef CLOCK_H_
#define CLOCK_H_

#define CLOCK_FILTER 1

// The class used to have an internal representation of the current
// turn. Now it is functional in respect to the turn, and the turn 
// attribute is not used.
class Clock {
public:
	Clock(void);
	void isr(void);
	void toggle_turn(void);
	unsigned long get_clock(Color color);
	void update_players_clock(Color turn);
	unsigned long get_turns_clock(void);
	void reset(void);
	void print(char clock_string[8], Color turn);
	void set_duration(unsigned long duration);
	bool is_player_over(Color turn);
	void rewind(void);
	bool done(Color turn);
private:
	Color _turn;
	unsigned long _clock_white, _clock_black;
	unsigned long _game_duration, _epoch;
	bool _flag;
	bool _counter;
	bool _first_update;
	bool _no_clock;
};

// void Clock::reset( void ){
// 	_epoch = millis();
// 	_time = 0;
// }

#endif
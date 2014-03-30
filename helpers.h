// Header for helpers.cpp
// Provides universally useful functions

#include "Arduino.h"

#ifndef HELPERS_H_
#define HELPERS_H_

struct Tuple {
	char row;
	char col;
	bool down;
};

struct Button {
	int id;
	bool pressed;
};

int sign(int i);

Button nil_button(void);

bool check_tuple(char row, char col);
Tuple nil_tuple(void);
Tuple new_tuple(char r, char c);
Tuple new_move(char r, char c, bool down);
bool check_tuple(Tuple tuple);
bool same_square(Tuple first, Tuple second);

char int_to_char(int i);


class BoolBoard {
public:
	BoolBoard(void);
	bool get(char r, char c) const;
	bool get(Tuple square) const ;
	void set(char r, char c, bool val);
	void set(Tuple square, bool val);
	bool toggle(char r, char c);
	bool any(void) const;
	void set_all(bool val);
private:
	bool pieces[8][8];
};

#endif
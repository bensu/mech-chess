// helpers.cpp
// Provides universally useful functions

#include "Arduino.h"
#include "helpers.h"

// Math

int sign(int i) {
	if (i == 0) {
		return 0;
	} else { 
		return (i > 0) ? 1 : -1;
	}
}

// Buttons

Button nil_button(void) {
	Button out = {-1,false};
	return out;
}

// Tuples and Moves

bool check_tuple(char r, char c) {
	return (r<8 && r>=0) && (c<8 && c>=0);
}
bool check_tuple(Tuple tuple) {
	return check_tuple(tuple.row,tuple.col);
}

Tuple new_tuple(char r, char c) {
	Tuple out = {.row = r, .col = c, .down = false };
	return out;
}

Tuple new_move(char r, char c, bool down) {
	Tuple out = {.row = r, .col = c, .down = down };
	return out;
}


Tuple nil_tuple(void) {
	Tuple nil = {.row = -1, .col = -1, .down = false };
	return nil;
}

bool same_square(Tuple first, Tuple second) {
	return ((first.row == second.row) && (first.col == second.col));
}

// Clck

char int_to_char(int i) {
  return (char)(((int)'0')+i);
}

// BoolBoard

BoolBoard::BoolBoard(void) 
:	pieces {{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false},
			{false,false,false,false,false,false,false,false}}
{
	// for(char r=0; r<8; ++r) {
 //    	for(char c=0; c<8; ++c) {
	// 		pieces[8][8] = false;
	// 	}
	// }
}

bool BoolBoard::toggle(char r, char c) {
	if (check_tuple(r,c)) {	pieces[r][c] = !pieces[r][c]; }
}

void BoolBoard::set(char r, char c, bool val) {
	if (check_tuple(r,c)) {	pieces[r][c] = val; }
}

void BoolBoard::set(Tuple square, bool val) {
	set(square.row,square.col,val);
}

bool BoolBoard::get(char r, char c) const {
	if (check_tuple(r,c)) {	return pieces[r][c]; }
}

bool BoolBoard::get(Tuple square) const {
	return get(square.row,square.col);
}

bool BoolBoard::any(void) const {
  bool any_on = false;
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      any_on |= get(r,c);
      if (any_on) { return any_on; }
    }
  }
  return any_on;
}

void BoolBoard::set_all(bool val) {
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      set(r,c,val);
    }
  }
}


// BoolBoard &get_boolboard() {
//     BoolBoard *bd = new BoolBoard;
//     return *bd;
// }

#include "hall_driver.h"
#include "Arduino.h"

#define PRESCALE 10
#define FILTER_VALUE 10

// Initialization
HallDriver::HallDriver(int row_in[], int col_on[]) {
  //  Set Pins
  for(char i=0; i<8; i++) {
    HALL_ROW_IN[i] = row_in[i];
    HALL_COL_ON[i] = col_on[i];
  }
  for(char i=0; i<8; i++) {
    pinMode(HALL_ROW_IN[i], INPUT);
    pinMode(HALL_COL_ON[i], OUTPUT);
  }
  // Set buffer empty
  for(char i=0; i<BUFFER_LIMIT; i++) {
    _buffer[i] = nil_tuple();
  }
  for(char r=0; r<8; r++) {
    for(char c=0; c<8; c++) {
      _track[r][c] = 0;
      hall_out[r][c] = false;
    }
  }
  _hall_flag = false;
  _stored_moves = 0;
  _buffer_index = 0;
  _counter = 0;
}

// BUFFER METHODS

bool HallDriver::is_buffer_full(void) {
  return _buffer_full;
}

int HallDriver::buffer_length(void){
  return _stored_moves;
}

bool HallDriver::add_to_buffer(Tuple new_move) {
  if (!is_buffer_full()) {
    _buffer[_stored_moves++] = new_move; 
  }
  if (_stored_moves == BUFFER_LIMIT) {
    _buffer_full = true;
  }
}


Tuple HallDriver::next_move(void) {
  return any_moves_left() ? _buffer[_buffer_index++] : nil_tuple();
  // if () {
  //   return _buffer[_buffer_index++];
  // } else {
  //   return nil_tuple();
  // }
}

Tuple HallDriver::previous_move(void) {
  return (_buffer_index > 1) ? _buffer[_buffer_index-2] : nil_tuple();
  // if (_buffer_index > 1) {
  //   return _buffer[_buffer_index-2];
  // } else {
  //   return nil_tuple();
  // }
}

bool HallDriver::any_moves_left(void) {
  // Check if I can access the moves
  if (_buffer_index < _stored_moves) {
    return true;
  } else {
    wipe_buffer();
    _buffer_full = false;
    return false;
  }
}

void HallDriver::wipe_buffer(void) {
  _buffer_index = 0;
  _stored_moves = 0;
  _buffer_full = false;
  for(char i=0; i<BUFFER_LIMIT; i++) {
    _buffer[i] = nil_tuple();
  }
}

// BASIC HARDWARE METHODS

void HallDriver::isr(void) {
  _hall_flag = true;
}

void HallDriver::attempt_scan(void){
  if (_hall_flag) {
    _hall_flag = false; 
    if (++_counter >= PRESCALE) {
      HallDriver::scan(); 
      _counter = 0;
    }
  } 
}

void HallDriver::scan(void) {
  // Checks all the sensors and stores their values
  // in hall_out[8][8]
  bool new_val;
  bool prev_val;
  // Tuple new_tuple;
  for(char c=0; c<8; c++) {
    // HALL_COL_ON and HALL_COL_ON are flipped!
    digitalWrite(HALL_COL_ON[c], HIGH);
    for(char r=0; r<8; r++) {
      prev_val = hall_out[r][c];
      new_val = !(HIGH == digitalRead(HALL_ROW_IN[r]));
      // The negation ! is because the sensors yield
      // HIGH when there is nothing there.
      if (new_val != prev_val) {
        // If there is a difference add the move to the buffer
        if (++_track[r][c] >= FILTER_VALUE) {
          hall_out[r][c] = new_val;
          _track[r][c] = 0;
          add_to_buffer(new_move(r,c,new_val));
        }
      }
    } 
    digitalWrite(HALL_COL_ON[c], LOW);
  } 
}

// HELPER METHODS

bool HallDriver::any_on(void) {
  bool any_on = false;
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      any_on |= hall_out[r][c];
      if (any_on) { return any_on; }
    }
  }
  return any_on;
}

// Testing

int HallDriver::get_index(void) {
  return _buffer_index;
}

bool HallDriver::is_square_on(char row, char col){
  // Setter for hall_out
  if (check_tuple(row,col)) { return hall_out[row][col]; }
}

Tuple HallDriver::first_on(void) {
  bool any_on = false;
  for(char r=0; r<8; r++){
    for(char c=0; c<8; c++){
      if (hall_out[r][c]) {
        return new_tuple(r,c); 
      }
    }
  }
  return nil_tuple();
}


// Deprecated
    
void HallDriver::col_on(char col) {
  digitalWrite(HALL_COL_ON[col], HIGH);
}

void HallDriver::scan_col(char col) {
  // For testing purposes
  bool aux;
  Tuple new_tuple;
  if (check_tuple(0,col)) {
    // digitalWrite(HALL_COL_ON[col], HIGH);
    for(char row=0; row<8; row++) {
      aux = hall_out[row][col];
      hall_out[row][col] = !(HIGH == digitalRead(HALL_ROW_IN[row]));
      // The negation ! is because the sensors yield
      // HIGH when there is nothing there.
      if (aux!=hall_out[row][col]) {
        // If there is a difference add the move to the buffer
        new_tuple.row = row; new_tuple.col = col;
        HallDriver::add_to_buffer(new_tuple);
      }
    }
  }
}

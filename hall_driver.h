/* hall_driver.h
Sets the appropiate definitios for hall_driver.cpp
The main functionality is to produce bool hall_out[8][8]
with the values of the squares with a piece
*/

#include "Arduino.h"
#include "helpers.h"

#define BUFFER_LIMIT 10

class HallDriver {
public:
	HallDriver(int row_in[], int col_on[]);
	void isr(void);
	bool is_square_on(char row, char col);
	void attempt_scan(void);

	bool any_on(void);
	void scan(void);

	Tuple next_move(void);
	void wipe_buffer(void);
	bool any_moves_left(void);
	bool is_buffer_full(void);
	Tuple first_on(void);
	//Testing
	Tuple _buffer[BUFFER_LIMIT];
	int buffer_length(void);
	Tuple previous_move(void);
	int get_index(void);
private:
	bool add_to_buffer(Tuple);
	int HALL_ROW_IN[8];
	int HALL_COL_ON[8];
	int _track[8][8];
	bool hall_out[8][8];
	bool _hall_flag;
	int _buffer_index;
	int _stored_moves;
	int _counter = 0;
	bool _buffer_full = 0;
	// Deprecated
	void scan_col(char col);
	void col_on(char col);
};


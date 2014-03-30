// Chess Game

#ifndef CHESS_GAME_H_
#define CHESS_GAME_H_


#ifndef LCD_
#define LCD_
#include <LiquidCrystal.h>
#endif

#include <helpers.h>
#include <chess.h>
#include <TimerOne.h>
#include <helpers.h>
#include <led_driver.h>
#include <hall_driver.h>
#include <button_driver.h>
#include <clock.h>


enum State {
	Error,		// Pieces are in the wrong place
	SetPieces,
	Waiting,	// All pieces are in their place
	Moving,		// There is a piece in the air of the correct player
	Promotion,	// Waiting for a player to choose a new piece
	Ended		// Checkmate or Draw
};

// First part of the state machine
enum Init {
	ModeSelect,
	ClockSelect,
	Start
};

// Two playing modes
enum Mode {
	Learning,
	Professional
};

class ChessGame {
public:
	ChessGame(int v_led[],int red_pins[],int blue_pins[],
						int col_in[],int row_on[],int lp[],
						int activate, int power, int buttons[]);
	void reset_game(void);
	// Main Driver Objects
	LiquidCrystal * lcd;
	HallDriver * hall_driver;
	LedDriver * led_driver;
	ButtonDriver * button_driver;
	Clock * clk;
	Board * bd;
	// Display flags/counter meant tow avoid refreshing too quickly
	bool _display_flag;
	int _display_counter;
	// Output Helpers
	void print_board(Light l);
	void display_clock();
	void display_promotion(Color turn);
	void display_ended(Color winner);

	void get_diff(const State previous_state);
	void display_isr(void);

	// State machines
	Init initialize(Init current_stage, int button);
	void output_init(Init _stage);
	void main_loop(void);
	void manage_transition(State current_state, Tuple new_move);
	void output(State current_state);

	void test_suite_loop(int program_num);
	void test_suite_setup(int program_num);
	
private:
	Tuple origin;
	State state;
	State previous_state;
	Color turn;
	Color player_won;
	BoolBoard * legal;
	BoolBoard * diff;
	// Button promotion_button;
	bool got_legal;
	bool _display_done;
	bool _display_init_done;
	char _promoted_col;
	// Initialize
	bool _initialized;
	Init _stage;
	Mode _mode;

	void transition_to(State new_state);
	
	void test_update_history(void);
	void test_cant_move(void);
	void test_check_mate(void);
	void test_is_move_legal(void); 
	void test_promoted_pawns(void);
	void test_clock(void);	
	void test_clock_setup(void);
	void test_buffer(void);
	void test_swap(void);
	void test_in_check_dinamic(void);
	void test_legal_squares_dinamic(void);
	void test_legal_squares_static(void);
	void test_hall_diff(void);
	void test_square(char row, char col); 
	void test_halls_in_loop(void);
	void test_buttons(void);
	void test_get_move(void);
	void test_print_board(void);
	void test_initialize(void);
};

#endif

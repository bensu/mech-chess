#include "Arduino.h"
#include "helpers.h"

#define N_BUTTONS 4	// There are 4 buttons in the board

#define B_BUFFER 10	// Size of buffer
#define B_FILTER 10	// Number of reads before a change is accepted

class ButtonDriver {
public:
	ButtonDriver(int activate_button, int power_button, int buttons[]) ;
	// Basic Hardware Methods
	void isr(void);	// not used
	void scan(void);
	void activate_buttons(bool in);
	bool is_power_on(void);

	bool any_on(void);	// Used only in testing

	// Is public for testing purposes.
	bool _on[N_BUTTONS] = {false,false,false,false};

	// Buffer methods
	Button next_on(void);
	void wipe_buffer(void);
	bool any_moves_left(void);
	bool is_buffer_full(void);

	bool any_button_on(void); // Deprecated

private:
	bool add_to_buffer(Button);
	// Pins
	int BUTTONS[N_BUTTONS];
	int _activate;
	int _power_button;

	bool _flag;		// Deprecated since isr is not used
	int _track[N_BUTTONS] = {0,0,0,0};	// counter for filter
	// Buffer data
	Button _buffer[B_BUFFER];
	int _buffer_index = 0;
	int _stored_moves = 0;
	int _counter = 0;
	bool _buffer_full = false;
};

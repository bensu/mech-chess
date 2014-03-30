#include "button_driver.h"
#include "Arduino.h"

// Initialization
ButtonDriver::ButtonDriver(int activate_button, int power_button, int buttons[]) {
	// Store pins in our own memory
	_activate = activate_button;
	_power_button = power_button;
	for (char i=0; i<N_BUTTONS; i++) {
		BUTTONS[i] = buttons[i];
	}
	// Select mode for pins
	pinMode(activate_button,OUTPUT);
	pinMode(power_button,INPUT);
	for(char i=0; i<N_BUTTONS; i++) {
		pinMode(BUTTONS[i], INPUT);
	}
	// Initialize empty buffer
	for(char i=0; i<B_BUFFER; i++) {
		_buffer[i] = nil_button();
	}
	// Buttons are deactivated by default
	activate_buttons(false);
}

// BASIC HARDWARE METHODS

void ButtonDriver::isr(void) {
	// not used.
	_flag = true;
}

void ButtonDriver::scan(void) {
	// Scans all the buttons and tracks changes
	// Once there is a change that lasts for a B_FILTER number
	// of reads, then save the button in the buffer. 
	for(char i=0; i<N_BUTTONS; i++) {
		bool new_read = HIGH == digitalRead(BUTTONS[i]);
		// Is the read different from the saved value?
		if (_on[i] != new_read) {
			// Has it been different for a while?				
			if (_track[i]++ >= B_FILTER) {
				// Yes, reset counter/filter, save new value, and store in buffer
				_track[i] = 0;
				_on[i] = new_read;
				Button b = { .id = i, .pressed = new_read};
				add_to_buffer(b);
			}
		}
	}
}

bool ButtonDriver::any_on(void) {
	// Checks if there is any button being pressed
	// in real time
	for(char i=0; i<N_BUTTONS; i++) {
		if (_on[i]) {
			return true;
		}
	}
	return false;
}

bool ButtonDriver::is_power_on(void) {
	// Checks if the reset button is on in real time
	return digitalRead(_power_button);
}

void ButtonDriver::activate_buttons(bool in) {
	// Turns on the signal for the buttons
	digitalWrite(_activate,(in ? HIGH : LOW));
}



bool ButtonDriver::is_buffer_full(void) {
	// Is the buffer full?
	return _buffer_full;
}

bool ButtonDriver::add_to_buffer(Button b) {
	// If the buffer is not full, add to buffer
	// Then check if it's full now.
	// Return type is wrong! but function is deprecated.
	// Next review should delete this
	if (!is_buffer_full()) {
		_buffer[_stored_moves++] = b; 
	}
	if (_stored_moves == B_BUFFER) {
		_buffer_full = true;
	}
}

Button ButtonDriver::next_on(void) {
	// Return the next button in the buffer
	// If the buffer is full
	return any_button_on() ? _buffer[_buffer_index++] : nil_button();
}

// int ButtonDriver::previous_move(void) {
//   return (_buffer_index > 1) ? _buffer[_buffer_index-2] : -1;
// }

bool ButtonDriver::any_button_on(void) {
	// Check if there is anything left in the buffer.
	// If I've already read everyrhing, delete the buffer
	// and return false
	if (_buffer_index < _stored_moves) {
		return true;
	} else {
		wipe_buffer();
		_buffer_full = false;
		return false;
	}
}

void ButtonDriver::wipe_buffer(void) {
	// Set all the buffer variables to 0 or nil
	_buffer_index = 0;
	_stored_moves = 0;
	_buffer_full = false;
	for(char i=0; i<B_BUFFER; i++) {
		_buffer[i] = nil_button();
	}
}


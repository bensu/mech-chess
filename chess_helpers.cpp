#include <chess.h>

// Chess Helpers
// Implements small, useful functions that are chess specific.

char move_row(char original_square, char square_num, Color color) {
	// Moves "forward" depending on the color side of the board
	return original_square + ((color == White) ? square_num : -square_num);
}

Color toggle_color(Color color_in) {
	// Returns opposite color
	// Meant to return a runtime error if None is supplied
	switch (color_in) {
		case White:
			return Black;
			break;
		case Black:
			return White;
			break;
	}
}
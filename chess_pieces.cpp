// Chess Logic
// This files contains all the logic necessary
// for the chess game.

// The main function is legal_squares(BoolBoard,Tuple,Board)

#include "chess.h"

// General SuperClass

Piece::Piece(Color color, PieceType type) {
	_color = color;
	_type = type;
}
// Bookeeping
void Piece::set_color(Color color) {
	_color = color;
}
void Piece::set_type(PieceType type) {
	_type = type;
}

bool Piece::is_empty(void) const {
	return (_type == Empty);
}
PieceType Piece::get_type(void) const {
	return _type;
}
Color Piece::get_color(void) const {
	return _color;
}

void Piece::legal_squares(BoolBoard & buffer, const Tuple square, Board & bd) { 
	// Fills buffer with all the legal moves for square in the bd context
	// Walk and try if move is legal
	for(char r=0; r<8; r++) 
		for(char c=0; c<8; c++) {
			bool is_legal = bd.get(square)->is_move_legal(square,new_tuple(r,c),bd,false);
			buffer.set(r,c,is_legal && !bd.move_leaves_check(square,new_tuple(r,c)));
		}
	}
}


bool Piece::is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	// returns true if the move is legal according to chess rules.
	// If side_effects is true, then make the move and remember what happened.
	Color color = bd.get(origin)->get_color();
	// If destination is empty or has opposite color then procede.
	bool out = false;
	if (bd.get(final)->is_empty() || bd.get(final)->get_color() == toggle_color(color)) {
		switch (get_type()) {
			case Empty:		out = empty_is_move_legal(origin,final,bd,false); 			break;		
			case Pawn:		out = pawn_is_move_legal(origin,final,bd,side_effects); 	break;
			case Knight:	out = knight_is_move_legal(origin,final,bd,false); 			break;
			case Bishop:	out = bishop_is_move_legal(origin,final,bd,false); 			break;
			case Rook:		out = rook_is_move_legal(origin,final,bd,side_effects); 	break;
			case Queen:		out = queen_is_move_legal(origin,final,bd,false); 			break;
			case King:		out = king_is_move_legal(origin,final,bd,side_effects); 	break;
		}
	}
	if (out && side_effects) {
		bd.move(origin,final);
	}
	return out;
}

bool Piece::empty_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	return false;
}

bool Piece::pawn_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	Color color = get_color();
	Color opposite = toggle_color(color);
	int dr = final.row - origin.row;
	int dc = final.col - origin.col;
	int dir = (color == White) ? 1 : -1;	// Which direction should is it moving?
	bool passant_move = false;			// Was it a passant capture? Needed to edit the board
	bool double_move = false;			// Was it a double move? Needed for future passants
	bool output = false;				// Return value, true if the move is legal.
	if (bd.get(final)->is_empty()) { 	// Destination square is unoccupied
		if (dc == 0) {					// Moving straight
			if (dr == dir) {			// Simple move
				output = true;
			} else if ((dr == dir*2) && (origin.row == ((color == White) ? 1 : 6))) {
				// Double move
				double_move = true;
				output = true;
			}
		} else if (((abs(dc) == 1) && (dr == dir)) 					&& 
				(bd.get_passant(opposite) == final.col) 			&&
				(bd.get(origin.row,final.col)->get_type()==Pawn)   	&&
				(bd.get(origin.row,final.col)->get_color()==opposite)) {
			// All the conditions for a passant move where true.
			passant_move = true; 	// Remember that
			output = true;
		}
	} else if ((abs(dc) == 1) && (dr == dir)) {
		output = true;	// Dest holds piece of opposite color
	}
	if (output && side_effects) {
		// If it was a legal move and there are side_effects
		// then save to history
		if (double_move) {
			bd.set_passant(color,origin.col);
		} else if (passant_move) {
			bd.empty_sq(new_tuple(origin.row,final.col));
			bd.no_passant();
		} else {
			bd.no_passant();
		}
	}
	return output;
}

bool Piece::knight_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	int dr = abs(final.row - origin.row);
	int dc = abs(final.col - origin.col);
	return ((dr==2 && dc == 1) || (dr==1 && dc == 2));
}

bool Piece::bishop_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	char row = origin.row;	
	char col = origin.col;
	int row_offset = sign(final.row - origin.row);
	int col_offset = sign(final.col - origin.col);
	row = row + row_offset;
	col = col + col_offset;
	// Is it diagonal?
	if ((row_offset != 0) && (col_offset != 0)) {
		while (!(row == final.row && col == final.col)) {
			if (!bd.get(row,col)->is_empty()) {
				return false;
			}
			row = row + row_offset;
			col = col + col_offset;
		}
		return true;
	} else {
		return false;
	}
}

bool Piece::rook_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	char row = origin.row;	
	char col = origin.col;
	int row_offset = sign(final.row - origin.row);
	int col_offset = sign(final.col - origin.col);
	row = row + row_offset;
	col = col + col_offset;
	bool output = false;
	// Is it straight?
	if ((row_offset == 0) != (col_offset == 0)) {
		bool all_empty = true;
		while (!(row == final.row && col == final.col)) {
			all_empty &= bd.get(row,col)->is_empty();
			row = row + row_offset;
			col = col + col_offset;
		}
		output = all_empty;
	}
	if (output && side_effects) {
		// Side effects needed for castling
		bd.no_castling(get_color());
	}
	return output;
}

bool Piece::queen_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	return (rook_is_move_legal(origin,final,bd,false)  || 
					bishop_is_move_legal(origin,final,bd,false));
}

bool Piece::king_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects) {
	int dr = abs(final.row - origin.row);
	int dc = abs(final.col - origin.col);
	bool normal_move = ((dr <= 1) && (dc <= 1));	// Check the non castling moves
	char first_row = (get_color()==White) ? 0 : 7;	// Was the first row for my color
	char rook_col = (final.col == 2) ? 0 : 7;		// Which rook am I working with?
	bool castling = false;							// Was it a castling move?
	// Check all the castling conditions
	if (dr == 0 && origin.row == first_row && origin.col == 4 && abs(dc)==2) {
	   	if (bd.get(first_row,rook_col)->get_type()==Rook 				&& 
	   			bd.get(first_row,rook_col)->get_color()==get_color()  	&& 
	   			!bd.is_in_check(get_color())) {
	   		// Check if any go-through square is being attacked.
	   		int delta = sign(final.col - origin.col);
	   		char col = origin.col;
	   		bool all_safe = true;
	   		while (col!=final.col) {
	   			col += delta;
	   			all_safe &= bd.get(first_row,col)->is_empty() &&
	   						!bd.is_attacked_by(toggle_color(get_color()),
	   											new_tuple(first_row,col));
	   		}
	   		castling = all_safe;
	   }
	}
	if (side_effects) {
		// If there was castling, set that the rook should move as well.
		if (castling) {
			char rook_final_col = (final.col == 2) ? 3 : 5;
			bd.move(new_tuple(first_row,rook_col),new_tuple(first_row,rook_final_col));
		}
		// If not, the king moved and castling is lost forever
		bd.no_castling(get_color());
	}
	// Inefficient since caslting might not be needed and expensive but is calculated anyway
	return (normal_move || castling);	
}
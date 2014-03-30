#include "chess.h"

Board::Board() { 
	init_empty();
	new_board();
}

Board::Board(const Board & bd) {
	// Copies a new one
	for (int r = 0; r < 8; ++r) {
		for (int c = 0; c < 8; ++c) {
			array[r][c] = new Piece(bd.get(r,c)->get_color(),
							bd.get(r,c)->get_type());
		}
	}
	// State values
	_w_castling = bd._w_castling; 	_b_castling = bd._b_castling; 
	_w_passant = bd._w_passant;		_b_passant = bd._b_passant;
}

Board::Board(int test_case) {
	Board::init_empty();
	switch (test_case) {
		// En passant Test
		case 0:
			array[3][2] = new Piece(White,Pawn);
			array[3][3] = new Piece(Black,Pawn);
			_w_passant = 2;
			_test_tuple = new_tuple(3,3);
			break;
		// Castling White's
		case 1:
			array[0][3] = new Piece(White,King);
			array[0][0] = new Piece(White,Rook);
			array[0][7] = new Piece(White,Rook);
			array[7][3] = new Piece(Black,King);
			_w_castling = true;
			_test_tuple = new_tuple(0,3);
			break;
		// Castling Black's
		case 2:
			array[4][4] = new Piece(White,King);
			array[7][4] = new Piece(Black,King);
			// array[7][0] = new Piece(Black,Rook);
			// array[7][7] = new Piece(Black,Rook);
			array[4][5] = new Piece(White,Queen);
			_test_tuple = new_tuple(7,3);
			break;
		case 3:
			_test_tuple = new_tuple(4,4);
			break;
		case 4:
			array[7][3] = new Piece(Black,King);
			array[7][0] = new Piece(Black,Rook);
			array[7][7] = new Piece(Black,Rook);
			array[4][4] = new Piece(White,King);
			//array[0][3] = new Piece(White,Queen);
			_b_castling = true;
			_test_tuple = new_tuple(7,3);
			break;
		// Starts in Checkmate White
		case 5:
			array[0][0] = new Piece(White,King);
			array[7][0] = new Piece(Black,Rook);
			array[7][1] = new Piece(Black,Rook);
			break;
		// White can't move
		case 6:
			array[0][0] = new Piece(White,King);
			array[1][7] = new Piece(Black,Rook);
			array[7][1] = new Piece(Black,Rook);
			break;
		// En passant within main_loop
		case 7:
			array[7][7] = new Piece(White,King);
			array[7][0] = new Piece(Black,King);
			array[1][4] = new Piece(White,Pawn);
			array[3][5] = new Piece(Black,Pawn);
			break;
		// Test Promotion
		case 8:
			array[7][7] = new Piece(White,King);
			array[7][0] = new Piece(Black,King);
			array[1][4] = new Piece(White,Pawn);
			array[0][5] = new Piece(Black,Pawn);
			break;
	}
}

void Board::new_board(void) {
	// Standard starting position
	// Loops through White (i=0) and Black (i=1)
	// to avoid duplication
	for (int i=0; i<2; i++) {
		int row, pawn_row;
		Color color;
		switch (i) {
			case 0:
				row = 0;
				pawn_row = 1;
				color = White;
				break;
			case 1:
				row = 7;
				pawn_row = 6;
				color = Black;
				break;
		}
		// Set Pawns
		for (int c = 0; c < 8; ++c) {
			set(pawn_row,c,color,Pawn);
		}
		// Set the rest
		set(row,0,color,Rook);
		set(row,1,color,Knight);
		set(row,2,color,Bishop);
		set(row,4,color,King);
		set(row,3,color,Queen);
		set(row,5,color,Bishop);
		set(row,6,color,Knight);
		set(row,7,color,Rook);
	}
	// Set the empty ones
	for (int r=2; r<6; r++) {
		for (int c=0; c<8; c++) {
			set(r,c,None,Empty);
		}
	}
}

void Board::init_empty() {
	// Initializes the empty pieces
	// and the values for the state.
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			array[r][c] = new Piece(None,Empty);
		}
	}
	// State values
	_w_castling = true; _b_castling = true;
	_w_passant = WRONG_ROW;	
	_b_passant = WRONG_ROW;
}

void Board::empty(void) {
	// Empties the whole board
	// Needs to be used after init
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			set(r,c,None,Empty);
		}
	}
}

Board::~Board() {
	for (int r = 0; r < 8; ++r) {
		for (int c = 0; c < 8; ++c) {
			delete array[r][c];
			array[r][c] = 0;
		}
	}
}


void Board::make_move(Tuple origin, Tuple final) {
	// Saves the move if it's legal/
	get(origin)->is_move_legal(origin,final,*this,true);
}

bool Board::any_pawn_promoted(void) {
	// Checks if there is a pawn at the end.
	for(char c=0; c<8; c++) {
		if ((get(0,c)->get_type()==Pawn) && (get(0,c)->get_color()==Black)) {
			return true;
		}
		if ((get(7,c)->get_type()==Pawn) && (get(7,c)->get_color()==White)) {
			return true;
		}
	}
	return false;
}

char Board::promoted_pawn(void) {
	// Returns the column where the pawn is promoted
	for(char c=0; c<8; c++) {
		if ((get(0,c)->get_type()==Pawn) && (get(0,c)->get_color()==Black)) {
			return c;
		}
		if ((get(7,c)->get_type()==Pawn) && (get(7,c)->get_color()==White)) {
			return c;
		}
	}
	return 9;	// Something very wrong! since it should have been checked by
				// any_pawn_promoted
}

bool Board::is_attacked_by(Color color, Tuple sq) {
	// Run through the colors's pieces and see if any can take the square
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			if (!get(r,c)->is_empty() && get(r,c)->get_color() == color) {
				if (get(r,c)->is_move_legal(new_tuple(r,c),sq,*this,false)) {
					return true;
				}
			}
		}
	}
	// There was nothing, return false
	return false;
}

bool Board::is_in_check(Color color) {
	Tuple kings_sq = find_king(color);
	Color opponent = toggle_color(color);
	// Is the King attacked by the opponents?
	return is_attacked_by(opponent, kings_sq);
}

bool Board::cant_move(Color color) {
	// Checks if the player color can move any piece without being
	// in check. Returns true if the player can't move.
	// Expensive operation
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			if (get(r,c)->get_color() == color) {
				BoolBoard moves;
				get(r,c)->legal_squares(moves,new_tuple(r,c),*this);
				if (moves.any()) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Board::move_leaves_check(Tuple origin, Tuple final) const {
	// Return true if the proposed move leaves the player in check
	// Try the move in a copy
	Color turn_color = this->get(origin)->get_color();
	Board * aux_board = new Board(*this);
	aux_board->make_move(origin,final);
	// If it where there, would it leave the king in check?
	bool in_check = aux_board->is_in_check(turn_color);
	delete aux_board;
	return in_check;
}

void Board::legal_moves(BoolBoard & buffer, const Tuple square) {
	// Stores in buffer all the legal moves for the piece in square
	// Syntactic sugar for Piece method
	get(square)->legal_squares(buffer,square,*this);
}

bool Board::is_move_legal(const Tuple origin_sq, const Tuple final_sq) {
	// Checks if the proposed move is legal.
	BoolBoard buffer;
	legal_moves(buffer,origin_sq);
	return buffer.get(final_sq.row,final_sq.col);
}

Tuple Board::get_test_tuple(void) {
	// Testing purposes
	return _test_tuple;
}

char Board::get_passant(Color color) const {
	// Returns the column of the pawn that
	// can be captured en passant of color.
	// If none, it will return WRONG_ROW
	// WRONG_ROW can bring future mistakes
	// since it is a char.
	switch (color) {
		case White: return _w_passant; break;
		case Black: return _b_passant; break;
	}
}

void Board::set_passant(Color color,char col) {
	// Getter for passant column
	if (check_tuple(new_tuple(0,col))) {
		switch (color) {
			case White: _w_passant = col; break;
			case Black: _b_passant = col; break;
		}
	}
}

void Board::no_passant(void) {
	// When there are no double moves, put a wrong value there
	_w_passant = WRONG_ROW;
	_b_passant = WRONG_ROW;
}

void Board::no_castling(Color color) {
	// Cancels tha possibilities of
	// castling for the team color.
	// To be used when the king or rook
	// are moved
	switch (color) {
		case White: _w_castling = false; break;
		case Black: _b_castling = false; break;
	}
}

bool Board::get_castling(Color color) const {
	// Returns if castling for the team
	// color is possible
	switch (color) {
		case White: return _w_castling; break;
		case Black: return _b_castling; break;
	}
}

Piece* Board::get(char r, char c) const {
	if (check_tuple(r,c)) {
		return array[r][c];
	}
}
Piece* Board::get(Tuple tuple) const {
	return get(tuple.row,tuple.col);
}

void Board::set(char r, char c, Color color, PieceType type) {
	Piece * piece = get(r,c);
	piece->set_color(color);
	piece->set_type(type);
}

void Board::set(Tuple tuple, Color color, PieceType type) {
	set(tuple.row,tuple.col,color,type);
}

void Board::move(Tuple origin, Tuple final) {
	Color color = get(origin)->get_color();
	PieceType type = get(origin)->get_type();
	set(final,color,type);
	empty_sq(origin);
}

// Deprecated
// void Board::swap(Tuple sq_one, Tuple sq_two) {
// 	Color color_two = get(sq_two)->get_color();
// 	PieceType type_one = get(sq_two)->get_type();
// 	set(sq_two,get(sq_one)->get_color(),get(sq_one)->get_type());
// 	set(sq_one,color_two,type_one);
// }

void Board::empty_sq(Tuple square) {
	// set the square empty
	Piece * piece = get(square);
	piece->set_color(None);
	piece->set_type(Empty);
}

Tuple Board::find_piece(Color color, PieceType type) {
	// Returns the first one it finds.
	// Needed for PieceType King, but for other pieces in testing 
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			if (!get(r,c)->is_empty() && get(r,c)->get_type() == type
				&& get(r,c)->get_color() == color) {
				return new_tuple(r,c);
			}
		}
	}
	return nil_tuple();
}

Tuple Board::find_king(Color color) {
	// shortcut
	return find_piece(color,King);
}

// Deprecated -> print_board()
BoolBoard Board::pieces(BoolBoard & bool_bd) {
	for (char r = 0; r < 8; ++r) {
		for (char c = 0; c < 8; ++c) {
			if (!array[r][c]->is_empty()){
				bool_bd.set(r,c,true);
			}
		}
	}
	return bool_bd;
}



// void Board::Print() {
// 	using namespace std;
// 	const int kiSquareWidth = 4;
// 	const int kiSquareHeight = 3;
// 	for (int r = 0; r < 8*kiSquareHeight; ++r) {
// 		int iSquareRow = r/kiSquareHeight;
// 		// Print side border with numbering
// 		if (r % 3 == 1) {
// 			cout << '-' << (char)('1' + 7 - iSquareRow) << '-';
// 		} else {
// 			cout << "---";
// 		}
// 		// Print the chess board
// 		for (int c = 0; c < 8*kiSquareWidth; ++c) {
// 			int iSquareCol = c/kiSquareWidth;
// 			if (((r % 3) == 1) && ((c % 4) == 1 || (c % 4) == 2) && array[7-iSquareRow][iSquareCol] != 0) {
// 				if ((c % 4) == 1) {
// 					cout << array[7-iSquareRow][iSquareCol]->get_color();
// 				} else {
// 					cout << array[7-iSquareRow][iSquareCol]->get_type();
// 				}
// 			} else {
// 				if ((iSquareRow + iSquareCol) % 2 == 1) {
// 					cout << '*';
// 				} else {
// 					cout << ' ';
// 				}
// 			}
// 		}
// 		cout << endl;
// 	}
// 	// Print the bottom border with numbers
// 	for (int r = 0; r < kiSquareHeight; ++r) {
// 		if (r % 3 == 1) {
// 			cout << "---";
// 			for (int c = 0; c < 8*kiSquareWidth; ++c) {
// 				int iSquareCol = c/kiSquareWidth;
// 				if ((c % 4) == 1) {
// 					cout << (iSquareCol + 1);
// 				} else {
// 					cout << '-';
// 				}
// 			}
// 			cout << endl;
// 		} else {
// 			for (int c = 1; c < 9*kiSquareWidth; ++c) {
// 				cout << '-';
// 			}
// 			cout << endl;
// 		}
// 	}
// }


/*
bool Board::CanMove(Color color) {
	// Run through all pieces
	for (int r = 0; r < 8; ++r) {
		for (int c = 0; c < 8; ++c) {
			if (array[r][c] != 0) {
				// If it is a piece of the current player, see if it has a legal move
				if (array[r][c]->get_color() == color) {
					for (int iMoveRow = 0; iMoveRow < 8; ++iMoveRow) {
						for (int iMoveCol = 0; iMoveCol < 8; ++iMoveCol) {
							if (array[r][c]->IsLegalMove(r, c, iMoveRow, iMoveCol, array)) {
								// Make move and check whether king is in check
								CAPiece* qpTemp					= array[iMoveRow][iMoveCol];
								array[iMoveRow][iMoveCol]	= array[r][c];
								array[r][c]			= 0;
								bool bCanMove = !IsInCheck(color);
								// Undo the move
								array[r][c]			= array[iMoveRow][iMoveCol];
								array[iMoveRow][iMoveCol]	= qpTemp;
								if (bCanMove) {
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
*/
// Chess Definitions

#include <helpers.h>

#ifndef CHESS_H_
#define CHESS_H_

#define WRONG_ROW 15	// Used as nil value, should be avoided though

// Used for Pieces and Turn
// None is reserved for Empty squares
enum Color {
	White, Black, None
};
enum PieceType {
	Empty,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

// Helpers
char move_row(char original_square, char square_num, Color color);
Color toggle_color(Color color_in);	// returns opposite color, problems with None
// Classes

class Piece;	//Forward Declaration

// Board contains all the pieces location and the game's history (castling and passant)
class Board {
public:
	Board();
	Board(const Board & bd);	// Copy method
	Board(int test_case);		// Initialize different cases for tests
	~Board();
	void init_empty(void);		// First time
	void new_board(void);		// For Reset
	void empty(void);			
	// Getter and setter for pieces
	Piece* get(char r, char c) const;
	Piece* get(Tuple tuple) const;
	void set(char r, char c, Color color, PieceType type);
	void set(Tuple tuple, Color color, PieceType type);
	BoolBoard pieces(BoolBoard & bool_bd);
	void empty_sq(Tuple square);
	// Getter and setter for history
	void set_passant(Color color, char row);
	void no_passant(void);
	char get_passant(Color color) const;
	bool get_castling(Color color) const;
	void no_castling(Color color);
	// Methods that walk the whole board
	Tuple find_king(Color color);
	bool is_in_check(Color color);
	bool is_attacked_by(Color color, Tuple sq);
	Tuple find_piece(Color color, PieceType type);
	void legal_moves(BoolBoard & buffer, const Tuple square);
	bool cant_move(Color color);
	bool move_leaves_check(Tuple origin, Tuple final) const;
	// Make move and check if it is legal
	bool is_move_legal(const Tuple origin_sq, const Tuple final_sq);
	void make_move(Tuple origin, Tuple final);
	void move(Tuple origin, Tuple final);
	char promoted_pawn(void);
	bool any_pawn_promoted(void);
	// Testing purposes
	Tuple get_test_tuple(void);
private:
	Piece* array[8][8];	// Stores all the pieces
	// History
	bool _w_castling; bool _b_castling;
	char _w_passant; char _b_passant;	// Last row where there was a double move
	Tuple _test_tuple;
};


class Piece {
public:
	Piece(Color color, PieceType type);	//INIT
	// Accessors
	PieceType get_type(void) const;
	Color get_color(void) const;
	bool is_empty(void) const;
	void set_color(Color color);
	void set_type(PieceType type);
	// Chess Logic
	void legal_squares(BoolBoard & buffer, const Tuple square, Board & bd);
	bool is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	// void legal_moves(BoolBoard & buffer, const Tuple square, Board & bd);
	// void filter_squares(BoolBoard & buffer, const Tuple square, Board & bd); 
	// Busines Logic
	// virtual BoolBoard legal_squares(Tuple square, Board board);

private:
	// virtual bool AreSquaresLegal(int iSrcRow, int iSrcCol, int iDestRow, int iDestCol, CAPiece* qpaaBoard[8][8]) = 0;
	Color _color;
	PieceType _type;
	// Polymorphism for all the PieceTypes, avoids a very long switch case.
	bool empty_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	bool king_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	bool queen_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects); 
	bool rook_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	bool bishop_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	bool knight_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
	bool pawn_is_move_legal(Tuple origin, Tuple final, Board & bd, bool side_effects);
};


#endif
#include "chess_game.h"

void ChessGame::test_suite_loop(int program_num) {
// Provides several different test cases
  switch (program_num) {
    case 0:   break;
    case 1:   led_driver->test_setup();             break;
    case 2:   test_halls_in_loop();                 break;
    case 3:   led_driver->set_square(RED,0,1,true); break;
    case 4:   test_hall_diff();                     break;
    case 5:   test_print_board();                   break;
    case 6:   test_legal_squares_static();          break;
    case 7:   test_legal_squares_dinamic();         break;
    case 8:   test_in_check_dinamic();              break;
    case 9:   test_buffer();                        break;
    case 10:  test_promoted_pawns();                break;
    case 11:  test_clock();                         break;
    case 12:  test_cant_move();						          break;
    case 13:  test_check_mate();					          break;
    case 14:  test_update_history();				        break;
    case 15:  test_get_move();                      break;
    case 16:  test_buttons();                       break;
    case 17:  test_initialize();                    break;
  }
}

void ChessGame::test_suite_setup(int program_num) {
// Provides several different test cases
  switch (program_num) {
    case 0:   break;
    case 1:   break;
    case 2:   break;
    case 3:   break;
    case 4:   break;
    case 5:   break;
    case 6:   break;  // requires board 4
    case 7:   break;  // requires board 4
    case 8:   break;
    case 9:   break;
    case 10:  break;	// requires board 8
    case 11:  test_clock_setup();   break;
    case 12:  break;	// requires board 6
    case 13:  break;	// requires board 5
    case 14:  break;	// requires board 7
    case 15:  test_clock_setup();   break;
    case 16:  break;
    case 17:  break;
  }
}

void ChessGame::test_initialize(void) {
  // Main Game
  if (button_driver->is_power_on()) {
    if (_stage != Start) {
      button_driver->activate_buttons(true);
      button_driver->scan();
      if (button_driver->any_button_on()) {
        Button selected_button = button_driver->next_on();
        if (selected_button.pressed) {
          _stage = initialize(_stage, selected_button.id);
        }
      }
      if (_stage == Start) {
        _display_done = false;
      }
      output_init(_stage);
    } else {
      main_loop();
    }
  } else {
    led_driver->grid_off(RED);
    led_driver->grid_off(BLUE);
    button_driver->activate_buttons(false);
    lcd->clear();
    reset_game();
  }
}

void ChessGame::test_buttons(void) {
	button_driver->activate_buttons(true);
  button_driver->scan();
  if (button_driver->any_button_on()) {
    Button b = button_driver->next_on();
    led_driver->set_square(BLUE,0,b.id,b.pressed);
  }
}

void ChessGame::test_update_history(void) {
	main_loop();
	char passant_col = bd->get_passant(toggle_color(turn));
	led_driver->set_square(RED,0,passant_col,true);
}

void ChessGame::test_check_mate(void) {
	// Needs board 5;
	print_board(RED);
	Light l = (bd->is_in_check(White) && bd->cant_move(White)) ? RED : BLUE;
	led_driver->set_square(l,7,7,true);
}

void ChessGame::test_cant_move(void) {
	// Needs board 5;
	print_board(RED);
	Light l = bd->cant_move(White) ? RED : BLUE;
	led_driver->set_square(l,7,7,true);
}

void ChessGame::test_get_move(void) {
	main_loop();
	// DEBUGGING
	int col_state, col_turn;
	switch (state) {
	  case Waiting: col_state = 0;   break;
	  case Moving:  col_state = 1;   break;
	  case Error:   col_state = 2;   break;
	  case Ended:	  col_state = 3;   break;
	}
	switch (turn) {
		case White: col_turn = 6;  	break;
		case Black: col_turn = 7;  	break;
		case None:	col_turn = 5;	break;
	}
	// if (state != Error) { print_board(RED); }
	// led_driver->set_square(RED,0,col_state,true); 
	// led_driver->set_square(RED,0,col_turn,true);
}

void ChessGame::test_is_move_legal(void) {
  print_board(RED);
  if (hall_driver->any_moves_left()) {
    Tuple move = hall_driver->next_move();
    if (bd->is_move_legal(bd->get_test_tuple(),move)) {
      // Wanted to use RED, but print_board erases.
      led_driver->set_square(BLUE,move.row,move.col,true);
    }
  }
}

void ChessGame::test_promoted_pawns(void) {
	print_board(RED);
	if (bd->any_pawn_promoted()) {
		_promoted_col = bd->promoted_pawn();
		led_driver->set_square(BLUE,0,_promoted_col,true);
	}
}

void ChessGame::test_clock(void) {
	if (hall_driver->any_on()) {
		Tuple new_move = hall_driver->next_move();
		if (new_move.down) {
			turn = toggle_color(turn);
		}
	}
  clk->update_players_clock(turn);
  display_clock();
}

void ChessGame::test_clock_setup(void) {
  clk->set_duration(0);
	lcd->begin(16, 2);
  lcd->print("White   Black");
}

void ChessGame::test_buffer(void) {
  if (hall_driver->any_moves_left()) {
    Tuple move_sq = hall_driver->next_move();
    Light l = move_sq.down ? BLUE : RED;
    led_driver->set_square(l,move_sq.row,move_sq.col,true);
  }
}

void ChessGame::test_in_check_dinamic(void) {
  print_board(RED);
  if (hall_driver->any_on()) {
    Tuple aux = hall_driver->first_on();
    bd->set(aux.row,aux.col,White,Queen);
  } else {
    //led_driver->grid_off(BLUE);
    Tuple queen_sq = bd->find_piece(White,Queen);
    if (check_tuple(queen_sq)) {
      bd->empty_sq(queen_sq);
    }
  }
  Tuple kings_sq = bd->find_king(Black);
  BoolBoard legal;
  bool in_check = bd->is_in_check(Black);
  led_driver->set_square(BLUE,kings_sq.row,kings_sq.col,in_check);
}

void ChessGame::test_legal_squares_dinamic(void) {
  print_board(RED);
  if (hall_driver->any_moves_left()) {
    Tuple move = hall_driver->next_move();
    if (move.down) {
		bd->set(move.row,move.col,White,Knight);
		BoolBoard legal;
		bd->get(move.row,move.col)->legal_squares(legal,move,*bd);
		led_driver->grid_on(BLUE,legal);
	} else {
		bd->empty_sq(move);
		led_driver->grid_off(BLUE);
	}
  }
}

void ChessGame::test_legal_squares_static(void) {
  print_board(RED);
  Tuple aux = bd->get_test_tuple();
  bd->set(aux.row,aux.col,White,Bishop);
  BoolBoard legal2;
  bd->get(aux.row,aux.col)->legal_squares(legal2,aux,*bd);
  led_driver->set_square(BLUE,aux.row,aux.col,true);
  led_driver->grid_on(BLUE,legal2);
  delete &legal2;
}

void ChessGame::test_print_board(void) {
	print_board(RED);
}

void ChessGame::test_hall_diff(void) {
  if (hall_driver->any_moves_left()) {
      Tuple move_sq = hall_driver->next_move();
      led_driver->set_square(BLUE,move_sq.row,move_sq.col,move_sq.down);
  }
  led_driver->set_square(BLUE,7,7,hall_driver->any_on());
  led_driver->set_square(BLUE,6,7,hall_driver->is_buffer_full());
}

void ChessGame::test_square(char row, char col) {
  if (hall_driver->is_square_on(row,col)) {
    led_driver->set_square(BLUE,row,col,true);
  } else { led_driver->set_square(BLUE,row,col,false); }
  led_driver->set_square(RED,row,col,true);
}

void ChessGame::test_halls_in_loop(void) {
  for(char c=0; c<8; c++){
    for(char r=0; r<8; r++){
      led_driver->set_square(BLUE,r,c,hall_driver->is_square_on(r,c));
      // led_driver->set_square(RED,r,c,hall_driver->is_square_on(r,c));
    }
	}
}
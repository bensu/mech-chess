#include "chess_game.h"

#define HALL_PRESCALE 10    // Filter to add lag to the sensors
#define DISPLAY_FILTER 300  // Filter to refresh LCD screen once in a while

ChessGame::ChessGame(int v_led[],int red_pins[],int blue_pins[],int row_in[],
                      int col_on[],int lp[], int activate, int power, int buttons[]) {
	// Driver Object Creation
	led_driver 	 = 	new LedDriver(v_led,red_pins,blue_pins);
	hall_driver  =	new HallDriver(row_in,col_on);
	lcd 		     = 	new LiquidCrystal(lp[0],lp[1],lp[2],lp[3],lp[4],lp[5]);
	clk 		     = 	new Clock();
  button_driver = new ButtonDriver(activate,power,buttons);
	bd 			     = 	new Board();
  legal        =  new BoolBoard();  got_legal = false;
  diff         =  new BoolBoard();  // used for Error
  lcd->begin(16,2);
	_display_flag = false;
  _display_init_done = false;
  _display_done = false;
	_display_counter = 0;
	origin = nil_tuple();
	state = SetPieces; 		previous_state = SetPieces;
	turn = White;
  player_won = None;
  _display_done = false;
  _promoted_col = 9;
  // promotion_button = {.id = 0, .pressed = false};
  // Initialize
  _initialized = false;
  _stage = ModeSelect;
  _mode = Learning;
	// Testing variables
}

void ChessGame::reset_game(void) {
  clk->rewind();
  bd->new_board();
  legal->set_all(false);  got_legal = false;
  diff->set_all(false);
  _display_flag = false;
  _display_init_done = false;
  _display_done = false;
  _display_counter = 0;
  origin = nil_tuple();
  state = SetPieces;    previous_state = SetPieces;
  turn = White;
  player_won = None;
  _display_done = false;
  _promoted_col = 9;
  // promotion_button = {.id = 0, .pressed = false};
  // Initialize
  _initialized = false;
  _stage = ModeSelect;
  _mode = Learning;
}

void ChessGame::main_loop(void) {
  if (state == Promotion) {
    button_driver->activate_buttons(true);
    button_driver->scan();
    if (button_driver->any_button_on()) {
      Button promotion_button = button_driver->next_on();
      if (promotion_button.pressed) {
       led_driver->set_square(RED,4,4,true);
        PieceType type;
        switch (promotion_button.id) {
          case 0:  type = Knight;   break;
          case 1:  type = Bishop;   break;
          case 2:  type = Rook;     break;
          case 3:  type = Queen;    break;
          default: break;
        }
        char row = (turn == White) ? 7 : 0;
        bd->set(new_tuple(row, _promoted_col),turn,type);
        turn = toggle_color(turn);
        transition_to(Waiting);
      }
    }
  }
  if (clk->done(turn)) {
    state = Ended;
    player_won = toggle_color(turn);
  } else {
    Tuple new_move = hall_driver->next_move();
    if (check_tuple(new_move)) {
      manage_transition(state,new_move);
    }
  }
  output(state);
}

Init ChessGame::initialize(Init current_stage, int button) {
  // Implement the transitions for the first three states
  // of the state machine
  Init new_stage = ModeSelect;
  switch (current_stage) {
    case ModeSelect:
      switch (button) {
        case 0:
          _mode = Learning;
          new_stage = ClockSelect;
          break;
        case 3:
          _mode = Professional;
          new_stage = ClockSelect;
          break;
        default:
          new_stage = ModeSelect;
          break;
      }
      clk->rewind();
      break;
    case ClockSelect: {
      unsigned long duration;
      switch (button) {
        case 0:  duration = 5;  break;
        case 1:  duration = 30; break;
        case 2:  duration = 60; break;
        case 3:  duration = 0;  break;          
        default:                break;
      }
      clk->rewind();
      clk->set_duration(duration);
      new_stage = Start;
      break;
    }
    case Start:
      clk->rewind();
      new_stage = Start;
      break;
  }
  if (new_stage != current_stage ) {
    _display_init_done = false;
  }
  return new_stage;
}

void ChessGame::output_init(Init current_stage) {
  // Implement the outputs for the first three states
  // of the state machine
  if (!_display_init_done) {
    switch (current_stage) {
      case ModeSelect:
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print("Knight to Learn");
        lcd->setCursor(0,1);
        lcd->print("Queen for Pros");
        break;
      case ClockSelect:
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print("N   B   R   Q");
        lcd->setCursor(0,1);
        lcd->print("5m  30m 60m Inf");
        break;
      case Start:
        lcd->clear();
        break;
    }
    _display_init_done = true;
  }
}

void ChessGame::manage_transition(State current_state, Tuple new_move) {
  // Implement the transitions for the main states
  // of the state machine
  State next_state;
  // Check if the game is done
  if (bd->cant_move(turn)) {
    if (bd->is_in_check(turn)) {
      player_won = toggle_color(turn);
      next_state = Ended;
    } else { // Draw
      player_won = None;
      next_state = Ended;
    }
  } else {
    // It is not done, then...
    switch (current_state) {
      case SetPieces:
        // ignore new_move
        get_diff(previous_state);
        next_state = diff->any() ? SetPieces : Waiting;
        break;
      case Waiting:
        // Did the game just started?
        if (previous_state == SetPieces) {
          // Yes, rewind the clock.
          clk->rewind();
        }
        // Is the new move a good one?
        if (!new_move.down && (bd->get(new_move)->get_color() == turn)) {
          origin = new_move;
          next_state = Moving;
        } else {  next_state = Error; }
        break;  // Waiting
      case Moving: {
        // If there are no legal moves for that piece? Solved by the undo.
        bool undo = (new_move.down && same_square(new_move,origin)); // Am I undoing?
        // turns_move_down -> Placing one of my pieces in a correct square.
        bool turns_move_down = (new_move.down && legal->get(new_move));
        // opposite_move_up -> Removing my opponents piece.
        bool opposite_move_up = (!new_move.down && bd->get(new_move)->get_color() != turn 
                      && legal->get(new_move));
        if (turns_move_down) {
          // shared state
          // bd->update_history(origin,new_move);
          bd->make_move(origin,new_move);  // make_move
          if (bd->any_pawn_promoted()) {
            _promoted_col = bd->promoted_pawn();
            next_state = Promotion;
          } else {
            turn = toggle_color(turn);
            //local state
            got_legal = false;                
            legal->set_all(false); 
            next_state = Waiting;
          }
        } else if (undo) {      // If I undo, back to Waiting.
          got_legal = false;
          legal->set_all(false);
          next_state = Waiting;
        } else if (opposite_move_up) {
          // Constraints every the legal moves to where the opposite piece was moved.
          legal->set_all(false);
          legal->set(new_move,true);
          next_state = Moving;
        } else {  next_state = Error; }
        break;
      }
      case Promotion:
        // Wait for buttons
        next_state = Promotion;
        break;
      case Ended:
        next_state = Ended;
        break;
      case Error:
        // ignore new_move
        get_diff(previous_state);
        next_state = diff->any() ? Error : previous_state;
        break;
    }
  }
  transition_to(next_state);
}

void ChessGame::output(State current_state) {
  // Implement the outputs for the main states
  // of the state machine
  switch (current_state) {
    case SetPieces:
      if (!_display_done) {
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print("Set Pieces");
        lcd->setCursor(0,1);
        lcd->print("<-White  Black->");
        _display_done = true;
      }
      button_driver->activate_buttons(false);
      get_diff(Waiting); // Should I need it? Seems so.
      led_driver->grid_off(BLUE);
      led_driver->grid_on(RED,*diff);
      break;
    case Waiting:
      button_driver->activate_buttons(false);
      if (!_display_done && (previous_state == SetPieces ||
                              previous_state == Promotion)) {
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print("White     Black");
        _display_done = true;
      }
      
      clk->update_players_clock(turn);
      display_clock();
      led_driver->grid_off(BLUE);
      led_driver->grid_off(RED);
      break;
    case Moving:
      // Get necessary values
      // should check where does origin come from
      if (!got_legal && check_tuple(origin)) {
        bd->legal_moves(*legal,origin);
        got_legal = true;
      }
      // Output
      button_driver->activate_buttons(false);
      led_driver->grid_off(RED);
      clk->update_players_clock(turn);
      display_clock();
      // Check the mode, to see if legal moves is needed
      switch (_mode) {
        case Learning:
          led_driver->set_square(RED,origin.row,origin.col,true); // Undo
          led_driver->grid_on(BLUE,*legal);
          break;
        case Professional:
          led_driver->grid_off(BLUE);
          break;
      }
      break;
    case Promotion:
      if (!_display_done) {
        _display_done = true;
        display_promotion(turn);
        led_driver->set_square(BLUE,(turn==White) ? 7 : 0,_promoted_col,true);
      }
      // The clock doesn't run during promotion
      // clk->update_players_clock(turn);
      break;
    case Ended: {
      if (!_display_done) {
        button_driver->activate_buttons(false);
        _display_done = true;
        display_ended(player_won);
        led_driver->grid_off(BLUE);
        led_driver->grid_off(RED);
        Tuple kings_sq = bd->find_king(player_won);
        Tuple looser_sq = bd->find_king(toggle_color(player_won));
        led_driver->set_square(BLUE,kings_sq.row,kings_sq.col,true);
        led_driver->set_square(RED,looser_sq.row,looser_sq.col,true);
      }
      break;
    }
    case Error:
      // Get necessary values
      get_diff(previous_state); // Should I need it? Seems so.
      // Output
      // turn off buttons
      // The clock doesn't run during error
      // clk->update_players_clock(turn);
      button_driver->activate_buttons(false);
      display_clock();
      led_driver->grid_off(BLUE);
      led_driver->grid_on(RED,*diff);
      break;
  }
}

void ChessGame::transition_to(State new_state) {
  // Works just as a setter
  if (new_state != state) {
    previous_state = state;
    state = new_state;
    _display_done = false;
  }
}

void ChessGame::print_board(Light l) {
  // Only for testing purposes
  for(char r=0; r<8; ++r) {
    for(char c=0; c<8; ++c) {
      bool not_empty = !bd->get(r,c)->is_empty();
      led_driver->set_square(l,r,c,not_empty);  
    }
  }
}

void ChessGame::display_promotion(Color turn) {
  // Sets the LCD for the Promotion state.
  // Prepare data
  switch (turn) {
    case White: 
      lcd->setCursor(0,0);  lcd->print("White promotes ");
      break;
    case Black:
      lcd->setCursor(0,0);  lcd->print("Black promotes ");
      break;
    case None: 
      lcd->setCursor(0,0);  lcd->print("Error promotes ");
      break;
  }
  lcd->setCursor(0,1);      lcd->print("Choose a piece ");
}

void ChessGame::display_ended(Color winner) {
  // Sets the LCD for the Ended state.
  // Prepare data
  switch (winner) {
    case White: 
      lcd->setCursor(0,0);  lcd->print("White won!     ");
      break;
    case Black:
      lcd->setCursor(0,0);  lcd->print("Black won!     ");
      break;
    case None: 
      lcd->setCursor(0,0);  lcd->print("Draw game...   ");
      break;
  }
  lcd->setCursor(0,1);      lcd->print("Reset to play  ");
}

void ChessGame::display_clock(void) {
  // Displays the clock string but does not update it
  // Sets the LCD for Moving, Waiting, and Error states.
  // Prepare data
  if (_display_flag) {
    _display_flag = false;
    if (_display_counter++ > DISPLAY_FILTER) {
      _display_counter = 0;
      // Get the data
      // clk->clk->update_players_clock(turn);
      char w_clock_string[8];
      clk->print(w_clock_string,White);
      char b_clock_string[8];
      clk->print(b_clock_string,Black);
      // I/O
      lcd->setCursor(0, 1);
      lcd->print(w_clock_string);
      lcd->setCursor(8, 1);
      lcd->print(b_clock_string);
    }
  }
}

void ChessGame::display_isr(void) {
  _display_flag = true;
}

void ChessGame::get_diff(const State previous_state) {
  // Since get_diff should only be called during an error
  // we take the previous state
  bool aux = false;
  for(char c=0; c<8; c++) {
    for(char r=0; r<8; r++) {
      // Shouldn't check against origin if I'm Moving
      if (!(previous_state == Moving && (r==origin.row && c==origin.col))) { 
        aux = (!bd->get(r,c)->is_empty() != hall_driver->is_square_on(r,c));
        diff->set(r,c,aux);  // attribute of the chessgame.
      }
    }
  }
}





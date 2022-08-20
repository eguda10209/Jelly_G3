#include "board.h"

using namespace std;

void Board::output_board() {
	std::ofstream ofs("./board_debug.txt");

	for (int y = BOARD_HEIGHT - 1; y >= 0; y--) ofs << bitset<10>(board[y]) << endl;
}


void Board::set_board(unsigned short board[BOARD_HEIGHT]) {
	for (int y = 0; y < BOARD_HEIGHT; y++) this->board[y] = board[y];
}

void Board::reset_board() {
	for (int y = 0; y < BOARD_HEIGHT; y++) this->board[y] = BOARD_EMPTY;
}

short Board::clear_line() {
	short clear_lines = 0;
	vector<int> clear_line_nums;
	
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		if (board[y] == BOARD_FILL) {
			clear_line_nums.push_back(y);
			clear_lines++;
		}
	}
	if (clear_lines == 0) return 0;

	int ind = 0, cnt = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		if (cnt != clear_lines && y == clear_line_nums[cnt]) {
			cnt++;
			continue;
		}
		board[ind] = board[y];
		ind++;
	}

	for (int y = ind; y < BOARD_HEIGHT; y++) board[y] = BOARD_EMPTY;

	return clear_lines;
}



short Board::get_clear_lines() {
	short clear_lines = 0;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (board[y] == BOARD_FILL) clear_lines++;
	return clear_lines;
}

void Board::place_piece(Piece piece) {
	for (int y = 0; y < 4; y++) {
		if (piece.x <= 6) board[piece.y - y] |= Defines::piece[piece.type][piece.r][y] << (6 - piece.x);
		else board[piece.y - y] |= Defines::piece[piece.type][piece.r][y] >> (piece.x - 6);
	}
}


bool Board::can_place(Piece piece, short dx, short dy) {
	short px = piece.x + dx;
	short py = piece.y + dy;
	for (int y = 0; y < 4; y++) {
		if (px < Defines::piece_place_range_X[piece.type][piece.r][0] || px > Defines::piece_place_range_X[piece.type][piece.r][1]) return false;
		else if (py - y < 0 && Defines::piece[piece.type][piece.r][y] != 0) return false;
		else if ((px <= 6) && (board[py - y] & Defines::piece[piece.type][piece.r][y] << (6 - px) & BOARD_FILL) != BOARD_EMPTY) return false;
		else if((board[py - y] & Defines::piece[piece.type][piece.r][y] >> (px - 6) & BOARD_FILL) != BOARD_EMPTY) return false;
	}
	return true;
}


SRS_data Board::can_rotate(Piece piece, Rotation rotation) {
	SRS_data result;
	if (piece.type == PIECE_TYPE_O) return result;
    short to_r;
    if (rotation == Rotate_right) to_r = (piece.r + 1) % 4;
    else if (rotation == Rotate_left) to_r = (piece.r + 3) % 4;
    for (int i = 0; i < 5; ++i) {
        int dx = 0, dy = 0;
        if (rotation == Rotate_left) {
            if (piece.type == PIECE_TYPE_I) {
                dx = Defines::I_LeftRotation_dxy[piece.r][i][0];
                dy = Defines::I_LeftRotation_dxy[piece.r][i][1];
            }
            else {
                dx = Defines::SZJLT_LeftRotation_dxy[piece.r][i][0];
                dy = Defines::SZJLT_LeftRotation_dxy[piece.r][i][1];
            }
        }
        else if(rotation == Rotate_right) {
            if (piece.type == PIECE_TYPE_I) {
                dx = Defines::I_LeftRotation_dxy[to_r][i][0] * -1;
                dy = Defines::I_LeftRotation_dxy[to_r][i][1] * -1;
            }
            else {
                dx = Defines::SZJLT_LeftRotation_dxy[to_r][i][0] * -1;
                dy = Defines::SZJLT_LeftRotation_dxy[to_r][i][1] * -1;
            }
        }
		if (can_place(Piece(piece.type, piece.x, piece.y, to_r), dx, dy)) {
			result.index = i;
			result.dx = dx;
			result.dy = dy;
			result.to_r = to_r;
			return result;
		}
    }
	return result;
}

int Board::try_place(Piece *piece, short dx, short dy, Rotation rotation, bool is_fall) {
	if(!piece->move(this, dx, dy)) return -1;
	if (rotation != Rotate_null && !(piece->rotate(this, rotation))) return -1;
	if(is_fall) piece->fall(this);
	this->place_piece(*piece);
	return this->clear_line();
}


Board* Board::copy_board() {
	//Board new_board;
	//new_board = (*this);
	//return &new_board;
	Board *new_board = (Board*) malloc(sizeof(Board));
	//*new_board = *this;
	//for (int y = 0; y < BOARD_HEIGHT; y++) new_board->board[y] = board[y];

	new_board->btb = this->btb;
	new_board->combo = this->combo;
	new_board->hold_piece = this->hold_piece;
	new_board->max_height = this->max_height;
	for (int i = 0; i < BOARD_HEIGHT; i++) new_board->board[i] = this->board[i];
	for (int i = 0; i < NEXT_MAX; i++) new_board->next_piece[i] = this->next_piece[i];

	return new_board;
}

/*combo btb pieces max_heightの更新*/
void Board::updata_board_status(short clear_lines, Rotation_type rot_type, bool use_hold) {
	/*max_heightの更新*/
	/*
	[1] 0000000000
	[0] 0000111100	-> max_height:1
	*/
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		if (this->board[y] == BOARD_EMPTY) {
			this->max_height = y;
			break;
		}
	}

	/*piecesの更新*/
	if (use_hold && this->hold_piece == -1) {
		this->hold_piece = this->next_piece[0];
		for (int i = 0; i < NEXT_MAX - 1; i++) this->next_piece[i] = this->next_piece[i + 1];
		this->next_piece[NEXT_MAX - 1] = -1;
	}
	else if (use_hold) this->hold_piece = this->next_piece[0];
	for(int i = 0; i < NEXT_MAX - 1; i++) this->next_piece[i] = this->next_piece[i + 1];
	this->next_piece[NEXT_MAX - 1] = -1;

	//combo, btb更新
	this->b_btb = this->btb;
	this->b_combo = this->combo;
	if (clear_lines == 0) {
		this->combo = -1;
		this->btb = false;
	}
	else if (clear_lines == 4) {
		this->combo++;
		this->btb = true;
	}
	else {
		this->combo++;
		if(rot_type == TSpinmini || rot_type == TSpin) this->btb = true;
		else this->btb = false;
	}
}


void Board::update_board_data(short player_num) {
	bool skip = false;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		short by = BOARD_EMPTY;
		if (skip) {
			this->board[y] = by;
			continue;
		}
		for (int x = 0; x < 10; x++) {
			if (ProcessMemory::GetBoard(x, y, player_num) == 1) {
				by |= 0b1000000000 >> x;
			}

		}
		this->board[y] = by;
		if (by == BOARD_EMPTY) skip = true;
	}
}

void Board::update_board_by_memory(short player_num, short nexts) {
	/*盤面*/
	this->update_board_data(player_num);

	/*piece*/
	this->next_piece[0] = ProcessMemory::GetCurrentmino(player_num);
	for(int i = 1; i <= nexts; i++) this->next_piece[i] = ProcessMemory::GetNextmino(i, player_num);
	this->hold_piece = ProcessMemory::GetHoldmino(player_num);


	/*max_height*/
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		if (this->board[y] == BOARD_EMPTY) {
			this->max_height = y;
			break;
		}
	}

	/*combo & btb*/
	this->b_btb = this->btb;
	this->b_combo = this->combo;
	short combo_btb = ProcessMemory::GetComboAndBtb(player_num);
	if (combo_btb >= 0xFF) {
		combo = combo_btb - 0xFF - 1;
		btb = true;
	}
	else {
		combo = combo_btb - 1;
		btb = false;
	}
}
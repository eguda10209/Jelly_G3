#include "controller.h"


int Controller::move(Piece_move_data pmd, short player_num) {
    /*ホールド操作*/
    if (pmd.use_hold) {
        bool is_first_hold = (ProcessMemory::GetHoldmino(player_num) == -1);
        input(MOVE_HOLD, MOVE_NONE, player_num);
        if (is_first_hold) wait_frame(6);
    }

    /*通常操作*/
    int dx, dr, drr, drl;
    int hd = 0;
    if (pmd.moves[0] == MOVE_HARDDROP) hd = 1;
    int piece_type = ProcessMemory::GetCurrentmino(player_num);
    int cr = 0;

    int ret, try_cnt = 0;
    while (true) {
        if (try_cnt >= 10) {
            return -1;
        }
        dx = pmd.to_x + 1 - ProcessMemory::GetX(player_num);
        cr = ProcessMemory::GetRotation(player_num);
        dr = 0;
        drr = (pmd.to_r - cr + 4) % 4;
        drl = (cr - pmd.to_r + 4) % 4;
        if (drr <= drl) dr = drr;
        else dr = -drl;
        if (piece_type == PIECE_TYPE_I && (pmd.to_r == 1 || pmd.to_r == 2)) dx++;

        if (dx == 0 && dr == 0 && hd == 0) {
            break;
        }
        else if (dx > 0 && dr > 0) {
            ret = input(MOVE_MOVE_RIGHT, MOVE_ROTATE_RIGHT, player_num);
        }
        else if (dx > 0 && dr < 0) {
            ret = input(MOVE_MOVE_RIGHT, MOVE_ROTATE_LEFT, player_num);
        }
        else if (dx < 0 && dr > 0) {
            ret = input(MOVE_MOVE_LEFT, MOVE_ROTATE_RIGHT, player_num);
        }
        else if (dx < 0 && dr < 0) {
            ret = input(MOVE_MOVE_LEFT, MOVE_ROTATE_LEFT, player_num);
        }
        else if (dr == 1 && hd == 1) {
            ret = input(MOVE_ROTATE_RIGHT, MOVE_HARDDROP, player_num);
            hd = 0;
            break;
        }
        else if (dr == -1 && hd == 1) {
            ret = input(MOVE_ROTATE_LEFT, MOVE_HARDDROP, player_num);
            hd = 0;
            break;
        }
        else if (dr > 0) {
            ret = input(MOVE_ROTATE_RIGHT, MOVE_NONE, player_num);
        }
        else if (dr < 0) {
            ret = input(MOVE_ROTATE_LEFT, MOVE_NONE, player_num);
        }
        else if (dx > 0) {
            ret = input(MOVE_MOVE_RIGHT, MOVE_NONE, player_num);
        }
        else if (dx < 0) {
            ret = input(MOVE_MOVE_LEFT, MOVE_NONE, player_num);
        }
        else if (hd == 1) {
            ret = input(MOVE_HARDDROP, MOVE_NONE, player_num);
            hd = 0;
            break;
        }
        if (ret == -1) {
            return -1;
        }
        try_cnt++;
    }

    if (pmd.moves[0] != MOVE_SOFTDROP) return 0;
    /*追加操作*/
    ret = input(MOVE_SOFTDROP, MOVE_NONE, player_num);
    hd = 1;
    for (int i = 1; i < pmd.moves_size; i++) {
        if (i == pmd.moves_size - 1 && pmd.moves[i] == MOVE_ROTATE_RIGHT) {
            ret = input(MOVE_ROTATE_RIGHT, MOVE_HARDDROP, player_num);
            hd = 0;
        }else if (i == pmd.moves_size - 1 && pmd.moves[i] == MOVE_ROTATE_LEFT) {
            ret = input(MOVE_ROTATE_LEFT, MOVE_HARDDROP, player_num);
            hd = 0;
        }
        else if (pmd.moves[i] == MOVE_ROTATE_RIGHT) {
            ret = input(MOVE_ROTATE_RIGHT, MOVE_NONE, player_num);
        }
        else if (pmd.moves[i] == MOVE_ROTATE_LEFT) {
            ret = input(MOVE_ROTATE_LEFT, MOVE_NONE, player_num);
        }
        else if (pmd.moves[i] == MOVE_MOVE_RIGHT) {
            ret = input(MOVE_MOVE_RIGHT, MOVE_NONE, player_num);
        }
        else if (pmd.moves[i] == MOVE_MOVE_LEFT) {
            ret = input(MOVE_MOVE_LEFT, MOVE_NONE, player_num);
        }
        else if (pmd.moves[i] == MOVE_MOVE_DOWN) {
            ret = input(MOVE_MOVE_DOWN, MOVE_NONE, player_num);
        }
        if (ret == -1) {
            return -1;
        }
    }
    if (hd == 1) {
        ret = input(MOVE_HARDDROP, MOVE_NONE, player_num);
    }
    return 0;
}


int Controller::input(char input1, char input2, short player_num) {
	/*入力*/
    if (input1 == MOVE_SOFTDROP) {
        XBoxController::Press(X360Buttons::Down);

        int piece_type = ProcessMemory::GetCurrentmino(player_num);
        int cx = ProcessMemory::GetX(player_num) - 1;
        int cy = ProcessMemory::GetY(player_num);
        int cr = ProcessMemory::GetRotation(player_num);
        int ty = 0;
        if (piece_type == PIECE_TYPE_I && (cr == 1 || cr == 2)) cx--;

        Piece piece(piece_type, cx, cy, cr);
        Board board(0);
        board.update_board_data(player_num);
        piece.fall(&board);
        ty = piece.y - 1;
        if (piece_type == PIECE_TYPE_I && cr == 2) ty--;
        if (piece_type == PIECE_TYPE_I && cr == 3) ty--;
        int by = 40;
        int wait_cnt = 0;

        while (true) {
            if (wait_frame(1) == -1) {
                XBoxController::Press(X360Buttons::None);
                return -1;
            }
            int cy = ProcessMemory::GetY(player_num);
            if (by == cy && wait_cnt >= 60) return -1;
            else wait_cnt = 0;
            if (cy == ty) break; 
            by = cy;
            wait_cnt++;
        }

        XBoxController::Press(X360Buttons::None);
        wait_frame(1);
        return 0;
    }
    else {
        if (input1 == MOVE_MOVE_DOWN && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::Down);
        if (input1 == MOVE_HARDDROP && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::Up);
        if (input1 == MOVE_MOVE_LEFT && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::Left);
        if (input1 == MOVE_MOVE_RIGHT && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::Right);
        if (input1 == MOVE_ROTATE_RIGHT && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::B);
        if (input1 == MOVE_ROTATE_LEFT && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::A);
        if (input1 == MOVE_MOVE_LEFT && input2 == MOVE_ROTATE_RIGHT) { XBoxController::Press(X360Buttons::Left); XBoxController::Press(X360Buttons::Left | X360Buttons::B); }
        if (input1 == MOVE_MOVE_RIGHT && input2 == MOVE_ROTATE_RIGHT) { XBoxController::Press(X360Buttons::Right); XBoxController::Press(X360Buttons::Right | X360Buttons::B); }
        if (input1 == MOVE_MOVE_LEFT && input2 == MOVE_ROTATE_LEFT) { XBoxController::Press(X360Buttons::Left); XBoxController::Press(X360Buttons::Left | X360Buttons::A); }
        if (input1 == MOVE_MOVE_RIGHT && input2 == MOVE_ROTATE_LEFT) { XBoxController::Press(X360Buttons::Right); XBoxController::Press(X360Buttons::Right | X360Buttons::A); }
        if (input1 == MOVE_HOLD && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::LeftBumper);
        if (input1 == VK_RETURN && input2 == MOVE_NONE) XBoxController::Press(X360Buttons::A);
        if (input1 == MOVE_ROTATE_RIGHT && input2 == MOVE_HARDDROP) { XBoxController::Press(X360Buttons::B); XBoxController::Press(X360Buttons::B | X360Buttons::Up); }
        if (input1 == MOVE_ROTATE_LEFT && input2 == MOVE_HARDDROP) { XBoxController::Press(X360Buttons::A); XBoxController::Press(X360Buttons::A | X360Buttons::Up); }
        if (input1 == MOVE_MOVE_LEFT && input2 == MOVE_HARDDROP) { XBoxController::Press(X360Buttons::Left); XBoxController::Press(X360Buttons::Left | X360Buttons::Up); }
        if (input1 == MOVE_MOVE_RIGHT && input2 == MOVE_HARDDROP) { XBoxController::Press(X360Buttons::Right); XBoxController::Press(X360Buttons::Right | X360Buttons::Up); }
    }
    int ret;

    /*1frame以上入力を行う*/
    ret = wait_frame(1);
    if (ret == -1) return -1;

    if (input1 == MOVE_MOVE_DOWN || input2 == MOVE_MOVE_DOWN) {
        ret = wait_frame(2);
        if (ret == -1) return -1;
    }
    XBoxController::Press(X360Buttons::None);

    /*入力後遅延*/
    if (input1 == MOVE_HARDDROP || input2 == MOVE_HARDDROP) {
        ret = wait_frame(7);
        if (ret == -1) return -1;
    }
    else {
        ret = wait_frame(1);
        if (ret == -1) return -1;
    }
    
    return 0;
}


int Controller::wait_frame(short frames) {
    int s_frame = ProcessMemory::GetFrames();
    int b_frame = s_frame;
    int c_frame;

    double update_t = (double)clock() / (double)CLOCKS_PER_SEC;
    while (true) {
        c_frame = ProcessMemory::GetFrames();
        if (c_frame >= s_frame + frames) return 0;
        if (c_frame != b_frame) {
            update_t = (double)clock() / (double)CLOCKS_PER_SEC;
            b_frame = c_frame;
        }
        if (((double)clock() / (double)CLOCKS_PER_SEC) - update_t >= 0.050) {
            //Comment(TEXT("wait_frame failed."), 0, 0);
            break;  //0.050ms以上フレームの更新がない場合終了。
        }
    }
    return -1;
}
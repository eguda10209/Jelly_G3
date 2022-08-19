#include "bot.h"

using namespace std;

Bot::Bot(int n) {
    before_score = 0;
    before_frame = 0;
    is_opening = true;

    player_num = 1;
    nexts = 5;
    board = new Board(0);
    evaluation = new Evaluation();
}

void Bot::run_bot() {
    if (!GetIsAION()) return;
    //if (GetForegroundWindow() != FindWindow(TEXT("TetPuyo"), TEXT("PuyoPuyoTetris"))) return; // ぷよテトが最前面でなければ終了 
	
    PPT_Status ppt_status = get_ppt_status();
    if (ppt_status == InMenu_Select_Character) {
        //キャラ選択
        Comment(TEXT("InMenu_Select_Character"), 0, 0);
    }
    
    if (ppt_status != InGame_Playing) return;

    //Comment(TEXT("y: "), ProcessMemory::GetY(1), 1);
    //return;

    /*盤面情報取得*/
    this->board->update_board_by_memory(this->player_num, this->nexts);

    /*最適解計算*/
    //次の解が未探索の場合
    Solution opt_sol = evaluation->find_optimal_solution(*(this->board), PIECE_SPAWN_X, PIECE_SPAWN_Y, PIECE_SPAWN_R);
    //死亡
    if (opt_sol.moves_data.size() == 0) return;


    std::ofstream ofs("./debug.txt");
    ofs << "tox: " << opt_sol.moves_data[0]->to_x << endl;
    ofs << "tor: " << opt_sol.moves_data[0]->to_r << endl;
    ofs << "movessize: " << opt_sol.moves_data[0]->moves_size << endl;
    for (int i = 0; i < opt_sol.moves_data[0]->moves_size; i++) {
        ofs << opt_sol.moves_data[0]->moves[i] << " ";
    }
    ofs << endl;
    for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(opt_sol.moves_data[0]->board->board[y]) << endl; ofs << endl;

    for (int y = BOARD_HEIGHT - 1 - 20; y >= 0; y--) ofs << bitset<10>(this->board->board[y]) << endl; ofs << endl;

    /*操作*/
    int ret = Controller::move(*(opt_sol.moves_data[0]), this->player_num);
    if(ret == -1) Comment(TEXT("failed: "), opt_sol.moves_data[0]->moves[0] == MOVE_HARDDROP, 1);
    else Comment(TEXT("scceed: "), opt_sol.moves_data[0]->moves[0] == MOVE_HARDDROP, 1);

    // 次の手が計算済みの場合、取得した盤面情報を比較？
    //最適解計算
    //操作
        //操作中次の手を計算

    for (int i = 0; i < opt_sol.moves_data.size(); i++) {
        free(opt_sol.moves_data[i]->board);
        free(opt_sol.moves_data[i]);
    }
}


PPT_Status Bot::get_ppt_status() {
    if (ProcessMemory::IsCharacterSelect()) { //キャラクター自動選択
        return InMenu_Select_Character;
    }
    else if (ProcessMemory::IsGame()) {
        int frames = ProcessMemory::GetFrames();
        int score = ProcessMemory::GetScore(this->player_num);

        if (frames == before_frame) {
            return InGame_Pauseing;
        }else if ((frames < before_frame)) {
            is_opening = true;
            //リセット
            Comment(TEXT("reset"),0,0);
        }
        else if (before_score > 0 && score == 0){
        is_opening = true;
        }
        else is_opening = false;

        before_frame = frames;
        before_score = score;
        return InGame_Playing;
    }
    else return InMenu_Main;
}
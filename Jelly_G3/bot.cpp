#include "bot.h"
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

using namespace std;

Bot::Bot(int n) {
    before_score = 0;
    before_frame = 0;
    is_opening = true;

    player_num = 1;
    nexts = 5;
    board = new Board(0);
    evaluation = new Evaluation();
    next_opt_sol = (Solution*) malloc(sizeof(Solution));
    is_find_next_opt_sol = false;
    executed_free_opt_sol = true;
}

Bot::~Bot() {
    delete board;
    delete evaluation;

    if (!executed_free_opt_sol && is_find_next_opt_sol) {
        for (int i = 0; i < next_opt_sol->moves_data_size; i++) {
            free(next_opt_sol->moves_data[i]->board);
            free(next_opt_sol->moves_data[i]);
        }
    }

    free(next_opt_sol);
}

void Bot::run_bot() {
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    if (!GetIsAION()) return;
    if (GetForegroundWindow() != FindWindow(TEXT("TetPuyo"), TEXT("PuyoPuyoTetris"))) return; // ぷよテトが最前面でなければ終了 
	
    PPT_Status ppt_status = get_ppt_status();
    if (ppt_status == InMenu_Select_Character) {
        //キャラ選択
        Comment(TEXT("InMenu_Select_Character"), 0, 0);
    }
    
    if (ppt_status != InGame_Playing) return;

    //Comment(TEXT("y: "), ProcessMemory::GetY(1), 1);
    //return;


    /*最適解計算*/
    Solution opt_sol;
    if (!is_find_next_opt_sol) { //次の解が未探索の場合
        /*盤面情報取得*/
        this->board->update_board_by_memory(this->player_num, this->nexts);
        
        this->IsMoveFinished = 100;
        opt_sol = evaluation->find_optimal_solution(*(this->board), PIECE_SPAWN_X, PIECE_SPAWN_Y, PIECE_SPAWN_R, &IsMoveFinished);
        if (opt_sol.moves_data_size == 0) return;    //死亡
    }
    else {
        opt_sol = *(next_opt_sol);
        for (int i = 0; i < 7; i++) {
            opt_sol.moves_data[opt_sol.moves_data_size - 1]->board->next_piece[i] = this->board->next_piece[i + 1];
        }
        if (opt_sol.moves_data_size == 0) return;    //死亡
        //Comment(TEXT("using nt"), 0, 0);
    }
    executed_free_opt_sol = false;

    /*操作(並列処理)*/
    this->IsMoveFinished = -1;
    std::thread move_thread(&Bot::move_thread_func, this, *(opt_sol.moves_data[opt_sol.moves_data_size - 1]), this->player_num);

    /*並列計算*/
    //Solution next_opt_sol_ = evaluation->find_optimal_solution(*(opt_sol.moves_data[0]->board), PIECE_SPAWN_X, PIECE_SPAWN_Y, PIECE_SPAWN_R);
    *next_opt_sol = evaluation->find_optimal_solution(*(opt_sol.moves_data[opt_sol.moves_data_size - 1]->board), PIECE_SPAWN_X, PIECE_SPAWN_Y, PIECE_SPAWN_R, &IsMoveFinished);
    if (next_opt_sol->moves_data_size == 0) is_find_next_opt_sol = false;
    else is_find_next_opt_sol = true;

    /*捜査終了したら計算中断させる？　MCTを引き継いで計算*/

    move_thread.join();


    /*盤面情報取得*/
    this->board->update_board_by_memory(this->player_num, this->nexts);

    // 次の手が計算済みの場合、取得した盤面情報を比較？
    //違う場合、is_find_next_opt_sol = falseにする？
    for (int y = 0; y < 40; y++) {
        if (is_find_next_opt_sol) {
            if (opt_sol.moves_data[opt_sol.moves_data_size - 1]->board->board[y] != this->board->board[y]) {
                //総数が4つ以上上昇 -> garbage
                int b_cell_cnt = 0, r_cell_cnt = 0;
                for (int y_ = 0; y_ < 40; y_++) {
                    for (int x = 0; x < 10; x++) {
                        if ((opt_sol.moves_data[opt_sol.moves_data_size - 1]->board->board[y_] & (0b1000000000 >> x)) != 0) b_cell_cnt++;
                        if ((this->board->board[y_] & (0b1000000000 >> x)) != 0) r_cell_cnt++;
                    }
                }
                if (r_cell_cnt == b_cell_cnt || ((r_cell_cnt - b_cell_cnt) % 9) != 0) {//置きミス
                    is_find_next_opt_sol = false; //置きミスの場合のみにするべき
                    Comment(TEXT("miss"), 0, 0);
                    for (int i = 0; i < next_opt_sol->moves_data_size; i++) {
                        free(next_opt_sol->moves_data[i]->board);
                        free(next_opt_sol->moves_data[i]);
                    }
                    break;
                }
                else {
                    Comment(TEXT("garbage: "), (r_cell_cnt - b_cell_cnt) / 9, 1);
                    //todo 盤面を更新する
                    break;
                }
            }
        }
    }

    for (int i = 0; i < opt_sol.moves_data_size; i++) {
        free(opt_sol.moves_data[i]->board);
        free(opt_sol.moves_data[i]);
    }
    executed_free_opt_sol = true;
    //_CrtDumpMemoryLeaks();
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
            if (is_find_next_opt_sol) {
                for (int i = 0; i < next_opt_sol->moves_data_size; i++) {
                    free(next_opt_sol->moves_data[i]->board);
                    free(next_opt_sol->moves_data[i]);
                }
            }
            is_find_next_opt_sol = false;
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


void Bot::move_thread_func(Piece_move_data pmd, int player_num) {
    int ret = Controller::move(pmd, player_num);

    /*ライン消去の遅延*/
    short final_clear_line = ProcessMemory::GetLinesCleared(player_num);
    if (final_clear_line == 1) Controller::wait_frame(36);
    else if (final_clear_line == 2 || final_clear_line == 3) Controller::wait_frame(41);
    else if (final_clear_line == 4) Controller::wait_frame(46);
    //pcの場合は1
    this->IsMoveFinished = 0;
}
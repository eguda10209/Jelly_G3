#include "include.h"
using namespace std;

HWND hWnd, hWnd2;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM mp, LPARAM lp);
LRESULT CALLBACK WndProcSub(HWND hWnd, UINT msg, WPARAM mp, LPARAM lp);
int GetAIType();
int Getnextthink();
HWND hWndPPT = FindWindow(TEXT("TetPuyo"), TEXT("PuyoPuyoTetris")); //PPTのハンドル取得
bool IsAION = true, IsControllerConnected = false;
PAINTSTRUCT ps;
HDC hdc, hdc1;
HWND check1, check2, check3, check4, combo1, combo_mode, comment, comment2, comment3, combo_player, button1, XBox_Up, XBox_Down, XBox_Left, XBox_Right,
XBox_A, XBox_B, XBox_Connect, XBox_DisConnect, XBox_LA, Reload, SpeedPercentage;
HPEN hpen;
int nextthink = 0;
bool initcalled = false;

int minocolor[9][3] = { {0,255,0},
                        {255,0,0},
                        {0,0,255},
                        {255,124,0},
                        {128,0,128},
                        {255,255,0},
                        {0,255,255},
                        {80,80,80},
                        {10,10,10} };

bool GetIsAION() {
    return IsAION;
}

int Getnextthink() {
    return  SendMessage(combo1, CB_GETCURSEL, 0, 0) + 1;
}

int GetAIType() {
    int type = SendMessage(combo_mode, CB_GETCURSEL, 0, 0);
    /*switch (type)
    {
    case 0:
        return AITYPE_NORMAL;
        break;
    case 1:
        return AITYPE_TETRIS;
        break;
    case 2:
        return AITYPE_TEST;
        break;
    case 3:
        return AITYPE_TSD_ONRY;
        break;
    case 4:
        return AITYPE_ADJUST;
        break;
    default:
        return AITYPE_NORMAL;
        break;
    }*/
    return 0;
}

int GetPlayerNumber() {
    return SendMessage(combo_player, CB_GETCURSEL, 0, 0) + 1;
}

void PrintColorBoard(int colorboard[THINKBOARD_H][THINKBOARD_W]) {
    if (IsIconic(hWnd2)) return;
    hdc = GetDC(hWnd2);
    int r = 0, g = 0, b = 0;

    /*SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 255)));
    PatBlt(hdc, 0, 0, 215, 440, PATCOPY);
    DeleteObject(
        SelectObject(
            hdc, GetStockObject(WHITE_BRUSH)
        )
    );*/

    for (int y = 0; y < THINKBOARD_H; y++) {
        for (int x = 0; x < THINKBOARD_W; x++) {
            if (colorboard[y][x] != -1) {
                r = minocolor[colorboard[y][x] % 10][0];
                g = minocolor[colorboard[y][x] % 10][1];
                b = minocolor[colorboard[y][x] % 10][2];
                if (colorboard[y][x] > 10) {
                    if (colorboard[y][x] == 13) {
                        r = 255; g = 218; b = 185;
                    }
                    else {
                        r += 150; g += 150; b += 150;
                        if (r > 255) r = 255;
                        if (g > 255) g = 255;
                        if (b > 255) b = 255;
                    }
                }
                SetTextColor(hdc, RGB(r, g, b));
                if (colorboard[y][x] >= 7) TextOut(hdc, 16 * x, 16 * y, TEXT("■"), lstrlen(TEXT("■")));
                else {
                    //TextOut(hdc, 16 * x, 16 * y, TEXT("□"), lstrlen(TEXT("□"))); 
                    TextOut(hdc, 16 * x, 16 * y, TEXT("■"), lstrlen(TEXT("■")));
                }
            }
            else {
                SetTextColor(hdc, RGB(255, 255, 255));
                TextOut(hdc, 16 * x, 16 * y, TEXT("■"), lstrlen(TEXT("■")));

            }
            //線表示
            if (y == THINKBOARD_H - 20 + 1 && x == 0) {
                hpen = CreatePen(PS_SOLID, 0, RGB(200, 200, 200)); //ペン作成　必ず削除
                SelectObject(hdc, hpen);
                MoveToEx(hdc, 0, 1 + 16 * (THINKBOARD_H - 20), NULL);
                LineTo(hdc, 200, 1 + 16 * (THINKBOARD_H - 20));
                DeleteObject(hpen);
            }

            //SetTextColor(hdc, RGB(0,0, 0));
            //TextOut(hdc, 20 * x, 20 * y, TEXT("■"), lstrlen(TEXT("■")));

        }
    }

    ReleaseDC(hWnd2, hdc);
}

void Comment(LPCTSTR text, int num, int usevar) {
    if (usevar == 0) {
        SendMessage(comment, LB_INSERTSTRING, 0, (LPARAM)text);
    }
    else if (usevar == 1) {
        static TCHAR output[40];
        _stprintf_s(output, 40, TEXT("%s%d"), text, num);
        SendMessage(comment, LB_INSERTSTRING, 0, (LPARAM)output);
    }
    else if (usevar == 2) {
        static TCHAR output[40];
        _stprintf_s(output, 40, TEXT("%s%d ms"), text, num);
        SendMessage(comment2, LB_INSERTSTRING, 0, (LPARAM)output);
    }
    else if (usevar == 3) {
        static TCHAR output[40];
        _stprintf_s(output, 40, TEXT("%s%d"), text, num);
        SendMessage(comment3, LB_INSERTSTRING, 0, (LPARAM)output);
    }
}

void CommentStr(std::string str) {
    size_t newsize = strlen(str.c_str()) + 1;
    size_t convertedChars = 0;
    wchar_t* wcstring = new wchar_t[newsize];
    mbstowcs_s(&convertedChars, wcstring, newsize, str.c_str(), _TRUNCATE);
    SendMessage(comment, LB_INSERTSTRING, 0, (LPARAM)wcstring);
    //WndProc(hWnd, WM_PAINT, 0, 0);
    delete[]wcstring;
}

int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdLine, int nCmdShow)
{

    ProcessMemory::GetHandle();
    //メインウィンドウ
    const TCHAR CLASSNAME[] = TEXT("Jelly/AI");
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);                         //構造体のサイズ
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;        //クラスのスタイル
    wc.lpfnWndProc = WndProc;                               //プロシージャ名　　//コールバック関数
    wc.cbClsExtra = 0;                                      //補助メモリ
    wc.cbWndExtra = 0;                                      //補助メモリ
    wc.hInstance = GetModuleHandle(NULL);                   //インスタンス
    wc.hIcon = (HICON)LoadImage(                            // アイコン
        NULL, TEXT("Jelly.ico"), IMAGE_ICON,
        0, 0, LR_SHARED | LR_LOADFROMFILE
    );
    wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);      //カーソル
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景ブラシ
    wc.lpszMenuName = MAKEINTRESOURCE(NULL);                //メニュー名
    wc.lpszClassName = CLASSNAME;                           //クラス名
    wc.hIconSm = NULL;                                       //小さいアイコン

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("RegisterClassEx function failed (Main)"), TEXT(""), MB_OK);
        return 0;
    }

    //サブウィンドウ
    const TCHAR CLASSNAMESUB[] = TEXT("Thinking");
    WNDCLASSEX subwc;
    subwc.cbSize = sizeof(WNDCLASSEX);                         //構造体のサイズ
    subwc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;        //クラスのスタイル
    subwc.lpfnWndProc = WndProcSub;                               //プロシージャ名　　//コールバック関数
    subwc.cbClsExtra = 0;                                      //補助メモリ
    subwc.cbWndExtra = 0;                                      //補助メモリ
    subwc.hInstance = GetModuleHandle(NULL);                   //インスタンス
    subwc.hIcon = (HICON)LoadImage(                            // アイコン
        NULL, TEXT("Jelly.ico"), IMAGE_ICON,
        0, 0, LR_SHARED | LR_LOADFROMFILE
    );
    subwc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);      //カーソル
    subwc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景ブラシ
    subwc.lpszMenuName = MAKEINTRESOURCE(NULL);                //メニュー名
    subwc.lpszClassName = CLASSNAMESUB;                           //クラス名
    subwc.hIconSm = NULL;                                       //小さいアイコン

    if (!RegisterClassEx(&subwc)) {
        MessageBox(NULL, TEXT("RegisterClassEx function failed (Sub)"), TEXT(""), MB_OK);
        return 0;
    }

    int IsPlayer2 = false;
    if (FindWindow(NULL, TEXT("Jelly/AI")) != NULL) {
        IsPlayer2 = true;
    }

    //ウィンドウ作成
    int windowx = 0, windowy = 0;
    if (IsPlayer2) {
        windowy = 250;
    }
    hWnd = CreateWindowEx(
        WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT,    //拡張ウィンドウスタイル
        CLASSNAME,                                  //ウィンドウ名
        CLASSNAME,                                  //タイトルバーにこの名前が表示されます
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,   //ウィンドウスタイル //& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX サイズ変更、最大化無効
        windowx,                                          //X座標　起動時
        windowy,                                          //Y座標　起動時
        400,                                        //幅
        470,                                        //高さ
        NULL,                                       //親ウィンドウのハンドル　親の場合はNULL
        NULL,                                       //メニューハンドルorリソースID
        GetModuleHandle(NULL),                      //インスタンス
        NULL                                        //ウィンドウ作成データ
    );

    hWnd2 = CreateWindowEx(
        WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT,    //拡張ウィンドウスタイル
        CLASSNAMESUB,                                  //ウィンドウ名
        CLASSNAMESUB,                                  //タイトルバーにこの名前が表示されます
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,   //ウィンドウスタイル //& ~WS_THICKFRAME & ~WS_MAXIMIZEBOX サイズ変更、最大化無効
        windowx + 400,                                          //X座標　起動時
        windowy,                                          //Y座標　起動時
        175,                                        //幅
        40 + 16 * THINKBOARD_H,                                        //高さ
        hWnd,                                       //親ウィンドウのハンドル　親の場合はNULL
        NULL,                                       //メニューハンドルorリソースID
        GetModuleHandle(NULL),                      //インスタンス
        NULL                                        //ウィンドウ作成データ
    );

    //ボタン作成
    button1 = CreateWindow(
        TEXT("BUTTON"), TEXT("ON/OFF"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        40, 45,      //x,y
        100, 25,    //size
        hWnd, (HMENU)1, hInstance, NULL
    );
    check1 = CreateWindow(
        TEXT("BUTTON"), TEXT("---"),
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        23, 105, 150, 25,
        hWnd, (HMENU)2, hInstance, NULL
    );
    check2 = CreateWindow(
        TEXT("BUTTON"), TEXT("---"),
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        23, 135, 150, 25,
        hWnd, (HMENU)3, hInstance, NULL
    );
    check4 = CreateWindow(
        TEXT("BUTTON"), TEXT("思考表示"),
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        23, 165, 150, 25,
        hWnd, (HMENU)5, hInstance, NULL
    );

    combo_mode = CreateWindow(
        TEXT("COMBOBOX"), NULL,
        WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST,
        23, 195, 150, 300,
        hWnd, (HMENU)4, hInstance, NULL
    );
    combo1 = CreateWindow(
        TEXT("COMBOBOX"), NULL,
        WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST,
        23, 225, 100, 300,
        hWnd, (HMENU)6, hInstance, NULL
    );
    comment = CreateWindow(
        TEXT("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
        3, 255, 379, 160,
        hWnd, (HMENU)7, hInstance, NULL
    );
    comment2 = CreateWindow(
        TEXT("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
        3, 405, 179, 30,
        hWnd, (HMENU)8, hInstance, NULL
    );
    comment3 = CreateWindow(
        TEXT("LISTBOX"), NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
        203, 405, 179, 30,
        hWnd, (HMENU)9, hInstance, NULL
    );
    XBox_Connect = CreateWindow(
        TEXT("BUTTON"), TEXT("接続"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        223, 55, 100, 25,
        hWnd, (HMENU)10, hInstance, NULL
    );
    XBox_DisConnect = CreateWindow(
        TEXT("BUTTON"), TEXT("切断"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        223, 85, 100, 25,
        hWnd, (HMENU)11, hInstance, NULL
    );
    combo_player = CreateWindow(
        TEXT("COMBOBOX"), NULL,
        WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST,
        223, 25, 100, 300,
        hWnd, (HMENU)12, hInstance, NULL
    );

    XBox_Up = CreateWindow(
        TEXT("BUTTON"), TEXT("↑"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        253, 115, 25, 25,
        hWnd, (HMENU)13, hInstance, NULL
    );
    XBox_Down = CreateWindow(
        TEXT("BUTTON"), TEXT("↓"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        253, 145, 25, 25,
        hWnd, (HMENU)14, hInstance, NULL
    );
    XBox_Left = CreateWindow(
        TEXT("BUTTON"), TEXT("←"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        223, 145, 25, 25,
        hWnd, (HMENU)15, hInstance, NULL
    );
    XBox_Right = CreateWindow(
        TEXT("BUTTON"), TEXT("→"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        283, 145, 25, 25,
        hWnd, (HMENU)16, hInstance, NULL
    );
    XBox_A = CreateWindow(
        TEXT("BUTTON"), TEXT("A"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        323, 115, 25, 25,
        hWnd, (HMENU)17, hInstance, NULL
    );
    XBox_B = CreateWindow(
        TEXT("BUTTON"), TEXT("B"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        353, 115, 25, 25,
        hWnd, (HMENU)18, hInstance, NULL
    );
    XBox_LA = CreateWindow(
        TEXT("BUTTON"), TEXT("LB+A"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        323, 145, 55, 25,
        hWnd, (HMENU)19, hInstance, NULL
    );
    Reload = CreateWindow(
        TEXT("BUTTON"), TEXT("Reload"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        223, 190, 100, 25,
        hWnd, (HMENU)20, hInstance, NULL
    );
    SpeedPercentage = CreateWindow(
        TEXT("EDIT"), TEXT("100"),
        WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
        263, 220, 30, 25,
        hWnd, (HMENU)21, hInstance, NULL
    );

    SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEXT("Next: 1"));
    SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEXT("Next: 2"));
    SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEXT("Next: 3"));
    SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEXT("Next: 4"));
    SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEXT("Next: 5"));
    SendMessage(combo_player, CB_ADDSTRING, 0, (LPARAM)TEXT("Player: 1"));
    SendMessage(combo_player, CB_ADDSTRING, 0, (LPARAM)TEXT("Player: 2"));
    SendMessage(combo_mode, CB_ADDSTRING, 0, (LPARAM)TEXT("1. Normal"));
    SendMessage(combo_mode, CB_ADDSTRING, 0, (LPARAM)TEXT("2. Tetris"));
    SendMessage(combo_mode, CB_ADDSTRING, 0, (LPARAM)TEXT("3. TEST"));
    SendMessage(combo_mode, CB_ADDSTRING, 0, (LPARAM)TEXT("4. TSD_Only"));
    SendMessage(combo_mode, CB_ADDSTRING, 0, (LPARAM)TEXT("5. Adjust"));
    SendMessage(check1, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(check2, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(check3, BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(combo1, CB_SETCURSEL, 4, 0);
    SendMessage(combo_mode, CB_SETCURSEL, 0, 0);

    if (!IsPlayer2) {//同プロセスが起動済みならばプレイヤーを2に設定
        SendMessage(combo_player, CB_SETCURSEL, 0, 0);
    }
    else {
        Comment(TEXT("=> Player: 2"), 0, 0);
        SendMessage(combo_player, CB_SETCURSEL, 1, 0);
    }

    //設定表示
    Comment(TEXT("=> /O2"), 0, 0);
    //if (SearchMethod == 0) Comment(TEXT("=> BS"), 0, 0);
    //if (SearchMethod == 1) Comment(TEXT("=> MCTS"), 0, 0);
    
    debug();

    XBoxController::PlugIn();//起動時にコントローラー接続
    Comment(TEXT("コントローラーを接続しました。"), NULL, 0);
    IsControllerConnected = true;

    if (!hWnd) {
        MessageBox(NULL, TEXT("CreatWindow function failed"), TEXT(""), MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //ShowWindow(hWnd2, SW_SHOWMINNOACTIVE);
    ShowWindow(hWnd2, nCmdShow);
    UpdateWindow(hWnd2);
    SendMessage(check4, BM_SETCHECK, BST_CHECKED, 0);
    //メッセージキュー
    MSG msg;
    msg.message = {};
    msg.hwnd = NULL;

    //メッセージループ
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else {
            //メッセージが来ない場合の処理

        }
    }
    return 0;
}

DWORD WINAPI ThreadFunc(LPVOID vdParam) {
    //while (true) AI::RunAI();
    return true;
}

//ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wc, LPARAM lp) {
    TCHAR Text1[] = TEXT("Jelly_G3 Ver1.0");
    TCHAR Text2[] = TEXT("AI: OFF");
    TCHAR Text3[] = TEXT("AI: ON ");
    static HFONT hFont1;
    DWORD dwID;
    switch (msg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        //SetBkColor(hdc, RGB(0, 255, 0)); //背景色

        //SetTextColor(hdc, RGB(0xff, 0x00, 0x00));  // 文字色を設定
        SetTextAlign(hdc, TA_CENTER);
        TextOut(hdc, 93, 20, Text1, lstrlen(Text1));   //描画するデバイスコンテキストのハンドル x,y 文字列、文字数
        TextOut(hdc, 93, 75, Text3, lstrlen(Text3));
        TextOut(hdc, 303, 220, TEXT("%"), lstrlen(TEXT("%")));
        TextOut(hdc, 233, 220, TEXT("Speed:"), lstrlen(TEXT("Speed:")));
        hpen = CreatePen(PS_SOLID, 0, RGB(128, 128, 128)); //ペン作成　必ず削除
        SelectObject(hdc, hpen);
        MoveToEx(hdc, 0, 100, NULL);
        LineTo(hdc, 200, 100);
        MoveToEx(hdc, 200, 0, NULL);
        LineTo(hdc, 200, 255);
        EndPaint(hWnd, &ps);
        DeleteObject(hpen); //ペン削除

        //開幕テンプレファイル読み込み。
        if (!initcalled) {
            /*if (AI::RoadOpeningTemplateFile() == -1) AI::LoadOpeningTemplateSucceed = false;
            else AI::LoadOpeningTemplateSucceed = true;*/
            initcalled = true;
        }
        return 0;
    case WM_CTLCOLORSTATIC:
    {
        HBRUSH hbrParent;
        SetBkMode((HDC)wc, TRANSPARENT);        // 背景透明モード
        hbrParent = GetSysColorBrush(COLOR_WINDOW);
        return (LRESULT)hbrParent;                              // 背景塗りつぶし
    }
    break;

    case WM_COMMAND:
        if (HIWORD(wc) == CBN_CLOSEUP) {
            SetForegroundWindow(hWndPPT);
        }
        switch (LOWORD(wc)) {
        case 0:
            SetForegroundWindow(hWndPPT); //PPTを最前面に
            break;
        case 1:
            SetForegroundWindow(hWndPPT);
            hdc = GetDC(hWnd);   // DC 取得 WM_PAINT 以外の時
            if (IsAION) {
                SetTextAlign(hdc, TA_CENTER);
                TextOut(hdc, 93, 75, Text2, lstrlen(Text2));
                Comment(TEXT("AI -> OFF！"), NULL, 0);
                IsAION = false;
            }
            else {
                SetTextAlign(hdc, TA_CENTER);
                TextOut(hdc, 93, 75, Text3, lstrlen(Text3));
                Comment(TEXT("AI -> ON！"), NULL, 0);
                IsAION = true;
            }
            ReleaseDC(hWnd, hdc);
            break;
        case 2:
            SetForegroundWindow(hWndPPT);
            if (SendMessage(check1, BM_GETCHECK, 0, 0)) SendMessage(check1, BM_SETCHECK, BST_UNCHECKED, 0);
            else SendMessage(check1, BM_SETCHECK, BST_CHECKED, 0);
            break;
        case 3:
            SetForegroundWindow(hWndPPT);
            if (SendMessage(check2, BM_GETCHECK, 0, 0)) SendMessage(check2, BM_SETCHECK, BST_UNCHECKED, 0);
            else SendMessage(check2, BM_SETCHECK, BST_CHECKED, 0);
            break;
        case 4:
            break;
        case 5:
            if (IsIconic(hWnd2)) {
                ShowWindow(hWnd2, SW_SHOWNORMAL);
                SendMessage(check4, BM_SETCHECK, BST_CHECKED, 0);
            }
            else {
                ShowWindow(hWnd2, SW_SHOWMINNOACTIVE);
                SendMessage(check4, BM_SETCHECK, BST_UNCHECKED, 0);
            }
            break;

        case 10:
            SetForegroundWindow(hWndPPT);
            if (IsControllerConnected == false) {
                XBoxController::PlugIn();
                Comment(TEXT("コントローラーを接続しました。"), NULL, 0);
                IsControllerConnected = true;
            }
            else {
                Comment(TEXT("既に接続されています！"), NULL, 0);
            }
            break;
        case 11:
            SetForegroundWindow(hWndPPT);
            if (IsControllerConnected == true) {
                XBoxController::UnplugAll();
                Comment(TEXT("コントローラーを切断しました。"), NULL, 0);
                IsControllerConnected = false;
            }
            else {
                Comment(TEXT("接続されていません！"), NULL, 0);
            }
            break;
        case 12:
            break;
        case 13:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::Up);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 14:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::Down);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 15:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::Left);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 16:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::Right);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 17:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::A);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 18:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::B);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 19:
            SetForegroundWindow(hWndPPT);
            XBoxController::Press(X360Buttons::LeftBumper); XBoxController::Press(X360Buttons::LeftBumper | X360Buttons::A);
            Sleep(30);
            XBoxController::Press(X360Buttons::None);
            break;
        case 20:
            SetForegroundWindow(hWndPPT);
            //AI::ReloadData();
            Comment(TEXT("Reload Completed!"), NULL, 0);
            break;
        case 21:
            if (HIWORD(wc) == EN_UPDATE) {
                LPTSTR strText = (LPTSTR)calloc((GetWindowTextLength(SpeedPercentage) + 1), sizeof(TCHAR));
                if (strText != NULL) {
                    GetWindowText(SpeedPercentage, strText, (GetWindowTextLength(SpeedPercentage) + 1));
                    char c[] = "";
                    c[0] = strText[0];
                    string persentage_0(c, sizeof(c) / sizeof(c[0]));
                    c[0] = strText[1];
                    string persentage_1(c, sizeof(c) / sizeof(c[0]));
                    c[0] = strText[2];
                    string persentage_2(c, sizeof(c) / sizeof(c[0]));
                    string str = persentage_0 + persentage_1 + persentage_2;
                    int speedpersentage = atoi(str.c_str());
                    if (speedpersentage < 0 || speedpersentage > 100) {
                        SendMessage(SpeedPercentage,
                            WM_SETTEXT,
                            NULL,
                            (LPARAM)L"100");
                        speedpersentage = 100;
                        //Controller::ControllerDelay = max(((float)Controller_Delay_Max / 100.0) * (101 - speedpersentage), 1);
                    }
                    else {
                        if (speedpersentage == 0) speedpersentage = 100;
                        //Controller::ControllerDelay = max(((float)Controller_Delay_Max / 100.0) * (101 - speedpersentage), 1);
                    }
                }
                free(strText);
            }
            else if (HIWORD(wc) == EN_KILLFOCUS) {
                LPTSTR strText = (LPTSTR)calloc((GetWindowTextLength(SpeedPercentage) + 1), sizeof(TCHAR));
                if (strText != NULL) {
                    GetWindowText(SpeedPercentage, strText, (GetWindowTextLength(SpeedPercentage) + 1));
                    char c[] = "";
                    c[0] = strText[0];
                    string persentage_0(c, sizeof(c) / sizeof(c[0]));
                    c[0] = strText[1];
                    string persentage_1(c, sizeof(c) / sizeof(c[0]));
                    c[0] = strText[2];
                    string persentage_2(c, sizeof(c) / sizeof(c[0]));
                    string str = persentage_0 + persentage_1 + persentage_2;
                    int speedpersentage = atoi(str.c_str());
                    if (speedpersentage <= 0 || speedpersentage > 100) {
                        SendMessage(SpeedPercentage,
                            WM_SETTEXT,
                            NULL,
                            (LPARAM)L"100");
                        speedpersentage = 100;
                        //Controller::ControllerDelay = max(((float)Controller_Delay_Max / 100.0) * (101 - speedpersentage), 1);
                    }
                }

            }
            break;
        }

        return 0;
    case WM_CREATE:
        hFont1 = CreateFont(9, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, 0,
            SHIFTJIS_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH, NULL);
        SendMessage(comment, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0));

        SetTimer(hWnd, 1, 1, nullptr);    //1000: 1sごと

        //別スレッドでAI実行
        //CreateThread(NULL, 0, ThreadFunc, (LPVOID)hWnd, 0, &dwID);
        return 0;
    case WM_TIMER:
        //AI::RunAI();
        //if (IsDebugMode && DebugDoOnce) DefWindowProc(hWnd, WM_CLOSE, wc, lp);
        return 0;
    case WM_CLOSE:
    case WM_DESTROY:
        XBoxController::UnplugAll();
        DeleteObject(hFont1);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wc, lp);
}

//ウィンドウプロシージャ
LRESULT CALLBACK WndProcSub(HWND hWnd, UINT msg, WPARAM wc, LPARAM lp) {
    static HFONT hFont1;
    int x = 0, y = 0, mino = 1;
    switch (msg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_CTLCOLORSTATIC:
    {
        HBRUSH hbrParent;
        SetBkMode((HDC)wc, TRANSPARENT);        // 背景透明モード
        hbrParent = GetSysColorBrush(COLOR_WINDOW);
        return (LRESULT)hbrParent;                              // 背景塗りつぶし
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wc)) {
        case 0:
            break;
        }
        return 0;
    case WM_CREATE:
        return 0;
    case WM_TIMER:
        return 0;
    case WM_CLOSE:
    case WM_DESTROY:
        ShowWindow(hWnd2, SW_SHOWMINNOACTIVE);
        SendMessage(check4, BM_SETCHECK, BST_UNCHECKED, 0);
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wc, lp);
}

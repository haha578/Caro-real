#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <cctype>
#include <conio.h>
#include <string>
#include "Board.h"
#include "SaveLoad.h"

// Vòng lặp chơi cờ (dùng chung cho ván mới và ván chơi tiếp)
// Hàm này KHÔNG tự reset bàn cờ, việc chuẩn bị do nơi gọi làm.
void GameLoop() {
    bool validEnter = true;
    while (1)
    {
        _COMMAND = toupper(_getch());
        if (_COMMAND == 27)                 // ESC: thoát giữa chừng
        {
            trySave(UNFINISHED);
            ExitGame();
            return;                         // quay về menu
        }
        else {
            if (_COMMAND == 'A') MoveLeft();
            else if (_COMMAND == 'W') MoveUp();
            else if (_COMMAND == 'S') MoveDown();
            else if (_COMMAND == 'D') MoveRight();
            else if (_COMMAND == 13) {
                int c = CheckBoard(_X, _Y);
                switch (c) {
                case -1:
                    printf_s("X"); recordMove(c); break;
                case 1:
                    printf_s("O"); recordMove(c); break;
                case 0: validEnter = false;
                }
                if (validEnter == true) {
                    int boardResult = TestBoard();
                    switch (ProcessFinish(boardResult)) {
                    case -1: case 1: case 0:
                        trySave(toRecordResult(boardResult));
                        if (AskContinue() != 'Y') {
                            ExitGame();
                            return;
                        }
                        else {
                            g_openedIndex = -1;
                            g_openedRecord = GameRecord();

                            StartGame();
                            moveCount = 0;
                        }
                    }
                }
                validEnter = true;
            }
        }
    }
}

// Ván mới
void PlayNewGame() {
    g_openedIndex = -1;
    g_openedRecord = GameRecord();

    StartGame();        // xóa màn hình, reset dữ liệu, vẽ bàn cờ
    moveCount = 0;
    GameLoop();
}

// Dựng lại ván đã lưu lên màn hình và vào lại các biến của Board.cpp
void RestoreGame(const GameRecord& r) {
    system("cls");
    ResetData();                    // _A trống, _X/_Y về ô đầu
    DrawBoard(BOARD_SIZE);

    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    rebuildBoard(r, board);         // board[i][j] = 0 / 1 / 2

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) continue;

            GotoXY(_A[i][j].x, _A[i][j].y);
            if (board[i][j] == 1) {         // người 1 = X
                _A[i][j].c = -1;
                printf_s("X");
            }
            else {                          // người 2 = O
                _A[i][j].c = 1;
                printf_s("O");
            }
        }
    }

    // Khôi phục danh sách nước đi để lưu lại được sau này
    moveCount = 0;
    for (size_t k = 0; k < r.moves.size(); k++) {
        moves[moveCount++] = r.moves[k];
    }

    _TURN = (nextPlayer(r) == 1);   // true = lượt X (người 1)

    _X = _A[0][0].x;                // đưa con trỏ về góc trên trái
    _Y = _A[0][0].y;
    GotoXY(_X, _Y);
}

// Chọn 1 ván trong danh sách để chơi tiếp
void DelAndSave() {
    system("cls");
    GameRecord ds[100];
    int n = loadGameRecords(ds, 100);   // cùng thứ tự với showSavedGames()

    showSavedGames();
    if (n <= 0) {
        printf("\nNhan phim bat ky de quay lai menu...");
        _getch();
        return;
    }

    printf("\nNhap STT van muon choi hoac xoa (0 = quay lai): ");
    std::string line;
    Readlines(stdin, line);
    int idx = atoi(line.c_str());       // đổi chuỗi sang số
    if (idx < 1 || idx > n) return;     // 0 hoặc nhập sai -> về menu

    printf_s("\n  1. Choi tiep\n  2. Xoa\n  0. Quay lai\nChon: ");
    int ch = _getch();

    if (ch == '1') {
        const GameRecord& r = ds[idx - 1];
        if (r.result != UNFINISHED) {
            printf("\nVan nay da ket thuc, khong the choi tiep.\n");
            printf("Nhan phim bat ky de quay lai menu...");
            _getch();
            return;
        }

        g_openedIndex = idx - 1;
        g_openedRecord = r;

        RestoreGame(r);
        GameLoop();
    }
    else if (ch == '2') {
        printf("\n\n");
        deleteSavedGame(idx);
        printf("\nNhan phim bat ky de tiep tuc...");
        _getch();
    }
}

int ShowMenu() {
    system("cls");
    printf("========== CARO ==========\n\n");
    printf("  1. Choi moi\n");
    printf("  2. Choi tiep van da luu\n");
    printf("  0. Thoat\n\n");
    printf("Chon: ");
    return _getch();
}

int main() {
    FixConsoleWindow();
    while (true) {
        int choice = ShowMenu();

        if (choice == '1') {
            PlayNewGame();
        }
        else if (choice == '2') {
            DelAndSave();
        }
        else if (choice == '0') {
            return 0;
        }
    }
}
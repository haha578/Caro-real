#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>

using namespace std;

const int MAX_BOARD_SIZE = 12;
const string SAVE_FILE = "caro_saves.txt";
const string SAVE_TEMP_FILE = "caro_temp_save.txt";

// Xoa van dau da luu
bool DeleteSavedGame(int k);
bool deleteRecord(int index);
bool deleteSavedGame(int k);


// Ket qua van dau
enum GameResult {
    UNFINISHED = 0,
    P1_WIN = 1,
    P2_WIN = 2,
    DRAW = 3
};

// Mot nuoc di (player = 1 hoac 2)
struct Move {
    int row;
    int col;
    int player;
};

// Mot van dau da luu
struct GameRecord {
    string       name;        // ten ban luu
    string       player1;     // ten nguoi choi 1
    string       player2;     // ten nguoi choi 2
    int          result;      // UNFINISHED / P1_WIN / P2_WIN / DRAW
    int          boardSize;   // kich thuoc ban co
    time_t       savedTime;   // thoi diem luu
    vector<Move> moves;       // danh sach nuoc di (so nuoc = moves.size())
};

bool saveRecordTo(const GameRecord& record, const char* path);
bool saveAllRecords(const vector<GameRecord>& list);

// ---- PHAN 1 (Mai Thy) ----
void resultToString(const GameRecord& r, char* out);
void timeToString(time_t t);
bool nameExists(const char* name);
bool askSaveOnExit(const char* p1, const char* p2, int result, int boardSize,
    const Move* moves, int moveCount,
    int openedIndex = -1, const GameRecord* opened = nullptr);
void rebuildBoard(const GameRecord& r, int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE]);
int  nextPlayer(const GameRecord& r);

// ---- PHAN 2 (Dvu) ----
bool                saveRecord(const GameRecord& record);   // truoc day dat ten GameRecord (trung ten struct)
bool                Readlines(FILE* fp, string& out);
bool                updateRecord(int index, const GameRecord& record);
vector<GameRecord>  ReadAllRecords();
int                 loadGameRecords(GameRecord* ds, int maxCount);  // goi ReadAllRecords, tra ve so ban ghi

// ---- PHAN 3 (Mkhang) ----
void showSavedGames();
// ---- Thong tin ban luu dang choi tiep ----
extern int        g_openedIndex;    // -1 = van moi, >= 0 = vi tri ban dang choi tiep
extern GameRecord g_openedRecord;   // noi dung ban dang choi tiep

#endif
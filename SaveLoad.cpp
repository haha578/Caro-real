#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <vector>
#include "SaveLoad.h"
int g_openedIndex = -1;
GameRecord g_openedRecord;

//PHẦN 1 CỦA MAI THY////////////////////
//Hàm tiện ích
void resultToString(const GameRecord& r, char* out)
{
    if (r.result == P1_WIN) {
        sprintf_s(out, 30, "%s thang", r.player1.c_str());
    }
    else if (r.result == P2_WIN) {
        sprintf_s(out, 30, "%s thang", r.player2.c_str());
    }
    else if (r.result == DRAW) {
        strcpy_s(out, 30, "Hoa");
    }
    else {
        strcpy_s(out, 30, "Chua ket thuc");
    }
}

//chuyển giây thành định dạng ngày/tháng/năm và giờ/phút/giây
void timeToString(time_t t) {
    char buf[32];
    struct tm tmInfo;
    localtime_s(&tmInfo, &t);
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &tmInfo);
    printf_s("%s", buf);
}

//Kiểm tra tên trùng
bool nameExists(const char* name) {
    vector<GameRecord> ds = ReadAllRecords();

    for (size_t i = 0; i < ds.size(); i++) {
        if (ds[i].name == name) {
            return true;
        }
    }
    return false;
}

//Hỏi lưu khi thoát
bool askSaveOnExit(const char* p1, const char* p2, int result, int boardSize,
    const Move* moves, int moveCount,
    int openedIndex, const GameRecord* opened)
{
    string choice;
    printf_s("Ban co muon luu lai tran dau nay khong? (y/n): ");
    Readlines(stdin, choice);

    if (choice.empty() || (choice[0] != 'y' && choice[0] != 'Y')) return false;

    // Đang chơi tiếp một bản đã lưu hay là ván mới?
    bool isContinue = (opened != nullptr && openedIndex >= 0);

    GameRecord r;
    if (isContinue) {
        r.name = opened->name;      // giữ nguyên tên cũ, không hỏi lại
    }
    else {
        while (true)
        {
            printf_s("Nhap ten ban luu: ");
            if (!Readlines(stdin, r.name)) {
                return false;       // hết dữ liệu nhập, tránh lặp vô hạn
            }
            if (r.name.empty())
            {
                printf_s("Ten khong duoc de trong!\n");
                continue;
            }
            if (nameExists(r.name.c_str()))
            {
                printf_s("Ten da ton tai, vui long chon ten khac!\n");
                continue;
            }
            break;
        }
    }

    r.player1 = p1;
    r.player2 = p2;
    r.result = result;
    r.boardSize = boardSize;
    if (moveCount > 0) {
        r.moves.assign(moves, moves + moveCount);
    }
    r.savedTime = time(NULL);

    // Chơi tiếp thì ghi đè, ván mới thì nối thêm vào cuối file
    bool ok = isContinue ? updateRecord(openedIndex, r) : saveRecord(r);

    if (ok) {
        // Cap nhat thong tin ban luu hien tai
        g_openedRecord = r;

        // Neu day la lan dau luu van moi
        if (!isContinue) {
            vector<GameRecord> ds = ReadAllRecords();
            g_openedIndex = (int)ds.size() - 1;
        }

        printf_s("Da luu thanh cong!\n");
    }
    else {
        printf_s("Loi: khong the ghi file!\n");
    }

    return true;
}


//Chơi tiếp từ trận đấu đã lưu
void rebuildBoard(const GameRecord& r, int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE])
{
    for (int i = 0; i < r.boardSize; i++)
    {
        for (int j = 0; j < r.boardSize; j++)
        {
            board[i][j] = 0;
        }
    }

    for (size_t k = 0; k < r.moves.size(); k++)
    {
        const Move& m = r.moves[k];
        board[m.row][m.col] = m.player;
    }
}

int nextPlayer(const GameRecord& r) {
    if (r.moves.empty()) return 1;
    return r.moves.back().player == 1 ? 2 : 1;
}

////////////////////////


//PHẦN 2 CỦA DVU ///////////////////
// Định dạng mỗi bản ghi:
//   name / player1 / player2
//   result savedTime boardSize moveCount
//   row col player   (lặp moveCount dòng)

void writeRecord(FILE* fp, const GameRecord& record) {
    fprintf_s(fp, "%s\n%s\n%s\n", record.name.c_str(), record.player1.c_str(), record.player2.c_str());
    fprintf_s(fp, "%d %lld %d %d\n", record.result, (long long)record.savedTime, record.boardSize, (int)record.moves.size());

    for (size_t i = 0; i < record.moves.size(); i++) {
        fprintf_s(fp, "%d %d %d\n", record.moves[i].row, record.moves[i].col, record.moves[i].player);
    }
}

bool saveRecordTo(const GameRecord& record, const char* path) {
    FILE* fp = nullptr;
    if (fopen_s(&fp, path, "a") != 0 || fp == nullptr) {
        return false;
    }

    fprintf_s(fp, "%s\n%s\n%s\n", record.name.c_str(), record.player1.c_str(), record.player2.c_str());
    fprintf_s(fp, "%d %lld %d %d\n", record.result, (long long)record.savedTime, record.boardSize, (int)record.moves.size());

    for (size_t i = 0; i < record.moves.size(); i++) {
        fprintf_s(fp, "%d %d %d\n", record.moves[i].row, record.moves[i].col, record.moves[i].player);
    }

    fclose(fp);
    return true;
}

bool saveRecord(const GameRecord& record) {
    FILE* fp = nullptr;
    if (fopen_s(&fp, SAVE_FILE.c_str(), "a") != 0 || fp == nullptr) {
        return false;
    }
    writeRecord(fp, record);

    fclose(fp);
    return true;
}

bool saveAllRecords(const vector<GameRecord>& list) {
    FILE* fp = nullptr;
    if (fopen_s(&fp, SAVE_TEMP_FILE.c_str(), "w") != 0 || fp == nullptr) {
        return false;
    }
    fclose(fp);

    for (size_t i = 0; i < list.size(); i++) {
        if (!saveRecordTo(list[i], SAVE_TEMP_FILE.c_str())) {
            remove(SAVE_TEMP_FILE.c_str());
            return false;
        }
    }

    remove(SAVE_FILE.c_str());
    return rename(SAVE_TEMP_FILE.c_str(), SAVE_FILE.c_str()) == 0;
}

bool deleteRecord(int index) {
    vector<GameRecord> list = ReadAllRecords();
    if (index < 0 || index >=(int)list.size()) {
        return false;
    }

    list.erase(list.begin() + index);
    return saveAllRecords(list);
}

bool deleteSavedGame(int k) {
    vector<GameRecord> list = ReadAllRecords();
    if (k < 1 || k >(int)list.size()) {
        return false;
    }

    printf_s("Ban co chac muon xoa tran dau nay (y/n)?");
    string c;
    Readlines(stdin, c);
    if (c.empty() || (c[0] != 'y' && c[0] != 'Y')) {
        printf_s("Da huy\n");
        return false;
    }

    if (deleteRecord(k - 1)) {
        printf_s("Da xoa ban ghi thanh cong!\n");
        return true;
    }
    printf_s("Loi: Khong the xoa ban ghi!\n");
    return false;
}

// Ghi 1 bản ghi vào file fp đã được mở sẵn (dùng chung cho nhiều hàm)


// Thay bản ghi thứ index (đếm từ 0) bằng record, rồi ghi lại cả file
// Update record thứ index, trả về true nếu thành công, false nếu lỗi
bool updateRecord(int index, const GameRecord& record) {
    vector<GameRecord> list = ReadAllRecords();
    if (index < 0 || index >= (int)list.size()) {
        return false;
    }
    list[index] = record;

    FILE* fp = nullptr;
    if (fopen_s(&fp, SAVE_FILE.c_str(), "w") != 0 || fp == nullptr) {
        return false;
    }
    for (size_t i = 0; i < list.size(); i++) {
        writeRecord(fp, list[i]);
    }
    fclose(fp);
    return true;
}

// Đọc 1 dòng từ file (hoặc stdin), bỏ ký tự xuống dòng
bool Readlines(FILE* fp, string& out) {
    char str_cap[256];
    if (fgets(str_cap, sizeof(str_cap), fp) == NULL) {
        out.clear();
        return false;
    }

    str_cap[strcspn(str_cap, "\r\n")] = '\0';

    out = str_cap;
    return true;
}

vector<GameRecord> ReadAllRecords() {
    vector<GameRecord> list;
    FILE* fp = nullptr;
    if (fopen_s(&fp, SAVE_FILE.c_str(), "r") != 0 || fp == nullptr) {
        return list;
    }

    string line;
    while (true) {
        GameRecord record;

        if (!Readlines(fp, record.name))    break;
        if (!Readlines(fp, record.player1)) break;
        if (!Readlines(fp, record.player2)) break;
        if (!Readlines(fp, line))           break;

        int result = 0, sizeBoard = 0, moveCount = 0;
        long long time = 0;
        if (sscanf_s(line.c_str(), "%d %lld %d %d", &result, &time, &sizeBoard, &moveCount) != 4) {
            break;
        }
        if (result < UNFINISHED || result > DRAW) break;
        if (sizeBoard <= 0 || sizeBoard > MAX_BOARD_SIZE) break;
        if (moveCount < 0 || moveCount > sizeBoard * sizeBoard) break;

        record.result = result;
        record.savedTime = (time_t)time;
        record.boardSize = sizeBoard;

        bool ok = true;
        for (int i = 0; i < moveCount; i++) {
            Move move;
            if (!Readlines(fp, line) ||
                sscanf_s(line.c_str(), "%d %d %d", &move.row, &move.col, &move.player) != 3 ||
                move.row < 0 || move.row >= sizeBoard ||
                move.col < 0 || move.col >= sizeBoard ||
                (move.player != 1 && move.player != 2)) {
                ok = false;
                break;
            }
            record.moves.push_back(move);
        }
        if (!ok) break;

        list.push_back(record);
    }

    fclose(fp);
    return list;
}

// Chép tối đa maxCount bản ghi vào mảng ds, trả về số bản ghi đã chép
int loadGameRecords(GameRecord* ds, int maxCount) {
    vector<GameRecord> list = ReadAllRecords();

    int n = (int)list.size();
    if (n > maxCount) n = maxCount;

    for (int i = 0; i < n; i++) {
        ds[i] = list[i];
    }
    return n;
}
/////////////

//PHẦN 3 của Mkhang/////////////
//Hiển thị danh sách các bản đã lưu
void showSavedGames() {
    GameRecord ds[100];
    int n = loadGameRecords(ds, 100);

    if (n <= 0) {
        printf_s("Chua co ban luu nao.\n");
        return;
    }

    //Dòng tiêu đề (%-Ns = canh trái, rộng N ký tự)
    printf_s("%-5s%-20s%-15s%-15s%-20s%s\n",
        "STT", "Ten ban luu", "Nguoi choi 1", "Nguoi choi 2",
        "Ket qua", "Thoi gian luu");

    //Dòng kẻ ngang (90 dấu '-')
    for (int k = 0; k < 90; k++) printf_s("-");
    printf_s("\n");

    for (int i = 0; i < n; i++) {
        const GameRecord& r = ds[i];

        char ketQua[30];
        resultToString(r, ketQua);      //hàm của bạn ghi kết quả vào chuỗi ketQua

        printf_s("%-5d%-20s%-15s%-15s%-20s",
            i + 1, r.name.c_str(), r.player1.c_str(), r.player2.c_str(), ketQua);

        timeToString(r.savedTime);      //hàm này tự in ra màn hình
        printf_s("\n");
    }
}
///////////////////////
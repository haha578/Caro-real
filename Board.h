#pragma once   // Tránh việc file .h bị include nhiều lần

//HẰNG SỐ//
// Kích thước bàn cờ 12x12, vị trí bắt đầu vẽ bàn cờ
#define BOARD_SIZE 12
#define LEFT 3
#define TOP 1

//KIỂU DỮ LIỆU
struct _POINT { int x; int y; int c; };

// BIẾN TOÀN CỤC
extern _POINT _A[BOARD_SIZE][BOARD_SIZE];
extern bool _TURN;
extern int _COMMAND;
extern int _X, _Y;


//KHAI BÁO HÀM//
// Console
void FixConsoleWindow();          // Khóa console (không cho phóng to/kéo giãn)
void GotoXY(int x, int y);        // Di chuyển con trỏ tới tọa độ (x, y)

// Dữ liệu và vẽ bàn cờ
void ResetData();                 // Reset bàn cờ
void DrawBoard(int pSize);        // Vẽ bàn cờ

// Điều khiển game
void StartGame();                 // Bắt đầu game
void GarbageCollect();            // Dọn dẹp bộ nhớ
void ExitGame();                  // Thoát game

// Kết quả
int ProcessFinish(int pWhoWin);   // Thông báo thắng/thua/hòa
int AskContinue();                // Hỏi có chơi tiếp không
int TestBoard();                  // Kiểm tra thắng/thua/hòa
int CheckBoard(int pX, int pY);   // Đánh dấu 1 ô

// Di chuyển con trỏ (WASD)
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();
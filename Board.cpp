#include <stdio.h>
#include <windows.h>
#include <cctype>
#include<conio.h>
#include "Board.h"
#include "SaveLoad.h"

//Hàm khóa console
void FixConsoleWindow() {
	HWND consolewindow = GetConsoleWindow();
	long style = GetWindowLong(consolewindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consolewindow, GWL_STYLE, style);
}
	//Hàm tạo tọa độ
void GotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


//Khai báo kiểu dữ liệu
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
int _COMMAND;
int _X, _Y;

//Khai báo biến toàn cục
Move moves[BOARD_SIZE * BOARD_SIZE];   // danh sách nước đi
int  moveCount = 0;
const char* PLAYER1_NAME = "Nguoi choi 1";   // X (c = -1)
const char* PLAYER2_NAME = "Nguoi choi 2";   // O (c =  1)

//Hàm reset bàn cờ
void ResetData() {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) {
			_A[i][j].x = LEFT + 4 * j;
			_A[i][j].y = TOP + 2 * i;
			_A[i][j].c = 0;
		}
	_TURN = true;
	_COMMAND = -1;
	_X = _A[0][0].x;
	_Y = _A[0][0].y;
}

//Hàm vẽ bàn cờ
void DrawBoard(int pSize) {
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++) {
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			printf_s(".");
		}
}

//Hàm Control
void StartGame() {
	system("cls");
	ResetData();
	DrawBoard(BOARD_SIZE);
	GotoXY(_X, _Y);
}

//Hàm dọn dẹp
void GarbageCollect()
{
	//Cần biến con trỏ để dùng hàm garbage
}

//Hàm Thoát game
void ExitGame() {
	system("cls");
	GarbageCollect();
	//Cần lưu game nữa
}

//Hàm Thông báo thắng/thua/hòa
int ProcessFinish(int pWhoWin) {
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 2);
	switch (pWhoWin) {
	case -1:
		printf_s("Nguoi choi %d da thang va nguoi choi %d da thua\n", true, false);
		break;
	case 1:
		printf_s("Nguoi choi %d da thang va nguoi choi %d da thua\n", false, true);
		break;
	case 0:
		printf_s("Nguoi choi %d da hoa nguoi choi %d\n", false, true);
		break;
	case 2:
		_TURN = !_TURN;
	}
	GotoXY(_X, _Y);
	return pWhoWin;
}

int AskContinue() {
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 8);  
	printf("Nhan 'y/n' de tiep tuc/dung: ");
	return toupper(_getch());
}

//Hàm Check Win/lose/draw
int TestBoard()
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			int c = _A[i][j].c;
			if (c == 0) continue;
			//Bởi vì index chạy từ trên xuống dưới, 
			//từ trái qua phải nên chỉ cần check bên phải và các hướng ở dưới:
			//check 4 ô bên phải 
			if (j + 4 < BOARD_SIZE
				&& _A[i][j + 1].c == c
				&& _A[i][j + 2].c == c
				&& _A[i][j + 3].c == c
				&& _A[i][j + 4].c == c)
				return c;
			//check 4 ô bên dưới
			if (i + 4 < BOARD_SIZE
				&& _A[i + 1][j].c == c
				&& _A[i + 2][j].c == c
				&& _A[i + 3][j].c == c
				&& _A[i + 4][j].c == c)
				return c;
			//check 4 ô xiên xuống bên phải
			if (i + 4 < BOARD_SIZE
				&& _A[i + 1][j +1].c == c
				&& _A[i + 2][j +2].c == c
				&& _A[i + 3][j +3].c == c
				&& _A[i + 4][j +4].c == c)
				return c;
			//check 4 ô xiên xuống bên trái
			if (i + 4 < BOARD_SIZE
				&& _A[i + 1][j -1].c == c
				&& _A[i + 2][j -2].c == c
				&& _A[i + 3][j -3].c == c
				&& _A[i + 4][j -4].c == c)
				return c;

		}
	}
	//Nếu còn ô trống thì continue
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (_A[i][j].c == 0)
				return 2;
		
	return 0; //Full ô -> Hòa
}

//Hàm đánh dấu 1 ô
int CheckBoard(int pX, int pY) {
	for (int i=0; i < BOARD_SIZE; i++){
		for (int j=0; j < BOARD_SIZE; j++){
			if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
				if (_TURN == true) _A[i][j].c = -1;//turn = true thì c=-1
				else _A[i][j].c = 1; //turn = false thì c=1
				return _A[i][j].c;

			}
		}
}
return 0;
}

//Hàm control (WASD)
void MoveRight() {
	if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x) {
		_X += 4;
		GotoXY(_X, _Y);
	}
}
void MoveLeft() {
	if (_X > _A[0][0].x) {
		_X -= 4;
		GotoXY(_X, _Y);
	}
}
void MoveDown() {
	if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y) {
		_Y += 2;
		GotoXY(_X, _Y);
	}
}
void MoveUp() {
	if (_Y > _A[0][0].y) {
		_Y -= 2;
		GotoXY(_X, _Y);
	}
}


// Ghi lại nước đi vừa đánh (c: -1 = X = người 1, 1 = O = người 2)
void recordMove(int c) {
	Move m;
	m.row = (_Y - TOP) / 2;
	m.col = (_X - LEFT) / 4;
	m.player = (c == -1) ? 1 : 2;
	moves[moveCount++] = m;
}

// Đổi kết quả của TestBoard() sang kết quả của GameRecord
int toRecordResult(int boardResult) {
	if (boardResult == -1) return P1_WIN;
	if (boardResult == 1)  return P2_WIN;
	if (boardResult == 0)  return DRAW;
	return UNFINISHED;
}

// Hỏi lưu (đặt con trỏ xuống dưới bàn cờ cho khỏi đè lên bàn cờ)
void trySave(int result) {
	if (moveCount == 0) return;   // chưa đi nước nào thì khỏi lưu
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 4);
	askSaveOnExit(
        PLAYER1_NAME,
        PLAYER2_NAME,
        result,
        BOARD_SIZE,
        moves,
        moveCount,
        g_openedIndex,
        &g_openedRecord
    );
}




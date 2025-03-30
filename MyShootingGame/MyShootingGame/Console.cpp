#include <windows.h>
#include <iostream>

#include "Console.h"

// 콘솔 화면 좌표 설정과 렌더링에 사용될 버퍼 출력에 관한 함수.

HANDLE  hConsole;

void cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;


	//-------------------------------------------------------------
	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
}

//-------------------------------------------------------------
// 콘솔 화면의 커서를 X, Y 좌표로 이동시킨다.
//
//-------------------------------------------------------------
void cs_MoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	//-------------------------------------------------------------
	// 원하는 위치로 커서를 이동시킨다.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(hConsole, stCoord);
}

//-------------------------------------------------------------
// 콘솔 화면을 조기화 한다.
//
//-------------------------------------------------------------
void cs_ClearScreen(void)
{
	// 실패하면 0
	DWORD dw;

	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 100 * 100, { 0, 0 }, &dw);

}

//--------------------------------------------------------------------
// 버퍼의 내용을 화면으로 찍어주는 함수.
//
// 적군,아군,총알 등을 szScreenBuffer 에 넣어주고, 
// 1 프레임이 끝나는 마지막에 본 함수를 호출하여 버퍼 -> 화면 으로 그린다.
//--------------------------------------------------------------------
void Buffer_Flip(void)
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		printf(szScreenBuffer[iCnt]);
	}
}


//--------------------------------------------------------------------
// 화면 버퍼를 지워주는 함수
//
// 매 프레임 그림을 그리기 직전에 버퍼를 지워 준다. 
// 안그러면 이전 프레임의 잔상이 남으니까
//--------------------------------------------------------------------
void Buffer_Clear(void)
{
	int iCnt;
	memset(szScreenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][dfSCREEN_WIDTH - 1] = '\0';
	}

}

//--------------------------------------------------------------------
// 버퍼의 특정 위치에 원하는 문자를 출력.
//
// 입력 받은 X,Y 좌표에 아스키코드 하나를 출력한다. (버퍼에 그림)
//--------------------------------------------------------------------
void Sprite_Draw(int iX, int iY, char chSprite)
{
	if (iX < 0 || iY < 0 || iX >= dfSCREEN_WIDTH - 1 || iY >= dfSCREEN_HEIGHT)
		return;

	szScreenBuffer[iY][iX] = chSprite;
}

void ChangeNullToEmpty()
{
	int iCnt, jCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		for (jCnt = 0; jCnt < (dfSCREEN_WIDTH - 2); jCnt++)
		{
			if (szScreenBuffer[iCnt][jCnt] == '\0')
				szScreenBuffer[iCnt][jCnt] = ' ';
		}
	}
}

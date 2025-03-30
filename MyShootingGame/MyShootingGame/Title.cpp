#include <iostream>
#include <Windows.h>

#include "Scene.h"
#include "Console.h"

#include "Title.h"

enum MenuInput
{
	m_start, m_exit
};

void UpdateTitle()
{
	Buffer_Clear();

	if ((GetAsyncKeyState(VK_SPACE) & 0x8001) != 0)
	{
		g_Scene = scene_LOAD;
	}
	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scene = scene_EXIT;
	}

	strcpy_s(&szScreenBuffer[0][0], dfSCREEN_WIDTH, "================================================================================");
	strcpy_s(&szScreenBuffer[1][0], dfSCREEN_WIDTH, "                                   My");
	strcpy_s(&szScreenBuffer[2][0], dfSCREEN_WIDTH, "                                Shooting");
	strcpy_s(&szScreenBuffer[3][0], dfSCREEN_WIDTH, "                                  Game");
	strcpy_s(&szScreenBuffer[4][0], dfSCREEN_WIDTH, "                         공격 : A, 이동 : 방향키");
	strcpy_s(&szScreenBuffer[5][0], dfSCREEN_WIDTH, "================================================================================");
	strcpy_s(&szScreenBuffer[6][0], dfSCREEN_WIDTH, " ");
	strcpy_s(&szScreenBuffer[7][0], dfSCREEN_WIDTH, "게임 시작 : SPACE");
	strcpy_s(&szScreenBuffer[8][0], dfSCREEN_WIDTH, "종료 : ESC");
	ChangeNullToEmpty();
	Buffer_Flip();
}
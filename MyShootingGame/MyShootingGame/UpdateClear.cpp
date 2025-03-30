#include "Console.h"
#include "Scene.h"
#include "Load.h"
#include "UpdateClear.h"

void UpdateClear()
{
	Buffer_Clear();

	if ((GetAsyncKeyState(VK_RETURN) & 0x8001) != 0)
	{
		g_NowStage = 1;
		g_Scene = scene_TITLE;
	}
	else if ((GetAsyncKeyState(VK_ESCAPE) & 0x8001) != 0)
	{
		g_Scene = scene_EXIT;
	}

	strcpy_s(&szScreenBuffer[0][0], dfSCREEN_WIDTH, "================================================================================");
	strcpy_s(&szScreenBuffer[1][0], dfSCREEN_WIDTH, "                                   Congrats!");
	strcpy_s(&szScreenBuffer[2][0], dfSCREEN_WIDTH, "");
	strcpy_s(&szScreenBuffer[3][0], dfSCREEN_WIDTH, "                             You cleared all stages..!");
	strcpy_s(&szScreenBuffer[4][0], dfSCREEN_WIDTH, "");
	strcpy_s(&szScreenBuffer[5][0], dfSCREEN_WIDTH, "================================================================================");
	strcpy_s(&szScreenBuffer[6][0], dfSCREEN_WIDTH, " ");
	strcpy_s(&szScreenBuffer[7][0], dfSCREEN_WIDTH, "처음으로 : ENTER");
	strcpy_s(&szScreenBuffer[8][0], dfSCREEN_WIDTH, "종료 : ESC");
	ChangeNullToEmpty();
	Buffer_Flip();

}

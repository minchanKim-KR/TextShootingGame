// MyShootingGame.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <Windows.h>
#pragma comment(lib,"Winmm.lib")

#include "Console.h"
#include "Scene.h"
#include "MyProfiler.h"

#include "Title.h"
#include "Load.h"
#include "Game.h"
#include "UpdateClear.h"
#include "UpdateOver.h"

int g_Scene;

char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

//FPS
DWORD old_for_1s;
DWORD old;
DWORD deltaTime;
int fps;
int fps_for_print;

int main()
{
	timeBeginPeriod(1);

	int t;
	cs_Initial();
	g_NowStage = 1;
	g_Scene = scene_TITLE;

	while (g_Scene != scene_EXIT)
	{
		
		switch (g_Scene)
		{
		case scene_TITLE:
			UpdateTitle();
			break;

		case scene_LOAD:
			if (!loadGameData(g_NowStage))
			{
				if (g_NowStage > g_TotalStage)
					g_Scene = scene_CLEAR;
				break;		
			}

			fps_for_print = 0;
			fps = 0;
			old_for_1s = timeGetTime();
			old = timeGetTime();
			g_Scene = scene_GAME;
			break;

		case scene_GAME:
			// 로직
			UpdateGame();

			//FPS
			fps++;
			if (timeGetTime() - old_for_1s >= 1000)
			{
				fps_for_print = fps;
				fps = 0;
				old_for_1s += 1000;
			}
			
			//Frame Skip
			deltaTime = timeGetTime() - old;

			t = 20 - deltaTime;
			if (t <= -20)
			{
				old += 20;
				continue;
			}
			Render();

			// 50 fps 고정
			deltaTime = timeGetTime() - old;
			t = 20 - deltaTime;
			if (t > 0)
				Sleep(t);
			old += 20;

			break;

		case scene_CLEAR:
			UpdateClear();
			break;
		case scene_OVER:
			UpdateOver();
			break;
		case scene_EXIT:
			break;
		}
		
	}

	timeEndPeriod(1);
	MyProfiler::ProfileDataOutText("C:\\Users\\minchan\\Desktop\\ShtGameProfiler.txt");
}
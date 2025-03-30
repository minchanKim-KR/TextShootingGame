#include <iostream>
#include "Console.h"
#include "Scene.h"
#include "Load.h"
#include "MyProfiler.h"

#include "Game.h"


void UpdateGame()
{
	PRO_BEGIN("UpdateGame");

	// 입력 : 유저의 이동 및 공격
	bool player_attack = false;
	Pos player_move = { 0,0 };

	// loop를 위한 변수
	int i;
	int b_chk;

	if ((GetAsyncKeyState('A') & 0x8001) != 0)
		player_attack = true;
	if ((GetAsyncKeyState(VK_UP) & 0x8001) != 0)
		player_move._y = -1;
	if ((GetAsyncKeyState(VK_DOWN) & 0x8001) != 0)
		player_move._y = 1;
	if ((GetAsyncKeyState(VK_LEFT) & 0x8001) != 0)
		player_move._x = -1;
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8001) != 0)
		player_move._x = 1;

	// 로직

	///////////////////////////////////////////////////
	// 1. 몬스터 위치 업데이트
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		///////////////////////////////////////////////////
		// 쿨타임, 넘버가 음수인경우 동작하지 않는다.
		///////////////////////////////////////////////////

		if (g_MonstersInStage[i]._monster_no > -1)
		{
			if (g_MonstersInStage[i]._mov_cool > 0)
			{
				g_MonstersInStage[i]._mov_cool--;
				continue;
			}
			else if(g_MonstersInStage[i]._mov_cool == 0)
			{
				int next_move_index = g_MonstersInStage[i]._next_move_Index;
				Pos now_pos = g_MonstersInStage[i]._now_pos;
				now_pos._y += g_MonstersInStage[i]._move_pattern[next_move_index]._y;
				now_pos._x += g_MonstersInStage[i]._move_pattern[next_move_index]._x;

				// 몬스터는 게임화면의 최하단으로 이동 할 수 없다.
				if (now_pos._y > -1 && now_pos._y < MAP_HEIGHT - 1 &&
					now_pos._x > -1 && now_pos._x < MAP_WIDTH)
				{
					g_MonstersInStage[i]._now_pos = now_pos;
				}

				// 다음에 이동할 좌표 인덱스 업데이트.
				g_MonstersInStage[i]._next_move_Index++;
				g_MonstersInStage[i]._next_move_Index =
					g_MonstersInStage[i]._next_move_Index % g_MonstersInStage[i]._num_pattern;

				// 쿨타임 적용
				g_MonstersInStage[i]._mov_cool = g_MonstersInStage[i]._movement_interval;
			}
		}
	}

	///////////////////////////////////////////////////
	// 2. 몬스터 공격 업데이트
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		///////////////////////////////////////////////////
		// 쿨타임, 넘버가 음수인경우 동작하지 않는다.
		///////////////////////////////////////////////////

		if (g_MonstersInStage[i]._monster_no > -1)
		{
			if (g_MonstersInStage[i]._att_cool > 0)
			{
				g_MonstersInStage[i]._att_cool--;
				continue;
			}
			else if(g_MonstersInStage[i]._att_cool == 0)
			{
				for (b_chk = 0; b_chk < MAX_MONSTER_BULLET; b_chk++)
				{
					if (g_monster_bullet[b_chk]._is_valid == false)
					{
						g_monster_bullet[b_chk]._is_valid = true;
						g_monster_bullet[b_chk]._damage = g_MonstersInStage[i]._damage;
						g_monster_bullet[b_chk]._pos._y = g_MonstersInStage[i]._now_pos._y;
						g_monster_bullet[b_chk]._pos._x = g_MonstersInStage[i]._now_pos._x;
						
						// 쿨타임 적용
						g_MonstersInStage[i]._att_cool = g_MonstersInStage[i]._attack_interval;
						break;
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////
	// 3. player의 위치 업데이트
	///////////////////////////////////////////////////

	player_move._y += g_p._now_pos._y;
	player_move._x += g_p._now_pos._x;

	if (player_move._y > -1 && player_move._y < MAP_HEIGHT &&
		player_move._x > -1 && player_move._x < MAP_WIDTH)
	{
		g_p._now_pos._y = player_move._y;
		g_p._now_pos._x = player_move._x;
	}

	///////////////////////////////////////////////////
	// 4. player의 공격 업데이트
	///////////////////////////////////////////////////

	if (player_attack)
	{
		for (i = 0; i < MAX_PLAYER_BULLET; i++)
		{
			if (g_player_bullet[i]._is_valid == false)
			{
				g_player_bullet[i]._is_valid = true;
				g_player_bullet[i]._pos._y = g_p._now_pos._y;
				g_player_bullet[i]._pos._x = g_p._now_pos._x;
				g_player_bullet[i]._damage = g_p._damage;
				break;
			}
		}
	}

	///////////////////////////////////////////////////
	// 4. 화면상의 총알 위치 업데이트
	///////////////////////////////////////////////////
	
	// 몬스터
	for (i = 0; i < MAX_MONSTER_BULLET; i++)
	{
		if (g_monster_bullet[i]._is_valid == true)
		{
			if ((g_monster_bullet[i]._pos._y + 1) >= MAP_HEIGHT)
			{
				g_monster_bullet[i]._is_valid = false;
			}
			else
				g_monster_bullet[i]._pos._y += 1;
		}
	}

	// 플레이어
	for (i = 0; i < MAX_PLAYER_BULLET; i++)
	{
		if (g_player_bullet[i]._is_valid == true)
		{
			if (g_player_bullet[i]._pos._y - 1 < 0)
				g_player_bullet[i]._is_valid = false;
			else
				g_player_bullet[i]._pos._y -= 1;
		}
	}

	///////////////////////////////////////////////////
	// 5. 피격판정
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		// 유효한 적
		if (g_MonstersInStage[i]._monster_no > -1)
		{
			for (b_chk = 0; b_chk < MAX_PLAYER_BULLET; b_chk++)
			{
				// 유효, 적이 탄에 맞은 경우.
				if (g_player_bullet[b_chk]._is_valid && 
					(g_player_bullet[b_chk]._pos._y == g_MonstersInStage[i]._now_pos._y) &&
					(g_player_bullet[b_chk]._pos._x == g_MonstersInStage[i]._now_pos._x))
				{
					g_MonstersInStage[i]._hp -= g_player_bullet[b_chk]._damage;
					g_player_bullet[b_chk]._is_valid = false;

					if (g_MonstersInStage[i]._hp <= 0)
					{
						// 적 체력이 0인경우 필드에서 지우기
						g_MonstersInStage[i]._monster_no = -1;
						g_TotalMonsterOnField--;
					}
				}
			}
		}
	}

	for (b_chk = 0; b_chk < MAX_MONSTER_BULLET; b_chk++)
	{
		if (g_monster_bullet[b_chk]._is_valid &&
			(g_monster_bullet[b_chk]._pos._y == g_p._now_pos._y) &&
			(g_monster_bullet[b_chk]._pos._x == g_p._now_pos._x))
		{
			g_p._hp -= g_monster_bullet[b_chk]._damage;
			g_monster_bullet[b_chk]._is_valid = false;

			if (g_MonstersInStage[i]._hp <= 0)
				g_is_player_dead = true;
		}

	}

	///////////////////////////////////////////////////
	// 스테이지 클리어, 유저 사망 처리
	///////////////////////////////////////////////////

	if (g_TotalMonsterOnField <= 0)
	{
		g_NowStage++;
		g_Scene = scene_LOAD;
	}
	else if (g_p._hp <= 0)
		g_Scene = scene_OVER;
}

void Render()
{
	PRO_BEGIN("Render");

	Buffer_Clear();
	int i;

	// 버퍼에 그리기
	// 버퍼 sz로 만들기
	for (i = 0; i < dfSCREEN_HEIGHT; i++)
	{
		szScreenBuffer[i][dfSCREEN_WIDTH - 1] = '\0';
	}
	// 몬스터
	for (i = 0; i < MAX_MONSTER; i++)
	{
		Pos now_pos;
		if (g_MonstersInStage[i]._monster_no > -1)
		{
			now_pos = g_MonstersInStage[i]._now_pos;
			szScreenBuffer[now_pos._y][now_pos._x] = g_MonstersInStage[i]._icon;
		}
	}

	// 몬스터 총알
	for (i = 0; i < MAX_MONSTER_BULLET; i++)
	{
		if (g_monster_bullet[i]._is_valid)
			szScreenBuffer[g_monster_bullet[i]._pos._y][g_monster_bullet[i]._pos._x] = '0';
	}
	// 플레이어
	szScreenBuffer[g_p._now_pos._y][g_p._now_pos._x] = g_p._icon;

	// 플레이어 총알
	for (i = 0; i < MAX_PLAYER_BULLET; i++)
	{
		if (g_player_bullet[i]._is_valid)
			szScreenBuffer[g_player_bullet[i]._pos._y][g_player_bullet[i]._pos._x] = '^';
	}

	// HACK : 일단 눈에 잘들어오게 짬
	char fps_ascii[10];
	char hp_ascii[10];
	char stage_ascii[10];

	_itoa_s(fps_for_print, fps_ascii, 10, 10);
	_itoa_s(g_p._hp, hp_ascii, 10, 10);
	_itoa_s(g_NowStage, stage_ascii, 10, 10);

	strcpy_s(&szScreenBuffer[MAP_HEIGHT][0], dfSCREEN_WIDTH, "FPS : ");
	strcat_s(&szScreenBuffer[MAP_HEIGHT][0], dfSCREEN_WIDTH, fps_ascii);
	for (i = 0; i < dfSCREEN_WIDTH - 1; i++)
	{
		if (szScreenBuffer[MAP_HEIGHT][i] == '\0')
			szScreenBuffer[MAP_HEIGHT][i] = ' ';
	}

	strcpy_s(&szScreenBuffer[MAP_HEIGHT + 1][0], dfSCREEN_WIDTH, "HP : ");
	strcat_s(&szScreenBuffer[MAP_HEIGHT + 1][0], dfSCREEN_WIDTH, hp_ascii);
	for (i = 0; i < dfSCREEN_WIDTH - 1; i++)
	{
		if (szScreenBuffer[MAP_HEIGHT + 1][i] == '\0')
			szScreenBuffer[MAP_HEIGHT + 1][i] = ' ';
	}

	strcpy_s(&szScreenBuffer[MAP_HEIGHT + 2][0], dfSCREEN_WIDTH, "STAGE ");
	strcat_s(&szScreenBuffer[MAP_HEIGHT + 2][0], dfSCREEN_WIDTH, stage_ascii);
	for (i = 0; i < dfSCREEN_WIDTH - 1; i++)
	{
		if (szScreenBuffer[MAP_HEIGHT][i] == '\0')
			szScreenBuffer[MAP_HEIGHT][i] = ' ';
	}

	// 렌더링
	Buffer_Flip();
}
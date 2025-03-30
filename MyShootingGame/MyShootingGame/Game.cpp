#include <iostream>
#include "Console.h"
#include "Scene.h"
#include "Load.h"
#include "MyProfiler.h"

#include "Game.h"


void UpdateGame()
{
	PRO_BEGIN("UpdateGame");

	// �Է� : ������ �̵� �� ����
	bool player_attack = false;
	Pos player_move = { 0,0 };

	// loop�� ���� ����
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

	// ����

	///////////////////////////////////////////////////
	// 1. ���� ��ġ ������Ʈ
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		///////////////////////////////////////////////////
		// ��Ÿ��, �ѹ��� �����ΰ�� �������� �ʴ´�.
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

				// ���ʹ� ����ȭ���� ���ϴ����� �̵� �� �� ����.
				if (now_pos._y > -1 && now_pos._y < MAP_HEIGHT - 1 &&
					now_pos._x > -1 && now_pos._x < MAP_WIDTH)
				{
					g_MonstersInStage[i]._now_pos = now_pos;
				}

				// ������ �̵��� ��ǥ �ε��� ������Ʈ.
				g_MonstersInStage[i]._next_move_Index++;
				g_MonstersInStage[i]._next_move_Index =
					g_MonstersInStage[i]._next_move_Index % g_MonstersInStage[i]._num_pattern;

				// ��Ÿ�� ����
				g_MonstersInStage[i]._mov_cool = g_MonstersInStage[i]._movement_interval;
			}
		}
	}

	///////////////////////////////////////////////////
	// 2. ���� ���� ������Ʈ
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		///////////////////////////////////////////////////
		// ��Ÿ��, �ѹ��� �����ΰ�� �������� �ʴ´�.
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
						
						// ��Ÿ�� ����
						g_MonstersInStage[i]._att_cool = g_MonstersInStage[i]._attack_interval;
						break;
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////
	// 3. player�� ��ġ ������Ʈ
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
	// 4. player�� ���� ������Ʈ
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
	// 4. ȭ����� �Ѿ� ��ġ ������Ʈ
	///////////////////////////////////////////////////
	
	// ����
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

	// �÷��̾�
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
	// 5. �ǰ�����
	///////////////////////////////////////////////////

	for (i = 0; i < MAX_MONSTER; i++)
	{
		// ��ȿ�� ��
		if (g_MonstersInStage[i]._monster_no > -1)
		{
			for (b_chk = 0; b_chk < MAX_PLAYER_BULLET; b_chk++)
			{
				// ��ȿ, ���� ź�� ���� ���.
				if (g_player_bullet[b_chk]._is_valid && 
					(g_player_bullet[b_chk]._pos._y == g_MonstersInStage[i]._now_pos._y) &&
					(g_player_bullet[b_chk]._pos._x == g_MonstersInStage[i]._now_pos._x))
				{
					g_MonstersInStage[i]._hp -= g_player_bullet[b_chk]._damage;
					g_player_bullet[b_chk]._is_valid = false;

					if (g_MonstersInStage[i]._hp <= 0)
					{
						// �� ü���� 0�ΰ�� �ʵ忡�� �����
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
	// �������� Ŭ����, ���� ��� ó��
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

	// ���ۿ� �׸���
	// ���� sz�� �����
	for (i = 0; i < dfSCREEN_HEIGHT; i++)
	{
		szScreenBuffer[i][dfSCREEN_WIDTH - 1] = '\0';
	}
	// ����
	for (i = 0; i < MAX_MONSTER; i++)
	{
		Pos now_pos;
		if (g_MonstersInStage[i]._monster_no > -1)
		{
			now_pos = g_MonstersInStage[i]._now_pos;
			szScreenBuffer[now_pos._y][now_pos._x] = g_MonstersInStage[i]._icon;
		}
	}

	// ���� �Ѿ�
	for (i = 0; i < MAX_MONSTER_BULLET; i++)
	{
		if (g_monster_bullet[i]._is_valid)
			szScreenBuffer[g_monster_bullet[i]._pos._y][g_monster_bullet[i]._pos._x] = '0';
	}
	// �÷��̾�
	szScreenBuffer[g_p._now_pos._y][g_p._now_pos._x] = g_p._icon;

	// �÷��̾� �Ѿ�
	for (i = 0; i < MAX_PLAYER_BULLET; i++)
	{
		if (g_player_bullet[i]._is_valid)
			szScreenBuffer[g_player_bullet[i]._pos._y][g_player_bullet[i]._pos._x] = '^';
	}

	// HACK : �ϴ� ���� �ߵ����� «
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

	// ������
	Buffer_Flip();
}
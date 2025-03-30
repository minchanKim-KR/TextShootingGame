#include "Console.h"
#include "MyParser.h"
#include "Load.h"
#include "MyProfiler.h"

int g_TotalStage;
int g_TotalMonster;

Monster g_MonsterIndex[MONSTER_INDEX_SIZE];
int g_NowStage;

// player 상태
bool g_is_player_dead = false;

int g_TotalMonsterOnField;
Player g_p;
Monster g_MonstersInStage[MAX_MONSTER];

// 화면상 탄환
// 화면 전환시 invalid
Bullet g_monster_bullet[MAX_MONSTER_BULLET];
Bullet g_player_bullet[MAX_PLAYER_BULLET];

//--------------------------------------------------------------------
// Monster.txt, gameinfo.txt
//--------------------------------------------------------------------
bool loadGameInfo()
{
	// 프로파일러
	PRO_BEGIN("LoadGameInfo");

	const int key_size = 100;
	const int val_size = 100;

	// gameinfo.txt
	MyParser* parser = new MyParser;
	parser->loadFile("GameInfo\\gameinfo.txt");

	//--------------------------------------------------------------------
	// 충분히 큰 크기의 버퍼가 필요.
	//--------------------------------------------------------------------
	char* val = new char[val_size];
	char* key = new char[key_size];
	int* mov_pattern = new int[val_size];
	int num_item;
	int num_pattern;

	if (!parser->getValue(val, "GAMEINFO:TotalStage"))
		return false;
	g_TotalStage = atoi(val);

	if (!parser->getValue(val, "GAMEINFO:TotalMonster"))
		return false;
	g_TotalMonster = atoi(val);

	// monster.txt
	parser->loadFile("GameInfo\\Monster.txt");
	for (int i = 0; i < g_TotalMonster; i++)
	{
		// MonsterIndex와 no는 일치
		sprintf_s(key,key_size,"MONSTER%d:monster_no",i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._monster_no = atoi(val);

		sprintf_s(key, key_size, "MONSTER%d:hp", i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._hp = atoi(val);

		sprintf_s(key, key_size, "MONSTER%d:damage", i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._damage = atoi(val);

		sprintf_s(key, key_size, "MONSTER%d:attack_interval", i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._attack_interval = atoi(val);

		sprintf_s(key, key_size, "MONSTER%d:movement_interval", i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._movement_interval = atoi(val);

		sprintf_s(key, key_size, "MONSTER%d:move_pattern", i);
		if (!parser->getValue(val, key))
			return false;
		
		// 읽은 값의 개수
		num_item = 0;

		// val 순회용 포인터
		char* i_ptr = val;

		// 좌표이동 : 1, -1만 인식
		while (*i_ptr != '\0')
		{
			if (*i_ptr == '1')
			{
				mov_pattern[num_item] = 1;
				num_item++;
			}
			else if (*i_ptr == '-' && *(i_ptr + 1) == '1')
			{
				mov_pattern[num_item] = -1;
				num_item++;
				i_ptr++;
			}
			i_ptr++;
		}
		// 좌표 항목의 합이 짝수개가 아니면 맨 뒤 하나 자름.
		if (num_item > 0)
		{
			num_pattern = num_item / 2;
			g_MonsterIndex[i]._num_pattern = num_pattern;
			g_MonsterIndex[i]._move_pattern = new Pos[num_pattern];
			for (int j = 0; j < num_pattern; j++)
			{
				g_MonsterIndex[i]._move_pattern[j]._y = mov_pattern[2 * j];
				g_MonsterIndex[i]._move_pattern[j]._x = mov_pattern[2 * j + 1];
			}
		}
		else
		{
			g_MonsterIndex[i]._num_pattern = 0;
			g_MonsterIndex[i]._move_pattern = nullptr;
		}

		sprintf_s(key, key_size, "MONSTER%d:icon", i);
		if (!parser->getValue(val, key))
			return false;
		g_MonsterIndex[i]._icon = val[0];
	}

	delete[] val;
	delete[] key;
	delete[] mov_pattern;
	delete parser;

	return true;
}

bool loadGameScene(int sceneNo)
{
	PRO_BEGIN("LoadGameScene");
	// 씬의 몬스터들을 초기화해준다.
	// 
	// stage.txt
	const int key_size = 100;
	const int val_size = 100;

	for (int i = 0; i < MAX_MONSTER; i++)
	{
		g_MonstersInStage[i]._monster_no = -1;
	}

	StageParser* stage_parser = new StageParser;
	MyParser* player_parser = new MyParser;
	char* key = new char[key_size];
	char* val = new char[val_size];

	sprintf_s(key, key_size, "Stage\\Stage%d.txt", sceneNo);

	if (!stage_parser->loadStage(key))
		return false;

	g_TotalMonsterOnField = stage_parser->_loaded_monsters;
	g_is_player_dead = false;

	// player.txt
	player_parser->loadFile("GameInfo\\Player.txt");

	if (!player_parser->getValue(val, "Player:hp"))
		return false;
	g_p._hp = atoi(val);

	if (!player_parser->getValue(val, "Player:damage"))
		return false;
	g_p._damage = atoi(val);

	if (!player_parser->getValue(val, "Player:icon"))
		return false;
	g_p._icon = val[0];

	g_p._now_pos._y = MAP_HEIGHT - 1;
	g_p._now_pos._x = (MAP_WIDTH - 1) / 2;

	// 씬의 총알을 초기화해준다.
	int i;
	for (i = 0; i < MAX_MONSTER_BULLET; i++)
	{
		g_monster_bullet[i]._is_valid = false;
	}
	for (i = 0; i < MAX_PLAYER_BULLET; i++)
	{
		g_player_bullet[i]._is_valid = false;
	}

	delete stage_parser;
	delete player_parser;
	delete[] key;
	delete[] val;
	return true;
}

bool loadGameData(int loadScene)
{
	static bool isFirst = true;
	Buffer_Clear();

	strcpy_s(szScreenBuffer[0], dfSCREEN_WIDTH, "Now Loading..");
	strcpy_s(szScreenBuffer[1], dfSCREEN_WIDTH, "Now Loading..");
	strcpy_s(szScreenBuffer[2], dfSCREEN_WIDTH, "Now Loading..");
	ChangeNullToEmpty();
	Buffer_Flip();

	// 현재 로드된 스테이지

	if (isFirst == true)
	{
		if (loadGameInfo())
			isFirst = false;
		else
			return false;
	}

	if(loadScene > 0 && loadScene <= g_TotalStage)
	{
		if (loadGameScene(loadScene))
			return true;
	}
	
	return false;
}
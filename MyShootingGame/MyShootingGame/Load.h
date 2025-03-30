#pragma once

// 게임 화면 크기
#define MAP_WIDTH 80
#define MAP_HEIGHT 24

// 도감에 등록 가능한 종류 수, A~Z, a~z
#define MONSTER_INDEX_SIZE 52

// 스테이지 당 최대 몬스터 수
#define MAX_MONSTER 50

// 화면상 최대 탄환
#define MAX_MONSTER_BULLET 100
#define MAX_PLAYER_BULLET 30

struct Pos
{
	int _y;
	int _x;
};
struct Monster
{
	// 로드가 필요한 데이터
	int _monster_no; // 음수면 존재x
	int _hp;
	int _damage;
	int _attack_interval;
	int _movement_interval;

	// 이동 패턴
	int _num_pattern;
	Pos* _move_pattern;
	char _icon;

	// 로직 내부적으로 사용
	int _att_cool;
	int _mov_cool;
	int _next_move_Index;
	Pos _now_pos;
};

struct Player
{
	int _hp;
	int _damage;
	char _icon;

	// 로직 내부적으로 사용
	Pos _now_pos;
};

struct Bullet
{
	bool _is_valid;
	Pos _pos;
	int _damage;
};

extern int g_TotalStage;
extern int g_TotalMonster;

extern Monster g_MonsterIndex[MONSTER_INDEX_SIZE];
extern int g_NowStage;

extern bool g_is_player_dead;
extern int g_TotalMonsterOnField;
extern Player g_p;
extern Monster g_MonstersInStage[MAX_MONSTER];

extern Bullet g_monster_bullet[MAX_MONSTER_BULLET];
extern Bullet g_player_bullet[MAX_PLAYER_BULLET];

bool loadGameData(int loadScene);
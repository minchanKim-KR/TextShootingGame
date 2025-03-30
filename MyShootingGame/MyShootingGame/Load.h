#pragma once

// ���� ȭ�� ũ��
#define MAP_WIDTH 80
#define MAP_HEIGHT 24

// ������ ��� ������ ���� ��, A~Z, a~z
#define MONSTER_INDEX_SIZE 52

// �������� �� �ִ� ���� ��
#define MAX_MONSTER 50

// ȭ��� �ִ� źȯ
#define MAX_MONSTER_BULLET 100
#define MAX_PLAYER_BULLET 30

struct Pos
{
	int _y;
	int _x;
};
struct Monster
{
	// �ε尡 �ʿ��� ������
	int _monster_no; // ������ ����x
	int _hp;
	int _damage;
	int _attack_interval;
	int _movement_interval;

	// �̵� ����
	int _num_pattern;
	Pos* _move_pattern;
	char _icon;

	// ���� ���������� ���
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

	// ���� ���������� ���
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
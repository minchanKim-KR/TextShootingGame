#pragma once
#include <iostream>
#include <ctime>

#include "Load.h"

using namespace std;

class MyParser
{
    //private
private:
    //--------------------------------------------------------------------
    // ���ڿ��� �հ� ���� ������ ����
    //--------------------------------------------------------------------
    bool trim(char* dst, const char* src);

    //--------------------------------------------------------------------
    // ���ڿ��� c�� �������� dst1, dst2�� ����ش�.
    //--------------------------------------------------------------------
    bool splitInHalf(char* dst1, int dst1_leng, char* dst2, int dst2_leng, const char* src, char c);

    //--------------------------------------------------------------------
    // console�� log.txt�� log�� �����ش�.
    //--------------------------------------------------------------------
    void WriteLog(const char* msg, const char* state = nullptr);

public:
    MyParser() :
        _f(nullptr),
        _buffer(nullptr),
        _buf_size(0),
        _sz_key_head(nullptr),
        _sz_key_tail(nullptr),
        _si(0),
        _di(0)
    {}

    ~MyParser()
    {
        if (_f != nullptr)
            fclose(_f);

        delete[] _buffer;
        delete[] _sz_key_head;
        delete[] _sz_key_tail;
    }

    //--------------------------------------------------------------------
    // file�� �ε��Ѵ�
    //--------------------------------------------------------------------
    bool loadFile(const char* path);

    //--------------------------------------------------------------------
    // key�� ���� ���� val�� �־��ش�.  ex) MONSTER0:monster_no
    // �� �׸��� 0x0d�� �������� �ν��ϹǷ�, �ּ����� �ּ��� �� ��.
    //--------------------------------------------------------------------
    // HACK : key�� �պκи� ��ġ�ص� ���� ã���ֱ���.
    //--------------------------------------------------------------------
    bool getValue(char* val, const char* key);

private:
    FILE* _f;
    char* _buffer;
    unsigned int _buf_size;

    char* _sz_key_head;
    char* _sz_key_tail;
    int _si;
    int _di;
};

class StageParser
{
private:

    //--------------------------------------------------------------------
    // �ΰ��ӿ� ��ġ�� ��ǥ (y,x), ��ġ�� ���� g_MonsterIndex[index]
    //--------------------------------------------------------------------
    bool loadMonster(int y, int x, int index);
    //--------------------------------------------------------------------
    // g_MonsterIndex���� ���͸� ã���ش�.
    //--------------------------------------------------------------------
    int lookUpInMonsterIndex(char c);
    void WriteLog(const char* msg, const char* state = nullptr);
public:
    //--------------------------------------------------------------------
    // MONSTERS + 0x0a + 0x00
    //--------------------------------------------------------------------
    StageParser() : _f(nullptr), _buf_size(MAP_WIDTH + 2), _loaded_monsters(0)
    {
        _buffer = new char[MAP_WIDTH + 2];
    }
    ~StageParser()
    {
        if (_f != nullptr)
            fclose(_f);

        delete[] _buffer;
    }
    bool loadStage(const char* path);
public:
    int _loaded_monsters;
private:
    FILE* _f;
    char* _buffer;
    unsigned int _buf_size;
};
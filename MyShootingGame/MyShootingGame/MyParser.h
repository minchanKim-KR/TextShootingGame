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
    // 문자열의 앞과 뒤의 공백을 제거
    //--------------------------------------------------------------------
    bool trim(char* dst, const char* src);

    //--------------------------------------------------------------------
    // 문자열을 c를 기준으로 dst1, dst2에 담아준다.
    //--------------------------------------------------------------------
    bool splitInHalf(char* dst1, int dst1_leng, char* dst2, int dst2_leng, const char* src, char c);

    //--------------------------------------------------------------------
    // console과 log.txt에 log를 남겨준다.
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
    // file을 로드한다
    //--------------------------------------------------------------------
    bool loadFile(const char* path);

    //--------------------------------------------------------------------
    // key에 대한 값을 val에 넣어준다.  ex) MONSTER0:monster_no
    // 각 항목은 0x0d를 기준으로 인식하므로, 주석줄은 주석만 쓸 것.
    //--------------------------------------------------------------------
    // HACK : key가 앞부분만 일치해도 값을 찾아주긴함.
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
    // 인게임에 배치될 좌표 (y,x), 배치될 몬스터 g_MonsterIndex[index]
    //--------------------------------------------------------------------
    bool loadMonster(int y, int x, int index);
    //--------------------------------------------------------------------
    // g_MonsterIndex에서 몬스터를 찾아준다.
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
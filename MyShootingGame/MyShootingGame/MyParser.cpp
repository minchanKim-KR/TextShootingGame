#include "MyParser.h"

bool MyParser::trim(char* dst, const char* src)
{
    if (src == nullptr)
        return false;

    int si = 0;
    int di = strlen(src) - 1; // ex) ABCDE\0 => 5 - 1 => INDEX 4
    int len = strlen(src);

    // 스페이스	    0x20
    // 백스페이스    0x08
    // 탭           0x09
    // 라인 피드	    0x0a
    // 캐리지 리턴   0x0d

    //--------------------------------------------------------------------
    // 문자열의 앞 공백 제거
    //--------------------------------------------------------------------
    for (int i = 0; i < len; i++)
    {
        if (src[i] != 0x20 &&
            src[i] != 0x09 &&
            src[i] != 0x08 &&
            src[i] != 0x0a &&
            src[i] != 0x0d)
        {
            si = i;
            break;
        }
    }
    //--------------------------------------------------------------------
    // 문자열 뒤 공백 제거
    //--------------------------------------------------------------------
    for (int i = 0; i < len; i++)
    {
        if (src[di - i] != 0x20 &&
            src[di - i] != 0x09 &&
            src[di - i] != 0x08 &&
            src[di - i] != 0x0a &&
            src[di - i] != 0x0d)
        {
            di = di - i;
            break;
        }
    }
    if (si <= di)
    {
        int i;
        for (i = 0; si <= di; i++, si++)
        {
            dst[i] = src[si];
        }
        dst[i] = '\0';
    }
    else
    {
        // 잘못된 문자열 (ex. 공백으로만 이루어진 문자열)
        return false;
    }

    return true;
}

bool MyParser::splitInHalf(char* dst1, int dst1_leng, char* dst2, int dst2_leng, const char* src, char c)
{
    int key_leng = strlen(src);

    if (key_leng < 3 || src[0] == c || src[key_leng - 1] == c)
        return false;
    else
    {
        int i;
        for (i = 1; i < key_leng; i++)
        {
            if (src[i] == c)
            {
                char* temp = new char[key_leng + 1];
                strcpy_s(temp, key_leng + 1, src);
                temp[i] = '\0';

                //HACK : 가독성을 위해 크기를 딱 맞추진 않음
                strcpy_s(dst1, dst1_leng, temp);
                strcpy_s(dst2, dst2_leng, temp + i + 1);
                delete[] temp;

                break;
            }
        }

        if (i == key_leng)
            return false;
        else
            return true;
    }
}

void MyParser::WriteLog(const char* msg, const char* state)
{
    time_t timer = time(NULL);
    tm t;
    localtime_s(&t, &timer);


    FILE* f;
    fopen_s(&f, "Log.txt", "at");

    // CONSOLE LOG
    printf("////////////////////////////////////////////////\n");
    printf("%d / %d / %d / %d:%d:%d \n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    printf("MSG : %s\n", msg);
    if (state != nullptr)
    {
        printf("TARGET : %s\n", state);
    }

    // FILE LOG
    if (f != nullptr)
    {
        fputs("////////////////////////////////////////////////\n", f);
        fprintf_s(f, "%d / %d / %d / %d:%d:%d \n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        fprintf_s(f, "MSG : %s\n", msg);

        if (state != nullptr)
        {
            fprintf_s(f, "TARGET : %s\n", state);
        }

        fclose(f);
    }
}

bool MyParser::loadFile(const char* path)
{
    // fopen_s : If successful, returns zero
    if (path == nullptr)
    {
        WriteLog("path가 NULL입니다.");
        return false;
    }

    if (_f != nullptr)
        fclose(_f);

    if (!fopen_s(&_f, path, "rb"))
    {
        fseek(_f, 0, SEEK_END);
        _buf_size = ftell(_f);
        _buffer = new char[_buf_size];
        fseek(_f, 0, SEEK_SET);
        fread(_buffer, _buf_size, 1, _f);

        return true;
    }
    else
    {
        // 파일을 열지 못함
        WriteLog("파일을 열지 못하였습니다.", path);
        return false;
    }
}

bool MyParser::getValue(char* val, const char* key)
{
    // ex) MONSTER0:monster_no

    if (key == nullptr)
    {
        WriteLog("key가 NULLPTR 입니다.");
        return false;
    }

    //--------------------------------------------------------------------
    // ex) :로 이름을 분할해, head와 tail에 저장
    //--------------------------------------------------------------------

    if (_sz_key_head != nullptr)
        delete[] _sz_key_head;
    if (_sz_key_tail != nullptr)
        delete[] _sz_key_tail;

    int key_len = strlen(key);
    _sz_key_head = new char[key_len + 1];
    _sz_key_tail = new char[key_len + 1];

    if (splitInHalf(_sz_key_head, key_len + 1, _sz_key_tail, key_len + 1, key, ':') == false)
    {
        // key의 이름이 잘못됨
        WriteLog("key의 이름이 잘못됨.", key);
        return false;
    }


    unsigned int i;
    bool is_annotation = false;
    for (i = 0; i < _buf_size; i++)
    {
        // 주석 건너뛰기 : //... 0d0a
        if (is_annotation && _buffer[i] == 0x0a)
        {
            is_annotation = false;
            continue;
        }
        else if (is_annotation)
            continue;
        else if (_buffer[i] == '/')
        {
            is_annotation = true;
            continue;
        }

        //--------------------------------------------------------------------
        // Key를 찾은 경우
        // _si,_di를 괄호 안쪽 범위로 잡는다.
        //--------------------------------------------------------------------
        if (_buffer[i] == ':' &&
            i + strlen(_sz_key_head) < _buf_size &&
            strncmp(_sz_key_head, &_buffer[i + 1], strlen(_sz_key_head)) == 0)
        {
            i += (strlen(_sz_key_head) + 1);

            while (i < _buf_size && _buffer[i] != '{')
            {
                i++;
            }
            _si = i + 1;
            while (i < _buf_size && _buffer[i] != '}')
            {
                i++;
            }
            _di = i - 1;

            break;
        }
    }

    if (i >= _buf_size)
    {
        // key를 찾을 수 없거나, 해당 key에 해당하는 괄호가 완전하지 않음. log
        WriteLog("key를 찾을 수 없거나, 해당 key에 해당하는 괄호가 완전하지 않음.", _sz_key_head);

        return false;
    }
    else
    {
        //--------------------------------------------------------------------
        // 이제 범위내 (_si ~ _di)에서 tail을 찾는다.
        //--------------------------------------------------------------------
        int tail_leng = strlen(_sz_key_tail);
        for (_si; _si < (_di - tail_leng + 1); _si++)
        {
            // 주석 건너뛰기 : //... 0d0a
            if (is_annotation && _buffer[_si] == 0x0a)
            {
                is_annotation = false;
                continue;
            }
            else if (is_annotation)
                continue;
            else if (_buffer[_si] == '/')
            {
                is_annotation = true;
                continue;
            }
            //
            if (strncmp(&_buffer[_si], _sz_key_tail, tail_leng) == 0)
            {
                //--------------------------------------------------------------------
                // tail을 찾음
                //--------------------------------------------------------------------
                i = _si;
                unsigned int opt_len = 0;

                //--------------------------------------------------------------------
                // 각 항목은 0x0d를 기준으로 인식
                //--------------------------------------------------------------------
                while (_buffer[i] != 0x0d)
                {
                    i++;
                    opt_len++;
                }

                //--------------------------------------------------------------------
                // tail에 해당하는 옵션 부분을 따로 저장. ex) movement_spd = 30
                //--------------------------------------------------------------------

                char* sz_opt = new char[opt_len + 1]; // option길이 + null
                strncpy_s(sz_opt, opt_len + 1, &_buffer[_si], opt_len);
                sz_opt[opt_len] = '\0';
                //cout << sz_opt << endl;

                //--------------------------------------------------------------------
                // = 을 공백문자로 치환
                //--------------------------------------------------------------------

                for (i = 0; i < opt_len; i++)
                {
                    if (sz_opt[i] == '=')
                    {
                        sz_opt[i] = ' ';
                        break;
                    }
                }
                if (i >= opt_len)
                {
                    WriteLog("OPTION이 완전한 형식이 아닙니다. ( = 을 찾지 못함)", key);
                    delete[] sz_opt;
                    return false;
                }
                else
                {
                    //--------------------------------------------------------------------
                    // 앞 뒤의 공백을 제거후 값 반환.
                    //--------------------------------------------------------------------
                    if (trim(val, &sz_opt[i]))
                    {
                        delete[] sz_opt;
                        return true;
                    }
                    else
                    {
                        delete[] sz_opt;
                        delete[] val;
                        val = nullptr;

                        WriteLog("OPTION이 완전한 형식이 아닙니다. (값이 공백임)", key);
                        return false;
                    }
                }
                break;
            }
        }

        // 해당 tail을 찾지 못함.
        if (_si >= (_di - tail_leng + 1))
        {
            WriteLog("속성을 찾지 못하였습니다.", key);
            return false;
        }

        // 여기로 안옴.
        WriteLog("알 수 없는 오류.", key);
        return false;
    }
}

bool StageParser::loadMonster(int y, int x, int index)
{
    if (y < 0 || y > MAP_HEIGHT ||
        x < 0 || x > MAP_WIDTH ||
        index < 0 || index >= g_TotalMonster)
        return false;

    int i;
    for (i = 0; i < MAX_MONSTER; i++)
    {
        if (g_MonstersInStage[i]._monster_no == -1)
        {
            g_MonstersInStage[i] = g_MonsterIndex[index];
            g_MonstersInStage[i]._att_cool = g_MonsterIndex[index]._attack_interval;
            g_MonstersInStage[i]._mov_cool = g_MonsterIndex[index]._movement_interval;
            g_MonstersInStage[i]._next_move_Index = 0;
            g_MonstersInStage[i]._now_pos = { y,x };
            break;
        }
    }
    if (i >= MAX_MONSTER)
        return false;

    return true;
}

int StageParser::lookUpInMonsterIndex(char c)
{
    for (int i = 0; i < MONSTER_INDEX_SIZE; i++)
    {
        if (g_MonsterIndex[i]._icon == c)
            return i;
    }
    return -1;
}

void StageParser::WriteLog(const char* msg, const char* state)
{
    time_t timer = time(NULL);
    tm t;
    localtime_s(&t, &timer);


    FILE* f;
    fopen_s(&f, "Log.txt", "at");

    // CONSOLE LOG
    printf("////////////////////////////////////////////////\n");
    printf("%d / %d / %d / %d:%d:%d \n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    printf("MSG : %s\n", msg);
    if (state != nullptr)
    {
        printf("TARGET : %s\n", state);
    }

    // FILE LOG
    if (f != nullptr)
    {
        fputs("////////////////////////////////////////////////\n", f);
        fprintf_s(f, "%d / %d / %d / %d:%d:%d \n", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        fprintf_s(f, "MSG : %s\n", msg);

        if (state != nullptr)
        {
            fprintf_s(f, "TARGET : %s\n", state);
        }

        fclose(f);
    }
}

bool StageParser::loadStage(const char* path)
{
    if (path == nullptr)
    {
        WriteLog("path가 NULL입니다.");
        return false;
    }

    if (_f != nullptr)
        fclose(_f);

    // 0x0a까지 파싱하면됨
    if (!fopen_s(&_f, path, "rt"))
    {
        int now_col = 0;
        int total_monsters = 0;

        // 파일의 끝이 아니면 줄 단위로 읽어온다.
        while (!feof(_f))
        {
            //memset(_buffer, 0, _buf_size);
            fgets(_buffer, _buf_size, _f);

            for (unsigned int row = 0; row < _buf_size; row++)
            {
                // 줄의 끝
                if (_buffer[row] == 0x0a || _buffer[row] == '\0')
                    break;
                else if (_buffer[row] >= 0x21 && _buffer[row] <= 0x7E)
                {
                    // 위치 : arr[col][row]
                    int index = lookUpInMonsterIndex(_buffer[row]);

                    if (loadMonster(now_col, row, index) == false)
                    {
                        char err_msg[100];
                        sprintf_s(err_msg, 100,
                            "POS[%d][%d]에 %d index 몬스터 배치 실패.(index -1은 도감에 없는 몬스터)", now_col, row, index);

                        _loaded_monsters = 0;
                        WriteLog(err_msg);
                        return false;
                    }
                    else
                        total_monsters++;
                }
            }

            now_col++;
        }

        _loaded_monsters = total_monsters;
        return true;
    }
    return false;
}

#include "MyParser.h"

bool MyParser::trim(char* dst, const char* src)
{
    if (src == nullptr)
        return false;

    int si = 0;
    int di = strlen(src) - 1; // ex) ABCDE\0 => 5 - 1 => INDEX 4
    int len = strlen(src);

    // �����̽�	    0x20
    // �齺���̽�    0x08
    // ��           0x09
    // ���� �ǵ�	    0x0a
    // ĳ���� ����   0x0d

    //--------------------------------------------------------------------
    // ���ڿ��� �� ���� ����
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
    // ���ڿ� �� ���� ����
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
        // �߸��� ���ڿ� (ex. �������θ� �̷���� ���ڿ�)
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

                //HACK : �������� ���� ũ�⸦ �� ������ ����
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
        WriteLog("path�� NULL�Դϴ�.");
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
        // ������ ���� ����
        WriteLog("������ ���� ���Ͽ����ϴ�.", path);
        return false;
    }
}

bool MyParser::getValue(char* val, const char* key)
{
    // ex) MONSTER0:monster_no

    if (key == nullptr)
    {
        WriteLog("key�� NULLPTR �Դϴ�.");
        return false;
    }

    //--------------------------------------------------------------------
    // ex) :�� �̸��� ������, head�� tail�� ����
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
        // key�� �̸��� �߸���
        WriteLog("key�� �̸��� �߸���.", key);
        return false;
    }


    unsigned int i;
    bool is_annotation = false;
    for (i = 0; i < _buf_size; i++)
    {
        // �ּ� �ǳʶٱ� : //... 0d0a
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
        // Key�� ã�� ���
        // _si,_di�� ��ȣ ���� ������ ��´�.
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
        // key�� ã�� �� ���ų�, �ش� key�� �ش��ϴ� ��ȣ�� �������� ����. log
        WriteLog("key�� ã�� �� ���ų�, �ش� key�� �ش��ϴ� ��ȣ�� �������� ����.", _sz_key_head);

        return false;
    }
    else
    {
        //--------------------------------------------------------------------
        // ���� ������ (_si ~ _di)���� tail�� ã�´�.
        //--------------------------------------------------------------------
        int tail_leng = strlen(_sz_key_tail);
        for (_si; _si < (_di - tail_leng + 1); _si++)
        {
            // �ּ� �ǳʶٱ� : //... 0d0a
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
                // tail�� ã��
                //--------------------------------------------------------------------
                i = _si;
                unsigned int opt_len = 0;

                //--------------------------------------------------------------------
                // �� �׸��� 0x0d�� �������� �ν�
                //--------------------------------------------------------------------
                while (_buffer[i] != 0x0d)
                {
                    i++;
                    opt_len++;
                }

                //--------------------------------------------------------------------
                // tail�� �ش��ϴ� �ɼ� �κ��� ���� ����. ex) movement_spd = 30
                //--------------------------------------------------------------------

                char* sz_opt = new char[opt_len + 1]; // option���� + null
                strncpy_s(sz_opt, opt_len + 1, &_buffer[_si], opt_len);
                sz_opt[opt_len] = '\0';
                //cout << sz_opt << endl;

                //--------------------------------------------------------------------
                // = �� ���鹮�ڷ� ġȯ
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
                    WriteLog("OPTION�� ������ ������ �ƴմϴ�. ( = �� ã�� ����)", key);
                    delete[] sz_opt;
                    return false;
                }
                else
                {
                    //--------------------------------------------------------------------
                    // �� ���� ������ ������ �� ��ȯ.
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

                        WriteLog("OPTION�� ������ ������ �ƴմϴ�. (���� ������)", key);
                        return false;
                    }
                }
                break;
            }
        }

        // �ش� tail�� ã�� ����.
        if (_si >= (_di - tail_leng + 1))
        {
            WriteLog("�Ӽ��� ã�� ���Ͽ����ϴ�.", key);
            return false;
        }

        // ����� �ȿ�.
        WriteLog("�� �� ���� ����.", key);
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
        WriteLog("path�� NULL�Դϴ�.");
        return false;
    }

    if (_f != nullptr)
        fclose(_f);

    // 0x0a���� �Ľ��ϸ��
    if (!fopen_s(&_f, path, "rt"))
    {
        int now_col = 0;
        int total_monsters = 0;

        // ������ ���� �ƴϸ� �� ������ �о�´�.
        while (!feof(_f))
        {
            //memset(_buffer, 0, _buf_size);
            fgets(_buffer, _buf_size, _f);

            for (unsigned int row = 0; row < _buf_size; row++)
            {
                // ���� ��
                if (_buffer[row] == 0x0a || _buffer[row] == '\0')
                    break;
                else if (_buffer[row] >= 0x21 && _buffer[row] <= 0x7E)
                {
                    // ��ġ : arr[col][row]
                    int index = lookUpInMonsterIndex(_buffer[row]);

                    if (loadMonster(now_col, row, index) == false)
                    {
                        char err_msg[100];
                        sprintf_s(err_msg, 100,
                            "POS[%d][%d]�� %d index ���� ��ġ ����.(index -1�� ������ ���� ����)", now_col, row, index);

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

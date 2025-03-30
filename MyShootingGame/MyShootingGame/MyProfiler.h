#pragma once
#include <Windows.h>
#define PROFILE_ARR_SIZE 64
#define PROFILE

#ifdef PROFILE
#define PRO_BEGIN(TagName) MyProfiler profile(TagName)
#else
#define PRO_BEGIN(TagName)
#endif

struct ProfileData
{
	bool _isValid;
	const char* _sz_name;

	LARGE_INTEGER _totalTime;
	LARGE_INTEGER _max[2];
	LARGE_INTEGER _min[2];
	int _count_call;

	ProfileData()
	{
		_isValid = false;
		_sz_name = nullptr;
		_totalTime.QuadPart = 0;

		_max[0].QuadPart = MINLONGLONG;
		_max[1].QuadPart = MINLONGLONG;
		_min[0].QuadPart = MAXLONGLONG;
		_min[1].QuadPart = MAXLONGLONG;

		_count_call = 0;
	}
};

class MyProfiler
{
public:
	MyProfiler(const char* tag) : _tag(tag)
	{
		Begin();
	}
	~MyProfiler()
	{
		End();
	}

	void Begin()
	{
		//LARGE_INTEGER freq;
		//QueryPerformanceFrequency(&freq);

		// _arr���� tag�� ������ ��ġ ã��.
		int index = -1;
		int empty_index = -1;
		for (int i = 0; i < PROFILE_ARR_SIZE; i++)
		{
			if (_arr[i]._isValid)
			{
				if (strcmp(_arr[i]._sz_name, _tag) == 0)
				{
					index = i;
					break;
				}
			}
			else if (empty_index == -1)
				empty_index = i;
		}

		// ���� �� ��ġ(index)
		if (index == -1)
		{
			// ��� �� �� ������ ������ throw
			if (empty_index == -1)
				throw "Arr for ProfileData is full.";

			_index = empty_index;
			_arr[_index]._isValid = true;
			_arr[_index]._sz_name = _tag;
		}
		else
			_index = index;

		// �ð� ����
		QueryPerformanceCounter(&_start);
	}

	void End()
	{
		// �� �ҿ�ð� ���.
		LARGE_INTEGER elapsed_t;
		QueryPerformanceCounter(&elapsed_t);
		elapsed_t.QuadPart -= _start.QuadPart;

		// �ּ� ����

		if (_arr[_index]._min[0].QuadPart > elapsed_t.QuadPart)
		{
			_arr[_index]._min[1].QuadPart = _arr[_index]._min[0].QuadPart;
			_arr[_index]._min[0].QuadPart = elapsed_t.QuadPart;
		}
		else if (_arr[_index]._min[1].QuadPart > elapsed_t.QuadPart)
		{
			_arr[_index]._min[1].QuadPart = elapsed_t.QuadPart;
		}

		// �ִ� ����
		if (_arr[_index]._max[0].QuadPart < elapsed_t.QuadPart)
		{
			_arr[_index]._max[1].QuadPart = _arr[_index]._max[0].QuadPart;
			_arr[_index]._max[0].QuadPart = elapsed_t.QuadPart;
		}
		else if (_arr[_index]._max[1].QuadPart < elapsed_t.QuadPart)
		{
			_arr[_index]._max[1].QuadPart = elapsed_t.QuadPart;
		}

		// �� �ð� �ջ�, ȣ�� Ƚ�� ����
		_arr[_index]._totalTime.QuadPart += elapsed_t.QuadPart;
		_arr[_index]._count_call++;
	}

	/////////////////////////////////////////////////////////
	// ��������� �������ϸ� ��� .txt�� ����
	/////////////////////////////////////////////////////////

	static void ProfileDataOutText(const char* szFileName)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		FILE* f;
		fopen_s(&f, szFileName, "wt");

		fprintf_s(f, "-------------------------------------------------------------------------------\n");
		fprintf_s(f, "\n");
		fprintf_s(f, "-------------------------------------------------------------------------------\n");
		fprintf_s(f, "           Name  |     Average  |        Min   |        Max   |      Call |\n");
		fprintf_s(f, "-------------------------------------------------------------------------------\n");
		for (int i = 0; i < PROFILE_ARR_SIZE; i++)
		{
			if (_arr[i]._isValid == false)
				continue;

			LARGE_INTEGER average = _arr[i]._totalTime;
			// �ش�ġ ����
			if (_arr[i]._count_call > 10)
			{
				average.QuadPart -= _arr[i]._max[0].QuadPart;
				average.QuadPart -= _arr[i]._max[1].QuadPart;
				average.QuadPart -= _arr[i]._min[0].QuadPart;
				average.QuadPart -= _arr[i]._min[1].QuadPart;
			}

			double aver_per_micro = (average.QuadPart * 1000 * 1000) / (double)freq.QuadPart;// ���� (����ũ��)
			if (_arr[i]._count_call > 10)
				aver_per_micro /= (_arr[i]._count_call - 4); // �ش�ġ ����
			else
				aver_per_micro /= _arr[i]._count_call;

			double min = (_arr[i]._min[0].QuadPart * 1000 * 1000) / (double)freq.QuadPart; // ���� (����ũ��)
			double max = (_arr[i]._max[0].QuadPart * 1000 * 1000) / (double)freq.QuadPart; // ���� (����ũ��)

			fprintf_s(f, " %s |    %lf�� |     %lf�� | %lf�� |     %d  \n",
				_arr[i]._sz_name,
				aver_per_micro,
				min,
				max,
				_arr[i]._count_call
			);
		}
		fprintf_s(f, "\n");
		fprintf_s(f, "-------------------------------------------------------------------------------\n");

		fclose(f);
	}



	/////////////////////////////////////////////////////////
	// ��� �ʱ�ȭ (name�� �ʱ�ȭ X)
	/////////////////////////////////////////////////////////

	static void Reset()
	{
		for (int i = 0; i < PROFILE_ARR_SIZE; i++)
		{
			if (_arr[i]._isValid == false)
				continue;

			_arr[i]._totalTime.QuadPart = 0;
			_arr[i]._min[0].QuadPart = MAXLONGLONG;
			_arr[i]._min[1].QuadPart = MAXLONGLONG;
			_arr[i]._max[0].QuadPart = MINLONGLONG;
			_arr[i]._max[1].QuadPart = MINLONGLONG;
			_arr[i]._count_call = 0;
		}
	}
private:
	static ProfileData _arr[PROFILE_ARR_SIZE];
	const char* _tag;

	// ������ ������ġ
	int _index;

	LARGE_INTEGER _start;
};

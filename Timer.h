#ifndef _CLASS_TIMER_H_
#define _CLASS_TIMER_H_

////////////////////////////////////////////////

#include <windows.h>

////////////////////////////////////////////////

class Timer
{
private:
	LARGE_INTEGER nFreq, nBefore, nAfter;
public:
	Timer()
	{
		nFreq.QuadPart = 0;
		nBefore.QuadPart = 0;
		nAfter.QuadPart = 0;

		//é¸îgêîÇéÊìæ
		QueryPerformanceFrequency(&nFreq);
	}
	~Timer() {end();}

	void start() {QueryPerformanceCounter(&nBefore);}
	void end()	 {QueryPerformanceCounter(&nAfter);}

	double get()
	{
		if(0 == nFreq.QuadPart) return -1;
		return (double)(nAfter.QuadPart - nBefore.QuadPart) * 1000.0 / (double)nFreq.QuadPart;
	}

};

////////////////////////////////////////////////

#endif

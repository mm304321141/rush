#pragma once

#include "../xlib/xf.h"

struct rmutex
{
	R_CRITICAL_SECTION mutex;

	rmutex()
	{
		xf::InitializeCriticalSection(&mutex);
	}

	~rmutex()
	{
		xf::DeleteCriticalSection(&mutex);
	}

	void enter()
	{
		xf::EnterCriticalSection(&mutex);
	}

	void leave()
	{
		xf::LeaveCriticalSection(&mutex);
	}
};

//用于整个函数的局部临时mutex
struct rmutex_t
{
	rmutex* point;

	rmutex_t()
	{
		point=null;
	}

	rmutex_t(rmutex& m)
	{
		point=&m;
		point->enter();
	}

	~rmutex_t()
	{
		if(point!=null)
		{
			point->leave();
		}
	}
};

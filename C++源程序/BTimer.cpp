#include "BTimer.h"

UINT CBTimer::ms_uIDIncrement = 0;


CBTimer::CBTimer(HWND hWndAsso_in, UINT Timer_FootLength_in, TIMERPROC ptrFuncCallBack_in)
{
	ms_uIDIncrement++;
	if_uTimerID = ms_uIDIncrement;

	if_hWndAsso = NULL;
	Timer_FootLength = 0;
	if_pFuncCallBack = NULL;
	if_TimerEnabled = false;

	hWndAssoSet(hWndAsso_in);
	IntervalSet(Timer_FootLength_in);
	pFuncCallBackSet(ptrFuncCallBack_in);

	EnableSet(true);
	return;
}

bool CBTimer::Enable()
{
	return if_TimerEnabled;
}

void CBTimer::EnableSet(bool value)
{
	if (value == if_TimerEnabled)
		return;

	if (value)
	{
		if (if_pFuncCallBack != NULL && Timer_FootLength != 0)
		{
			//调用API函数SetTimer创建定时器
			//若此时未关联窗口，则定时器ID将使用定时器的现有值
			//SetTimer调用成功时也返回该值
			//若此时未关联窗口，SetTimer将忽略现有ID
			//SetTimer在调用成功时返回一个API分配的定时器ID
			UINT ret = SetTimer(if_hWndAsso, if_uTimerID, Timer_FootLength, if_pFuncCallBack);

			if (ret)
				if_uTimerID = ret;

			if_TimerEnabled = (ret != 0);
		}
	}
	else
	{
		KillTimer(if_hWndAsso, if_uTimerID);
		if (if_hWndAsso == NULL)
		{
			ms_uIDIncrement++;
			if_uTimerID = ms_uIDIncrement;
		}
		if_TimerEnabled = false;
	}
	return;
}

UINT CBTimer::Interval()
{
	return Timer_FootLength;
}

void CBTimer::IntervalSet(UINT uInterval)
{
	if (Timer_FootLength == uInterval)
		return;
	//设置新的时间间隔
	Timer_FootLength = uInterval;

	//使新的时间间隔生效
	if (if_TimerEnabled)
	{
		//删除原有的定时器后，再重新启动该定时器
		EnableSet(false);
		EnableSet(true);
	}
	return;
}

TIMERPROC CBTimer::pFuncCallBack()
{
	return if_pFuncCallBack;
}

void CBTimer::pFuncCallBackSet(TIMERPROC ptrFuncCallBack)
{
	if (if_pFuncCallBack == ptrFuncCallBack)
		return;
	//设置新的回调函数地址
	if_pFuncCallBack = ptrFuncCallBack;
	
	//使新的回调函数地址生效
	if (if_TimerEnabled)
	{
		//删除原有的定时器后，再重新启动该定时器
		EnableSet(false);
		EnableSet(true);
	}
	return;
}

HWND CBTimer::hWndAsso()
{
	return if_hWndAsso;
}

void CBTimer::hWndAssoSet(HWND hWnd)
{
	if (if_hWndAsso == hWnd)
		return;
	//关联窗口发生变动，先记录当前定时器是否被启用，然后再重启定时器
	bool LastTimerState = if_TimerEnabled;
	if (if_TimerEnabled)
		EnableSet(false);

	//设置新的关联窗口
	if_hWndAsso = hWnd;

	//使新的关联窗口生效
	if (LastTimerState)
		EnableSet(true);
	return;
}

UINT CBTimer::Identifier()
{
	if (if_TimerEnabled)
		return if_uTimerID;
	else
		return 0;
}

CBTimer::~CBTimer()
{
	EnableSet(false);
	return;
}

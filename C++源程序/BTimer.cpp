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
			//����API����SetTimer������ʱ��
			//����ʱδ�������ڣ���ʱ��ID��ʹ�ö�ʱ��������ֵ
			//SetTimer���óɹ�ʱҲ���ظ�ֵ
			//����ʱδ�������ڣ�SetTimer����������ID
			//SetTimer�ڵ��óɹ�ʱ����һ��API����Ķ�ʱ��ID
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
	//�����µ�ʱ����
	Timer_FootLength = uInterval;

	//ʹ�µ�ʱ������Ч
	if (if_TimerEnabled)
	{
		//ɾ��ԭ�еĶ�ʱ���������������ö�ʱ��
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
	//�����µĻص�������ַ
	if_pFuncCallBack = ptrFuncCallBack;
	
	//ʹ�µĻص�������ַ��Ч
	if (if_TimerEnabled)
	{
		//ɾ��ԭ�еĶ�ʱ���������������ö�ʱ��
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
	//�������ڷ����䶯���ȼ�¼��ǰ��ʱ���Ƿ����ã�Ȼ����������ʱ��
	bool LastTimerState = if_TimerEnabled;
	if (if_TimerEnabled)
		EnableSet(false);

	//�����µĹ�������
	if_hWndAsso = hWnd;

	//ʹ�µĹ���������Ч
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

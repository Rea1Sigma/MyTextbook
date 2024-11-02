#pragma once
#include<windows.h>


class CBTimer{

public:
	//ptrFunCallBack为定时器触发时所要调用的回调函数的地址
	//该回调函数结构为：
	//void CALLBACK TimerN_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
	//必须设置ptrFuncCallBack 和 Timer_FootLength 才能启动定时器
	//若在构造函数时，上述必须变量均不为0，则直接启动定时器
	//否则必须通过相应的成员函数，对其进行赋值

	CBTimer(HWND hWndAsso_in = NULL,UINT Timer_FootLength_in = 1000,TIMERPROC ptrFuncCallBack_in = NULL);
	bool Enable();
	void EnableSet(bool value);
	UINT Interval();
	void IntervalSet(UINT uInterval);
	//返回或设置定时器触发时所要调用的回调函数的地址

	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);
	//返回或设置关联窗口句柄，为0表示定时器未关联窗口
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

	//返回定时器的ID，只在启动时返回值有效，禁用时返回值为0
	UINT Identifier();

	//析构函数
	~CBTimer();

private:	
	//定义私有静态成员
	//私有静态成员为该类的对象内的“全局变量”
	//但只能该类的对象自己调用，其他类的对象或类外不能调用
	static UINT ms_uIDIncrement;

	UINT if_uTimerID;											//定时器标识，未关联窗口时为API分配的ID，关联后为本类分配的ID
	HWND if_hWndAsso;											//定时器所关联的窗口的句柄，若未关联窗口则为0
	UINT Timer_FootLength;										//定时器触发间隔时间（ms），每隔Timer_FootLength毫秒就会调用回调函数一次
	TIMERPROC if_pFuncCallBack;									//定时器触发时回调函数的地址							
	bool if_TimerEnabled;										//定时器的状态,即是否启动

};


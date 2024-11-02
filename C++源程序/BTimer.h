#pragma once
#include<windows.h>


class CBTimer{

public:
	//ptrFunCallBackΪ��ʱ������ʱ��Ҫ���õĻص������ĵ�ַ
	//�ûص������ṹΪ��
	//void CALLBACK TimerN_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
	//��������ptrFuncCallBack �� Timer_FootLength ����������ʱ��
	//���ڹ��캯��ʱ�����������������Ϊ0����ֱ��������ʱ��
	//�������ͨ����Ӧ�ĳ�Ա������������и�ֵ

	CBTimer(HWND hWndAsso_in = NULL,UINT Timer_FootLength_in = 1000,TIMERPROC ptrFuncCallBack_in = NULL);
	bool Enable();
	void EnableSet(bool value);
	UINT Interval();
	void IntervalSet(UINT uInterval);
	//���ػ����ö�ʱ������ʱ��Ҫ���õĻص������ĵ�ַ

	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);
	//���ػ����ù������ھ����Ϊ0��ʾ��ʱ��δ��������
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

	//���ض�ʱ����ID��ֻ������ʱ����ֵ��Ч������ʱ����ֵΪ0
	UINT Identifier();

	//��������
	~CBTimer();

private:	
	//����˽�о�̬��Ա
	//˽�о�̬��ԱΪ����Ķ����ڵġ�ȫ�ֱ�����
	//��ֻ�ܸ���Ķ����Լ����ã�������Ķ�������ⲻ�ܵ���
	static UINT ms_uIDIncrement;

	UINT if_uTimerID;											//��ʱ����ʶ��δ��������ʱΪAPI�����ID��������Ϊ��������ID
	HWND if_hWndAsso;											//��ʱ���������Ĵ��ڵľ������δ����������Ϊ0
	UINT Timer_FootLength;										//��ʱ���������ʱ�䣨ms����ÿ��Timer_FootLength����ͻ���ûص�����һ��
	TIMERPROC if_pFuncCallBack;									//��ʱ������ʱ�ص������ĵ�ַ							
	bool if_TimerEnabled;										//��ʱ����״̬,���Ƿ�����

};


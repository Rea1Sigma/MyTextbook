//////////////////////////////////////////////////////////////////////
// CBMPLoad �ࣺ���� BMP �ļ����ɻ�����е���������ֵ
// һ���������һ��ֻ�ɼ���һ�� BMP �ļ�
//
// ֧�֣���Ҫ BWindows��mdlFileSys ģ���֧��
//////////////////////////////////////////////////////////////////////

#include "BWindows.h"


#pragma once

class CBBMPLoad  
{
public:
	CBBMPLoad(LPCTSTR szBmpFile=NULL);				// lpszBmpFile Ϊ��ǰҪ���ص� BMP �ļ�������ȫ·������Ҳ��Ϊ�գ��Ժ�ͨ������ LoadBitmapFile ����ʱ�����á�
	virtual ~CBBMPLoad();
	
	bool LoadBitmapFile(LPCTSTR szBmpFile=NULL);	// ����һ��BMP�ļ���lpszBmpFileΪ��ʱʹ�������õ��ļ���
													// �����ڴ�������ʱͨ�����캯�������ˣ�������δ�����ļ�������
													// �������á�lpszBmpFile �粻Ϊ�ս�������ǰ���캯��ʱ�������ļ���
													// ��������캯��ʱҲ�����Ļ���
	long Width();									// ����λͼ��ȣ��������� LoadBitmapFile �����Ч
	long Height();									// ����λͼ�߶ȣ��������� LoadBitmapFile �����Ч
	long Type();
	long WidthBytes();
	WORD Planes();
	WORD BitsPixel();
	
	LPTSTR GetErrDesp();							// ���ش�����Ϣ��ָ������ڲ������ַ�����ָ�룩������еĻ���

	unsigned char ** GetBMPBytes();					// ���λͼ���������أ�����ֵ ָ�� �������ڲ������ݿռ䣬Ϊһ��ָ���������ʼ��ַ��ָ�������Ԫ�طֱ�ָ����������ÿ�е��׵�ַ
	void ReleaseResource(bool bFreeSzBmpFile=true);	// �ͷ���Դ��bFreeSzBmpFile==true ʱ��Ҳ�ͷű����ļ����� m_szBmpFile ����Դ�������ͷ� m_szBmpFile�������ͷ�������Դ

private:
	LPTSTR m_szBmpFile;			// ���浱ǰ���ص� BMP �ļ�������ȫ·���������� new ���ٿռ䣬�ڶ�������ʱ�Զ��ͷſռ�
	HBITMAP m_hBitmap;			// λͼ�����Ϊ0ʱ��ʾ��δ��λͼ��
	unsigned char * m_ptrPixels;// λͼ���ݣ���ά�������к������к���ɵ��ֽ�����
								// �������� char *m_pLines[] ����ĸ�Ԫ��ָ�����и����С�����ʼ��ַ���� 
								// �� m_pLines[row][col] �ɷ��ʾ����ĳ��Ԫ��
								// ����λͼ��ʽ��m_Pixels һ��һ���ֽڣ���һ������Ԫ�أ���һ�����أ��ʱ�����ֻ��8λ���������µ�λͼ
	unsigned char **m_pLines;	// ָ�����飬����Ŀռ��ɱ������Զ������� LoadBitmapFile ֮���Զ��������Ԫ��ֵΪλͼÿ�����ص���ʼ��ַ
	BITMAP m_bm;				// λͼ��Ϣ
	LPTSTR m_ErrDesp;			// ������Ϣ�ַ��������� new ���ٿռ䣬�ڶ�������ʱ�Զ��ͷſռ�


	void SetErrDesp(LPCTSTR errDesp);	// ���ô�����Ϣ�ַ������Զ����ٿռ䣬���浽 m_ErrDesp
};

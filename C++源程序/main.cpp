#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include <chrono>
#include <vector>
#include <memory>
#include "BReadLinesEx.h"
#include "BTimer.h"
#include "mdlShellExec.h"
#include "mdlFileSys.h"
#include "mdlPathDlg.h"

CBForm form1(ID_form1);
//����from_InputKey����,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_InputKey;
//�������ڴ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_About;
//�������Ҵ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_Find;
//�����滻����,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_Replace;
//�������д���,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_Run;
//�������ļ�Ŀ¼����,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_FileLists;
//�����ĵ����ִ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
extern CBForm form_Main;

void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

CBTimer Timer1(0, 1000, Timer1_Timer);											//�����ʱ��ʱ��
CBTimer Timer2;																	//���𴰿ڵ���ĺ���

bool TextShow = false;

//Timer1�Ļص�����
void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	TCHAR szTimeStr[64];

	_stprintf(szTimeStr, TEXT("%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);

	form1.TextSet(StrAppend(TEXT("�ҵļ��±�--------�����Ǳ���ʱ�䣺"),szTimeStr));
	return;
}

//Timer2�Ļص�����
void CALLBACK Timer2_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	const int WindowShow_Step = 35;

	int Form1_Opacity = form1.Opacity();
	Form1_Opacity += WindowShow_Step;
	if (Form1_Opacity >= 255)
	{
		Timer2.EnableSet(false);
		form1.OpacitySet(-1);
	}
	else
		form1.OpacitySet(Form1_Opacity);
	return;
}

bool if_TopMost;																//����Ƿ񴰿��ö�
bool if_ShowingStatus;															//����Ƿ���ʾ״̬��
bool if_TextChanged;															//����ı��Ƿ��ѱ��޸�

bool if_InputKeyOk;																//����Ƿ��Ѿ���������
char * AimKey;																	//����

int Text_Main_FontSize = 0;														//��¼��ǰ�ֺ�
TCHAR save_FileName[1024];														//�ļ�����������

struct UndoSpace {

	LPTSTR Undo_Text;
	// ���浱ǰ���ı����ݣ�����ʱֱ�Ӹ�����һ�α�����ı����ݼ��ɡ�
	COLORREF Undo_FontColor;
	// COLOREF �洢����RGB�������룬ÿ��λ��һ����ɫ���Ե�λ����λ�ֱ��Ǻ�����
	// ��RGB��255,0,0������ {00000000��00000000��11111111}
	// ��RGB��255,0,255������ {11111111��00000000��11111111}
	// ����������ֵ�ĸ�ʽ�����,��int���ͱ���
	// ����ͨ����λ������λ�������ȡ���д洢�ĸ�����ɫ����
	LPTSTR Undo_FontType;
};

std::vector<UndoSpace> Undo_Total;												
//��vector������ṹ�壬ʵ���˶�̬�ṹ��ָ��
//vector����ڳ���Ķ�̬����ָ�룬ά�����򵥷��㡣



//����InputKey���ڽ���ĺ���������main��ִ��
extern void form_InputKey_PreSet();
extern void form_InputKey_main();
//����About���ڽ���ĺ���������main��ִ��
extern void form_About_main();
extern void form_About_PreSet();
//����Find���ڽ���ĺ���������main��ִ��
extern void form_Find_main();
extern void form_Find_PreSet();
extern void form_Find_Move();
//����Replace���ڽ���ĺ���������main��ִ��
extern void form_Replace_main();
extern void form_Replace_PreSet();
extern void form_Replace_Move();
//����Run���ڽ���ĺ���������main��ִ��
extern void form_Run_main();
//����FileLists���ڽ���ĺ���������main��ִ��
extern void form_FileLists_main();
//����StringCount���ڽ���ĺ���������main��ִ��
extern int form_StringCount_main();

//�ƶ��������ô��������ġ�
void form_InputKey_Move()
{
	form_InputKey.Move(
		form1.Left()+(form1.Width()- form_InputKey.Width())/2,
		form1.Top()+(form1.Height()- form_InputKey.Height())/2
	);
	return;
}
//��ʾ�������ô��ڣ��������Ƿ�ɹ���������
bool InputKey()
{

	form_InputKey.Load();

	form_InputKey_PreSet();

	form_InputKey_Move();

	if_InputKeyOk = false;

	form_InputKey.Show(1,form1.hWnd());

	return if_InputKeyOk;
}
//�ƶ����ڴ��������ġ�
void form_About_Move()
{
	form_About.Move(
		form1.Left() + (form1.Width() - form_About.Width()) / 2,
		form1.Top() + (form1.Height() - form_About.Height()) / 2
	);
	return;
}
//�����ڴ�С�ı�ʱ���ؼ���λ�ô�С��֮�ı��
void form1_Resize()
{
	int StaticText_Main_Height = 0;
	if (if_ShowingStatus)
	{
		StaticText_Main_Height = form1.Control(IDC_StaticText_Main).Height()+6;
		form1.Control(IDC_StaticText_Main).Move(4, form1.ClientHeight() - StaticText_Main_Height + 4,
			form1.ClientWidth() - 4, StaticText_Main_Height - 6);
		//����Ҫ��ʾ״̬���������״̬���봰��ͬ��
	}
	form1.Control(IDC_Text_Main).Move(0, 0, form1.ClientWidth(),
		form1.ClientHeight() - StaticText_Main_Height);
	//�������ı����봰��ͬ��
	return;
}
//������Ԥ�����
void PreSet()
{
	//��ʼ��������Ǳ���
	if_ShowingStatus = true;
	if_TopMost = false;
	if_TextChanged = false;
	
	save_FileName[0] = 0;														//��ʼ�������ļ�������
	//
	Text_Main_FontSize = form1.Control(IDC_Text_Main).FontSize();		//�����ʼ���ֺŴ�С�����ں�����ݵ����ֺŴ�С
	form1_Resize();																//���ô���λ��
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("��ӭʹ�ã�"));
	form1.Control(IDC_StaticText_Main).FontSizeSet(14);
	form1.Control(IDC_Text_Main).FontSizeSet(14);
	form1.Menu(ID_MenuEdit_FontType_SongTi).CheckedSet(true, true, 0, 2, true);
	form1.Menu(ID_MenuEdit_FontColor_Black).CheckedSet(true, true, 0, 3, true);
	
	Timer1_Timer(form1.hWnd(), 0, 0, 0);

	form1.OpacitySet(0);
	Timer2.IntervalSet(50);
	Timer2.hWndAssoSet(form1.hWnd());
	Timer2.pFuncCallBackSet(Timer2_Timer);
	Timer2.EnableSet(true);
	//��ʼ�������ؼ��Ͳ˵���
	//////////////////////////////////////////////
	//////////////////////////////////////////////
	//���³��ش���ṹ�壬���浱ǰ�ĳ�ʼ״̬�����ں������ز���
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });

	return;
}
//���ļ��ĺ�����
bool OpenFile(LPCTSTR Aim_FileName,char * encKey)
{
	//��ʼ�������ı����ݵı���
	LPTSTR ContentNow = TEXT("");
	LPTSTR Line = NULL, Temp = NULL;
	CBReadLinesEx FinalFiles;													
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));		//��յ�ǰ�ı����Ա��ڴ��뼴���򿪵��ļ�����
	
	if (!FinalFiles.OpenFile(Aim_FileName))								//���ļ���ʧ�ܣ��򷵻�false
		return false;

	FinalFiles.EncryptKey = encKey;												//ȡ��Ŀ���ļ�������


	pApp->MousePointerGlobalSet(IDC_Wait);									//���û�ָ��ת��Ϊ�ȴ�״̬������ʾ�û���ǰ���ڽ��д��ĵ�����

	while (!FinalFiles.IsEndRead())												//IsEndRead()�������Լ���ĵ��Ƿ�������һ�У����������һ���򷵻�true
	{
		//���ļ���δ��ȡ�꣬��һֱѭ����ֱ��ȫ����ȡ
		FinalFiles.GetNextLine(Line);
		if (FinalFiles.IsErrOccured())
		{
			//ÿ��ѭ��������ļ���ȡ�Ƿ�ʧ�ܣ����ļ���ȡʧ����ԭ���ָ�벢��ֹ������
			pApp->MousePointerGlobalSet(0);
			return false;
		}
		if (FinalFiles.iEndLineSign > 0)										
			ContentNow = StrAppend(ContentNow, TEXT("\r\n"));			//�����в�Ϊ�գ�����ĩβ����\r\n�Ա���������һ��
		ContentNow = StrAppend(ContentNow, Line);						//������һ�����ݡ�
	}
	form1.Control(IDC_Text_Main).TextSet(ContentNow);			//�����ı�����ʾ��Ŀ���ļ��ж�ȡ��������

	_tcscpy(save_FileName, Aim_FileName);										//Ŀ���ļ����ݸ��Ƹ�save_FileName����

	form1.TextSet(save_FileName);											
	form1.TextAdd(TEXT(" - �ҵļ��±�"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("�ļ��򿪳ɹ�!"));

	pApp->MousePointerGlobalSet(0);									//�ļ���ȡ��ɣ������ָ�뻹ԭ������ʾ�û��������

	if_TextChanged = false;														//�����ı��Ƿ񱻸ı���

	return true;																//�ɹ����򷵻�true�����򷵻�false
}
//�����ļ��ĺ�����
bool SaveFile(LPCTSTR Aim_FileName,char * encKey)
{
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));		
	HANDLE Handle_AimFile = EFOpen(Aim_FileName, EF_OpStyle_Output);
	// �� API �������ļ��������ļ��ľ����ʧ�ܷ��� -1 ���� INVALID_HANDLE_VALUE ��ֵ��
	EFPrint(Handle_AimFile, form1.Control(IDC_Text_Main).Text(),EF_LineSeed_None,-1,1,TEXT("�޷����ļ���д���ַ���"), encKey);
	// ���ļ� llWritePos ��λ�ô�ӡһ���ַ�
	// llWritePos ��Ϊ <0 ��ֵ����ʱΪ�ӵ�ǰ��дλ�ÿ�ʼд�����ƶ��ļ�λ��ָ��
	// iShowResume ָ�������ȡ�����Ƿ񵯳��Ի�����ʾ
	//   �� iShowResume=1����ʾ�����С����ԡ��͡�ȡ����������ť��
	// ����д����ֽ�������ʧ�ܷ��� < 0�����û������ԡ��򷵻� -2��
	EFClose(Handle_AimFile);
	// �رմ򿪵�һ���ļ������� hFile Ϊ EFOpen �������ص��ļ����

	_tcscpy(save_FileName, Aim_FileName);										//����Ŀ���ļ����������ļ���

	form1.TextSet(save_FileName);
	form1.TextAdd(TEXT(" - �ҵļ��±�"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("�ļ�����ɹ���"));

	if_TextChanged = false;														//�����ı��Ƿ񱻸ı���

	return true;																//������ɹ��򷵻�true�����򷵻�false
}
//�رճ���ʱѯ���Ƿ���Ҫ�����
bool Ques_ifSaveNeed()
{
	if (if_TextChanged)															//���ı��ѱ����ĺ�δ���棬��ִ�����£�
	{
		EDlgBoxCmdID Ques_Message;												//����MsgBox��Ϣ���enum���ͣ��洢�ı���Ϊ��Ϣ��İ�ťѡ��
		Ques_Message = MsgBox(TEXT("�������������޸ġ�\r\n�Ƿ񱣴��ļ���"), TEXT("�ҵļ��±�"), mb_YesNoCancel, mb_IconQuestion);
		if (Ques_Message == idYes)
		{
			//���û����ȷ����ť������б������
			if (*save_FileName == 0)
			{
				LPTSTR FileSave;
				OsdSetFilter(TEXT("�ı��ļ�(*.txt)|*.txt"));
				FileSave = OsdSaveDlg(form1.hWnd());
				if (*FileSave)
					if (InputKey())
						SaveFile(FileSave, AimKey);
			}
			else
				SaveFile(save_FileName, AimKey);
			if (if_TextChanged)
				return true;
		}
		else if (Ques_Message == idCancel)
			return true;
	}
	return false;
	//��ѯ�ʴ��ڣ��û���ûѡ���ǡ�Ҳûѡ�񡰷񡱣�������û����������ڹرմ��ڣ����������false
	//�����������true
}
//�������ִ�С���ܡ�
void Text_Main_MouseWheel(int wParam, int lParam)
{
	
	int fwKeys, zDelta;
	int xPos, yPos;

	fwKeys = LOWORD(wParam);
	zDelta = (short)HIWORD(wParam);
	xPos = (short)LOWORD(lParam);
	yPos = (short)HIWORD(lParam);
	//�û��������ֲ����ͼ��̲����ֱ𱣴��ڸ�λ�͵�λ
	//��Ҫ����λ�������ȡ
	if (fwKeys & MK_CONTROL && zDelta > 0)
	{
		//���û���סCtrl��ͬʱ���ϻ������֣���ִ������Ŵ����
		Text_Main_FontSize++;
		form1.Control(IDC_Text_Main).FontSizeSet(Text_Main_FontSize);
		return;
	}
	if (fwKeys & MK_CONTROL && zDelta < 0)
	{
		//���û���סCtrl��ͬʱ���»������֣���ִ��������С����
		if (Text_Main_FontSize > 1)
		{
			//�ֺ���С��С��1����ִ�д˲���ʱ�ֺ���Ϊ1����ִ�иò���
			Text_Main_FontSize--;
			form1.Control(IDC_Text_Main).FontSizeSet(Text_Main_FontSize);
			return;
		}
	}
	return;
}
/////////////////////////////
/////////////////////////////
//�ļ��˵��µĸ����Ӳ˵�������

//�ر������ڡ�
void MainApplication_Exit()
{
	form1.UnLoad();	
	return;
}
//�����µ��ı���
void MenuFile_New_Click()
{
	if (Ques_ifSaveNeed())														//���û��ڹر�ѯ�ʵ��ȡ����ر�ѯ�ʴ��ڣ��򲻽����½�����
		return;

	form1.Control(IDC_Text_Main).TextSet(TEXT(""));
	form1.TextSet(TEXT("�ҵļ��±�"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));

	if_InputKeyOk = false;
	if_TextChanged = false;
	AimKey = NULL;
	*save_FileName = 0;
	//�½�֮������������������������
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	//�����ļ�ʱ����ճ�������ռ�����ݣ����ڱ����µ��ļ����ݡ�
	Undo_Total.clear();
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });
	return;
}
//���Ϊ�����ĵ���
void MenuFile_SaveAs_Click()
{
	//if (Ques_ifSaveNeed())														//���û��ڹر�ѯ�ʵ��ȡ����ر�ѯ�ʴ��ڣ��򲻽������Ϊ����
	//	return;
	LPTSTR FileSave;
	OsdSetFilter(TEXT("�ı��ļ�(*.txt)|*.txt"));
	FileSave = OsdSaveDlg(form1.hWnd());
	if (*FileSave)
		if (InputKey())
			SaveFile(FileSave, AimKey);							//���ñ�������
	return;
}
//�������ĵ��Ĺ��ܡ�
void MenuFile_Open_Click()
{
	if (Ques_ifSaveNeed())														//���û��ڹر�ѯ�ʵ��ȡ����ر�ѯ�ʴ��ڣ��򲻽��д������ĵ�����
		return;
	LPTSTR FileOpen;
	OsdSetFilter(TEXT("�ı��ļ�(*.txt)|*.txt"));
	FileOpen = OsdOpenDlg(form1.hWnd());
	if (*FileOpen)			
		if (InputKey())
			OpenFile(FileOpen, AimKey);							//��������ĵ�����������������ʾ������ĵ�����

	//�����ļ�ʱ����ճ�������ռ�����ݣ����ڱ����µ��ļ����ݡ�
	Undo_Total.clear();
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });

	return;
}
//�ļ��˵��µĸ����Ӳ˵�������
/////////////////////////////
/////////////////////////////
//�༭�˵��µĸ����Ӳ˵�������

//���в�����
void MenuEdit_Cut_Click()
{
	//Cut()�����ɼ���ѡ�е����ݵ����а�
	form1.Control(IDC_Text_Main).Cut();
	//������ɺ����ˢ�£���ֹչʾ�ַ������л���
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//���Ʋ�����
void MenuEdit_Copy_Click()
{
	//Copy()�������Ա�����ѡ���ݵ����а�
	form1.Control(IDC_Text_Main).Copy();
	return;
}
//ճ��������
void MenuEdit_Paste_Click()
{
	//Paste()�������Դ�ӡճ�����б�������ݵ��ı���
	form1.Control(IDC_Text_Main).Paste();
	return;
}
//�������Ϊ�����
void MenuEdit_FontType_SongTi_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("����"));
	form1.Menu(ID_MenuEdit_FontType_SongTi).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//�������Ϊ�����
void MenuEdit_FontType_HeiTi_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("����"));
	form1.Menu(ID_MenuEdit_FontType_HeiTi).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//�������Ϊ�����
void MenuEdit_FontType_LiShu_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("����"));
	form1.Menu(ID_MenuEdit_FontType_LiShu).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//��������ɫ��Ϊ��ɫ��
void MenuEdit_FontColor_Red_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(255, 0, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));				
	//���ڸı�ForeColor��ͬʱ��ı䱳��ɫ�����Խ��������ñ���ɫΪ��ɫ�������ı�������ɫ�Ĳ������ڴ�ͬ�����ٹ������
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Red).CheckedSet(true, true, 0, 3, true);
	return;
}
//��������ɫ��Ϊ��ɫ��
void MenuEdit_FontColor_Green_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 255, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Green).CheckedSet(true, true, 0, 3, true);
	return;
}
//��������ɫ��Ϊ��ɫ��
void MenuEdit_FontColor_Blue_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 0, 255));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Blue).CheckedSet(true, true, 0, 3, true);
	return;
}
//��������ɫ��Ϊ��ɫ��
void MenuEdit_FontColor_Black_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 0, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Black).CheckedSet(true, true, 0, 3, true);
	return;
}
//���ҹ��ܡ�
void MenuEdit_Find_Click()
{
	form_Find_PreSet();
	form_Find_Move();
	form_Find.Show(0, form1.hWnd());
	return;
}
// �滻���ܡ�
void MenuEdit_Replace_Click()
{
	form_Replace_PreSet();
	form_Replace_Move();
	form_Replace.Show(0, form1.hWnd());
	return;
}
//���ع��ܡ�
void MenuEdit_Undo_Click()
{
	//���ȼ�鳷�����ݴ���ռ��Ƿ�Ϊ�գ���Ϊ���������е��ʼ�Ĳ������޷��������г��ز���,ֱ���˳��ú���
	if (Undo_Total.empty()) 
		return;
	else 
	{
		// ɾ�������һ����¼
		Undo_Total.pop_back();
		if (!Undo_Total.empty()) 
		{
			auto& lastRecord = Undo_Total.back();
			unsigned int RedLoad, GreenLoad, BlueLoad;
			//ͨ��λ�������λ����������ȡ������ɫ������ֵ
			RedLoad = lastRecord.Undo_FontColor & 0xFF;
			GreenLoad = (lastRecord.Undo_FontColor >> 8) & 0xFF;
			BlueLoad = (lastRecord.Undo_FontColor >> 16) & 0xFF;
			//�����ı�������Ϊ��һ�β��������������
			form1.Control(IDC_Text_Main).TextSet(lastRecord.Undo_Text);
			//�л�������ɫ
			if (RedLoad == 255 && GreenLoad == 0 && BlueLoad == 0)
				MenuEdit_FontColor_Red_Click();
			if (RedLoad == 0 && GreenLoad == 255 && BlueLoad == 0)
				MenuEdit_FontColor_Green_Click();
			if (RedLoad == 0 && GreenLoad == 0 && BlueLoad == 255)
				MenuEdit_FontColor_Blue_Click();
			if (RedLoad == 0 && GreenLoad == 0 && BlueLoad == 0)
				MenuEdit_FontColor_Black_Click();
			//�л���������
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("����")) == 0)
				MenuEdit_FontType_HeiTi_Click();
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("����")) == 0)
				MenuEdit_FontType_SongTi_Click();
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("����")) == 0)
				MenuEdit_FontType_LiShu_Click();
			//ˢ���ı���,��ֹ����չʾ����
			form1.Control(IDC_Text_Main).Refresh();
		}
	}
	return;
}
//ȫѡ�ı����ݡ�
void MenuEdit_SetAll_Click()
{
	//SelSet()��������ѡ���ı����е������ַ�����
	form1.Control(IDC_Text_Main).SelSet();
	return;
}
//����ʱ���
void MenuEdit_DateTime_Click()
{
	LPTSTR DataTime = StrAppend(TEXT("�����Ǳ���ʱ�䣺"), Now());
	form1.Control(IDC_Text_Main).SelTextSet(StrAppend(TEXT("\r\n"), DataTime));
	MsgBox(DataTime, TEXT("���ں�ʱ��"));
	return;
}
//�༭�˵��µĸ����Ӳ˵�������
/////////////////////////////
/////////////////////////////
//�鿴�˵��µĸ����Ӳ˵�������

//�Ƿ���ʾ״̬����
void MenuView_Status_Click()
{
	//ÿ�ε���л�״̬���Ƿ���ʾ(��ʾ/����)
	if (if_ShowingStatus)
	{
		form1.Control(IDC_StaticText_Main).VisibleSet(false);
		form1.Menu(ID_MenuView_Status).CheckedSet(false);
		if_ShowingStatus = false;
	}
	else
	{
		form1.Control(IDC_StaticText_Main).VisibleSet(true);
		form1.Menu(ID_MenuView_Status).CheckedSet(true);
		if_ShowingStatus = true;
	}
	form1_Resize();
	return;
}
//�Ƿ�Դ���������������
void MenuView_TopMost_Click()
{
	//ÿ�ε���л����ô����Ƿ��������(��/��)
	if_TopMost = !if_TopMost;
	if (if_TopMost)
		SetWindowPos(form1.hWnd(), HWND_TOPMOST, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(form1.hWnd(), HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	form1.Menu(ID_MenuView_TopMost).CheckedSet(if_TopMost);
	return;
}
//�鿴�˵��µĸ����Ӳ˵�������
/////////////////////////////
/////////////////////////////
//�����˵��µĸ����Ӳ˵�������

//��ʾ����������Ϣ��
void MenuHelp_About_Click()
{
	//MsgBox(TEXT("�� �� �� �� ��\t�汾��1.0\r\n���ߣ�Ѧ����\t����ѧҽѧԺ\r\n������������ϵ���䣺1537361071@qq.com\r\n\r\n��Ȩ����(C) 2024"),
	//			TEXT("���� - �ҵļ��±�"),mb_OK,mb_IconInformation);
	form_About_PreSet();
	form_About_Move();
	form_About.Show(1,form1.hWnd());
	return;
}
//��help�ĵ���
void MenuHelp_HelpHTM_Click()
{
	LPTSTR HelpFile;
	HelpFile = StrAppend(pApp->Path(), TEXT("Mytxt.chm"));
	SEShellRun(HelpFile);
	return;
}
//�����˵��µĸ����Ӳ˵�������
/////////////////////////////
/////////////////////////////
//�˵����������
void Main_Menu_Click(int menuID, int bIsFromAcce, int bIsFromSysMenu)
{
	//ʵ�������������������Ĳ˵����Ӳ˵��������
	//ÿ���˵�ID�·���Ӧ�ľ��ǵ���ò˵���ʵ�ֵĹ��ܣ��ٴβ�����ע��
	switch (menuID)
	{
	//�ļ��˵��µĸ����Ӳ˵�������
	case ID_MenuFile_Exit:
		MainApplication_Exit();
		break;
	case ID_MenuFile_New:
		MenuFile_New_Click();
		break;
	case ID_MenuFile_Open:
		MenuFile_Open_Click();
		break;
	case ID_MenuFile_Save:
		if (*save_FileName == 0)
			Main_Menu_Click(ID_MenuFile_SaveAs, bIsFromAcce, bIsFromSysMenu);
		else
			SaveFile(save_FileName, AimKey);
		break;
	case ID_MenuFile_SaveAs:
		MenuFile_SaveAs_Click();
		break;
	//�ļ��˵��µĸ����Ӳ˵�������
	/////////////////////////////
	/////////////////////////////
	//�༭�˵��µĸ����Ӳ˵�������
	case ID_MenuEdit_Cut:
		MenuEdit_Cut_Click();
		break;
	case ID_MenuEdit_Copy:
		MenuEdit_Copy_Click();
		break;
	case ID_MenuEdit_Paste:
		MenuEdit_Paste_Click();
		break;
	case ID_MenuEdit_FontType_SongTi:
		MenuEdit_FontType_SongTi_Click();
		break;
	case ID_MenuEdit_FontType_HeiTi:
		MenuEdit_FontType_HeiTi_Click();
		break;
	case ID_MenuEdit_FontType_LiShu:
		MenuEdit_FontType_LiShu_Click();
		break;
	case ID_MenuEdit_FontColor_Red:
		MenuEdit_FontColor_Red_Click();
		break;
	case ID_MenuEdit_FontColor_Green:
		MenuEdit_FontColor_Green_Click();
		break;
	case ID_MenuEdit_FontColor_Blue:
		MenuEdit_FontColor_Blue_Click();
		break;
	case ID_MenuEdit_FontColor_Black:
		MenuEdit_FontColor_Black_Click();
		break;
	case ID_MenuEdit_Find:
		MenuEdit_Find_Click();
		break;
	case ID_MenuEdit_Replace:
		MenuEdit_Replace_Click();
		break;
	case ID_MenuEdit_Undo:
		MenuEdit_Undo_Click();
		break;
	case ID_MenuEdit_SetAll:
		MenuEdit_SetAll_Click();
		break;
	case ID_MenuEdit_DateTime:
		MenuEdit_DateTime_Click();
		break;
	//�༭�˵��µĸ����Ӳ˵�������
	/////////////////////////////
	/////////////////////////////
	//�鿴�˵��µĸ����Ӳ˵�������
	case ID_MenuView_Status:
		MenuView_Status_Click();
		break;
	case ID_MenuView_TopMost:
		MenuView_TopMost_Click();
		break;
	//�鿴�˵��µĸ����Ӳ˵�������
	/////////////////////////////
	/////////////////////////////
	//���߲˵��µĸ����Ӳ˵�������
	case ID_MenuTool_Run:
		form_Run.Show(0, form1.hWnd());
		break; 
	case ID_MenuTool_ListFiles:
		form_FileLists.Show(0, form1.hWnd());
		break;
	case ID_MenuTool_TextCount:
		form_StringCount_main();
		break;
	//���߲˵��µĸ����Ӳ˵�������
	/////////////////////////////
	/////////////////////////////
	//�����˵��µĸ����Ӳ˵�������
	case ID_MenuHelp_About:
		MenuHelp_About_Click();
		break;
	case ID_MenuHelp_HelpHTM:
		MenuHelp_HelpHTM_Click();
		break;
	//�����˵��µĸ����Ӳ˵�������
	/////////////////////////////
	/////////////////////////////
	//�����˵��µĸ����Ӳ˵�������
	case ID_MenuPop_Open:
		Main_Menu_Click(ID_MenuFile_Open, bIsFromAcce, bIsFromSysMenu);
		break;
	case ID_MenuPop_Save:
		Main_Menu_Click(ID_MenuFile_Save, bIsFromAcce, bIsFromSysMenu);
		break;
	case ID_MenuPop_About:
		Main_Menu_Click(ID_MenuHelp_About, bIsFromAcce, bIsFromSysMenu);
		break;
	default:
		break;
	}
	return;
}
//�����˵������ı���Ĵ�����
void Text_Main_MouseDown(int button, int shift, int x, int y)
{
	//button == 2 Ϊ����Ҽ�
	if (button == 2)
	{
		x = form1.Control(IDC_Text_Main).Left() + x;
		y = form1.Control(IDC_Text_Main).Top() + y;
		form1.PopupMenu(IDR_MENU2, x, y);
	}
	return;
}

void Text_Main_KeyPress(int button, int shift)
{
	//button == 9 ΪTab����
	if (button == 9)
	{
		form1.Control(IDC_Text_Main).SelSet(-1);
		form1.Control(IDC_Text_Main).SelTextSet(TEXT("\t"));
	}
	return;
}
//�����˵���״̬���Ĵ�����
void StaticText_Main_MouseDown(int button, int shift, int x, int y)
{
	//button == 2 Ϊ����Ҽ�
	if (button == 2)
	{
		x = form1.Control(IDC_StaticText_Main).Left() + x;
		y = form1.Control(IDC_StaticText_Main).Top() + y;
		form1.PopupMenu(IDR_MENU2, x, y);
	}
	return;
}
//�رճ���ʱѯ�ʡ�
void form_QuesWhenUnload(int pCancel)
{
	//���û����Ƿ񱣴�״̬��ѡ��û��ѡ�������ǣ���ȡ���رղ���
	if (Ques_ifSaveNeed())
		*(int*)pCancel = 1;
	return;
}
//���ı������ݸı�ʱ�������ú�����
void Text_Main_Change()
{
	//���ı����ݱ��ı䣬���ı��Ƿ񱻸Ķ��������Ϊtrue
	if_TextChanged = true;
	form1.Control(IDC_Text_Main).Refresh();
	//ÿ�θı��ı��ͽ���ǰ���ݴ��볷�����ݱ���ռ䣬�Ա��ں����ĳ��ز���
	if (!Undo_Total.empty())
	{
		LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
		COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
		LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
		Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });
	}
	else
	{
		LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
		COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
		LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
		// �������Ϊ�գ����ǵ�һ�β���
		Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });
	}

	/*���������Ϣ������ʾ��
	auto& UndoTest = Undo_Total.back();
	LPTSTR TESTTEXT = UndoTest.Undo_Text;
	TESTTEXT = StrAppend(TESTTEXT, Str(UndoTest.Undo_FontColor), UndoTest.Undo_FontType);
	MsgBox(TESTTEXT);*/
	return;
}
//ʵ���϶��������ļ��Ĺ��ܡ�
void Text_Main_FileDrop(int ptrArrFiles,int count,int x,int y)
{
	LPTSTR* AimFile = (LPTSTR*)ptrArrFiles;
	//����Ŀ���ļ�Ϊ���ָ��Ŀǰ��ѡ�е��ļ�
	if (Ques_ifSaveNeed())
		return;
	if (InputKey())
		OpenFile(AimFile[1], AimKey);
}
//״̬����ʾ��ǰָ���λ��
void Menu_Select(int ptrCBMenuItem)
{
	CBMenuItem* pMenu = (CBMenuItem*)ptrCBMenuItem;
	LPTSTR ptrPossitionNow = TEXT("");
	if (ptrCBMenuItem)
	{
		if (pMenu->IsSeparator())
			ptrPossitionNow = TEXT("��ָ����һ���˵��ķָ���");
		else
		{
			ptrPossitionNow = TEXT("��ָ���˲˵���");
			ptrPossitionNow = StrAppend(ptrPossitionNow, pMenu->Text());
			ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" λ���ǣ�"), Str(pMenu->Position()));
			if (pMenu->hSubMenu())
				ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" �������Ĳ˵�����ǣ�"), Str((unsigned int)pMenu->hSubMenu()));
			else
				ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" ��ԴID�ǣ�"), Str(pMenu->IDRes()));
		}
	}
	form1.Control(IDC_StaticText_Main).TextSet(ptrPossitionNow);
	return;
}

void form1_Unload()
{
	SetWindowPos(form1.hWnd(), HWND_NOTOPMOST, 0, 0, 0, 0,
		SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	form_InputKey.UnLoad();
	//�������ڴ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_About.UnLoad();
	//�������Ҵ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_Find.UnLoad();
	//�����滻����,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_Replace.UnLoad();
	//�������д���,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_Run.UnLoad();
	//�������ļ�Ŀ¼����,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_FileLists.UnLoad();
	//�����ĵ����ִ���,������main.cpp��ִ�иô��ڵĲ��ֲ���
	form_Main.UnLoad();
	return;
}

int main()
{
	form1.EventAdd(0, eForm_Resize, form1_Resize);
	form1.EventAdd(IDC_Text_Main, eMouseWheel, Text_Main_MouseWheel);
	form1.EventAdd(0, eMenu_Select, Menu_Select);
	form1.EventAdd(0, eMenu_Click, Main_Menu_Click);
	form1.EventAdd(IDC_Text_Main, eEdit_Change, Text_Main_Change);
	form1.EventAdd(0, eForm_QueryUnload, form_QuesWhenUnload);
	form1.EventAdd(IDC_Text_Main, eFilesDrop, Text_Main_FileDrop);
	form1.EventAdd(IDC_Text_Main, eMouseDown, Text_Main_MouseDown);
	//form1.EventAdd(IDC_Text_Main,eKeyPress,Text_Main_KeyPress);

	form1.EventAdd(IDC_StaticText_Main, eMouseDown, StaticText_Main_MouseDown);
	form1.EventAdd(0, eForm_Unload, form1_Unload);
	//���������ؼ����书�ܺ�����
	///////////////////////////////////
	///////////////////////////////////
	//Ԥ�����������봰�ڡ�
	form_InputKey_main();
	//Ԥ������ڴ��ڡ�
	form_About_main();
	//Ԥ������Ҵ��ڡ�
	form_Find_main();
	//Ԥ�����滻���ڡ�
	form_Replace_main();
	//Ԥ�������д��ڡ�
	form_Run_main();
	//Ԥ�������ļ�Ŀ¼���ڡ�
	form_FileLists_main();

	//���ÿ�ݼ���
	form1.SetAccelerator(IDR_ACCELERATOR1);
	//ִ�������ڵ�Ԥ�����
	PreSet();
	//��ʾ�����ڡ�
	form1.IconSet(IDI_ICON1);
	form1.Show();
	return 0;
}
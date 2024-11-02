#include "resource.h"
#include "BForm.h"
#include <cctype>  

CBForm form_Main(IDD_MainForm);

extern void Login_main();
bool if_LoginReady = false;
bool if_Exit = false;

//�ж��Ƿ�Ϊ���ĵĺ�����
bool isChinese(const wchar_t& c) 
{
	// ��Unicode�����У������ַ��ķ�Χ��0x4E00��0x9FFF
	return c >= 0x4E00 && c <= 0x9FFF;
}
//ͳ����Ч�ַ������֡���ĸ�����֣������ĺ�����
int countValidCharacters(LPTSTR String_in) 
{
	// ���������UNICODE����s�ǿ��ַ��ַ���
	int count = 0;
	for (int i = 0; i < wcslen(String_in); i++) 
	{
		if (iswalpha(String_in[i]) || iswdigit(String_in[i])) 
		{
			// Ӣ����ĸ������
			count++;
		}
		else if (isChinese(String_in[i])) 
		{
			// �����ַ�
			count++;
		}
	}
	return count;
}

void Button_MainExit_Click()
{
	form_Main.UnLoad();
	return;
}

int Get_Text_Position()
{
	int AimPosition = 5;
	if (lstrcmp(form_Main.Control(IDC_Text_Position).Text(), TEXT("")) == 0)
		return AimPosition;
	AimPosition = (int)form_Main.Control(IDC_Text_Position).TextVal();
	return AimPosition;
}

//Text_PackageNumber_C�ı���İ�����չ��
void Text_Position_KeyPress(int KeyAscii, int pbCancel)
{
	//Ascii���� ECS=27 �˸�=8
	if (KeyAscii == 27)
		Button_MainExit_Click();
	else if (!(KeyAscii >= '0' && KeyAscii <= '9' || KeyAscii == 8))
		*(int*)pbCancel = 1;
	return;
}

LPTSTR Get_Text_AimS()
{
	LPTSTR AimS = TEXT("abc");
	if (lstrcmp(form_Main.Control(IDC_Text_AimS).Text(), TEXT("")) == 0)
		return AimS;
	AimS = form_Main.Control(IDC_Text_AimS).Text();
	return AimS;
}

LPTSTR Get_Text_DividePoint()
{
	LPTSTR DividePoint = TEXT(",");
	if (lstrcmp(form_Main.Control(IDC_Text_DividePoint).Text(), TEXT("")) == 0)
		return DividePoint;
	DividePoint = form_Main.Control(IDC_Text_DividePoint).Text();
	return DividePoint;
}

void Text_AimString_Change()
{
	LPTSTR AimString = form_Main.Control(IDC_Text_AimString).Text();
	int Length_AimString = _tcslen(AimString);
	int AimLocation = Get_Text_Position();
	LPTSTR AimS = Get_Text_AimS();
	LPTSTR DividePoint = Get_Text_DividePoint();
	LPTSTR Result_Info = TEXT("������������ǣ�\r\n");
	Result_Info = StrAppend(Result_Info, AimString, TEXT("\r\n"));
	Result_Info = StrAppend(Result_Info, TEXT("�ַ����ĳ����ǣ�"), Str(Length_AimString), TEXT("\r\n"));
	int Length_AbleString = 0;;
	//ͳ����Ч�ַ������֡���ĸ�����֣�����
	if (Length_AimString > 0)
	{
		Length_AbleString = countValidCharacters(AimString);
		LPTSTR TextTemp = TEXT("��Ч�ַ������֡���ĸ�����֣����У�");
		TextTemp = StrAppend(TextTemp, Str(Length_AbleString), TEXT("����\r\n"));
		Result_Info = StrAppend(Result_Info, TextTemp);
	}
	
	if (Length_AimString > 0)
	{
		LPTSTR TempString = new TCHAR[2];
		TempString[0] = AimString[0]; // ��ֵ
		TempString[1] = 0;
		Result_Info = StrAppend(Result_Info, TEXT("�ַ����е�"), Str(1), TEXT("���ַ��ǣ�["), TempString, TEXT("]��\r\n"));
		delete[] TempString;
	}
	if (Length_AimString > AimLocation - 1)
	{
		LPTSTR TempString = new TCHAR[2];
		TempString[0] = AimString[AimLocation - 1]; // ��ֵ
		TempString[1] = 0;
		Result_Info = StrAppend(Result_Info, TEXT("�ַ����е�"), Str(AimLocation), TEXT("���ַ��ǣ�["), TempString, TEXT("]��\r\n"));
		delete[] TempString;
	}
	LPTSTR AimS_Possition = _tcsstr(AimString, AimS);
	if (AimS_Possition != NULL)
	{
		int Possiton = AimS_Possition - AimString;
		Result_Info = StrAppend(Result_Info, TEXT("�ַ�������["), AimS, TEXT("]��λ�ã�"), Str(Possiton), TEXT("��\r\n"));
	}
	else
		Result_Info = StrAppend(Result_Info, TEXT("�ַ�������["), AimS, TEXT("]��λ�ã�"), TEXT("δ�ҵ����ַ�����\r\n"));

	LPTSTR* p;
	int DivideCnt;
	DivideCnt = Split(AimString, p, DividePoint);
	Result_Info = StrAppend(Result_Info, TEXT("���ָ���["), DividePoint, TEXT("]�ֽ�õ������ַ������£�\r\n"));
	for (int i = 1; i <= DivideCnt; i++)
	{
		Result_Info = StrAppend(Result_Info, TEXT("["), Str(i), TEXT("]��"));
		Result_Info = StrAppend(Result_Info, p[i], TEXT("\r\n"));
	}

	form_Main.Control(IDC_Text_Result).TextSet(Result_Info);
	return;
}

void Main_PreSet()
{
	form_Main.Control(IDC_Text_AimString).TextSet(TEXT(""));

	form_Main.Move((pApp->ScreenWidth() - form_Main.Width()) / 2,
		(pApp->ScreenHeight() - form_Main.Height()) / 2);

	return;
}

void form1_Question_Unload(int pCancel)
{
	if (MsgBox(TEXT("ȷ��Ҫ�˳���"), TEXT("ȷ���˳�"), mb_YesNo, mb_IconQuestion) == idNo)
		*((int*)pCancel) = 1;
	return;
}

int form_StringCount_main()
{
	form_Main.EventAdd(IDC_Button_MainExit, eCommandButton_Click, Button_MainExit_Click);
	form_Main.EventAdd(IDC_Text_Position, eKeyPress, Text_Position_KeyPress);
	form_Main.EventAdd(IDC_Text_AimString, eTextBox_Change, Text_AimString_Change);
	form_Main.EventAdd(IDC_Text_Position, eTextBox_Change, Text_AimString_Change);
	form_Main.EventAdd(IDC_Text_AimS, eTextBox_Change, Text_AimString_Change);
	form_Main.EventAdd(IDC_Text_DividePoint, eTextBox_Change, Text_AimString_Change);
	Login_main();

	while (1)
	{
		if (if_LoginReady)
			break;
		if (if_Exit)
			return 0;
		DoEvents();
	}

	form_Main.IconSet(IDI_ICON1);
	form_Main.Show();
	form_Main.EventAdd(0, eForm_QueryUnload, form1_Question_Unload);

	return 0;
}
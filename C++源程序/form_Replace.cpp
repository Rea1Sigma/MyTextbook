#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form_Replace(IDD_form_Replace);
extern CBForm form1;

int formReplace_DoFind()
{
	CBControl Text_Main(IDC_Text_Main);
	tstring TextAll = Text_Main.Text();
	tstring AimText = form_Replace.Control(IDC_Combo_ReplaceInfo).Text();

	if (AimText == TEXT(""))
	{
		MsgBox(TEXT("������������ݣ�"), TEXT("δ����������ݡ�"), mb_OK, mb_IconExclamation);
		return -1;
	}

	if (form_Replace.Control(IDC_Check_ReplaceCaseSens).ValueChecked() == 0)
	{
		CharUpper((LPTSTR)TextAll.c_str());
		CharUpper((LPTSTR)AimText.c_str());
	}

	int Direction = 1;
	if (form_Replace.Control(IDC_Radio_ReplaceUp).ValueChecked())
		Direction = -1;

	int StartPosition = Text_Main.SelStart();
	if (Direction > 0)
		StartPosition += Text_Main.SelLength();
	else
		StartPosition -= Text_Main.SelLength();

	int FindPositon;
	if (Direction > 0)
		FindPositon = TextAll.find(AimText, StartPosition);
	else
		FindPositon = TextAll.rfind(AimText, StartPosition);

	if (string::npos == FindPositon ||
		Direction > 0 && FindPositon < StartPosition ||
		Direction < 0 && FindPositon > StartPosition)
	{
		//δ�ҵ�
		MsgBox(TEXT("δ�ҵ��������ݣ�"), TEXT("����"), mb_OK, mb_IconExclamation);
		return -1;
	}
	else
	{
		//�ҵ�����
		Text_Main.SelSet(FindPositon, AimText.length());
		SendMessage(Text_Main.hWnd(), EM_SCROLLCARET, 0, 0);
		return FindPositon;
	}
}

void form_Replace_Move()
{
	form_Replace.Move(
		form1.Left() + (form1.Width() - form_Replace.Width()) / 2,
		form1.Top() + (form1.Height() - form_Replace.Height()) / 2
	);
	return;
}

void form_Replace_PreSet()
{
	form_Replace.Control(IDC_Radio_ReplaceDown).ValueCheckedSet(true);
	form_Replace.IconSet(IDI_ICON1);
	return;
}

void Button_ReplaceNext_Click()
{
	formReplace_DoFind();
	return;
}

void Button_ReplaceSingle_Click()
{
	CBControl Text_Main(IDC_Text_Main);
	tstring Replace_Text = form_Replace.Control(IDC_Combo_ReplaceAs).Text();
	if (Replace_Text == TEXT(""))
	{
		MsgBox(TEXT("������Ҫ�滻�����ݣ�"), TEXT("δ�����滻���ݡ�"), mb_OK, mb_IconExclamation);
		return;
	}

	int FindPositon = formReplace_DoFind();
	if (FindPositon >= 0)
	{
		//�ҵ�Ŀ��λ�ã�����ѡ��,��ֱ���滻ѡ�����������
		Text_Main.SelTextSet(Replace_Text);
		//ѡ���滻�������
		Text_Main.SelSet(FindPositon, Replace_Text.length());
	}
	return;
}

void Button_ReplaceAll_Click()
{
	CBControl Text_Main(IDC_Text_Main);
	tstring TextAll = Text_Main.Text();
	tstring TextOrigin = Text_Main.Text();
	tstring AimText = form_Replace.Control(IDC_Combo_ReplaceInfo).Text();
	tstring Replace_Text = form_Replace.Control(IDC_Combo_ReplaceAs).Text();

	if (AimText == TEXT(""))
	{
		MsgBox(TEXT("������Ҫ���ҵ����ݣ�"), TEXT("δ����������ݡ�"), mb_OK, mb_IconExclamation);
		return;
	}

	if (Replace_Text == TEXT(""))
	{
		MsgBox(TEXT("������Ҫ�滻�����ݣ�"), TEXT("δ�����滻���ݡ�"), mb_OK, mb_IconExclamation);
		return;
	}

	if (form_Replace.Control(IDC_Check_ReplaceCaseSens).ValueChecked())
	{
		CharUpper((LPTSTR)TextAll.c_str());
		CharUpper((LPTSTR)AimText.c_str());
	}

	tstring Result(TEXT(""));
	int StartPosition = 0, FindPosition = -1, Times = 0;

	FindPosition = TextAll.find(AimText, StartPosition);

	while (FindPosition != string::npos)
	{
		Times++;

		Result += TextOrigin.substr(StartPosition, FindPosition - StartPosition);

		Result += Replace_Text;

		StartPosition = FindPosition + AimText.length();

		FindPosition = TextAll.find(AimText, StartPosition);
	}
	Result += TextOrigin.substr(StartPosition);
	Text_Main.TextSet(Result);
	LPTSTR ReplaceFeedback = TEXT("");
	ReplaceFeedback = StrAppend(ReplaceFeedback, TEXT("����� "), Str(Times), TEXT(" ���滻��"));
	MsgBox(ReplaceFeedback, TEXT("ȫ���滻"), mb_OK, mb_IconExclamation);

	return;
}

void Button_ReplaceExit_Click()
{
	form_Replace.UnLoad();
	return;
}

void form_Replace_main()
{
	form_Replace.EventAdd(IDC_Button_ReplaceExit, eCommandButton_Click, Button_ReplaceExit_Click);
	form_Replace.EventAdd(IDC_Button_ReplaceNext, eCommandButton_Click, Button_ReplaceNext_Click);
	form_Replace.EventAdd(IDC_Button_ReplaceAll, eCommandButton_Click, Button_ReplaceAll_Click);
	form_Replace.EventAdd(IDC_Button_ReplaceSingle, eCommandButton_Click, Button_ReplaceSingle_Click);

	form_Replace_PreSet();
	form_Replace_Move();

	return;
}
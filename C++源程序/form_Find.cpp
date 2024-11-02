#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form_Find(IDD_form_Find);
extern CBForm form1;

int formFind_DoFind()
{
	CBControl Text_Main(IDC_Text_Main);
	tstring TextAll = Text_Main.Text();
	tstring AimText = form_Find.Control(IDC_Text_FindInfo).Text();

	if (AimText == TEXT(""))
	{
		MsgBox(TEXT("请输入查找内容！"), TEXT("未输入查找内容。"), mb_OK, mb_IconExclamation);
		return -1;
	}

	if (form_Find.Control(IDC_Check_FindCaseSens).ValueChecked() == 0)
	{
		CharUpper((LPTSTR)TextAll.c_str());
		CharUpper((LPTSTR)AimText.c_str());
	}

	int Direction = 1;
	if (form_Find.Control(IDC_Radio_FindUp).ValueChecked())
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
		//未找到
		MsgBox(TEXT("未找到查找内容！"), TEXT("查找"), mb_OK, mb_IconExclamation);
		return -1;
	}
	else
	{
		//找到内容
		Text_Main.SelSet(FindPositon, AimText.length());
		SendMessage(Text_Main.hWnd(), EM_SCROLLCARET, 0, 0);
		return FindPositon;
	}
}

void form_Find_Move()
{
	form_Find.Move(
		form1.Left() + (form1.Width() - form_Find.Width()) / 2,
		form1.Top() + (form1.Height() - form_Find.Height()) / 2
	);
	return;
}

void form_Find_PreSet()
{
	form_Find.Control(IDC_Radio_FindDown).ValueCheckedSet(true);
	form_Find.IconSet(IDI_ICON1);
	return;
}

void Button_FindNext_Click()
{
	formFind_DoFind();
	return;
}

void Button_FindExit_Click()
{
	form_Find.UnLoad();
	return;
}

void form_Find_main()
{
	form_Find.EventAdd(IDC_Button_FindExit, eCommandButton_Click, Button_FindExit_Click);
	form_Find.EventAdd(IDC_Button_FindNext, eCommandButton_Click, Button_FindNext_Click);

	form_Find_PreSet();
	form_Find_Move();

	return;
}
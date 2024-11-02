#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"
#include "mdlShellExec.h"

CBForm form_Run(IDD_form_Run);

void form_Run_PreSet()
{
	// ����������ʾ��ʽ������Ͽ�
	CBControl  cboExeStyle(IDC_Combo_RunFileStyle);
	cboExeStyle.AddItem(TEXT("SW_HIDE(=0)�����ش���"));
	cboExeStyle.AddItem(TEXT("SW_SHOWNORMAL(=1)����������(����󻯻���С����ԭ)"));
	cboExeStyle.AddItem(TEXT("SW_SHOWMINIMIZED(=2)����С������"));
	cboExeStyle.AddItem(TEXT("SW_SHOWMAXIMIZED(=3)����󻯴���"));
	cboExeStyle.AddItem(TEXT("SW_SHOWNOACTIVATE(=4)����ʾ���ڵ���������"));
	cboExeStyle.AddItem(TEXT("SW_SHOW(=5)����ʾ����(����״̬����)"));
	cboExeStyle.ListIndexSet(2);
	form_Run.KeyPreview = true;

	form_Run.IconSet(IDI_ICON3);
	form_Run.Control(IDC_StaticText_Run).FontSizeSet(14);

	return;
}

void Button_RunOk_Click()
{
	LPTSTR AimProgram = form_Run.Control(IDC_Combo_RunFile).Text();
	//��δѡ���κ�Ŀ�������ֱ���˳����塣
	if (*AimProgram == TEXT('\0'))
	{
		MsgBox(TEXT("������Ҫ���еĳ���"), TEXT("����ѡ���������"), mb_OK, mb_IconExclamation);
		return;
	}

	form_Run.Control(IDC_Combo_RunFile).AddItem(AimProgram, 1);

	SEShellRun(AimProgram, true, (EShowWindowCmd)(form_Run.Control(IDC_Combo_RunFileStyle).ListIndex() - 1));
	
	return;
}

void Button_RunBrowsFile_Click()
{
	LPTSTR FileList;
	OsdSetFilter(TEXT("����|*.exe;*.com;*.bat;*.cmd;*.pif;*.lnk"));
	FileList = OsdOpenDlg(form_Run.hWnd(), TEXT("���"));
	if (*FileList)
		form_Run.Control(IDC_Combo_RunFile).TextSet(StrAppend(TEXT("\""), FileList, TEXT("\"")));
	return;
}

void Button_RunCancel_Click()
{
	form_Run.UnLoad();
	return;
}

void form_Run_KeyUp(int keyCode, int shift, int pbCancel)
{
	if (keyCode == 13)
		Button_RunOk_Click();
	return;
}

void form_Run_main()
{
	form_Run.EventAdd(0, eForm_Load, form_Run_PreSet);
	form_Run.EventAdd(IDC_Button_RunBrowsFile, eCommandButton_Click, Button_RunBrowsFile_Click);
	form_Run.EventAdd(IDC_Button_RunOk, eCommandButton_Click, Button_RunOk_Click);
	form_Run.EventAdd(IDC_Button_RunCancel, eCommandButton_Click, Button_RunCancel_Click);
	form_Run.EventAdd(0, eKeyUp, form_Run_KeyUp);

	return;
}
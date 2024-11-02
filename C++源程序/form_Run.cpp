#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"
#include "mdlShellExec.h"

CBForm form_Run(IDD_form_Run);

void form_Run_PreSet()
{
	// 将各窗口显示方式加入组合框
	CBControl  cboExeStyle(IDC_Combo_RunFileStyle);
	cboExeStyle.AddItem(TEXT("SW_HIDE(=0)：隐藏窗口"));
	cboExeStyle.AddItem(TEXT("SW_SHOWNORMAL(=1)：正常激活(若最大化或最小化则还原)"));
	cboExeStyle.AddItem(TEXT("SW_SHOWMINIMIZED(=2)：最小化窗口"));
	cboExeStyle.AddItem(TEXT("SW_SHOWMAXIMIZED(=3)：最大化窗口"));
	cboExeStyle.AddItem(TEXT("SW_SHOWNOACTIVATE(=4)：显示窗口但不激活它"));
	cboExeStyle.AddItem(TEXT("SW_SHOW(=5)：显示窗口(窗口状态不变)"));
	cboExeStyle.ListIndexSet(2);
	form_Run.KeyPreview = true;

	form_Run.IconSet(IDI_ICON3);
	form_Run.Control(IDC_StaticText_Run).FontSizeSet(14);

	return;
}

void Button_RunOk_Click()
{
	LPTSTR AimProgram = form_Run.Control(IDC_Combo_RunFile).Text();
	//若未选则任何目标程序，则直接退出窗体。
	if (*AimProgram == TEXT('\0'))
	{
		MsgBox(TEXT("请输入要运行的程序。"), TEXT("忘记选择程序啦！"), mb_OK, mb_IconExclamation);
		return;
	}

	form_Run.Control(IDC_Combo_RunFile).AddItem(AimProgram, 1);

	SEShellRun(AimProgram, true, (EShowWindowCmd)(form_Run.Control(IDC_Combo_RunFileStyle).ListIndex() - 1));
	
	return;
}

void Button_RunBrowsFile_Click()
{
	LPTSTR FileList;
	OsdSetFilter(TEXT("程序|*.exe;*.com;*.bat;*.cmd;*.pif;*.lnk"));
	FileList = OsdOpenDlg(form_Run.hWnd(), TEXT("浏览"));
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
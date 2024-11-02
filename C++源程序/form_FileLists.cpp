#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"
#include "mdlShellExec.h"
#include "mdlFileSys.h"
#include "mdlPathDlg.h"

CBForm form_FileLists(IDD_form_FileLists);
extern CBForm form1;

void form_FileLists_PreSet()
{
	CBControl cbo = form_FileLists.Control(IDC_Combo_Option);
	cbo.AddItem(TEXT("列出文件全路径"));
	cbo.AddItem(TEXT("列出文件夹全路径"));
	cbo.AddItem(TEXT("列出文件+文件夹全路径"));
	cbo.AddItem(TEXT("列出文件名"));
	cbo.AddItem(TEXT("列出文件夹名"));
	cbo.AddItem(TEXT("列出文件+文件夹名"));
	cbo.ListIndexSet(3);

	return;
}

bool ListFiles(LPCTSTR FilePath, int style, bool fRecu)
{
	TCHAR ** files = 0, ** folders = 0;
	int ctFiles = 0, ctFolders = 0;
	tstring sList(TEXT(""));

	FMListFilesAPI(FilePath, files, folders, &ctFiles, &ctFolders, style < 4);

	if (ctFiles && style != 2 && style != 5)
	{
		sList = TEXT("");
		//先将数组files中的各个文件夹名以\r\n间隔开
		for (int i = 1; i <= ctFiles; i++)
		{
			sList += files[i];
			sList += TEXT("\r\n");
		}
		//将间隔后的字符串显示到控件的当前插入点处
		form1.Control(IDC_Text_Main).SelTextSet(sList);
	}
	if (ctFolders)
	{
		//若ctFolders不为0，则代表有子文件夹
		sList = TEXT("");
		for (int i = 1; i <= ctFolders; i++)
		{
			//先将数组folders中的各个文件夹名以\r\n间隔开
			if (style != 1 && style != 4)
			{
				sList += folders[i];
				sList += TEXT("\r\n");
			}
			//若要继续读取子文件夹，则递归调用本函数
			if (fRecu)
			{
				//设置递归边界
				if (style < 4)
				{
					if (!ListFiles(folders[i], style, fRecu))
						return false;
				}
				else
				{
					if (!ListFiles(StrAppend(FMAddBackSlash(FilePath), folders[i]), style, fRecu))
						return false;
				}
			}
		}
		if (style != 1 && style != 4)
			form1.Control(IDC_Text_Main).SelTextSet(sList);
	}
	return true;
}

void Button_FileListsBrows_Click()
{
	LPTSTR FilePath = BrowPath(
		form_FileLists.hWnd(),												//浏览文件夹对话框的父窗口为本窗口
		TEXT("请选择您要列目录的一个文件夹："),								//浏览文件夹对话框中的提示文本
		true,														//进现实文件系统
		form_FileLists.Control(IDC_Text_FilePath).Text(),		//IDC_Text_FilePath文本框中的文件夹初始被自动选中
		false,															//仅显示目录，不显示文件
		true																//对话框包含了一个文本框
	);
	if (*FilePath)
		form_FileLists.Control(IDC_Text_FilePath).TextSet(FilePath);
	return;
}

void Button_FileListsOk_Click()
{
	ListFiles(
		form_FileLists.Control(IDC_Text_FilePath).Text(),
		form_FileLists.Control(IDC_Combo_Option).ListIndex(),
		form_FileLists.Control(IDC_Check_IncludeSon).ValueChecked()
	);

	form_FileLists.UnLoad();
	return;
}

void Button_FileListsCancel_Click()
{
	form_FileLists.UnLoad();
	return;
}

void form_FileLists_main()
{
	form_FileLists.EventAdd(0, eForm_Load, form_FileLists_PreSet);
	form_FileLists.EventAdd(IDC_Button_FileListsBrows, eCommandButton_Click, Button_FileListsBrows_Click);
	form_FileLists.EventAdd(IDC_Button_FileListsOk, eCommandButton_Click, Button_FileListsOk_Click);
	form_FileLists.EventAdd(IDC_Button_FileListsCancel, eCommandButton_Click, Button_FileListsCancel_Click);

	return;
}
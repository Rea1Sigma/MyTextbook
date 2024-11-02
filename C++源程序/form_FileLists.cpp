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
	cbo.AddItem(TEXT("�г��ļ�ȫ·��"));
	cbo.AddItem(TEXT("�г��ļ���ȫ·��"));
	cbo.AddItem(TEXT("�г��ļ�+�ļ���ȫ·��"));
	cbo.AddItem(TEXT("�г��ļ���"));
	cbo.AddItem(TEXT("�г��ļ�����"));
	cbo.AddItem(TEXT("�г��ļ�+�ļ�����"));
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
		//�Ƚ�����files�еĸ����ļ�������\r\n�����
		for (int i = 1; i <= ctFiles; i++)
		{
			sList += files[i];
			sList += TEXT("\r\n");
		}
		//���������ַ�����ʾ���ؼ��ĵ�ǰ����㴦
		form1.Control(IDC_Text_Main).SelTextSet(sList);
	}
	if (ctFolders)
	{
		//��ctFolders��Ϊ0������������ļ���
		sList = TEXT("");
		for (int i = 1; i <= ctFolders; i++)
		{
			//�Ƚ�����folders�еĸ����ļ�������\r\n�����
			if (style != 1 && style != 4)
			{
				sList += folders[i];
				sList += TEXT("\r\n");
			}
			//��Ҫ������ȡ���ļ��У���ݹ���ñ�����
			if (fRecu)
			{
				//���õݹ�߽�
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
		form_FileLists.hWnd(),												//����ļ��жԻ���ĸ�����Ϊ������
		TEXT("��ѡ����Ҫ��Ŀ¼��һ���ļ��У�"),								//����ļ��жԻ����е���ʾ�ı�
		true,														//����ʵ�ļ�ϵͳ
		form_FileLists.Control(IDC_Text_FilePath).Text(),		//IDC_Text_FilePath�ı����е��ļ��г�ʼ���Զ�ѡ��
		false,															//����ʾĿ¼������ʾ�ļ�
		true																//�Ի��������һ���ı���
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
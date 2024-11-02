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
//关联from_InputKey窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_InputKey;
//关联关于窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_About;
//关联查找窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_Find;
//关联替换窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_Replace;
//关联运行窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_Run;
//关联列文件目录窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_FileLists;
//关联文档助手窗口,便于在main.cpp里执行该窗口的部分操作
extern CBForm form_Main;

void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

CBTimer Timer1(0, 1000, Timer1_Timer);											//负责计时的时钟
CBTimer Timer2;																	//负责窗口淡入的函数

bool TextShow = false;

//Timer1的回调函数
void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	TCHAR szTimeStr[64];

	_stprintf(szTimeStr, TEXT("%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);

	form1.TextSet(StrAppend(TEXT("我的记事本--------现在是北京时间："),szTimeStr));
	return;
}

//Timer2的回调函数
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

bool if_TopMost;																//标记是否窗口置顶
bool if_ShowingStatus;															//标记是否显示状态栏
bool if_TextChanged;															//标记文本是否已被修改

bool if_InputKeyOk;																//标记是否已经设置密码
char * AimKey;																	//密码

int Text_Main_FontSize = 0;														//记录当前字号
TCHAR save_FileName[1024];														//文件保存后的名字

struct UndoSpace {

	LPTSTR Undo_Text;
	// 保存当前的文本内容，撤回时直接更新上一次保存的文本内容即可。
	COLORREF Undo_FontColor;
	// COLOREF 存储的是RGB二进制码，每八位存一个颜色，自低位至高位分别是红绿蓝
	// 如RGB（255,0,0）就是 {00000000，00000000，11111111}
	// 如RGB（255,0,255）就是 {11111111，00000000，11111111}
	// 最终是以数值的格式保存的,即int类型变量
	// 可以通过移位操作和位与操作提取其中存储的各个颜色变量
	LPTSTR Undo_FontType;
};

std::vector<UndoSpace> Undo_Total;												
//用vector来储存结构体，实现了动态结构体指针
//vector相较于常规的动态数组指针，维护更简单方便。



//关联InputKey窗口界面的函数，可以main中执行
extern void form_InputKey_PreSet();
extern void form_InputKey_main();
//关联About窗口界面的函数，可以main中执行
extern void form_About_main();
extern void form_About_PreSet();
//关联Find窗口界面的函数，可以main中执行
extern void form_Find_main();
extern void form_Find_PreSet();
extern void form_Find_Move();
//关联Replace窗口界面的函数，可以main中执行
extern void form_Replace_main();
extern void form_Replace_PreSet();
extern void form_Replace_Move();
//关联Run窗口界面的函数，可以main中执行
extern void form_Run_main();
//关联FileLists窗口界面的函数，可以main中执行
extern void form_FileLists_main();
//关联StringCount窗口界面的函数，可以main中执行
extern int form_StringCount_main();

//移动密码设置窗口至中心↓
void form_InputKey_Move()
{
	form_InputKey.Move(
		form1.Left()+(form1.Width()- form_InputKey.Width())/2,
		form1.Top()+(form1.Height()- form_InputKey.Height())/2
	);
	return;
}
//显示密码设置窗口，并返回是否成功设置密码
bool InputKey()
{

	form_InputKey.Load();

	form_InputKey_PreSet();

	form_InputKey_Move();

	if_InputKeyOk = false;

	form_InputKey.Show(1,form1.hWnd());

	return if_InputKeyOk;
}
//移动关于窗口至中心↓
void form_About_Move()
{
	form_About.Move(
		form1.Left() + (form1.Width() - form_About.Width()) / 2,
		form1.Top() + (form1.Height() - form_About.Height()) / 2
	);
	return;
}
//主窗口大小改变时，控件的位置大小随之改变↓
void form1_Resize()
{
	int StaticText_Main_Height = 0;
	if (if_ShowingStatus)
	{
		StaticText_Main_Height = form1.Control(IDC_StaticText_Main).Height()+6;
		form1.Control(IDC_StaticText_Main).Move(4, form1.ClientHeight() - StaticText_Main_Height + 4,
			form1.ClientWidth() - 4, StaticText_Main_Height - 6);
		//若需要显示状态栏，则调整状态栏与窗口同步
	}
	form1.Control(IDC_Text_Main).Move(0, 0, form1.ClientWidth(),
		form1.ClientHeight() - StaticText_Main_Height);
	//调整主文本框与窗口同步
	return;
}
//主窗口预处理↓
void PreSet()
{
	//初始化各个标记变量
	if_ShowingStatus = true;
	if_TopMost = false;
	if_TextChanged = false;
	
	save_FileName[0] = 0;														//初始化保存文件的名字
	//
	Text_Main_FontSize = form1.Control(IDC_Text_Main).FontSize();		//保存初始的字号大小，便于后续快捷调整字号大小
	form1_Resize();																//重置窗口位置
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("欢迎使用！"));
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
	//初始化各个控件和菜单↑
	//////////////////////////////////////////////
	//////////////////////////////////////////////
	//更新撤回储存结构体，保存当前的初始状态，便于后续撤回操作
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });

	return;
}
//打开文件的函数↓
bool OpenFile(LPCTSTR Aim_FileName,char * encKey)
{
	//初始化储存文本内容的变量
	LPTSTR ContentNow = TEXT("");
	LPTSTR Line = NULL, Temp = NULL;
	CBReadLinesEx FinalFiles;													
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));		//清空当前文本框，以便于存入即将打开的文件内容
	
	if (!FinalFiles.OpenFile(Aim_FileName))								//若文件打开失败，则返回false
		return false;

	FinalFiles.EncryptKey = encKey;												//取得目标文件的密码


	pApp->MousePointerGlobalSet(IDC_Wait);									//令用户指针转换为等待状态，以提示用户当前正在进行打开文档操作

	while (!FinalFiles.IsEndRead())												//IsEndRead()函数可以检测文档是否读到最后一行，若读到最后一行则返回true
	{
		//若文件尚未读取完，则一直循环，直到全部读取
		FinalFiles.GetNextLine(Line);
		if (FinalFiles.IsErrOccured())
		{
			//每次循环都检测文件读取是否失败，若文件读取失败则还原鼠标指针并终止函数。
			pApp->MousePointerGlobalSet(0);
			return false;
		}
		if (FinalFiles.iEndLineSign > 0)										
			ContentNow = StrAppend(ContentNow, TEXT("\r\n"));			//若该行不为空，则在末尾加入\r\n以便于连接下一行
		ContentNow = StrAppend(ContentNow, Line);						//连接下一行内容。
	}
	form1.Control(IDC_Text_Main).TextSet(ContentNow);			//在主文本框显示从目标文件中读取到的内容

	_tcscpy(save_FileName, Aim_FileName);										//目标文件内容复制给save_FileName保存

	form1.TextSet(save_FileName);											
	form1.TextAdd(TEXT(" - 我的记事本"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("文件打开成功!"));

	pApp->MousePointerGlobalSet(0);									//文件读取完成，将鼠标指针还原，以提示用户操作完成

	if_TextChanged = false;														//重置文本是否被改变标记

	return true;																//成功打开则返回true，否则返回false
}
//保存文件的函数↓
bool SaveFile(LPCTSTR Aim_FileName,char * encKey)
{
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));		
	HANDLE Handle_AimFile = EFOpen(Aim_FileName, EF_OpStyle_Output);
	// 用 API 函数打开文件，返回文件的句柄；失败返回 -1 （即 INVALID_HANDLE_VALUE 的值）
	EFPrint(Handle_AimFile, form1.Control(IDC_Text_Main).Text(),EF_LineSeed_None,-1,1,TEXT("无法向文件中写入字符串"), encKey);
	// 向文件 llWritePos 的位置打印一行字符
	// llWritePos 可为 <0 的值，此时为从当前读写位置开始写并不移动文件位置指针
	// iShowResume 指定如果读取出错，是否弹出对话框提示
	//   若 iShowResume=1，提示框中有“重试”和“取消”两个按钮；
	// 返回写入的字节数，若失败返回 < 0；若用户“忽略”则返回 -2；
	EFClose(Handle_AimFile);
	// 关闭打开的一个文件，参数 hFile 为 EFOpen 函数返回的文件句柄

	_tcscpy(save_FileName, Aim_FileName);										//复制目标文件名给保存文件名

	form1.TextSet(save_FileName);
	form1.TextAdd(TEXT(" - 我的记事本"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT("文件保存成功！"));

	if_TextChanged = false;														//重置文本是否被改变标记

	return true;																//若保存成功则返回true，否则返回false
}
//关闭程序时询问是否需要保存↓
bool Ques_ifSaveNeed()
{
	if (if_TextChanged)															//若文本已被更改后未保存，则执行如下：
	{
		EDlgBoxCmdID Ques_Message;												//保存MsgBox消息框的enum类型，存储的变量为消息框的按钮选项
		Ques_Message = MsgBox(TEXT("您对内容做了修改。\r\n是否保存文件？"), TEXT("我的记事本"), mb_YesNoCancel, mb_IconQuestion);
		if (Ques_Message == idYes)
		{
			//若用户点击确定按钮，则进行保存操作
			if (*save_FileName == 0)
			{
				LPTSTR FileSave;
				OsdSetFilter(TEXT("文本文件(*.txt)|*.txt"));
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
	//若询问窗口，用户既没选择“是”也没选择“否”，则代表用户并不想现在关闭窗口，此情况返回false
	//其他情况返回true
}
//缩放文字大小功能↓
void Text_Main_MouseWheel(int wParam, int lParam)
{
	
	int fwKeys, zDelta;
	int xPos, yPos;

	fwKeys = LOWORD(wParam);
	zDelta = (short)HIWORD(wParam);
	xPos = (short)LOWORD(lParam);
	yPos = (short)HIWORD(lParam);
	//用户的鼠标滚轮操作和键盘操作分别保存在高位和低位
	//需要进行位与操作提取
	if (fwKeys & MK_CONTROL && zDelta > 0)
	{
		//若用户按住Ctrl的同时向上滑动滚轮，则执行字体放大操作
		Text_Main_FontSize++;
		form1.Control(IDC_Text_Main).FontSizeSet(Text_Main_FontSize);
		return;
	}
	if (fwKeys & MK_CONTROL && zDelta < 0)
	{
		//若用户按住Ctrl的同时向下滑动滚轮，则执行字体缩小操作
		if (Text_Main_FontSize > 1)
		{
			//字号最小缩小到1，若执行此操作时字号已为1，则不执行该操作
			Text_Main_FontSize--;
			form1.Control(IDC_Text_Main).FontSizeSet(Text_Main_FontSize);
			return;
		}
	}
	return;
}
/////////////////////////////
/////////////////////////////
//文件菜单下的各个子菜单操作↓

//关闭主窗口↓
void MainApplication_Exit()
{
	form1.UnLoad();	
	return;
}
//创建新的文本↓
void MenuFile_New_Click()
{
	if (Ques_ifSaveNeed())														//若用户在关闭询问点击取消或关闭询问窗口，则不进行新建操作
		return;

	form1.Control(IDC_Text_Main).TextSet(TEXT(""));
	form1.TextSet(TEXT("我的记事本"));
	form1.Control(IDC_StaticText_Main).TextSet(TEXT(""));

	if_InputKeyOk = false;
	if_TextChanged = false;
	AimKey = NULL;
	*save_FileName = 0;
	//新建之后重置上述基本标记与密码↑
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	//打开新文件时，清空撤销保存空间的内容，用于保存新的文件内容。
	Undo_Total.clear();
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });
	return;
}
//另存为其他文档↓
void MenuFile_SaveAs_Click()
{
	//if (Ques_ifSaveNeed())														//若用户在关闭询问点击取消或关闭询问窗口，则不进行另存为操作
	//	return;
	LPTSTR FileSave;
	OsdSetFilter(TEXT("文本文件(*.txt)|*.txt"));
	FileSave = OsdSaveDlg(form1.hWnd());
	if (*FileSave)
		if (InputKey())
			SaveFile(FileSave, AimKey);							//设置保存密码
	return;
}
//打开已有文档的功能↓
void MenuFile_Open_Click()
{
	if (Ques_ifSaveNeed())														//若用户在关闭询问点击取消或关闭询问窗口，则不进行打开其他文档操作
		return;
	LPTSTR FileOpen;
	OsdSetFilter(TEXT("文本文件(*.txt)|*.txt"));
	FileOpen = OsdOpenDlg(form1.hWnd());
	if (*FileOpen)			
		if (InputKey())
			OpenFile(FileOpen, AimKey);							//用密码打开文档，若密码错误则会显示错误的文档内容

	//打开新文件时，清空撤销保存空间的内容，用于保存新的文件内容。
	Undo_Total.clear();
	LPTSTR TextNow = form1.Control(IDC_Text_Main).Text();
	COLORREF FontColorNow = form1.Control(IDC_Text_Main).ForeColor();
	LPTSTR FontTypeNow = form1.Control(IDC_Text_Main).FontName();
	Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });

	return;
}
//文件菜单下的各个子菜单操作↑
/////////////////////////////
/////////////////////////////
//编辑菜单下的各个子菜单操作↓

//剪切操作↓
void MenuEdit_Cut_Click()
{
	//Cut()函数可剪切选中的内容到剪切板
	form1.Control(IDC_Text_Main).Cut();
	//剪切完成后进行刷新，防止展示字符串序列混乱
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//复制操作↓
void MenuEdit_Copy_Click()
{
	//Copy()函数可以保存所选内容到剪切板
	form1.Control(IDC_Text_Main).Copy();
	return;
}
//粘贴操作↓
void MenuEdit_Paste_Click()
{
	//Paste()函数可以打印粘贴板中保存的内容到文本框
	form1.Control(IDC_Text_Main).Paste();
	return;
}
//将字体改为宋体↓
void MenuEdit_FontType_SongTi_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("宋体"));
	form1.Menu(ID_MenuEdit_FontType_SongTi).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//将字体改为黑体↓
void MenuEdit_FontType_HeiTi_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("黑体"));
	form1.Menu(ID_MenuEdit_FontType_HeiTi).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//将字体改为隶书↓
void MenuEdit_FontType_LiShu_Click()
{
	form1.Control(IDC_Text_Main).FontNameSet(TEXT("隶书"));
	form1.Menu(ID_MenuEdit_FontType_LiShu).CheckedSet(true, true, 0, 2, true);
	form1.Control(IDC_Text_Main).Refresh();
	return;
}
//将字体颜色改为红色↓
void MenuEdit_FontColor_Red_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(255, 0, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));				
	//由于改变ForeColor的同时会改变背景色，所以紧接着重置背景色为白色，其他改变字体颜色的操作均于此同理，则不再过多解释
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Red).CheckedSet(true, true, 0, 3, true);
	return;
}
//将字体颜色改为绿色↓
void MenuEdit_FontColor_Green_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 255, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Green).CheckedSet(true, true, 0, 3, true);
	return;
}
//将字体颜色改为蓝色↓
void MenuEdit_FontColor_Blue_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 0, 255));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Blue).CheckedSet(true, true, 0, 3, true);
	return;
}
//将字体颜色改为黑色↓
void MenuEdit_FontColor_Black_Click()
{
	form1.Control(IDC_Text_Main).ForeColorSet(RGB(0, 0, 0));					//RGB(red,green,blue)
	form1.Control(IDC_Text_Main).BackColorSet(RGB(255, 255, 255));
	form1.Control(IDC_Text_Main).Refresh();
	form1.Menu(ID_MenuEdit_FontColor_Black).CheckedSet(true, true, 0, 3, true);
	return;
}
//查找功能↓
void MenuEdit_Find_Click()
{
	form_Find_PreSet();
	form_Find_Move();
	form_Find.Show(0, form1.hWnd());
	return;
}
// 替换功能↓
void MenuEdit_Replace_Click()
{
	form_Replace_PreSet();
	form_Replace_Move();
	form_Replace.Show(0, form1.hWnd());
	return;
}
//撤回功能↓
void MenuEdit_Undo_Click()
{
	//首先检查撤回内容储存空间是否为空，若为空则代表进行到最开始的操作，无法继续进行撤回操作,直接退出该函数
	if (Undo_Total.empty()) 
		return;
	else 
	{
		// 删除最近的一条记录
		Undo_Total.pop_back();
		if (!Undo_Total.empty()) 
		{
			auto& lastRecord = Undo_Total.back();
			unsigned int RedLoad, GreenLoad, BlueLoad;
			//通过位与运算和位移运算来获取各个颜色变量的值
			RedLoad = lastRecord.Undo_FontColor & 0xFF;
			GreenLoad = (lastRecord.Undo_FontColor >> 8) & 0xFF;
			BlueLoad = (lastRecord.Undo_FontColor >> 16) & 0xFF;
			//更新文本框内容为上一次操作所保存的内容
			form1.Control(IDC_Text_Main).TextSet(lastRecord.Undo_Text);
			//切换字体颜色
			if (RedLoad == 255 && GreenLoad == 0 && BlueLoad == 0)
				MenuEdit_FontColor_Red_Click();
			if (RedLoad == 0 && GreenLoad == 255 && BlueLoad == 0)
				MenuEdit_FontColor_Green_Click();
			if (RedLoad == 0 && GreenLoad == 0 && BlueLoad == 255)
				MenuEdit_FontColor_Blue_Click();
			if (RedLoad == 0 && GreenLoad == 0 && BlueLoad == 0)
				MenuEdit_FontColor_Black_Click();
			//切换字体类型
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("黑体")) == 0)
				MenuEdit_FontType_HeiTi_Click();
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("宋体")) == 0)
				MenuEdit_FontType_SongTi_Click();
			if (lstrcmp(lastRecord.Undo_FontType, TEXT("隶书")) == 0)
				MenuEdit_FontType_LiShu_Click();
			//刷新文本框,防止内容展示混乱
			form1.Control(IDC_Text_Main).Refresh();
		}
	}
	return;
}
//全选文本内容↓
void MenuEdit_SetAll_Click()
{
	//SelSet()函数可以选中文本框中的所有字符内容
	form1.Control(IDC_Text_Main).SelSet();
	return;
}
//日期时间↓
void MenuEdit_DateTime_Click()
{
	LPTSTR DataTime = StrAppend(TEXT("现在是北京时间："), Now());
	form1.Control(IDC_Text_Main).SelTextSet(StrAppend(TEXT("\r\n"), DataTime));
	MsgBox(DataTime, TEXT("日期和时间"));
	return;
}
//编辑菜单下的各个子菜单操作↑
/////////////////////////////
/////////////////////////////
//查看菜单下的各个子菜单操作↓

//是否显示状态栏↓
void MenuView_Status_Click()
{
	//每次点击切换状态栏是否显示(显示/隐藏)
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
//是否对窗口设置置于最顶层↓
void MenuView_TopMost_Click()
{
	//每次点击切换设置窗口是否置于最顶层(是/否)
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
//查看菜单下的各个子菜单操作↑
/////////////////////////////
/////////////////////////////
//帮助菜单下的各个子菜单操作↓

//显示程序的相关信息↓
void MenuHelp_About_Click()
{
	//MsgBox(TEXT("我 的 记 事 本\t版本：1.0\r\n作者：薛茗阳\t天津大学医学院\r\n若有疑问请联系邮箱：1537361071@qq.com\r\n\r\n版权所有(C) 2024"),
	//			TEXT("关于 - 我的记事本"),mb_OK,mb_IconInformation);
	form_About_PreSet();
	form_About_Move();
	form_About.Show(1,form1.hWnd());
	return;
}
//打开help文档↓
void MenuHelp_HelpHTM_Click()
{
	LPTSTR HelpFile;
	HelpFile = StrAppend(pApp->Path(), TEXT("Mytxt.chm"));
	SEShellRun(HelpFile);
	return;
}
//帮助菜单下的各个子菜单操作↑
/////////////////////////////
/////////////////////////////
//菜单点击函数↓
void Main_Menu_Click(int menuID, int bIsFromAcce, int bIsFromSysMenu)
{
	//实现所有主窗口所关联的菜单的子菜单点击操作
	//每个菜单ID下方对应的就是点击该菜单后实现的功能，再次不过多注释
	switch (menuID)
	{
	//文件菜单下的各个子菜单操作↓
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
	//文件菜单下的各个子菜单操作↑
	/////////////////////////////
	/////////////////////////////
	//编辑菜单下的各个子菜单操作↓
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
	//编辑菜单下的各个子菜单操作↑
	/////////////////////////////
	/////////////////////////////
	//查看菜单下的各个子菜单操作↓
	case ID_MenuView_Status:
		MenuView_Status_Click();
		break;
	case ID_MenuView_TopMost:
		MenuView_TopMost_Click();
		break;
	//查看菜单下的各个子菜单操作↑
	/////////////////////////////
	/////////////////////////////
	//工具菜单下的各个子菜单操作↓
	case ID_MenuTool_Run:
		form_Run.Show(0, form1.hWnd());
		break; 
	case ID_MenuTool_ListFiles:
		form_FileLists.Show(0, form1.hWnd());
		break;
	case ID_MenuTool_TextCount:
		form_StringCount_main();
		break;
	//工具菜单下的各个子菜单操作↑
	/////////////////////////////
	/////////////////////////////
	//帮助菜单下的各个子菜单操作↓
	case ID_MenuHelp_About:
		MenuHelp_About_Click();
		break;
	case ID_MenuHelp_HelpHTM:
		MenuHelp_HelpHTM_Click();
		break;
	//帮助菜单下的各个子菜单操作↑
	/////////////////////////////
	/////////////////////////////
	//弹出菜单下的各个子菜单操作↓
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
//弹出菜单在主文本框的触发↓
void Text_Main_MouseDown(int button, int shift, int x, int y)
{
	//button == 2 为鼠标右键
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
	//button == 9 为Tab键盘
	if (button == 9)
	{
		form1.Control(IDC_Text_Main).SelSet(-1);
		form1.Control(IDC_Text_Main).SelTextSet(TEXT("\t"));
	}
	return;
}
//弹出菜单在状态栏的触发↓
void StaticText_Main_MouseDown(int button, int shift, int x, int y)
{
	//button == 2 为鼠标右键
	if (button == 2)
	{
		x = form1.Control(IDC_StaticText_Main).Left() + x;
		y = form1.Control(IDC_StaticText_Main).Top() + y;
		form1.PopupMenu(IDR_MENU2, x, y);
	}
	return;
}
//关闭程序时询问↓
void form_QuesWhenUnload(int pCancel)
{
	//若用户在是否保存状态框选择没有选择否或者是，则取消关闭操作
	if (Ques_ifSaveNeed())
		*(int*)pCancel = 1;
	return;
}
//主文本框内容改变时，触发该函数↓
void Text_Main_Change()
{
	//若文本内容被改变，则将文本是否被改动标记设置为true
	if_TextChanged = true;
	form1.Control(IDC_Text_Main).Refresh();
	//每次改变文本就将当前内容存入撤回内容保存空间，以便于后续的撤回操作
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
		// 如果数组为空，这是第一次操作
		Undo_Total.push_back({ TextNow, FontColorNow, FontTypeNow });
	}

	/*输出操作信息，仅作示例
	auto& UndoTest = Undo_Total.back();
	LPTSTR TESTTEXT = UndoTest.Undo_Text;
	TESTTEXT = StrAppend(TESTTEXT, Str(UndoTest.Undo_FontColor), UndoTest.Undo_FontType);
	MsgBox(TESTTEXT);*/
	return;
}
//实现拖动打开已有文件的功能↓
void Text_Main_FileDrop(int ptrArrFiles,int count,int x,int y)
{
	LPTSTR* AimFile = (LPTSTR*)ptrArrFiles;
	//定义目标文件为鼠标指针目前所选中的文件
	if (Ques_ifSaveNeed())
		return;
	if (InputKey())
		OpenFile(AimFile[1], AimKey);
}
//状态栏显示当前指针的位置
void Menu_Select(int ptrCBMenuItem)
{
	CBMenuItem* pMenu = (CBMenuItem*)ptrCBMenuItem;
	LPTSTR ptrPossitionNow = TEXT("");
	if (ptrCBMenuItem)
	{
		if (pMenu->IsSeparator())
			ptrPossitionNow = TEXT("您指向了一条菜单的分割线");
		else
		{
			ptrPossitionNow = TEXT("您指向了菜单：");
			ptrPossitionNow = StrAppend(ptrPossitionNow, pMenu->Text());
			ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" 位置是："), Str(pMenu->Position()));
			if (pMenu->hSubMenu())
				ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" 所引出的菜单句柄是："), Str((unsigned int)pMenu->hSubMenu()));
			else
				ptrPossitionNow = StrAppend(ptrPossitionNow, TEXT(" 资源ID是："), Str(pMenu->IDRes()));
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
	//关联关于窗口,便于在main.cpp里执行该窗口的部分操作
	form_About.UnLoad();
	//关联查找窗口,便于在main.cpp里执行该窗口的部分操作
	form_Find.UnLoad();
	//关联替换窗口,便于在main.cpp里执行该窗口的部分操作
	form_Replace.UnLoad();
	//关联运行窗口,便于在main.cpp里执行该窗口的部分操作
	form_Run.UnLoad();
	//关联列文件目录窗口,便于在main.cpp里执行该窗口的部分操作
	form_FileLists.UnLoad();
	//关联文档助手窗口,便于在main.cpp里执行该窗口的部分操作
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
	//关联各个控件与其功能函数↑
	///////////////////////////////////
	///////////////////////////////////
	//预处理密码输入窗口↓
	form_InputKey_main();
	//预处理关于窗口↓
	form_About_main();
	//预处理查找窗口↓
	form_Find_main();
	//预处理替换窗口↓
	form_Replace_main();
	//预处理运行窗口↓
	form_Run_main();
	//预处理列文件目录窗口↓
	form_FileLists_main();

	//设置快捷键↓
	form1.SetAccelerator(IDR_ACCELERATOR1);
	//执行主窗口的预处理↓
	PreSet();
	//显示主窗口↓
	form1.IconSet(IDI_ICON1);
	form1.Show();
	return 0;
}
#include "resource.h"
#include "BForm.h"
#include "UserDataBase.h"
#include <vector>

CBForm form_Login(IDD_LoginForm);

extern void Button_MainExit_Click();
extern bool if_LoginReady;
extern bool if_Exit;

bool Check_IfAccount_Exist = false;
bool Check_IfPassword_Correct = false;
std::vector<UserData> usersList;

//登录界面预处理↓
int Login_Preset()
{
	Check_IfAccount_Exist = false;
	Check_IfPassword_Correct = false;
	if_LoginReady = false;
	if_Exit = false;
	form_Login.Control(IDC_StaticPic_Login).PictureSet(IDB_BITMAP3);
	form_Login.Control(IDC_StaticPic_Login).StretchSet(true);
	form_Login.Control(IDC_Text_Account).TextSet(TEXT(""));
	form_Login.Control(IDC_Text_Password).TextSet(TEXT(""));

	form_Login.Move((pApp->ScreenWidth() - form_Login.Width()) / 2,
		(pApp->ScreenHeight() - form_Login.Height()) / 2);

	if (MsgBox(TEXT("欢迎使用VIP功能！该部分可实现下列功能：\
		\r\n1.用户需要注册VIP账号并登录才能进入主程序。\
		\r\n2.用户可输入任意字符串，随后显示出字符串的相关信息。\
		\r\n3.相关信息包括但不限于：显示第1个和第x个字符。\
		\r\n4.显示某个字符或字符串在用户输入的字符串中首次出现的位置。\
		\r\n5.对用户输入的字符串按照任意分隔符C进行分割，C可用户自定义。\
		\r\n点击“确定”按钮进入程序。\r\n注意：若密码输入框无输入则默认为0,程序出现问题或有疑问请联系邮箱：1537361071@qq.com"), TEXT("欢迎！"), mb_OkCancel, mb_IconNone) == idOk)
		return 1;
	//通过对话框介绍程序功能，并为欧拉筛的运行留出时间。
	return 0;
}
//寻找用户是否存在，若存在则返回用户位置↓
UserData* FindUser(std::vector<UserData>& usersList, LPTSTR userName)
{
	for (auto& user : usersList)
	{
		if (lstrcmp(user.Get_Account(), userName) == 0)
		{
			return &user;
		}
	}
	return nullptr;
}



void Button_LoginExit_Click()
{
	form_Login.UnLoad();
	return;
}

void Button_Login_Click()
{
	LPTSTR UserAccount = form_Login.Control(IDC_Text_Account).Text();
	LPTSTR UserPassword = form_Login.Control(IDC_Text_Password).Text();
	UserData* user = FindUser(usersList, UserAccount);
	if (user)
	{
		Check_IfAccount_Exist = true;
		if (lstrcmp(UserPassword, user->Get_Password()) == 0)
			Check_IfPassword_Correct = true;
		else
			Check_IfPassword_Correct = false;
	}
	else
		Check_IfAccount_Exist = false;
	if (!Check_IfAccount_Exist)
	{
		MsgBox(TEXT("用户名不存在，请检查用户名输入或进行注册！"), TEXT("用户名不存在！"), mb_OK, mb_IconInformation);
		form_Login.Control(IDC_Text_Password).TextSet(TEXT(""));
		return;
	}
	if (!Check_IfPassword_Correct)
	{
		MsgBox(TEXT("密码错误，请输入正确密码！"), TEXT("密码错误！"), mb_OK, mb_IconInformation);
		form_Login.Control(IDC_Text_Password).TextSet(TEXT(""));
		return;
	}
	Button_LoginExit_Click();
	if_LoginReady = true;
}

void Text_Password_KeyPress(int KeyAscii, int shift, int pbCancel)
{
	if (KeyAscii == 13)
		Button_Login_Click();
	return;
}

void Text_Account_KeyPress(int KeyAscii, int shift, int pbCancel)
{
	if (KeyAscii == 13)
		form_Login.Control(IDC_Text_Password).SetFocus();
	return;
}

void Button_Register_Click()
{
	LPTSTR UserAccount = form_Login.Control(IDC_Text_Account).Text();
	LPTSTR UserPassword = form_Login.Control(IDC_Text_Password).Text();
	UserData* user = FindUser(usersList, UserAccount);
	if (user)
	{
		MsgBox(TEXT("该用户名已被注册，请更改需要注册的账户！"), TEXT("用户名已被注册！"), mb_OK, mb_IconInformation);
		return;
	}
	else
	{
		LPTSTR RegisterInfo = TEXT("");
		RegisterInfo = StrAppend(RegisterInfo, TEXT("注册成功！请牢记您的用户信息。\n账户："), UserAccount);
		RegisterInfo = StrAppend(RegisterInfo, TEXT("\n密码："), UserPassword);
		usersList.emplace_back(UserAccount, UserPassword);
		MsgBox(RegisterInfo, TEXT("注册成功！"), mb_OK, mb_IconNone);
		form_Login.Control(IDC_Button_Login).SetFocus();
		return;
	}
}

void Button_Exit_Click()
{
	if_Exit = true;
	Button_LoginExit_Click();
	Button_MainExit_Click();
}

void Login_main()
{
	form_Login.EventAdd(IDC_Button_Login, eCommandButton_Click, Button_Login_Click);
	form_Login.EventAdd(IDC_Button_Exit, eCommandButton_Click, Button_Exit_Click);
	form_Login.EventAdd(IDC_Button_Register, eCommandButton_Click, Button_Register_Click);
	form_Login.EventAdd(IDC_Text_Password, eKeyDown, Text_Password_KeyPress);
	form_Login.EventAdd(IDC_Text_Account, eKeyDown, Text_Account_KeyPress);

	if (!Login_Preset())
	{
		//若用户选择“取消”按钮，则直接关闭程序。
		Button_Exit_Click();
		return;
	}

	form_Login.IconSet(IDI_ICON1);
	form_Login.Show();
	return;
}
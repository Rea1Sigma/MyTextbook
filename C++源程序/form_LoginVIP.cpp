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

//��¼����Ԥ�����
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

	if (MsgBox(TEXT("��ӭʹ��VIP���ܣ��ò��ֿ�ʵ�����й��ܣ�\
		\r\n1.�û���Ҫע��VIP�˺Ų���¼���ܽ���������\
		\r\n2.�û������������ַ����������ʾ���ַ����������Ϣ��\
		\r\n3.�����Ϣ�����������ڣ���ʾ��1���͵�x���ַ���\
		\r\n4.��ʾĳ���ַ����ַ������û�������ַ������״γ��ֵ�λ�á�\
		\r\n5.���û�������ַ�����������ָ���C���зָC���û��Զ��塣\
		\r\n�����ȷ������ť�������\r\nע�⣺�������������������Ĭ��Ϊ0,����������������������ϵ���䣺1537361071@qq.com"), TEXT("��ӭ��"), mb_OkCancel, mb_IconNone) == idOk)
		return 1;
	//ͨ���Ի�����ܳ����ܣ���Ϊŷ��ɸ����������ʱ�䡣
	return 0;
}
//Ѱ���û��Ƿ���ڣ��������򷵻��û�λ�á�
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
		MsgBox(TEXT("�û��������ڣ������û�����������ע�ᣡ"), TEXT("�û��������ڣ�"), mb_OK, mb_IconInformation);
		form_Login.Control(IDC_Text_Password).TextSet(TEXT(""));
		return;
	}
	if (!Check_IfPassword_Correct)
	{
		MsgBox(TEXT("���������������ȷ���룡"), TEXT("�������"), mb_OK, mb_IconInformation);
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
		MsgBox(TEXT("���û����ѱ�ע�ᣬ�������Ҫע����˻���"), TEXT("�û����ѱ�ע�ᣡ"), mb_OK, mb_IconInformation);
		return;
	}
	else
	{
		LPTSTR RegisterInfo = TEXT("");
		RegisterInfo = StrAppend(RegisterInfo, TEXT("ע��ɹ������μ������û���Ϣ��\n�˻���"), UserAccount);
		RegisterInfo = StrAppend(RegisterInfo, TEXT("\n���룺"), UserPassword);
		usersList.emplace_back(UserAccount, UserPassword);
		MsgBox(RegisterInfo, TEXT("ע��ɹ���"), mb_OK, mb_IconNone);
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
		//���û�ѡ��ȡ������ť����ֱ�ӹرճ���
		Button_Exit_Click();
		return;
	}

	form_Login.IconSet(IDI_ICON1);
	form_Login.Show();
	return;
}
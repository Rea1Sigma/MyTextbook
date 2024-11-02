#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form_InputKey(IDD_form_InputKey);
//关联main中的全局变量密码，便于两个窗口通讯。
extern bool if_InputKeyOk;
extern char * AimKey;
//设置密码界面预处理↓
void form_InputKey_PreSet()
{
    form_InputKey.Control(IDC_Text_Key).TextSet(TEXT(""));
    form_InputKey.IconSet(IDI_ICON2);
    form_InputKey.Control(IDC_StaticPic_InputKey).PictureSet(IDB_BITMAP1);
    form_InputKey.Control(IDC_StaticPic_InputKey).StretchSet(true);
    form_InputKey.Control(IDC_StaticPic_InputKey).PictureSetIcon(IDI_ICON2);
    form_InputKey.Control(IDC_Text_Key).SetFocus();
    //还原各个基本控件
    return;
}
//确定按钮↓
void Button_Ok_Click()
{
	LPTSTR Key = form_InputKey.Control(IDC_Text_Key).Text();
    if (*Key == TEXT('\0'))
    {
        //若密码框输入为空，则直接不设置密码
        AimKey = NULL;
        if_InputKeyOk = true;
        form_InputKey.UnLoad();
        return;
    }
    
    AimKey = StrConvFromUnicode(Key);                           //转换Unicode
    if_InputKeyOk = true;
    form_InputKey.UnLoad();                                             //密码设置完成后关闭该窗口
    return;
}
//取消按钮↓
void Button_Cancel_Click()
{
    form_InputKey.UnLoad();                                             
    return;
}
//密码输入框的拓展功能
void Text_Key_KeyPress(int KeyAscii, int pbCancel)
{
    //在密码输入框按回车可以快捷设置密码
    if (KeyAscii == 13)
        Button_Ok_Click();
    return;
}

void form_InputKey_main()
{
    //关联各个控件与其对应的功能函数
    form_InputKey.EventAdd(IDC_Button_Ok, eCommandButton_Click, Button_Ok_Click);
    form_InputKey.EventAdd(IDC_Button_Cancel, eCommandButton_Click, Button_Cancel_Click);
    form_InputKey.EventAdd(IDC_Text_Key, eKeyPress, Text_Key_KeyPress);

    return;
}

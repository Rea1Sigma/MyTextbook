#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form_InputKey(IDD_form_InputKey);
//����main�е�ȫ�ֱ������룬������������ͨѶ��
extern bool if_InputKeyOk;
extern char * AimKey;
//�����������Ԥ�����
void form_InputKey_PreSet()
{
    form_InputKey.Control(IDC_Text_Key).TextSet(TEXT(""));
    form_InputKey.IconSet(IDI_ICON2);
    form_InputKey.Control(IDC_StaticPic_InputKey).PictureSet(IDB_BITMAP1);
    form_InputKey.Control(IDC_StaticPic_InputKey).StretchSet(true);
    form_InputKey.Control(IDC_StaticPic_InputKey).PictureSetIcon(IDI_ICON2);
    form_InputKey.Control(IDC_Text_Key).SetFocus();
    //��ԭ���������ؼ�
    return;
}
//ȷ����ť��
void Button_Ok_Click()
{
	LPTSTR Key = form_InputKey.Control(IDC_Text_Key).Text();
    if (*Key == TEXT('\0'))
    {
        //�����������Ϊ�գ���ֱ�Ӳ���������
        AimKey = NULL;
        if_InputKeyOk = true;
        form_InputKey.UnLoad();
        return;
    }
    
    AimKey = StrConvFromUnicode(Key);                           //ת��Unicode
    if_InputKeyOk = true;
    form_InputKey.UnLoad();                                             //����������ɺ�رոô���
    return;
}
//ȡ����ť��
void Button_Cancel_Click()
{
    form_InputKey.UnLoad();                                             
    return;
}
//������������չ����
void Text_Key_KeyPress(int KeyAscii, int pbCancel)
{
    //����������򰴻س����Կ����������
    if (KeyAscii == 13)
        Button_Ok_Click();
    return;
}

void form_InputKey_main()
{
    //���������ؼ������Ӧ�Ĺ��ܺ���
    form_InputKey.EventAdd(IDC_Button_Ok, eCommandButton_Click, Button_Ok_Click);
    form_InputKey.EventAdd(IDC_Button_Cancel, eCommandButton_Click, Button_Cancel_Click);
    form_InputKey.EventAdd(IDC_Text_Key, eKeyPress, Text_Key_KeyPress);

    return;
}

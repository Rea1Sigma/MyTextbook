#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form_About(IDD_form_About);

void form_About_PreSet()
{
	form_About.IconSet(IDI_ICON1);
	form_About.Control(IDC_StaticPic_About).PictureSet(IDB_BITMAP2);
	form_About.Control(IDC_StaticPic_About).StretchSet(true);

	return;
}
void Button_About_Click()
{
	form_About.UnLoad();
	return;
}

void form_About_main()
{
	form_About.EventAdd(IDC_Button_About, eCommandButton_Click, Button_About_Click);

	form_About_PreSet();
	return;
}
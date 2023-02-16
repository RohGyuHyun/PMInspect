#include "StdAfx.h"
#include "MsgBox.h"



CMsgBox g_cMsgBox;

CMsgBox::CMsgBox(void)
{

}


CMsgBox::~CMsgBox(void)
{
}


int CMsgBox::YesNoMsgBox(CString strMsg, int Level)
{
	is_displaying = TRUE;

	CDialogMessageBox dlg(MS_YESNO, Level ,strMsg);
	int rval = dlg.DoModal();
	is_displaying = FALSE;
	return rval;
}

void CMsgBox::OkMsgBox(CString strMsg, int Level)
{
	is_displaying = TRUE;

	CDialogMessageBox dlg(MS_OK, Level, strMsg);
	dlg.DoModal();
	is_displaying = FALSE;
}

int CMsgBox::NGViewBox(CString strMsg, int Level)
{
	is_displaying = TRUE;

	CDialogMessageBox dlg(MS_NGVIEW, Level, strMsg);
	int rval = dlg.DoModal();
	is_displaying = FALSE;
	return rval;
}

#pragma once

#include "..\..\DialogMessageBox.h"

class CMsgBox
{
public:
	CMsgBox(void);
	~CMsgBox(void);
public:
	BOOL is_displaying;

	int YesNoMsgBox(CString strMsg, int Level = 0);
	void OkMsgBox(CString strMsg, int Level = 0);
	int NGViewBox(CString strMsg, int Level = 0);
};

extern CMsgBox g_cMsgBox;

// PMInspect.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CPMInspectApp:
// �� Ŭ������ ������ ���ؼ��� PMInspect.cpp�� �����Ͻʽÿ�.
//

class CPMInspectApp : public CWinApp
{
public:
	CPMInspectApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CPMInspectApp theApp;
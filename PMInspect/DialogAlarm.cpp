// DialogAlarm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogAlarm.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"


// CDialogAlarm ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogAlarm, CDialogEx)

CDialogAlarm::CDialogAlarm(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MSG_ALARM, pParent)
{

}

CDialogAlarm::~CDialogAlarm()
{
}

void CDialogAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDS_MESSAGE, m_sMessage);
	DDX_Control(pDX, IDS_ERR_CODE, m_sCode);
}


BEGIN_MESSAGE_MAP(CDialogAlarm, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogAlarm �޽��� ó�����Դϴ�.


BOOL CDialogAlarm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDialogAlarm::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (bShow)
	{
		//m_btn_msg.SetCaption(m_sMessage);
		MoveWindow(245, 280, 690, 450, TRUE);
	}
	else
	{

	}

}

BEGIN_EVENTSINK_MAP(CDialogAlarm, CDialog)
	ON_EVENT(CDialogAlarm, IDB_BUZZER_OFF, DISPID_CLICK, CDialogAlarm::ClickBuzzerOff, VTS_NONE)
	ON_EVENT(CDialogAlarm, IDB_OK, DISPID_CLICK, CDialogAlarm::ClickOk, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogAlarm::ClickBuzzerOff()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//Buzzer off
	//pdlg->m_pDlgManual->WriteOutput(BUZZER_ERROR_SIGNAL, FALSE);
}

void CDialogAlarm::SetMessage(char *code, char *msg)
{
	TCHAR* sData = new TCHAR[512];

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)msg, -1, sData, 512);
	m_sMessage.SetCaption(sData);

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)code, -1, sData, 512);
	m_sCode.SetCaption(sData);

	delete[]sData;
}

void CDialogAlarm::ClickOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->AutoReset();

	CDialog::OnOK();
}


BOOL CDialogAlarm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

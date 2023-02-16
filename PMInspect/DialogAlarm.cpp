// DialogAlarm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogAlarm.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"


// CDialogAlarm 대화 상자입니다.

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


// CDialogAlarm 메시지 처리기입니다.


BOOL CDialogAlarm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->AutoReset();

	CDialog::OnOK();
}


BOOL CDialogAlarm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

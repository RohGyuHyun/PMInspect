// DialogMessageBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogMessageBox.h"
#include "afxdialogex.h"


// CDialogMessageBox 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogMessageBox, CDialogEx)

CDialogMessageBox::CDialogMessageBox(int iStyle, int level, CString strMsg, CWnd* pParent)
	: CDialogEx(IDD_DIALOG_MESSAGEBOX, pParent)
{
	m_iStyle = iStyle;
	m_strMsg = strMsg;
	m_iLevel = level;
}

CDialogMessageBox::~CDialogMessageBox()
{
}

void CDialogMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDB_NO, m_ctrlNo);
	DDX_Control(pDX, IDB_OK, m_ctrlOk);
	DDX_Control(pDX, IDB_YES, m_ctrlYes);
	DDX_Control(pDX, IDC_MARK_1, m_ctrlMark1);
	DDX_Control(pDX, IDC_MARK_2, m_ctrlMark2);
	DDX_Control(pDX, IDS_MESSAGE, m_ctrlMsg);
}


BEGIN_MESSAGE_MAP(CDialogMessageBox, CDialogEx)
END_MESSAGE_MAP()


// CDialogMessageBox 메시지 처리기입니다.

BOOL CDialogMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	switch (m_iStyle)
	{
	case MS_YESNO:
		m_ctrlMark1.ShowWindow(SW_HIDE);
		m_ctrlOk.ShowWindow(SW_HIDE);
		break;
	case MS_NGVIEW:
		m_ctrlMark1.ShowWindow(SW_SHOW);
		m_ctrlYes.SetWindowText(_T("양품"));
		m_ctrlYes.ShowWindow(SW_SHOW);
		m_ctrlNo.SetWindowText(_T("불량"));
		m_ctrlNo.ShowWindow(SW_SHOW);
		m_ctrlOk.ShowWindow(SW_HIDE);
		break;
	case MS_OK:
	default:
		m_ctrlMark2.ShowWindow(SW_HIDE);
		m_ctrlYes.ShowWindow(SW_HIDE);
		m_ctrlNo.ShowWindow(SW_HIDE);
		break;
	}

	if (m_iLevel == 1)
		m_ctrlMsg.SetForeColor(RGB(255, 0, 0));
	else
		m_ctrlMsg.SetForeColor(RGB(0, 0, 0));

	m_ctrlMsg.SetCaption(m_strMsg);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
BEGIN_EVENTSINK_MAP(CDialogMessageBox, CDialogEx)
	ON_EVENT(CDialogMessageBox, IDB_YES, DISPID_CLICK, CDialogMessageBox::ClickYes, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_OK, DISPID_CLICK, CDialogMessageBox::ClickOk, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_STOP, DISPID_CLICK, CDialogMessageBox::ClickStop, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_NO, DISPID_CLICK, CDialogMessageBox::ClickNo, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogMessageBox::ClickYes()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int ret = IDYES;
	EndDialog(ret);
	return;
}


void CDialogMessageBox::ClickOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CDialogMessageBox::ClickStop()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}


BOOL CDialogMessageBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDialogMessageBox::ClickNo()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//m_ctrlNo.SetWindowText(L"NO");

	int ret = IDNO;
	EndDialog(ret);
	return;
}

// DialogMessageBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogMessageBox.h"
#include "afxdialogex.h"


// CDialogMessageBox ��ȭ �����Դϴ�.

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


// CDialogMessageBox �޽��� ó�����Դϴ�.

BOOL CDialogMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	switch (m_iStyle)
	{
	case MS_YESNO:
		m_ctrlMark1.ShowWindow(SW_HIDE);
		m_ctrlOk.ShowWindow(SW_HIDE);
		break;
	case MS_NGVIEW:
		m_ctrlMark1.ShowWindow(SW_SHOW);
		m_ctrlYes.SetWindowText(_T("��ǰ"));
		m_ctrlYes.ShowWindow(SW_SHOW);
		m_ctrlNo.SetWindowText(_T("�ҷ�"));
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
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
BEGIN_EVENTSINK_MAP(CDialogMessageBox, CDialogEx)
	ON_EVENT(CDialogMessageBox, IDB_YES, DISPID_CLICK, CDialogMessageBox::ClickYes, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_OK, DISPID_CLICK, CDialogMessageBox::ClickOk, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_STOP, DISPID_CLICK, CDialogMessageBox::ClickStop, VTS_NONE)
	ON_EVENT(CDialogMessageBox, IDB_NO, DISPID_CLICK, CDialogMessageBox::ClickNo, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogMessageBox::ClickYes()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	int ret = IDYES;
	EndDialog(ret);
	return;
}


void CDialogMessageBox::ClickOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CDialog::OnOK();
}


void CDialogMessageBox::ClickStop()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

}


BOOL CDialogMessageBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDialogMessageBox::ClickNo()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//m_ctrlNo.SetWindowText(L"NO");

	int ret = IDNO;
	EndDialog(ret);
	return;
}

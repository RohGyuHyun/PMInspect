#pragma once


// CDialogAlarm ��ȭ �����Դϴ�.

class CDialogAlarm : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlarm)

public:
	CDialogAlarm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogAlarm();

	CBtnenhctrl m_sMessage;
	CBtnenhctrl m_sCode;

	void SetMessage(char *code, char *msg);

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MSG_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	void ClickBuzzerOff();
	void ClickOk();
	virtual BOOL OnInitDialog();
};

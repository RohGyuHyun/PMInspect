#pragma once


// CDialogAlarm 대화 상자입니다.

class CDialogAlarm : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAlarm)

public:
	CDialogAlarm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogAlarm();

	CBtnenhctrl m_sMessage;
	CBtnenhctrl m_sCode;

	void SetMessage(char *code, char *msg);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MSG_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	void ClickBuzzerOff();
	void ClickOk();
	virtual BOOL OnInitDialog();
};

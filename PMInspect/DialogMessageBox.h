#pragma once


// CDialogMessageBox 대화 상자입니다.
#define  MS_OK   			  0x10
#define  MS_OKCANCEL 		  0x20
#define  MS_YESNO			  0x30
#define  MS_NGVIEW			  0x40

class CDialogMessageBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMessageBox)

public:
	CDialogMessageBox(int iStyle, int level, CString strMsg, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogMessageBox();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MESSAGEBOX
	};
#endif

	CBtnenhctrl	m_ctrlNo;
	CBtnenhctrl	m_ctrlOk;
	CBtnenhctrl	m_ctrlYes;
	CBtnenhctrl	m_ctrlMark1;
	CBtnenhctrl	m_ctrlMark2;
	CBtnenhctrl	m_ctrlMsg;

private:
	CString m_strMsg;
	int m_iStyle;
	int m_iLevel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickYes();
	void ClickOk();
	void ClickStop();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClickNo();
};

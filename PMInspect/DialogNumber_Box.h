#pragma once


// CDialogNumber_Box 대화 상자입니다.

class CDialogNumber_Box : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogNumber_Box)

public:
	//CDialogNumber_Box(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogNumber_Box();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NUMBER_BOX };
//#endif

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	CBtnenhctrl	m_sSetValue;
	CBtnenhctrl	m_sDispNum;
	CString GetstrNum();
	void SetstrNum(CString strNum);
	void SetstrNum(double dNum);
	void SetstrNum(int nNum);

	void SetWindowTitle(LPCTSTR title);
	CDialogNumber_Box(int maxchar = 10, char* getnum = "0.0", char* title = "Insert Number Only.",
		CWnd* pParent = NULL, BOOL pwd = FALSE);   // standard constructor

private:
	int m_cyChar;
	int m_cxChar;
	BOOL m_numFlag;
	CRect m_rect;
	void UpdateDisplay(COLORREF bkcolor = RGB(255, 255, 255));

protected:
	int  m_nPointNum;
	BOOL m_bCalu;
	int  m_nCalu;
	BOOL m_FirstFlag;
	BOOL m_bpwd;
	CString m_strGetNum;
	CString m_strOrg;
	int m_intMaxChar;
	CString m_strWndText;
	BOOL m_bVkShift;

	BOOL VerifyMaxChar();
	void InitStaticDispWnd();

	afx_msg void OnBtnPlus();
	afx_msg void OnBtnMinus();
	afx_msg void OnBtnEqual();
	afx_msg void OnBtnDot();
	afx_msg void OnBtnSign();
	afx_msg void OnBtnBack();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnEsc();
	afx_msg void OnBtn1();
	afx_msg void OnBtn2();
	afx_msg void OnBtn3();
	afx_msg void OnBtn4();
	afx_msg void OnBtn5();
	afx_msg void OnBtn6();
	afx_msg void OnBtn7();
	afx_msg void OnBtn8();
	afx_msg void OnBtn9();
	afx_msg void OnBtn0();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()


	DECLARE_MESSAGE_MAP()
};

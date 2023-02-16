#pragma once


// CDialogSystem ��ȭ �����Դϴ�.
typedef struct TRIGGER_PARA {
	int m_nTime;
	int m_nEnable;
	int m_nCycle;
	int m_nNumber;
	int m_nStart;
	int m_nEnd;

}TRIGGER_PARAMETER;

class CDialogSystem : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSystem)

public:
	CDialogSystem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogSystem();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SYSTEM };
#endif
protected:
	CBtnenhctrl	m_btn_IP_Address;	// ����
	CBtnenhctrl	m_btn_Port;			// ����
	CBtnenhctrl	m_btn_Version;		// ����
	CBtnenhctrl	m_btn_XudCnt;		// ���� ����
	CBtnenhctrl m_btn_TrgCnt;		//		"

	CBtnenhctrl	m_btn_TrgTime;
	CBtnenhctrl	m_btn_TrgEnbl;
	CBtnenhctrl	m_btn_Cycle;
	CBtnenhctrl	m_btn_TrgNo;
	CBtnenhctrl	m_btn_TrgStart;
	CBtnenhctrl	m_btn_TrgEnd;
	CBtnenhctrl m_btn_ReadData;

	PARAMANAGER m_Paramanager;

	TRIGGER_PARAMETER m_TrgPara;

public:
	void SetFilePath();
	void SetLinkPara();
	void UpdatePara(BOOL bShow);

	void SetTriggerInit();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlSystemButtonCntReset();
	void ClickBtnenhctrlSystemButtonParameterSet();
	void DblClickBtnenhctrlSystemLabelTriggerTrgtime1();
	void DblClickBtnenhctrlSystemLabelTriggerTrgenbl1();
	void DblClickBtnenhctrlSystemLabelTriggerXlcycle1();
	void DblClickBtnenhctrlSystemLabelTriggerXltrgno1();
	void DblClickBtnenhctrlSystemLabelTriggerXltrgstart1();
	void DblClickBtnenhctrlSystemLabelTriggerXltrgend1();
	void ClickBtnenhctrlSystemButtonApply();
	void ClickBtnenhctrlSystemButtonOk();
	void ClickBtnenhctrlSystemButtonReaddata();
	void ClickBtnenhctrlSystemButtonCntReset2();
};

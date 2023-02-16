#pragma once


// CDialogAxisManage 대화 상자입니다.
#define TIMER_AXIS_MANAGE			506

class CDialogAxisManage : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAxisManage)

public:
	CDialogAxisManage(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogAxisManage();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AXIS_MANAGER };
#endif
	CBtnenhctrl	m_ctrlMaxVel;
	CBtnenhctrl	m_ctrlMaxAcc;
	CBtnenhctrl	m_ctrlMinAcc;
	CBtnenhctrl	m_ctrlMinVel;
	CBtnenhctrl	m_ctrlAxisName;
	CBtnenhctrl	m_ctrlCurPos;
	CBtnenhctrl	m_ctrlAmpState;
	CBtnenhctrl	m_ctrlPosSensor;
	CBtnenhctrl	m_ctrlNegSensor;
	CBtnenhctrl	m_ctrlHomeSensor;
	CBtnenhctrl	m_ctrlVel;
	CBtnenhctrl	m_ctrlAcc;
	CBtnenhctrl	m_ctrlOriginVel;
	CBtnenhctrl	m_ctrlOriginVel2;
	CBtnenhctrl	m_ctrlOriginVel3;
	CBtnenhctrl	m_ctrlJogVel;
	CBtnenhctrl	m_ctrlOriginAcc;
	CBtnenhctrl	m_ctrlJogAcc;
	CBtnenhctrl	m_ctrlPosSWLimit;
	CBtnenhctrl	m_ctrlNegSWLimit;
	CBtnenhctrl m_ctrlTriggerPeriod;
private:
	void DispMotorParam();
	void DispUnitState();

	int m_iAxis;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnClickVelocity();
	void OnClickAccTime();
	void OnClickOriginVel();
	void OnClickOriginAcc();
	void OnClickJogVel();
	void OnClickJogAcc();
	void OnClickMaxVel();
	void OnClickMinVel();
	void OnClickMaxAcc();
	void OnClickMinAcc();
	void OnClickPosSwLimit();
	void OnClickNegSwLimit();
	void ClickSaveAxis();
	void ClickAmpOn();
	void ClickAmpOff();
	void ClickAmpReset();
	void ClickMotorParamPrev();
	void ClickMotorParamNext();
	void ClickTriggerPeriod();
	void ClickOriginVel2();
	void ClickOriginVel3();
};

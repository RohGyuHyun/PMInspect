#pragma once


// CDialogMotion 대화 상자입니다.

#include "Motion\Motion.h"
#include "DialogAxisManage.h"
#include "afxwin.h"

#define MAX_INPUT_NUM		10
#define MAX_OUTPUT_NUM		10

#define JOG_MODE			0
#define STEP_MODE			1

#define TIMER_IO_MONITORING			0
#define TIMER_ORIGIN_DLG			1


//INPUT
#define START_SWITCH				0
#define STOP_SWITCH					1
#define STOP_SWITCH_ON				2
#define STOP_SWITCH_OFF				3
#define OP_EMS_CHECK				4
#define MAIN_AIR_CHECK				5

#define MAX_POSITION_NUM			5

#define MOTION_VEL_MAGNIFICATION	10

//OUTPUT



class CDialogMotion : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMotion)

public:
	CDialogMotion(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogMotion();

private:
	CDialogAxisManage *m_pDlgAxisManager;
	PARAMANAGER m_Paramanager;
	void SetLinkPara();
	void UpdatePara(BOOL bShow);
	//MOTION
	CMotion *m_pMotion;
	BOOL	m_bAxisHomeState[MAX_AXIS_NUM];
	BOOL	m_bAmpOnFlag[MAX_AXIS_NUM];

	CBtnenhctrl m_CBtnAxisName[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisPulse[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisSensorP[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisSensorH[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisSensorN[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisAmp[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisPosition[MAX_AXIS_NUM][MAX_POSITION_NUM];
	CBtnenhctrl m_CBtnLabelAxis8;
	CBtnenhctrl m_CBtnLabelAxisName4;
	CBtnenhctrl m_CBtnJog;
	CBtnenhctrl m_CBtnAmpON;
	CBtnenhctrl m_CBtnAmpOff;

	CBtnenhctrl m_ctrl_progress;
	int m_nProcessMoveIdx;

	int m_nRadioAxisName;
	int m_nRadioMoveType;			// 0 : Jog 1: Step
	int m_nRadioAxis;
	int m_nRadioPos;
	int m_edit_nStepVal;
	CComboBox m_combo_jog_speed;
	double m_dJogSpeed;

	double m_dAxisPosition[MAX_AXIS_NUM][MAX_POSITION_NUM];

	void AxisMove();
	void ClickOutput();
	void DblClickPosition();
	
	//IO
	BOOL	m_bOldInput[MAX_INPUT_NUM];
	BOOL	m_bOldOutput[MAX_OUTPUT_NUM];

	CBtnenhctrl m_CBtnInput[MAX_INPUT_NUM];
	CBtnenhctrl m_CBtnOutput[MAX_OUTPUT_NUM];
	

	void IOMonitoring();

	//	MOTION
	BOOL InitializeAJINX();
	BOOL DestroyAJINX();

public:
	void DispCurPos();
	void DispAxisState();

	BOOL MotInitialize() { return m_pMotion->Initialize(); }
	BOOL MotAmpEnableAll() { return m_pMotion->AmpEnableAll(); }
	BOOL MotAmpDisableAll() { return m_pMotion->AmpDisableAll(); }

	BOOL AxisMove(int axis_no, double dPos, double dVelRatio, BOOL bWaitFlag = TRUE) { return m_pMotion->m_pAxis[axis_no]->Move(dPos, dVelRatio, bWaitFlag); }
	BOOL AxisVMove(int axis_no, double dVel, double dAcc, double dDec) { return m_pMotion->m_pAxis[axis_no]->VMove(dVel, dAcc, dDec); }
	BOOL AxisRMove(int axis_no, double dPos, BOOL bWaitFlag = TRUE) { return m_pMotion->m_pAxis[axis_no]->RMove(dPos, bWaitFlag); }
	BOOL AxisMoveDone(int axis_no) { return m_pMotion->m_pAxis[axis_no]->MotionDone(); }
	BOOL AxisStop(int axis_no) { return m_pMotion->m_pAxis[axis_no]->Stop(); }
	BOOL AxisEStop(int axis_no) { return m_pMotion->m_pAxis[axis_no]->EStop(); }
	BOOL AxisIsHomeFinished(int axis_no) { return m_pMotion->m_pAxis[axis_no]->IsHomeFinished(); }
	BOOL AxisGetAmpFaultState(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetAmpFaultState(); }
	BOOL MotSetOriginMethod(int axis_no) { return m_pMotion->SetOriginMethod(axis_no); }
	BOOL MotStartOrigin(int axis_no) { return m_pMotion->StartOrigin(axis_no); }
	//BOOL AxisGetPosition(int axis_no, double &rPos, BOOL bEncoder = TRUE) { return m_pMotion->m_pAxis[axis_no]->GetPosition(rPos, bEncoder); }
	BOOL AxisSetPosition(int axis_no, double dPos) { return m_pMotion->m_pAxis[axis_no]->SetPosition(dPos); }
	BOOL AxisGetAmpOnState(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetAmpOnState(); }
	BOOL AxisGetPosition(int axis_no, double &rPos, BOOL bEncoder = TRUE) { return m_pMotion->m_pAxis[axis_no]->GetPosition(rPos, bEncoder); }

	CString AxisGetAxisName(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetAxisName(); }
	void AxisSetAxisName(int axis_no, CString strName) { m_pMotion->m_pAxis[axis_no]->SetAxisName(strName); }
	int AxisGetAxisUse(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetAxisUse(); }
	void AxisSetAxisUse(int axis_no, BOOL bUse) { m_pMotion->m_pAxis[axis_no]->SetAxisUse(bUse); }
	double AxisGetNormVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetNormVel(); }
	void AxisSetNormVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetNormVel(dVel); }
	double AxisGetHomeVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeVel(); }
	void AxisSetHomeVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetHomeVel(dVel); }
	double AxisGetHomeVel2(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeVel2(); }
	void AxisSetHomeVel2(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetHomeVel2(dVel); }
	double AxisGetHomeVel3(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeVel3(); }
	void AxisSetHomeVel3(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetHomeVel3(dVel); }

	double AxisGetJogVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetJogVel(); }
	void AxisSetJogVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetJogVel(dVel); }
	double AxisGetJogAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetJogAcc(); }
	void AxisSetJogAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetJogAcc(dVel); }
	double AxisGetJogDec(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetJogDec(); }
	void AxisSetJogDec(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetJogDec(dVel); }
	double AxisGetNormAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetNormAcc(); }
	void AxisSetNormAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetNormAcc(dVel); }
	double AxisGetNormDec(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetNormDec(); }
	void AxisSetNormDec(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetNormDec(dVel); }
	double AxisGetHomeAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeAcc(); }
	void AxisSetHomeAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetHomeAcc(dVel); }
	double AxisGetHomeDec(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeDec(); }
	void AxisSetHomeDec(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetHomeDec(dVel); }
	double AxisGetMaxVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxVel(); }
	void AxisSetMaxVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxVel(dVel); }
	double AxisGetMinVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinVel(); }
	void AxisSetMinVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinVel(dVel); }
	double AxisGetMaxAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxAcc(); }
	void AxisSetMaxAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxAcc(dVel); }
	double AxisGetMinAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinAcc(); }
	void AxisSetMinAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinAcc(dVel); }
	double AxisGetPosSWLimit(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetPosSWLimit(); }
	void AxisSetPosSWLimit(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetPosSWLimit(dVel); }
	double AxisGetNegSWLimit(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetNegSWLimit(); }
	void AxisSetNegSWLimit(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetNegSWLimit(dVel); }
	double AxisGetMaxHomeVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxHomeVel(); }
	void AxisSetMaxHomeVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxHomeVel(dVel); }
	double AxisGetMinHomeVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinHomeVel(); }
	void AxisSetMinHomeVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinHomeVel(dVel); }
	double AxisGetMaxHomeAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxHomeAcc(); }
	void AxisSetMaxHomeAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxHomeAcc(dVel); }
	double AxisGetMinHomeAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinHomeAcc(); }
	void AxisSetMinHomeAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinHomeAcc(dVel); }
	double AxisGetMaxJogVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxJogVel(); }
	void AxisSetMaxJogVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxJogVel(dVel); }
	double AxisGetMinJogVel(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinJogVel(); }
	void AxisSetMinJogVel(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinJogVel(dVel); }
	double AxisGetMaxJogAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMaxJogVel(); }
	void AxisSetMaxJogAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMaxJogAcc(dVel); }
	double AxisGetMinJogAcc(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetMinJogAcc(); }
	void AxisSetMinJogAcc(int axis_no, double dVel) { m_pMotion->m_pAxis[axis_no]->SetMinJogAcc(dVel); }
	double AxisGetTriggerPeriod(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetTriggerPeriod(); }
	void AxisSetTriggerPeriod(int axis_no, double nPos) { return m_pMotion->m_pAxis[axis_no]->SetTriggerPeriod(nPos); }
	BOOL AxisGetPosLimitLevel(int axis_no, long level) { return m_pMotion->m_pAxis[axis_no]->GetPosLimitLevel(level); }
	BOOL AxisGetPosLimitSensor(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetPosLimitSensor(); }
	BOOL AxisGetNegLimitLevel(int axis_no, long level) { return m_pMotion->m_pAxis[axis_no]->GetNegLimitLevel(level); }
	BOOL AxisGetNegLimitSensor(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetNegLimitSensor(); }
	BOOL AxisGetHomeSensor(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeSensor(); }
	BOOL AxisAmpEnable(int axis_no) { return m_pMotion->m_pAxis[axis_no]->AmpEnable(); }
	BOOL AxisAmpDisable(int axis_no) { return m_pMotion->m_pAxis[axis_no]->AmpDisable(); }

	DWORD MotReadOriginResult(int axis_no) { return m_pMotion->ReadOriginResult(axis_no); }

	void AxisSetHomeFinished(int axis_no, BOOL bFinished) { return m_pMotion->m_pAxis[axis_no]->SetHomeFinished(bFinished); }
	void AxisSetOriginReturn(int axis_no, BOOL bState) { m_pMotion->m_pAxis[axis_no]->SetOriginReturn(bState); }

	void SaveAxisConfig(int axis_no) { m_pMotion->m_pAxis[axis_no]->SaveAxisConfig(); }

public:
	CAxis* GetMotAxis(int axis_no) { return m_pMotion->m_pAxis[axis_no]; }

	void SetAxisPos(int axis_no, int pos_no, double dVal) { m_dAxisPosition[axis_no][pos_no] = dVal; }
	double GetAxisPos(int axis_no, int pos_no) { return m_dAxisPosition[axis_no][pos_no]; }
	
	double GetJogSpeed() { return m_dJogSpeed;  }	//20180910 by add

	BOOL ReadInput(int axis_no, long lBitNo) { return m_pMotion->m_pAxis[axis_no]->ReadInput(lBitNo);  }
	BOOL ReadOutput(int axis_no, long lBitNo) { return m_pMotion->m_pAxis[axis_no]->ReadOutput(lBitNo); }	//20180910 by add
	void WriteOutput(int axis_no, long lBitNo, DWORD dwVal) { return m_pMotion->WriteOutput(axis_no, lBitNo, dwVal); }

	void SetFilePath();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MOTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlButtonJogType1();
	void ClickBtnenhctrlButtonJogType2();
	void ClickBtnenhctrlLabelAxis8();
	void ClickBtnenhctrlLabelAxis9();
	void ClickBtnenhctrlLabelAxis10();
	void ClickBtnenhctrlLabelAxisName4();
	void ClickBtnenhctrlLabelAxisName5();
	void ClickBtnenhctrlLabelAxisName6();
	void ClickBtnenhctrlLabelAxisName7();
	void ClickBtnenhctrlButtonMovepos();
	void ClickBtnenhctrlButtonSetpos();
	void ClickBtnenhctrlButtonApply();			//MOTION 저장
	void ClickBtnenhctrlButtonOk();
	void ClickBtnenhctrlButtonHome();
	void ClickBtnenhctrlLabelAxisName1();
	void ClickBtnenhctrlLabelAxisName2();
	void ClickBtnenhctrlLabelAxisName3();
	void ClickBtnenhctrlButtonAmpon();
	void ClickBtnenhctrlButtonAmpoff();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	void ClickBtnenhctrlButtonEmo();
	void ClickBtnenhctrlButtonMotPara();
	afx_msg void OnCbnSelchangeComboJogSpeed();
	void ClickBtnenhctrlButtonMovehome();
	void ClickBtnenhctrlLabelAxisName8();
	void ClickBtnenhctrlLabelAxisAmp1();
	void ClickBtnenhctrlLabelAxisAmp2();
	void ClickBtnenhctrlLabelAxisAmp3();
};

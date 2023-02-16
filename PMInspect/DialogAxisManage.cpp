// DialogAxisManage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogAxisManage.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"
// CDialogAxisManage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogAxisManage, CDialogEx)

CDialogAxisManage::CDialogAxisManage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_AXIS_MANAGER, pParent)
{
	m_iAxis = 0;
}

CDialogAxisManage::~CDialogAxisManage()
{
}

void CDialogAxisManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDS_MAX_VEL, m_ctrlMaxVel);
	DDX_Control(pDX, IDS_MAX_ACC, m_ctrlMaxAcc);
	DDX_Control(pDX, IDS_MIN_ACC, m_ctrlMinAcc);
	DDX_Control(pDX, IDS_MIN_VEL, m_ctrlMinVel);
	DDX_Control(pDX, IDC_AXIS_NAME, m_ctrlAxisName);
	DDX_Control(pDX, IDS_CUR_POS, m_ctrlCurPos);
	DDX_Control(pDX, IDS_AMP_STATE, m_ctrlAmpState);
	DDX_Control(pDX, IDS_POSITIVE_SENSOR, m_ctrlPosSensor);
	DDX_Control(pDX, IDS_NEGATIVE_SENSOR, m_ctrlNegSensor);
	DDX_Control(pDX, IDS_HOME_SENSOR, m_ctrlHomeSensor);
	DDX_Control(pDX, IDS_VELOCITY, m_ctrlVel);
	DDX_Control(pDX, IDS_ACC_TIME, m_ctrlAcc);
	DDX_Control(pDX, IDS_ORIGIN_VEL, m_ctrlOriginVel);
	DDX_Control(pDX, IDS_ORIGIN_VEL2, m_ctrlOriginVel2);
	DDX_Control(pDX, IDS_ORIGIN_VEL3, m_ctrlOriginVel3);
	DDX_Control(pDX, IDS_JOG_VEL, m_ctrlJogVel);
	DDX_Control(pDX, IDS_ORIGIN_ACC, m_ctrlOriginAcc);
	DDX_Control(pDX, IDS_JOG_ACC, m_ctrlJogAcc);
	DDX_Control(pDX, IDS_POS_SW_LIMIT, m_ctrlPosSWLimit);
	DDX_Control(pDX, IDS_NEG_SW_LIMIT, m_ctrlNegSWLimit);
	DDX_Control(pDX, IDS_TRIGGER_PERIOD, m_ctrlTriggerPeriod);
}


BEGIN_MESSAGE_MAP(CDialogAxisManage, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDialogAxisManage, CDialog)
	ON_EVENT(CDialogAxisManage, IDS_VELOCITY, DISPID_CLICK, OnClickVelocity, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_ACC_TIME, DISPID_CLICK, OnClickAccTime, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_ORIGIN_VEL, DISPID_CLICK, OnClickOriginVel, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_ORIGIN_ACC, DISPID_CLICK, OnClickOriginAcc, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_JOG_VEL, DISPID_CLICK, OnClickJogVel, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_JOG_ACC, DISPID_CLICK, OnClickJogAcc, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_MAX_VEL, DISPID_CLICK, OnClickMaxVel, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_MIN_VEL, DISPID_CLICK, OnClickMinVel, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_MAX_ACC, DISPID_CLICK, OnClickMaxAcc, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_MIN_ACC, DISPID_CLICK, OnClickMinAcc, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_POS_SW_LIMIT, DISPID_CLICK, OnClickPosSwLimit, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_NEG_SW_LIMIT, DISPID_CLICK, OnClickNegSwLimit, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDB_OK, DISPID_CLICK, CDialogAxisManage::ClickOk, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDB_SAVE_AXIS, DISPID_CLICK, CDialogAxisManage::ClickSaveAxis, VTS_NONE)
	//ON_EVENT(CDialogAxisManage, IDB_AMP_ON, DISPID_CLICK, CDialogAxisManage::ClickAmpOn, VTS_NONE)
	//ON_EVENT(CDialogAxisManage, IDB_AMP_OFF, DISPID_CLICK, CDialogAxisManage::ClickAmpOff, VTS_NONE)
	//ON_EVENT(CDialogAxisManage, IDB_AMP_RESET, DISPID_CLICK, CDialogAxisManage::ClickAmpReset, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDB_MOTOR_PARAM_PREV, DISPID_CLICK, CDialogAxisManage::ClickMotorParamPrev, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDB_MOTOR_PARAM_NEXT, DISPID_CLICK, CDialogAxisManage::ClickMotorParamNext, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_TRIGGER_PERIOD, DISPID_CLICK, CDialogAxisManage::ClickTriggerPeriod, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_ORIGIN_VEL2, DISPID_CLICK, CDialogAxisManage::ClickOriginVel2, VTS_NONE)
	ON_EVENT(CDialogAxisManage, IDS_ORIGIN_VEL3, DISPID_CLICK, CDialogAxisManage::ClickOriginVel3, VTS_NONE)
END_EVENTSINK_MAP()


// CDialogAxisManage 메시지 처리기입니다.
void CDialogAxisManage::DispMotorParam()
{
	CString strVal;
	CString tmpAxisName;

	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	if (AXIS_0 == m_iAxis)
	{
		GetDlgItem(IDS_TRIGGER_PERIOD)->ShowWindow(TRUE);
		GetDlgItem(IDB_TRIGGER_PERIOD)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDS_TRIGGER_PERIOD)->ShowWindow(FALSE);
		GetDlgItem(IDB_TRIGGER_PERIOD)->ShowWindow(FALSE);
	}

	tmpAxisName = pdlg->m_pDlgMotion->AxisGetAxisName(m_iAxis);
	strVal.Format(_T("%d. %s"), m_iAxis, tmpAxisName);
	m_ctrlAxisName.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetNormVel(m_iAxis));
	m_ctrlVel.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetHomeVel(m_iAxis));
	m_ctrlOriginVel.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetHomeVel2(m_iAxis));
	m_ctrlOriginVel2.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetHomeVel3(m_iAxis));
	m_ctrlOriginVel3.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetJogVel(m_iAxis));
	m_ctrlJogVel.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetNormAcc(m_iAxis));
	m_ctrlAcc.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetHomeAcc(m_iAxis));
	m_ctrlOriginAcc.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetJogAcc(m_iAxis));
	m_ctrlJogAcc.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetMaxVel(m_iAxis));
	m_ctrlMaxVel.SetCaption(strVal);

	strVal.Format(_T("%d"), (int)pdlg->m_pDlgMotion->AxisGetMinVel(m_iAxis));
	m_ctrlMinVel.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetMaxAcc(m_iAxis));
	m_ctrlMaxAcc.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetMinAcc(m_iAxis));
	m_ctrlMinAcc.SetCaption(strVal);

	strVal.Format(_T("%6.3f"), pdlg->m_pDlgMotion->AxisGetPosSWLimit(m_iAxis));
	m_ctrlPosSWLimit.SetCaption(strVal);

	strVal.Format(_T("%6.3f"), pdlg->m_pDlgMotion->AxisGetNegSWLimit(m_iAxis));
	m_ctrlNegSWLimit.SetCaption(strVal);

	strVal.Format(_T("%3.2f"), pdlg->m_pDlgMotion->AxisGetTriggerPeriod(m_iAxis));
	m_ctrlTriggerPeriod.SetCaption(strVal);

	//>> 130401 JSPark
	if (pdlg->m_pDlgMotion->AxisGetAxisUse(m_iAxis) == USE)
	{
		// Pos Sensor
		m_ctrlPosSensor.SetCaption(_T("Off"));
		m_ctrlPosSensor.SetBackColor(GREEN);
		m_ctrlPosSensor.SetForeColor(BLACK);
		m_ctrlPosSensor.EnableWindow(TRUE);
		// Home Sensor
		m_ctrlHomeSensor.SetCaption(_T("Off"));
		m_ctrlHomeSensor.SetBackColor(GREEN);
		m_ctrlHomeSensor.SetForeColor(BLACK);
		m_ctrlHomeSensor.EnableWindow(TRUE);
		// Neg Sensor
		m_ctrlNegSensor.SetCaption(_T("Off"));
		m_ctrlNegSensor.SetBackColor(GREEN);
		m_ctrlNegSensor.SetForeColor(BLACK);
		m_ctrlNegSensor.EnableWindow(TRUE);
	}
	else
	{
		// Pos Sensor
		m_ctrlPosSensor.SetCaption(_T("Not Use"));
		m_ctrlPosSensor.SetBackColor(LTGRAY);
		m_ctrlPosSensor.SetForeColor(RED);
		m_ctrlPosSensor.EnableWindow(FALSE);
		// Home Sensor
		m_ctrlHomeSensor.SetCaption(_T("Not Use"));
		m_ctrlHomeSensor.SetBackColor(LTGRAY);
		m_ctrlHomeSensor.SetForeColor(RED);
		m_ctrlHomeSensor.EnableWindow(FALSE);
		// Neg Sensor
		m_ctrlNegSensor.SetCaption(_T("Not Use"));
		m_ctrlNegSensor.SetBackColor(LTGRAY);
		m_ctrlNegSensor.SetForeColor(RED);
		m_ctrlNegSensor.EnableWindow(FALSE);
	}
}

void CDialogAxisManage::DispUnitState()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	double dCurPos = 0;
	CString strValue;

	pdlg->m_pDlgMotion->AxisGetPosition(m_iAxis, dCurPos);
	strValue.Format(_T("%6.3f"), dCurPos);
	m_ctrlCurPos.SetCaption(strValue);

	//>> 130401 JSPark
	if (pdlg->m_pDlgMotion->AxisGetAxisUse(m_iAxis) == USE)
	{
		if (pdlg->m_pDlgMotion->AxisGetPosLimitSensor(m_iAxis))
		{
			if (m_ctrlPosSensor.GetBackColor() != YELLOW)
			{
				m_ctrlPosSensor.SetCaption(_T("On"));
				m_ctrlPosSensor.SetBackColor(YELLOW);
				m_ctrlPosSensor.SetForeColor(RED);
			}
		}
		else
		{
			if (m_ctrlPosSensor.GetBackColor() != GREEN)
			{
				m_ctrlPosSensor.SetCaption(_T("Off"));
				m_ctrlPosSensor.SetBackColor(GREEN);
				m_ctrlPosSensor.SetForeColor(BLACK);
			}
		}

		if (pdlg->m_pDlgMotion->AxisGetNegLimitSensor(m_iAxis))
		{
			if (m_ctrlNegSensor.GetBackColor() != YELLOW)
			{
				m_ctrlNegSensor.SetCaption(_T("On"));
				m_ctrlNegSensor.SetBackColor(YELLOW);
				m_ctrlNegSensor.SetForeColor(RED);
			}
		}
		else
		{
			if (m_ctrlNegSensor.GetBackColor() != GREEN)
			{
				m_ctrlNegSensor.SetCaption(_T("Off"));
				m_ctrlNegSensor.SetBackColor(GREEN);
				m_ctrlNegSensor.SetForeColor(BLACK);
			}
		}

		if (pdlg->m_pDlgMotion->AxisGetHomeSensor(m_iAxis))
		{
			if (m_ctrlHomeSensor.GetBackColor() != YELLOW)
			{
				m_ctrlHomeSensor.SetCaption(_T("On"));
				m_ctrlHomeSensor.SetBackColor(YELLOW);
				m_ctrlHomeSensor.SetForeColor(RED);
			}
		}
		else
		{
			if (m_ctrlHomeSensor.GetBackColor() != GREEN)
			{
				m_ctrlHomeSensor.SetCaption(_T("Off"));
				m_ctrlHomeSensor.SetBackColor(GREEN);
				m_ctrlHomeSensor.SetForeColor(BLACK);
			}
		}
	}

	if (pdlg->m_pDlgMotion->AxisGetAmpFaultState(m_iAxis)) //Amp Fault 상태 체크 함수 수정
	{
		if (m_ctrlAmpState.GetBackColor() != GREEN)
		{
			m_ctrlAmpState.SetCaption(_T("Normal"));
			m_ctrlAmpState.SetBackColor(GREEN);
			m_ctrlAmpState.SetForeColor(BLACK);
		}
	}
	else
	{
		if (m_ctrlAmpState.GetBackColor() != YELLOW)
		{
			m_ctrlAmpState.SetCaption(_T("Fault"));
			m_ctrlAmpState.SetBackColor(YELLOW);
			m_ctrlAmpState.SetForeColor(RED);
		}
	}
}

void CDialogAxisManage::ClickOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	KillTimer(TIMER_AXIS_MANAGE);
	CDialog::OnOK();
}


BOOL CDialogAxisManage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	DispMotorParam();
	SetTimer(TIMER_AXIS_MANAGE, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogAxisManage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case TIMER_AXIS_MANAGE:
		DispUnitState();
		break;
	}

	CDialog::OnTimer(nIDEvent);
}


void CDialogAxisManage::ClickSaveAxis()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CAxisManageDlg::OnClickSaveAxis()-Start"), SYSTEM_LOG);

	if (AfxMessageBox(_T("Really Save Motor Parameter?"), MB_YESNO) == IDNO)
	{
		//WriteLog(_T("CAxisManageDlg::OnClickSaveAxis()-Cancel"), SYSTEM_LOG);

		return;
	}

	CString strVal;
	pdlg->m_pDlgMotion->AxisSetNormVel(m_iAxis, _ttof(m_ctrlVel.GetCaption()));
	pdlg->m_pDlgMotion->AxisSetHomeVel(m_iAxis, _ttof(m_ctrlOriginVel.GetCaption()));
	pdlg->m_pDlgMotion->AxisSetHomeVel2(m_iAxis, _ttof(m_ctrlOriginVel2.GetCaption()));
	pdlg->m_pDlgMotion->AxisSetHomeVel3(m_iAxis, _ttof(m_ctrlOriginVel3.GetCaption()));
	pdlg->m_pDlgMotion->AxisSetJogVel(m_iAxis, _ttof(m_ctrlJogVel.GetCaption()));

	strVal = m_ctrlAcc.GetCaption();
	pdlg->m_pDlgMotion->AxisSetNormAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetNormDec(m_iAxis, _ttof(strVal));

	strVal = m_ctrlOriginAcc.GetCaption();
	pdlg->m_pDlgMotion->AxisSetHomeAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetHomeDec(m_iAxis, _ttof(strVal));

	strVal = m_ctrlJogAcc.GetCaption();
	pdlg->m_pDlgMotion->AxisSetJogAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetJogDec(m_iAxis, _ttof(strVal));

	strVal = m_ctrlPosSWLimit.GetCaption();
	pdlg->m_pDlgMotion->AxisSetPosSWLimit(m_iAxis, _ttof(strVal));

	strVal = m_ctrlNegSWLimit.GetCaption();
	pdlg->m_pDlgMotion->AxisSetNegSWLimit(m_iAxis, _ttof(strVal));

	strVal = m_ctrlMaxVel.GetCaption();
	pdlg->m_pDlgMotion->AxisSetMaxVel(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMaxHomeVel(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMaxJogVel(m_iAxis, _ttof(strVal));

	strVal = m_ctrlMinVel.GetCaption();
	pdlg->m_pDlgMotion->AxisSetMinVel(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMinHomeVel(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMinJogVel(m_iAxis, _ttof(strVal));

	strVal = m_ctrlMaxAcc.GetCaption();
	pdlg->m_pDlgMotion->AxisSetMaxAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMaxHomeAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMaxJogAcc(m_iAxis, _ttof(strVal));

	strVal = m_ctrlMinAcc.GetCaption();
	pdlg->m_pDlgMotion->AxisSetMinAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMinHomeAcc(m_iAxis, _ttof(strVal));
	pdlg->m_pDlgMotion->AxisSetMinJogAcc(m_iAxis, _ttof(strVal));

	strVal = m_ctrlTriggerPeriod.GetCaption();
	pdlg->m_pDlgMotion->AxisSetTriggerPeriod(m_iAxis, _ttof(strVal));

	pdlg->m_pDlgMotion->SaveAxisConfig(m_iAxis);

	//WriteLog(_T("CAxisManageDlg::OnClickSaveAxis()-End"), SYSTEM_LOG);
}


void CDialogAxisManage::ClickAmpOn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//BOOL rslt = pdlg->m_pDlgMotion->AxisAmpEnable(m_iAxis);
}


void CDialogAxisManage::ClickAmpOff()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	/*
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_pDlgMotion->AxisAmpDisable(m_iAxis);
	pdlg->m_pDlgMotion->AxisSetHomeFinished(m_iAxis, FALSE);
	pdlg->m_pDlgMotion->AxisSetOriginReturn(m_iAxis, FALSE);
	*/
}


void CDialogAxisManage::ClickAmpReset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//pdlg->m_pDlgMotion->AxisClearState();
	//pdlg->m_pDlgMotion->AxisClearAxis(m_iAxis);
	//pdlg->m_pDlgMotion->AxisResetAmpFault(m_iAxis);
}


void CDialogAxisManage::ClickMotorParamPrev()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_iAxis--;

	if (0 > m_iAxis)
		m_iAxis = MAX_AXIS_NUM - 1;

	DispMotorParam();
}


void CDialogAxisManage::ClickMotorParamNext()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_iAxis++;

	if (MAX_AXIS_NUM == m_iAxis)
		m_iAxis = 0;

	DispMotorParam();
}

void CDialogAxisManage::OnClickVelocity()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CDialogAxisManage::OnClickVelocity()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlVel.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());
		int iMin = (int)pdlg->m_pDlgMotion->AxisGetMinVel(m_iAxis);
		int iMax = (int)pdlg->m_pDlgMotion->AxisGetMaxVel(m_iAxis);

		if (iVal < iMin || iVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CCDialogAxisManage::OnClickVelocity(F)-End"), SYSTEM_LOG);
			return;
		}
		strVal.Format(_T("%d"), iVal);
		m_ctrlVel.SetCaption(strVal);

		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickVelocity(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickAccTime()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickAccTime()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlAcc.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		double dVal = _ttof(dlg.GetstrNum());
		double iMin = pdlg->m_pDlgMotion->AxisGetMinAcc(m_iAxis);
		double iMax = pdlg->m_pDlgMotion->AxisGetMaxAcc(m_iAxis);

		if (dVal < iMin || dVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %3.2f ~ %3.2f is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickAccTime(F)-End"), SYSTEM_LOG);
			return;
		}
		strVal.Format(_T("%3.2f"), dVal);
		m_ctrlAcc.SetCaption(strVal);

		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickAccTime(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickOriginVel()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlOriginVel.GetCaption();

	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());
		int iMin = (int)pdlg->m_pDlgMotion->AxisGetMinHomeVel(m_iAxis);
		int iMax = (int)pdlg->m_pDlgMotion->AxisGetMaxHomeVel(m_iAxis);

		if (iVal < iMin || iVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickOriginVel(F)-End"), SYSTEM_LOG);
			return;
		}
		strVal.Format(_T("%d"), iVal);
		m_ctrlOriginVel.SetCaption(strVal);

		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickOriginAcc()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CDialogAxisManage::OnClickOriginAcc()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlOriginAcc.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		double dVal = _ttof(dlg.GetstrNum());
		double dMin = pdlg->m_pDlgMotion->AxisGetMinHomeAcc(m_iAxis);
		double dMax = pdlg->m_pDlgMotion->AxisGetMaxHomeAcc(m_iAxis);

		if (dVal < dMin || dVal > dMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %3.2f ~ %3.2f is available"), dMin, dMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickOriginAcc(F)-End"), SYSTEM_LOG);
			return;
		}

		strVal.Format(_T("%3.2f"), dVal);
		m_ctrlOriginAcc.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickOriginAcc(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickJogVel()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CDialogAxisManage::OnClickJogVel()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlJogVel.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());
		int iMin, iMax;
		iMin = (int)pdlg->m_pDlgMotion->AxisGetMinJogVel(m_iAxis);
		iMax = (int)pdlg->m_pDlgMotion->AxisGetMaxJogVel(m_iAxis);

		if (iVal < iMin || iVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickJogVel(F)-End"), SYSTEM_LOG);
			return;
		}

		strVal.Format(_T("%d"), iVal);
		m_ctrlJogVel.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickJogVel(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickJogAcc()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CDialogAxisManage::OnClickJogAcc()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlJogAcc.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		double dVal = _ttof(dlg.GetstrNum());
		double dMin = pdlg->m_pDlgMotion->AxisGetMinJogAcc(m_iAxis);
		double dMax = pdlg->m_pDlgMotion->AxisGetMaxJogAcc(m_iAxis);

		if (dVal < dMin || dVal > dMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %3.2f ~ %3.2f is available"), dMin, dMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickJogAcc(F)-End"), SYSTEM_LOG);
			return;
		}

		strVal.Format(_T("%3.2f"), dVal);
		m_ctrlJogAcc.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickJogAcc(C)-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickMaxVel()
{
	//WriteLog(_T("CDialogAxisManage::OnClickMaxVel()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlMaxVel.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());

		strVal.Format(_T("%d"), iVal);
		m_ctrlMaxVel.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickMaxVel()-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickMinVel()
{
	//WriteLog(_T("CDialogAxisManage::OnClickMinVel()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlMinVel.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());

		strVal.Format(_T("%d"), iVal);
		m_ctrlMinVel.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickMinVel()-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickMaxAcc()
{
	//WriteLog(_T("CDialogAxisManage::OnClickMaxAcc()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlMaxAcc.GetCaption();
	CDialogNumber_Box dlg;
	if (dlg.DoModal() == IDOK)
	{
		double dVal = _ttof(dlg.GetstrNum());

		strVal.Format(_T("%3.2f"), dVal);
		m_ctrlMaxAcc.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickMaxAcc()-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickMinAcc()
{
	//WriteLog(_T("CDialogAxisManage::OnClickMinAcc()-Start"), SYSTEM_LOG);
	CString strVal = m_ctrlMinAcc.GetCaption();
	CDialogNumber_Box dlg;
	if (dlg.DoModal() == IDOK)
	{
		double dVal = _ttof(dlg.GetstrNum());

		strVal.Format(_T("%3.2f"), dVal);
		m_ctrlMinAcc.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickMaxAcc()-End"), SYSTEM_LOG);
}

void CDialogAxisManage::OnClickPosSwLimit()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickPosSwLimit()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlPosSWLimit.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		strVal = dlg.GetstrNum();
		double dVal = _ttof(strVal);
		double dMin = pdlg->m_pDlgMotion->AxisGetNegSWLimit(m_iAxis);
		if (dVal <= dMin)
		{
			CString strMsg;
			strMsg.Format(_T("Must be more than %6.3f"), dMin);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickPosSwLimit(F)-End"), SYSTEM_LOG);
			return;
		}
		m_ctrlPosSWLimit.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::OnClickPosSwLimit(C)-End"), SYSTEM_LOG);

}

void CDialogAxisManage::OnClickNegSwLimit()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickNegSwLimit()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlNegSWLimit.GetCaption();
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		strVal = dlg.GetstrNum();
		double dVal = _ttof(strVal);
		double dMax = pdlg->m_pDlgMotion->AxisGetPosSWLimit(m_iAxis);
		if (dVal >= dMax)
		{
			CString strMsg;
			strMsg.Format(_T("Must be less than %6.3f"), dMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickNegSwLimit(F)-End"), SYSTEM_LOG);
			return;
		}
		m_ctrlNegSWLimit.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickNegSwLimit(C)-End"), SYSTEM_LOG);
}


void CDialogAxisManage::ClickTriggerPeriod()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//WriteLog(_T("CDialogAxisManage::ClickTriggerPeriod()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlTriggerPeriod.GetCaption();
	CDialogNumber_Box dlg;

	int min, max;
	min = 1;
	max = 3000;

	if (dlg.DoModal() == IDOK)
	{
		int dVal = _ttoi(dlg.GetstrNum());

		if (dVal < min || dVal > max)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), min, max);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::ClickTriggerPeriod(F)-End"), SYSTEM_LOG);
			return;
		}

		strVal.Format(_T("%d"), dVal);
		m_ctrlTriggerPeriod.SetCaption(strVal);
		//WriteLog(strVal, SYSTEM_LOG);
	}
	//WriteLog(_T("CDialogAxisManage::ClickTriggerPeriod(C)-End"), SYSTEM_LOG);
}


void CDialogAxisManage::ClickOriginVel2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel2()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlOriginVel2.GetCaption();

	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());
		int iMin = (int)pdlg->m_pDlgMotion->AxisGetMinHomeVel(m_iAxis);
		int iMax = (int)pdlg->m_pDlgMotion->AxisGetMaxHomeVel(m_iAxis);

		if (iVal < iMin || iVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickOriginVel2(F)-End"), SYSTEM_LOG);
			return;
		}
		strVal.Format(_T("%d"), iVal);
		m_ctrlOriginVel2.SetCaption(strVal);

		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel2(C)-End"), SYSTEM_LOG);
}


void CDialogAxisManage::ClickOriginVel3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;
	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel3()-Start"), SYSTEM_LOG);

	CString strVal = m_ctrlOriginVel3.GetCaption();

	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK)
	{
		int iVal = _ttoi(dlg.GetstrNum());
		int iMin = (int)pdlg->m_pDlgMotion->AxisGetMinHomeVel(m_iAxis);
		int iMax = (int)pdlg->m_pDlgMotion->AxisGetMaxHomeVel(m_iAxis);

		if (iVal < iMin || iVal > iMax)
		{
			CString strMsg;
			strMsg.Format(_T("Only %d ~ %d is available"), iMin, iMax);
			AfxMessageBox(strMsg);
			//WriteLog(_T("CDialogAxisManage::OnClickOriginVel3(F)-End"), SYSTEM_LOG);
			return;
		}
		strVal.Format(_T("%d"), iVal);
		m_ctrlOriginVel3.SetCaption(strVal);

		//WriteLog(strVal, SYSTEM_LOG);
	}

	//WriteLog(_T("CDialogAxisManage::OnClickOriginVel3(C)-End"), SYSTEM_LOG);
}

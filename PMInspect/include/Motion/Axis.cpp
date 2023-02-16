#include "StdAfx.h"
#include "Axis.h"



CCriticalSection g_AxisSyncIntlock;

CAxis::CAxis(void)
{
	m_lAxis = 0;
	m_iAxisUse = 0;
	m_NormVel = 0.0;
	m_JogVel = 0.0;
	m_iMotorType = SERVO_MOTOR;
	m_IOriginMethod = 0;
	m_bOriginReturnFlag = FALSE;
	m_bHomingErr = FALSE;
	m_bThreadStarted = FALSE;

	m_dTriggerPeriodPos = 0.0;
}

CAxis::~CAxis(void)
{
	m_Paramanager.Release();
}

//본 프로젝트에서는 사용하지 않는다.
BOOL CAxis::AXTLibInit()
{
	BOOL bRet(TRUE);
	/*
	// Initialize library
	DWORD Code = AxlOpenNoReset(7);
	if (Code == AXT_RT_SUCCESS)
	TRACE("Library is initialized .\n");
	else
	{
	AfxMessageBox(_T("Failed initialization."));
	return FALSE;
	}
	*/

	return TRUE;
}

BOOL CAxis::AXTMotionInit()
{
	/*
	CString strInfo;
	DWORD Code;
	//Module 확인
	DWORD dwStatus;

	if (!AxlIsOpened())
	{
	if (!AXTLibInit())
	return FALSE;
	}


	// ※ [CAUTION] 아래와 다른 Mot파일(모션 설정파일)을 사용할 경우 경로를 변경하십시요.
	char szFilePath[] = "D:\\SDC_WindowControlPrj\\MotFile\\MotionDefault.mot";

	//++ 지정한 Mot파일의 설정값들로 모션보드의 설정값들을 일괄변경 적용합니다.
	if(AxmMotLoadParaAll(szFilePath) != AXT_RT_SUCCESS)
	{
	TRACE(_T("Mot File Not Found."));
	}


	Code = AxmInfoIsMotionModule(&dwStatus);

	if (Code == AXT_RT_SUCCESS)
	{
	if (dwStatus == STATUS_EXIST)
	{
	// 축 정보를 읽어 온다. 기본 설정...이후 공통 항목으로 다시 설정 한다.
	//m_pAxis[0]->bAjinMotorParamLoadAll();
	}
	else
	AfxMessageBox(_T("Ajin Motion Module 이 확인되지 않았습니다."));
	}
	*/
	return TRUE;
}

/*
// 단일 축인 경우 AXIS 클래스에서 사용하면 되지만 하위 클래스에서 다수 축으로 사용할 경우 적합하지 않음.
BOOL CAxis::bAjinMotorParamSaveAll()
{
	BOOL rslt = TRUE;
	
	DWORD ret(TRUE);
	CString strFileName = SYSTEM_AXIS_MOTION_PATH;

	int len = strFileName.GetLength();
	char* mbszFileName = new char[len + 1];
	//wcstombs(mbszFileName, strFileName.GetBuffer(0), len);
	sprintf(mbszFileName, "%S", strFileName);
	ret = AxmMotSaveParaAll(mbszFileName); // 모든 축의 초기화 Data 저장
	delete[] mbszFileName;

	if (!ret)
	rslt = TRUE;
	else
	rslt = FALSE;
	
	return rslt;
}

BOOL CAxis::bAjinMotorParamLoadAll()
{
	BOOL rslt = TRUE;
	

	DWORD ret(TRUE);
	CString strFileName = SYSTEM_AXIS_MOTION_PATH;

	int len = strFileName.GetLength();
	char* mbszFileName = new char[len + 1];
	//wcstombs(mbszFileName, strFileName.GetBuffer(0), len);
	sprintf(mbszFileName, "%S", strFileName);
	ret = AxmMotLoadParaAll(mbszFileName); // 모든 축의 초기화 Data 저장
	delete[] mbszFileName;

	if (!ret)
	rslt = TRUE;
	else
	rslt = FALSE;
	
	return rslt;
}
*/

//범용 입/출력 제어
BOOL CAxis::ReadInput(long lBitNo)
{
	DWORD dwOn = 0;

	AxmSignalReadInputBit(m_lAxis, lBitNo, &dwOn);

	return (BOOL)dwOn;
}

void CAxis::WriteOutput(long lBitNo, DWORD dwVal)
{
	AxmSignalWriteOutputBit(m_lAxis, lBitNo, dwVal);
}

BOOL CAxis::ReadOutput(long lBitNo)
{
	DWORD dwOn = 0;

	AxmSignalReadOutputBit(m_lAxis, lBitNo, &dwOn);

	return (BOOL)dwOn;
}


BOOL CAxis::SetTriggerTimeLevel()
{
	BOOL rslt = TRUE;

	if (AXT_RT_SUCCESS != AxmTriggerSetTimeLevel(m_lAxis, 1, HIGH, 0, 0))
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetTriggerBlock(double dStartPos, double dEndPos, double dPeriodPos)
{
	BOOL rslt = TRUE;

	AxmTriggerSetBlock(m_lAxis, dStartPos, dEndPos, dPeriodPos);

	return rslt;
}

void CAxis::SetTriggerReset()
{
	AxmTriggerSetReset(m_lAxis);
}

DWORD CAxis::ReadOriginResult()
{
	DWORD	dwRetCode;

	AxmHomeGetResult(m_lAxis, &dwRetCode);

	return dwRetCode;
}

BOOL CAxis::StartOrigin()
{
	BOOL rslt = TRUE;
	DWORD	dwRetCode;

	//++ 지정한 축에 원점검색을 진행합니다.
	dwRetCode = AxmHomeSetStart(m_lAxis);
	if (dwRetCode != AXT_RT_SUCCESS) {
		rslt = FALSE;
	}

	return rslt;
}


BOOL CAxis::GetNegLimitLevel(long &level)
{
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalGetLimit(m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);
	g_AxisSyncIntlock.Unlock();
	level = NegativeLevel;

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::GetPosLimitLevel(long &level)
{
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalGetLimit(m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);
	g_AxisSyncIntlock.Unlock();
	level = PositiveLevel;

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

void CAxis::SetAxisAccelUnit()
{
	AxmMotSetAccelUnit(m_lAxis, 1);
	AxmMotSetMinVel(m_lAxis, 1);
}

void CAxis::SetAxisConfig()
{
	int msec = 0;
	double ACC_DEC_RATIO(1000.0); //뒤에서 계산 한다. 

	m_Paramanager.Create();
	m_Paramanager.SetPath(DATA_SYSTEM_PATH, _T("Motion.ini"));

	//m_Paramanager.LinkPara(섹션이름, 키이름, 값, 기본값)
	m_Paramanager.LinkPara(m_strAxisName, _T("Event E-Stop Rate"), &msec, _T("100"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Velocity"), &m_NormVel, _T("10000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Max Velocity"), &m_dMaxVel, _T("1000000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Min Velocity"), &m_dMinVel, _T(" 1.0"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Velocity"), &m_HomeVel, _T("10000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Velocity2"), &m_HomeVel2, _T("1000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Velocity3"), &m_HomeVel3, _T("100."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Fine Velocity"), &m_HomeFineVel, _T("100."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Velocity"), &m_JogVel, _T("10000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Max Velocity"), &m_dMaxJogVel, _T("200000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Min Velocity"), &m_dMinJogVel, _T("1."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Positive SW Limit Pos"), &m_dPosSWLimit, _T("100000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Negative SW Limit Pos"), &m_dNegSWLimit, _T("-100000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Axis Orgin Offset"), &m_dOrgOffset, _T("100"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Axis Init Pos"), &m_dAxisInitPos, _T("0."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Axis Safety Pos"), &m_dAxisSafetyPos, _T("0."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Axis Orgin Offset"), &m_dOrgOffset, _T("0."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Axis Trigger Period"), &m_dTriggerPeriodPos, _T("0."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Max Velocity"), &m_dMaxHomeVel, _T("100000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Min Velocity"), &m_dMinHomeVel, _T("1."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Accel Time"), &m_NormAcc, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Decel Time"), &m_NormDec, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Max Accel Time"), &m_dMaxAccTime, _T("2000000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Normal Min Accel Time"), &m_dMinAccTime, _T("20000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Accel Time"), &m_HomeAcc, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Decel Time"), &m_HomeDec, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Fine Accel Time"), &m_HomeFineAcc, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Max Accel Time"), &m_dMaxHomeAccTime, _T("2000000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Home Min Accel Time"), &m_dMinHomeAccTime, _T("20000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Accel Time"), &m_JogAcc, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Decel Time"), &m_JogDec, _T("50000."));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Max Accel Time"), &m_dMaxJogAccTime, _T("2000000"));
	m_Paramanager.LinkPara(m_strAxisName, _T("Jog Min Accel Time"), &m_dMinJogAccTime, _T("20000"));
}

//{ _T("LINEAR_X"), _T("LINEAR_Y"), _T("THETA_T") };
BOOL CAxis::LoadAxisConfig()
{
	BOOL rslt = TRUE;

	if (m_Paramanager.LoadPara())
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

//{ _T("LINEAR_X"), _T("LINEAR_Y"), _T("THETA_T") };
BOOL CAxis::SaveAxisConfig()
{
	BOOL rslt = TRUE;

	if (m_Paramanager.SavePara())
		rslt = TRUE;
	else
		rslt = FALSE;

	return TRUE;
}

BOOL CAxis::VMove(double Velocity, double Acceleration, double Deceleration)
{
	DWORD ret;

	//20130901 lee
	if (GetAxisUse() == NOT_USE) return TRUE;

	g_AxisSyncIntlock.Lock();
	//AxmMotSetAccelUnit(m_lAxis, 1);
	//AxmMotSetMinVel(m_lAxis, 1);
	ret = AxmMoveVel(m_lAxis, Velocity, Acceleration, Deceleration);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::GetHomeSensor()
{
	DWORD upStatus;
	g_AxisSyncIntlock.Lock();
	AxmHomeReadSignal(m_lAxis, &upStatus);
	g_AxisSyncIntlock.Unlock();

	if (upStatus)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}
BOOL CAxis::GetNegLimitSensor()
{
	DWORD upPositiveLevel, upNegativeLevel;
	g_AxisSyncIntlock.Lock();
	AxmSignalReadLimit(m_lAxis, &upPositiveLevel, &upNegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (upNegativeLevel)
		return TRUE;
	else
		return FALSE;
	return TRUE;
}
BOOL CAxis::GetPosLimitSensor()
{
	DWORD upPositiveLevel, upNegativeLevel;
	g_AxisSyncIntlock.Lock();
	AxmSignalReadLimit(m_lAxis, &upPositiveLevel, &upNegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (upPositiveLevel)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::CheckSWLimit(double dPos)
{
	if (dPos > m_dPosSWLimit)
	{
		return FALSE;
	}

	if (dPos < m_dNegSWLimit)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAxis::StartRMove(double dPos)
{
	int val = 0;
	double dVel(0.);
	int nAcc(0), nDec(0);
	DWORD ret;
	CString str;


	if (GetAxisUse() == NOT_USE)
		return TRUE;

	if (!GetAmpOnState())
		return FALSE;

	if (!GetAmpFaultState())
		return FALSE;

	if (!isOriginReturn())
		return FALSE;

	if (!CheckSWLimit(dPos))
		return FALSE;

	dPos = dPos * GetPulse4Mm();
	dVel = GetNormVel();

	AxmMotSetAbsRelMode(m_lAxis, POS_REL_MODE);

	ret = Wait4Done();
	if (!ret)
	{
		str.Format(_T("Ajin B/D Error발생  %s축 Motion Done Error"), GetAxisName());
		AfxMessageBox(str);
		return FALSE;
	}


	g_AxisSyncIntlock.Lock();
	ret = AxmMoveStartPos(m_lAxis, dPos, dVel, GetNormAcc(), GetNormDec());
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}


BOOL CAxis::RMove(double dPos, BOOL bCheckFlag)
{
	if (dPos == 0.0)
		return TRUE;

	int ret = StartRMove(dPos);

	if (!ret)
		return FALSE;

	if (bCheckFlag)
		ret = Wait4Done();

	return ret;

}

BOOL CAxis::StartMove(double dPos, double dVelRatio)
{
	int val = 0;
	DWORD ret(0);

	double dVel(0.);
	int    nAcc(0), nDec(0);
	CString str;

	if (GetAxisUse() == NOT_USE)
		return TRUE;

	if (!GetAmpOnState())
		return FALSE;

	if (!GetAmpFaultState())
		return FALSE;

	if (!isOriginReturn())
		return FALSE;

	if (!CheckSWLimit(dPos))
		return FALSE;

	dPos = dPos * GetPulse4Mm();

	//구간별 속도를 다르게 하기 위해 속도비를 추가함.
	if ((1. > dVelRatio) || (20. < dVelRatio))
		dVelRatio = 1.;

	dVel = GetNormVel() * dVelRatio;

	if (dVel == 0) dVel = 10000.0;

	//시스템 속도 적용 
	//dVel *= (pdlg->m_pDlgMainData->GetSystemVelRatio() / 100.);

	//AxmMotSetAccelUnit(m_lAxis, 1);
	//AxmMotSetMinVel(m_lAxis, 1);
	AxmMotSetAbsRelMode(m_lAxis, POS_ABS_MODE);
	//AxmMotSetProfileMode(m_lAxis, SYM_S_CURVE_MODE);

	g_AxisSyncIntlock.Lock();
	ret = AxmMoveStartPos(m_lAxis, dPos, dVel, GetNormAcc(), GetNormDec());
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
	{
		str.Format(_T("Ajin B/D Error발생  %s축 Error CODE : %d"), GetAxisName(), ret);
		AfxMessageBox(str);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAxis::Move(double dPos, double dVelRatio, BOOL bWaitFlag)
{
	int ret;
	double dCurPos;

	GetPosition(dCurPos);
	double temp = fabs(dPos - dCurPos);

	if (temp == 0) return TRUE;

	ret = StartMove(dPos, dVelRatio);

	if (!ret)
		return FALSE;

	if (bWaitFlag)
		ret = Wait4Done(dPos);
	return ret;
}

BOOL CAxis::GetPosition(double &rPos, BOOL bEncoder)
{
	int ret(0);

	g_AxisSyncIntlock.Lock();

	if (bEncoder)
		ret = AxmStatusGetActPos(m_lAxis, &rPos);
	else
		ret = AxmStatusGetCmdPos(m_lAxis, &rPos);
	
	rPos *= GetMm4Pulse();
	
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::SetStopRate(int msec)
{
	DWORD ret(TRUE);
	g_AxisSyncIntlock.Lock();
	m_lStopDec = msec;
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::SetServoLodRatio(int iType)
{
	g_AxisSyncIntlock.Lock();
	DWORD ret = AxmStatusSetReadServoLoadRatio(m_lAxis, iType);
	g_AxisSyncIntlock.Unlock();
	if (ret != AXT_RT_SUCCESS)
		return FALSE;

	return TRUE;
}

BOOL CAxis::SetPositiveLimit(int act)
{
	/*
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;
	g_AxisSyncIntlock.Lock();
	AxmSignalGetLimit (m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);

	if(act == STOP_EVENT)
	ret = AxmSignalSetLimit (m_lAxis, SLOWDOWN_STOP, PositiveLevel, NegativeLevel);
	else if(act == E_STOP_EVENT || act == ABORT_EVENT)
	ret = AxmSignalSetLimit (m_lAxis, EMERGENCY_STOP, PositiveLevel, NegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
	return TRUE;
	else
	return FALSE;

	*/
	return TRUE;
}

BOOL CAxis::SetNegativeLimit(int act)
{
	/*
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;
	g_AxisSyncIntlock.Lock();
	AxmSignalGetLimit (m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);

	if(act == STOP_EVENT)
	ret = AxmSignalSetLimit (m_lAxis, SLOWDOWN_STOP, PositiveLevel, NegativeLevel);
	else if(act == E_STOP_EVENT || act == ABORT_EVENT)
	ret = AxmSignalSetLimit (m_lAxis, EMERGENCY_STOP, PositiveLevel, NegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
	return TRUE;
	else
	return FALSE;

	*/
	return TRUE;
}


BOOL CAxis::EStop()
{
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmMoveEStop(m_lAxis);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::Stop()
{
	int msec(100);
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	GetStopRate(msec);
	ret = AxmMoveStop(m_lAxis, msec * 0.001);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

BOOL CAxis::MotionDone()
{
	DWORD Status;

	g_AxisSyncIntlock.Lock();
	AxmStatusReadInMotion((int)m_lAxis, &Status); //Motion 중이면 TRUE / 아니면 FALSE
	g_AxisSyncIntlock.Unlock();

	if (Status != FALSE)
		return FALSE;
	else
		return TRUE;

	return TRUE;
}

BOOL CAxis::CheckHomingDone()
{
	if (!IsHomeFinished())
		return FALSE;

	return TRUE;
}

BOOL CAxis::Wait4Done(double dPos, BOOL bUpperInterfaceInterlockCheck, BOOL bLowerInterfaceInterlockCheck)
{
	/*
	CTimerCheck Timer(20000);
	Timer.StartTimer();

	if (GetAxisUse() == NOT_USE) return TRUE;

	while (!MotionDone())
	{
		Delay(2, TRUE);
		if (Timer.IsTimeOver())
			return FALSE;
	}
	*/
	return TRUE;
}



BOOL CAxis::SetAmpFault()
{
	BOOL rslt;

	if (ResetAmpFault())
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::ResetAmpFault()
{
	BOOL rslt = TRUE;
	/*
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalServoAlarmReset(m_lAxis, ENABLE);
	if (ret != AXT_RT_SUCCESS)
	{
		ret = AxmSignalServoAlarmReset(m_lAxis, DISABLE);
		g_AxisSyncIntlock.Unlock();
		return FALSE;
	}

	Delay(100, TRUE);

	ret = AxmSignalServoAlarmReset(m_lAxis, DISABLE);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;
	*/
	return rslt;
}

BOOL CAxis::SetPosition(double pos)
{
	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	//RTEX 전용 함수이나 ML2와 함께 사용해도 무관하다.
	ret = AxmStatusSetPosMatch(m_lAxis, pos * GetMm4Pulse());

	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetElectricGear()
{
	// 설정 속도 단위가 RPM(Revolution Per Minute)으로 맞추고 싶다면.
	// ex>    rpm 계산:
	// 4500 rpm ?
	// unit/ pulse = 1 : 1이면      pulse/ sec 초당 펄스수가 되는데
	// 4500 rpm에 맞추고 싶다면     4500 / 60 초 : 75회전/ 1초
	// 모터가 1회전에 몇 펄스인지 알아야 된다. 이것은 Encoder에 Z상을 검색해보면 알수있다.
	// 1회전:1800 펄스라면 75 x 1800 = 135000 펄스가 필요하게 된다.
	// AxmMotSetMoveUnitPerPulse에 Unit = 1, Pulse = 1800 넣어 동작시킨다.
	// 주의사항 : rpm으로 제어하게 된다면 속도와 가속도 도 rpm단위 값으로 생각하여야 한다.

	// 지정 축의 펄스 당 움직이는 거리를 설정한다.
	BOOL rslt;

	DWORD ret;
	long lAxisNo = 0;
	long lPulse = 1;
	double dUnit = 1;

	g_AxisSyncIntlock.Lock();
	ret = AxmMotSetMoveUnitPerPulse(m_lAxis, dUnit, lPulse);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetDirection(DWORD dwMethod)
{
	//uMethod  0 :OneHighLowHigh, 1 :OneHighHighLow, 2 :OneLowLowHigh, 3 :OneLowHighLow, 4 :TwoCcwCwHigh
	//         5 :TwoCcwCwLow,    6 :TwoCwCcwHigh,   7 :TwoCwCcwLow,   8 :TwoPhase,      9 :TwoPhaseReverse

	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmMotSetPulseOutMethod(m_lAxis, dwMethod); //20130330 lee
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetAmpFaultResetLevel(DWORD dwLv)
{
	// 지정 축의 Servo-Alarm Reset 신호의 출력 레벨을 설정한다.
	// uLevel : LOW(0), HIGH(1)

	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalSetServoAlarmResetLevel(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetAmpfaultlevel(DWORD dwLv)
{
	// 지정 축의 알람 신호 입력 시 비상 정지의 사용 여부 및 신호 입력 레벨을 설정한다.
	// uLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalSetServoAlarm(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetAmpEnableLevel(DWORD dwLv)
{
	// 지정 축의 Servo-On신호의 출력 레벨을 설정한다
	// uLevel : LOW(0), HIGH(1)

	BOOL rslt;
	DWORD ret;

	g_AxisSyncIntlock.Lock();
	ret = AxmSignalSetServoOnLevel(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetHomeLevel(DWORD dwLv)
{
	// 지정 축의 Home 센서 Level 을 설정한다.
	// uLevel : LOW(0), HIGH(1)

	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmHomeSetSignalLevel(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

// 지정 축의 end limit sensor의 사용 유무 및 신호의 입력 레벨을 설정한다. 
// end limit sensor 신호 입력 시 감속정지 또는 급정지에 대한 설정도 가능하다.
// uStopMode: EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
// uPositiveLevel, uNegativeLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
BOOL CAxis::SetNegativeLevel(DWORD dwLv)
{
	BOOL rslt;
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;
	g_AxisSyncIntlock.Lock();
	AxmSignalGetLimit(m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);

	NegativeLevel = dwLv;
	ret = AxmSignalSetLimit(m_lAxis, StopMode, PositiveLevel, NegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

// 지정 축의 end limit sensor의 사용 유무 및 신호의 입력 레벨을 설정한다. 
// end limit sensor 신호 입력 시 감속정지 또는 급정지에 대한 설정도 가능하다.
// uStopMode: EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
// uPositiveLevel, uNegativeLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
BOOL CAxis::SetPositiveLevel(DWORD dwLv)
{
	BOOL rslt;
	DWORD ret;
	DWORD StopMode, PositiveLevel, NegativeLevel;

	g_AxisSyncIntlock.Lock();
	AxmSignalGetLimit(m_lAxis, &StopMode, &PositiveLevel, &NegativeLevel);
	PositiveLevel = dwLv;
	ret = AxmSignalSetLimit(m_lAxis, StopMode, PositiveLevel, NegativeLevel);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetEncoderInput(DWORD dwLv)
{
	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmMotSetEncInputMethod(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::SetZPhase(DWORD dwLv)
{
	BOOL rslt;
	DWORD ret;
	g_AxisSyncIntlock.Lock();
	ret = AxmSignalSetZphaseLevel(m_lAxis, dwLv);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::AmpEnable()
{
	DWORD ret;
	BOOL rslt;

	g_AxisSyncIntlock.Lock();
	ret = AxmSignalServoOn(m_lAxis, ENABLE);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::AmpDisable()
{
	DWORD ret;
	BOOL rslt;

	g_AxisSyncIntlock.Lock();
	ret = AxmSignalServoOn(m_lAxis, DISABLE);
	g_AxisSyncIntlock.Unlock();

	if (ret == AXT_RT_SUCCESS)
	{
		m_bOriginReturnFlag = FALSE;//JMHWANG 130915 
		rslt = TRUE;
	}
	else
		rslt = FALSE;

	return rslt;
}

BOOL CAxis::GetAmpOnState()
{
	DWORD upStatus;
	BOOL rslt = FALSE;

	g_AxisSyncIntlock.Lock();
	AxmSignalIsServoOn(m_lAxis, &upStatus);
	g_AxisSyncIntlock.Unlock();

	if (upStatus)
		rslt = TRUE;
	else
	{
		SetOriginReturn(false); //131020 JSPark
		rslt = FALSE;
	}

	return rslt;
}

BOOL CAxis::GetAmpFaultState()
{
	DWORD upStatus;
	BOOL rslt = FALSE;

	g_AxisSyncIntlock.Lock();
	AxmSignalReadServoAlarm(m_lAxis, &upStatus);
	g_AxisSyncIntlock.Unlock();

	if (upStatus)
	{
		SetOriginReturn(FALSE);
		rslt = FALSE;
	}
	else
		rslt = TRUE;

	return rslt;
}

BOOL CAxis::IsHomeFinished()
{
	BOOL rslt = FALSE;

	g_AxisSyncIntlock.Lock();
	if (isOriginReturn())
	{
		rslt = TRUE;
	}
	else
	{
		rslt = FALSE;
	}
	g_AxisSyncIntlock.Unlock();

	return rslt;
}

void CAxis::SetOriginReturn(BOOL org)
{
	CString strMsg;

	g_AxisSyncIntlock.Lock();
	m_bOriginReturnFlag = org;
	g_AxisSyncIntlock.Unlock();
}

BOOL CAxis::isOriginReturn()
{
	return m_bOriginReturnFlag;
}

BOOL CAxis::SetOriginMethod()
{
	DWORD	dwRetCode;
	BOOL rslt = TRUE;

	//++ 지정한 축의 원점검색 방법을 변경합니다.

	/*
	dwRetCode = AxmHomeSetSignalLevel(m_lAxis, 1);

	if (dwRetCode != AXT_RT_SUCCESS) {
		rslt = FALSE;
	}
	*/

	//AxmHomeSetMethod(m_lAxisNo, lHomeDir, dwHomeSignal, dwZPhaseUse, dHomeClrTime, dHomeOffset);
	dwRetCode = AxmHomeSetMethod(m_lAxis, 0, 1, 0, 1000.0, 0.0);

	if (dwRetCode != AXT_RT_SUCCESS) {
		rslt = FALSE;
	}

	//AxmHomeSetVel(m_lAxisNo, dVelFirst, dVelSecond, dVelThird, dVelLast, dAccFirst, dAccSecond);
	//dwRetCode = AxmHomeSetVel(m_lAxis, 500000, 200000, 10000.0, 500.0, 1000.0, 1000.0);
	dwRetCode = AxmHomeSetVel(m_lAxis, m_HomeVel, m_HomeVel2, m_HomeVel3, 500.0, m_HomeAcc, 1000.0);

	if (dwRetCode != AXT_RT_SUCCESS) {
		rslt = FALSE;
	}

	return rslt;
}

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

//�� ������Ʈ������ ������� �ʴ´�.
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
	//Module Ȯ��
	DWORD dwStatus;

	if (!AxlIsOpened())
	{
	if (!AXTLibInit())
	return FALSE;
	}


	// �� [CAUTION] �Ʒ��� �ٸ� Mot����(��� ��������)�� ����� ��� ��θ� �����Ͻʽÿ�.
	char szFilePath[] = "D:\\SDC_WindowControlPrj\\MotFile\\MotionDefault.mot";

	//++ ������ Mot������ ��������� ��Ǻ����� ���������� �ϰ����� �����մϴ�.
	if(AxmMotLoadParaAll(szFilePath) != AXT_RT_SUCCESS)
	{
	TRACE(_T("Mot File Not Found."));
	}


	Code = AxmInfoIsMotionModule(&dwStatus);

	if (Code == AXT_RT_SUCCESS)
	{
	if (dwStatus == STATUS_EXIST)
	{
	// �� ������ �о� �´�. �⺻ ����...���� ���� �׸����� �ٽ� ���� �Ѵ�.
	//m_pAxis[0]->bAjinMotorParamLoadAll();
	}
	else
	AfxMessageBox(_T("Ajin Motion Module �� Ȯ�ε��� �ʾҽ��ϴ�."));
	}
	*/
	return TRUE;
}

/*
// ���� ���� ��� AXIS Ŭ�������� ����ϸ� ������ ���� Ŭ�������� �ټ� ������ ����� ��� �������� ����.
BOOL CAxis::bAjinMotorParamSaveAll()
{
	BOOL rslt = TRUE;
	
	DWORD ret(TRUE);
	CString strFileName = SYSTEM_AXIS_MOTION_PATH;

	int len = strFileName.GetLength();
	char* mbszFileName = new char[len + 1];
	//wcstombs(mbszFileName, strFileName.GetBuffer(0), len);
	sprintf(mbszFileName, "%S", strFileName);
	ret = AxmMotSaveParaAll(mbszFileName); // ��� ���� �ʱ�ȭ Data ����
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
	ret = AxmMotLoadParaAll(mbszFileName); // ��� ���� �ʱ�ȭ Data ����
	delete[] mbszFileName;

	if (!ret)
	rslt = TRUE;
	else
	rslt = FALSE;
	
	return rslt;
}
*/

//���� ��/��� ����
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

	//++ ������ �࿡ �����˻��� �����մϴ�.
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
	double ACC_DEC_RATIO(1000.0); //�ڿ��� ��� �Ѵ�. 

	m_Paramanager.Create();
	m_Paramanager.SetPath(DATA_SYSTEM_PATH, _T("Motion.ini"));

	//m_Paramanager.LinkPara(�����̸�, Ű�̸�, ��, �⺻��)
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
		str.Format(_T("Ajin B/D Error�߻�  %s�� Motion Done Error"), GetAxisName());
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

	//������ �ӵ��� �ٸ��� �ϱ� ���� �ӵ��� �߰���.
	if ((1. > dVelRatio) || (20. < dVelRatio))
		dVelRatio = 1.;

	dVel = GetNormVel() * dVelRatio;

	if (dVel == 0) dVel = 10000.0;

	//�ý��� �ӵ� ���� 
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
		str.Format(_T("Ajin B/D Error�߻�  %s�� Error CODE : %d"), GetAxisName(), ret);
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
	AxmStatusReadInMotion((int)m_lAxis, &Status); //Motion ���̸� TRUE / �ƴϸ� FALSE
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
	//RTEX ���� �Լ��̳� ML2�� �Բ� ����ص� �����ϴ�.
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
	// ���� �ӵ� ������ RPM(Revolution Per Minute)���� ���߰� �ʹٸ�.
	// ex>    rpm ���:
	// 4500 rpm ?
	// unit/ pulse = 1 : 1�̸�      pulse/ sec �ʴ� �޽����� �Ǵµ�
	// 4500 rpm�� ���߰� �ʹٸ�     4500 / 60 �� : 75ȸ��/ 1��
	// ���Ͱ� 1ȸ���� �� �޽����� �˾ƾ� �ȴ�. �̰��� Encoder�� Z���� �˻��غ��� �˼��ִ�.
	// 1ȸ��:1800 �޽���� 75 x 1800 = 135000 �޽��� �ʿ��ϰ� �ȴ�.
	// AxmMotSetMoveUnitPerPulse�� Unit = 1, Pulse = 1800 �־� ���۽�Ų��.
	// ���ǻ��� : rpm���� �����ϰ� �ȴٸ� �ӵ��� ���ӵ� �� rpm���� ������ �����Ͽ��� �Ѵ�.

	// ���� ���� �޽� �� �����̴� �Ÿ��� �����Ѵ�.
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
	// ���� ���� Servo-Alarm Reset ��ȣ�� ��� ������ �����Ѵ�.
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
	// ���� ���� �˶� ��ȣ �Է� �� ��� ������ ��� ���� �� ��ȣ �Է� ������ �����Ѵ�.
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
	// ���� ���� Servo-On��ȣ�� ��� ������ �����Ѵ�
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
	// ���� ���� Home ���� Level �� �����Ѵ�.
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

// ���� ���� end limit sensor�� ��� ���� �� ��ȣ�� �Է� ������ �����Ѵ�. 
// end limit sensor ��ȣ �Է� �� �������� �Ǵ� �������� ���� ������ �����ϴ�.
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

// ���� ���� end limit sensor�� ��� ���� �� ��ȣ�� �Է� ������ �����Ѵ�. 
// end limit sensor ��ȣ �Է� �� �������� �Ǵ� �������� ���� ������ �����ϴ�.
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

	//++ ������ ���� �����˻� ����� �����մϴ�.

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

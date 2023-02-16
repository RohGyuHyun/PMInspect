#include "StdAfx.h"

#include "Motion.h"

CMotion::CMotion(void)
{
	for (int i = 0; i < MAX_AXIS_NUM; i++)
		m_pAxis[i] = new CAxis;

	SetAxis();
	LoadAxisConfig();
}


CMotion::~CMotion(void)
{
	for (int i = 0; i<MAX_AXIS_NUM; i++){
		delete m_pAxis[i];
		m_pAxis[i] = NULL;
	}
}

BOOL CMotion::Release()
{
	//MotAmpDisableAll();
	BOOL bRet;

	bRet = AxlClose();

	if (bRet)
		TRACE("Library is Close OK!! .\n");
	else
		TRACE("Failed AXL Library Close!!.");

	return bRet;
}

BOOL CMotion::Initialize()
{
	// ※ [CAUTION] 아래와 다른 Mot파일(모션 설정파일)을 사용할 경우 경로를 변경하십시요.
	char szFilePath[MAX_CHAR];
	sprintf(szFilePath, "%S", SYSTEM_AXIS_MOTION_PATH);


	//++ 지정한 Mot파일의 설정값들로 모션보드의 설정값들을 일괄변경 적용합니다.
	if (AxmMotLoadParaAll(szFilePath) != AXT_RT_SUCCESS) {
		AfxMessageBox(_T("Mot File Not Found."));
	}

	
	for (int axis_no = 0; axis_no < MAX_AXIS_NUM; axis_no++)
	{
		if (!SetAxisConfig(axis_no))
		{
			AfxMessageBox(_T("SetAxisConfig Failed."));
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CMotion::bAjinMotorParamSaveAll()
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

BOOL CMotion::bAjinMotorParamLoadAll()
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

void CMotion::SetAxis()
{
	CString strName[MAX_AXIS_NUM] = { _T("LINEAR_X"), _T("LINEAR_Y"), _T("THETA_T") };

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		m_pAxis[i]->SetAxisNo(i);
		m_pAxis[i]->SetAxisName(strName[i]);

		m_pAxis[i]->SetAxisConfig();
	}
}


BOOL CMotion::AmpEnableAll()
{
	for (int i = 0; i<MAX_AXIS_NUM; i++)
	{
		if (!m_pAxis[i]->AmpEnable())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMotion::AmpDisableAll()
{
	for (int i = 0; i<MAX_AXIS_NUM; i++)
	{
		//m_pAxis[i]->ClearState();
		//m_pAxis[i]->ClearAxis();
		//m_pAxis[i]->ResetAmpFault();
		//m_pAxis[i]->SetThreadStarted(FALSE);
		//m_pAxis[i]->SetHomingErr(FALSE);
		m_pAxis[i]->SetHomeFinished(FALSE);
		m_pAxis[i]->AmpDisable();
	}

	return TRUE;
}


BOOL CMotion::LoadAxisConfig()
{
	int ret;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		ret = m_pAxis[i]->LoadAxisConfig();

		m_pAxis[i]->SetAxisAccelUnit();

		if (!ret)
			return FALSE;
	}

	return TRUE;
}

BOOL CMotion::SaveAxisConfig()
{
	int ret;
	//CMainFrame* pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		ret = m_pAxis[i]->SaveAxisConfig();
		if (!ret)
			return FALSE;
	}

	return TRUE;
}

BOOL CMotion::SetAxisConfig(int axis_no)
{
	switch (axis_no)
	{
	case AXIS_0:
		// for Motor Type X
		m_pAxis[axis_no]->SetAxisUse(USE);
		m_pAxis[axis_no]->ResetAmpFault();
		m_pAxis[axis_no]->AmpEnable();
		m_pAxis[axis_no]->SetMm4Pulse(0.0001);
		m_pAxis[axis_no]->SetPulse4Mm(10000);

		/*
		//m_pAxis[axis_no]->SetElectricGear();
		//m_pAxis[axis_no]->SetMotorType(SERVO_MOTOR);
		//m_pAxis[axis_no]->SetOriginMethod();		//원점 설정
		//m_pAxis[axis_no]->SetAbsMode();
		//m_pAxis[axis_no]->SetAmpfaultlevel(0);
		//m_pAxis[axis_no]->SetAmpFaultResetLevel(0);
		//m_pAxis[axis_no]->SetZPhase(0);

		m_pAxis[axis_no]->SetDirection(4);
		m_pAxis[axis_no]->SetEncoderInput(7);
		
		m_pAxis[axis_no]->SetAccUnit(UNIT_SEC2);
		m_pAxis[axis_no]->SetAmpEnableLevel(HIGH);
		m_pAxis[axis_no]->SetInPosition(HIGH);
		m_pAxis[axis_no]->SetHomeLevel(HIGH);
		m_pAxis[axis_no]->SetPositiveLevel(LOW);
		m_pAxis[axis_no]->SetNegativeLevel(LOW);	
		m_pAxis[axis_no]->SetProfileMode(SYM_TRAPEZOIDE_MODE);		
		*/
		break;
	case AXIS_1:
		// for Motor Type Y
		m_pAxis[axis_no]->SetAxisUse(USE);
		m_pAxis[axis_no]->ResetAmpFault();
		m_pAxis[axis_no]->AmpEnable();
		m_pAxis[axis_no]->SetMm4Pulse(0.001);
		m_pAxis[axis_no]->SetPulse4Mm(1000);

		/*
		//m_pAxis[axis_no]->SetElectricGear();
		//m_pAxis[axis_no]->SetAmpfaultlevel(0);
		//m_pAxis[axis_no]->SetAmpFaultResetLevel(0);
		//m_pAxis[axis_no]->SetZPhase(0);
		//m_pAxis[axis_no]->SetMotorType(SERVO_MOTOR);
		//m_pAxis[axis_no]->SetOriginMethod();		//원점 설정
		//m_pAxis[axis_no]->SetAbsMode();
		m_pAxis[axis_no]->SetProfileMode(SYM_TRAPEZOIDE_MODE);			
		m_pAxis[axis_no]->SetAccUnit(UNIT_SEC2);

		// for Level/Event
		m_pAxis[axis_no]->SetPositiveLevel(LOW);
		m_pAxis[axis_no]->SetNegativeLevel(LOW);
		m_pAxis[axis_no]->SetHomeLevel(HIGH);
		m_pAxis[axis_no]->SetAmpEnableLevel(LOW);		

		// (Direction CW 0/CCW 1)
		m_pAxis[axis_no]->SetDirection(4);		
		m_pAxis[axis_no]->SetEncoderInput(7);		
		//m_pAxis[axis_no]->SetPosition(0.0);
		m_pAxis[axis_no]->SetInPosition(HIGH);
		*/
		break;
	case AXIS_2:
		// for Motor Type T
		m_pAxis[axis_no]->SetAxisUse(USE);
		m_pAxis[axis_no]->ResetAmpFault();
		m_pAxis[axis_no]->AmpEnable();
		m_pAxis[axis_no]->SetMm4Pulse(0.001);
		m_pAxis[axis_no]->SetPulse4Mm(1000);

		//m_pAxis[axis_no]->SetElectricGear();
		//m_pAxis[axis_no]->SetAmpfaultlevel(0);
		//m_pAxis[axis_no]->SetAmpFaultResetLevel(0);
		//m_pAxis[axis_no]->SetZPhase(0);
		//m_pAxis[axis_no]->SetMotorType(SERVO_MOTOR);
		//m_pAxis[axis_no]->SetOriginMethod();		//원점 설정
		//m_pAxis[axis_no]->SetAbsMode();
		// Software Gear
		/*
		m_pAxis[axis_no]->SetProfileMode(SYM_TRAPEZOIDE_MODE);
		
		m_pAxis[axis_no]->SetAccUnit(UNIT_SEC2);

		// for Level/Event
		m_pAxis[axis_no]->SetPositiveLevel(HIGH);
		m_pAxis[axis_no]->SetNegativeLevel(HIGH);
		m_pAxis[axis_no]->SetHomeLevel(HIGH);
		m_pAxis[axis_no]->SetAmpEnableLevel(LOW);

		// (Direction CW 0/CCW 1)
		m_pAxis[axis_no]->SetDirection(4);
		m_pAxis[axis_no]->SetEncoderInput(7);
		//m_pAxis[axis_no]->SetPosition(0.0);
		m_pAxis[axis_no]->SetInPosition(HIGH);
		*/
		break;
	}
	return TRUE;
}

BOOL CMotion::AxisGetPosLimitLevel(int axis_no, long level)
{ 
	return m_pAxis[axis_no]->GetPosLimitLevel(level); 
}

BOOL CMotion::AxisGetPosLimitSensor(int axis_no)
{ 
	return m_pAxis[axis_no]->GetPosLimitSensor(); 
}

BOOL CMotion::AxisGetNegLimitLevel(int axis_no, long level)
{ 
	return m_pAxis[axis_no]->GetNegLimitLevel(level);
}

BOOL CMotion::AxisGetNegLimitSensor(int axis_no)
{
	return m_pAxis[axis_no]->GetNegLimitSensor();
}
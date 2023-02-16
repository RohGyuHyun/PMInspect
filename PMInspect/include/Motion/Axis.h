#pragma once

#include "AXL.h"
#include "AXM.h"			//MOTION

////////////////////////////////////////////////////////////////////////
//MOTION
#define	SERVO_MOTOR		0
#define NOT_USE			0
#define USE				1

#define	MOTINO_STR_PARAFILENAME		 _T("Motion.par");
#define SYSTEM_AXIS_MOTION_PATH		 _T("D:\\DATABASE\\SYSTEM\\Motion.mot")
//MOTION END
////////////////////////////////////////////////////////////////////////

#define UNIVERSAL_INPUT_X00				0	//원점신호
#define UNIVERSAL_INPUT_X01				1	//ENCODER Z상 신호
#define UNIVERSAL_INPUT_X02				2	//범용 입력 신호				
#define UNIVERSAL_INPUT_X03				3	//범용 입력 신호				
#define UNIVERSAL_INPUT_X04				4	//범용 입력 신호
#define UNIVERSAL_INPUT_X05				5	//범용 입력(SMC - 2V04)	//사용안함

#define UNIVERSAL_OUTPUT_Y00			0	//서보 on/off 신호
#define UNIVERSAL_OUTPUT_Y01			1	//알람 제거 신호
#define UNIVERSAL_OUTPUT_Y02			2	//범용 출력 신호		100mA
#define UNIVERSAL_OUTPUT_Y03			3	//범용 출력 신호		100mA
#define UNIVERSAL_OUTPUT_Y04			4	//범용 출력 신호		20mA (릴레이 권장)
#define UNIVERSAL_OUTPUT_Y05			5	//범용 출력(SMC - 2V04)	//사용안함

/*
// 지정 축의 Inpositon 신호 사용 여부 및 신호 입력 레벨을 설정한다
// uLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
DWORD    __stdcall AxmSignalSetInpos(long lAxisNo, DWORD uUse);
// 지정 축의 Inpositon 신호 사용 여부 및 신호 입력 레벨을 반환한다.
DWORD    __stdcall AxmSignalGetInpos(long lAxisNo, DWORD *upUse);
// 지정 축의 Inpositon 신호 입력 상태를 반환한다.
DWORD    __stdcall AxmSignalReadInpos(long lAxisNo, DWORD *upStatus);
*/
class CAxis
{
public:
	CAxis(void);
	~CAxis(void);

protected:
	long m_lAxis;
	int m_iAxisUse;
	int m_iMotorType;
	int m_IOriginMethod;
	int m_iHomingOrder;
	int m_iServoRatioType;
	double m_dMm4Pulse;
	double m_dPulse4Mm;
	double m_dNegSWLimit;
	double m_dPosSWLimit;
	double m_dMinJogVel;
	double m_dMaxJogVel;
	double m_dMinHomeVel;
	double m_dMaxHomeVel;
	double m_dMinJogAccTime;
	double m_dMaxJogAccTime;
	double m_dMinHomeAccTime;
	double m_dMaxHomeAccTime;
	double m_NormVel, m_NormAcc, m_NormDec;
	double m_JogVel, m_JogAcc, m_JogDec;
	double m_dMaxVel, m_dMinVel, m_dMaxAccTime, m_dMinAccTime;
	double m_dOrgOffset, m_dAxisInitPos, m_dAxisSafetyPos;
	double m_HomeVel, m_HomeVel2, m_HomeVel3, m_HomeFineVel, m_HomeAcc, m_HomeDec, m_HomeFineAcc;
	long m_lStopDec;
	double m_dTriggerPeriodPos;

	CString m_strAxisName;

	BOOL m_bOriginReturnFlag;
	BOOL m_bHomingErr;
	BOOL m_bThreadStarted;

	PARAMANAGER m_Paramanager;


public:

	enum OriginStep
	{
		ORG_INIT = 0,
		ORG_POS_SENSOR_SEARCH,
		ORG_POS_SENSOR_OUT,
		ORG_NEG_SENSOR_SEARCH,
		ORG_NEG_SENSOR_OUT,
		ORG_HOME_SENSOR_SEARCH_4_POS,
		ORG_HOME_SENSOR_SEARCH_4_NEG,
		ORG_HOME_SENSOR_OUT,
		ORG_Z_PHASE_SEARCH,
		ORG_ORIGIN_OFFSET_MOVE,
		ORG_ORIGIN_COMPLETE_CHECK
	};

	enum HomeStatus
	{
		HomeStatusReady = 0,
		HomeStatusError,
		HomeStatusProcessing,
		HomeStatusFinish
	};

	enum OrignMethod
	{
		ORG_METHOD_POS = 0,
		ORG_METHOD_NEG,
		ORG_METHOD_POS_HOME,
		ORG_METHOD_NEG_HOME,
		ORG_METHOD_HOME,
		ORG_METHOD_POS_HOME_Z,
		ORG_METHOD_NEG_HOME_Z
	};
	//>> 130220 JSPark
	enum AxisUse
	{
		AXIS_NOT_USE = 0,
		AXIS_USE,
		AXIS_NOT_ORIGIN
	};

	//고정 초기 셋팅 함수
	BOOL SetOriginMethod();
	BOOL StartOrigin();
	DWORD ReadOriginResult();

	BOOL SetPositiveLevel(DWORD dwLv);	// + End Limit
	BOOL SetNegativeLevel(DWORD dwLv);	// - End Limit
	BOOL SetHomeLevel(DWORD dwLv);		// HOME
	BOOL SetAmpEnableLevel(DWORD dwLv);				// SERVO On Level
	BOOL SetAmpfaultlevel(DWORD dwLv);				// Emergency
	BOOL SetAmpFaultResetLevel(DWORD dwLv);			// Alarm
	BOOL SetDirection(DWORD dwMethod);				// Pulse Output
	BOOL SetEncoderInput(DWORD dwLv);				// Enc. Input
	BOOL SetZPhase(DWORD dwLv);						// Z-Phase

													//MOTOR MODE
	BOOL SetPosition(double pos);
	void SetProfileMode(DWORD dwMode = SYM_TRAPEZOIDE_MODE) { AxmMotSetProfileMode(m_lAxis, dwMode); }
	void SetAbsMode() { AxmMotSetAbsRelMode(m_lAxis, POS_ABS_MODE); }		//	절대좌표
	void SetRelMode() { AxmMotSetAbsRelMode(m_lAxis, POS_REL_MODE); }		//	상대좌표
	void SetAccUnit(DWORD dwMode = UNIT_SEC2) { AxmMotSetAccelUnit(m_lAxis, dwMode); }			// Accel Unit
	void SetInPosition(DWORD dwUse) { AxmSignalSetInpos(m_lAxis, dwUse); }					// 사용 시 이동명령이 완료되야 루틴 진행. 
	void ReadInPosition(DWORD *dwState) { AxmSignalReadInpos(m_lAxis, dwState); }


	BOOL AXTLibInit();
	BOOL AXTMotionInit();
	BOOL AXTLibClose();

	//BOOL bAjinMotorParamSaveAll();
	//BOOL bAjinMotorParamLoadAll();

	void SetAxisConfig();
	BOOL LoadAxisConfig(); //130219 JSPark
	BOOL SaveAxisConfig(); //130219 JSPark

	BOOL SetTriggerTimeLevel();
	BOOL SetTriggerBlock(double dStartPos, double dEndPos, double dPeriodPos = 100);
	void SetTriggerReset();

	BOOL VMove(double Velocity, double Acceleration, double Deceleration);		//JOG
	BOOL StartRMove(double dPos);												//상대값
	BOOL RMove(double dPos, BOOL bCheckFlag = TRUE);
	BOOL Move(double dPos, double dVelRatio, BOOL bWaitFlag = TRUE);
	BOOL StartMove(double dPos, double dVelRatio);								//절대값

	BOOL CheckSWLimit(double dPos);

	BOOL SetPositiveLimit(int act);
	BOOL SetNegativeLimit(int act);

	BOOL SetElectricGear();

	void SetAxisAccelUnit();

	BOOL AmpDisable();
	BOOL AmpEnable();
	BOOL SetAmpFault();
	BOOL GetAmpFaultState();
	BOOL GetAmpOnState();
	BOOL ResetAmpFault();
	BOOL IsHomeFinished();
	BOOL isOriginReturn();
	BOOL IsThreadStarted() { return m_bThreadStarted; }

	BOOL EStop();
	BOOL Stop();
	BOOL CheckHomingDone();
	BOOL MotionDone();
	BOOL SetStopRate(int msec);
	BOOL Wait4Done(double dPos = 0.0, BOOL bUpperInterfaceInterlockCheck = false, BOOL bLowerInterfaceInterlockCheck = false); //130315 JSPark


	BOOL GetNegLimitLevel(long &level);
	BOOL GetPosLimitLevel(long &level);
	BOOL GetPosLimitSensor();
	BOOL GetNegLimitSensor();
	BOOL GetHomeSensor();
	void SetOriginReturn(BOOL org);
	void SetHomeFinished(BOOL bFinished) { SetOriginReturn(bFinished); }
	void SetThreadStarted(BOOL bStarted) { m_bThreadStarted = bStarted; }


	//범용 입/출력 제어
	BOOL ReadInput(long lBitNo);
	void WriteOutput(long lBitNo, DWORD dwVal);
	BOOL ReadOutput(long lBitNo);

	void SetAxisName(CString strName) { m_strAxisName = strName; }
	CString GetAxisName() { return m_strAxisName; }
	void SetHomingErr(BOOL bErr) { m_bHomingErr = bErr; }
	BOOL GetHomingErr() { return m_bHomingErr; }
	void SetAxisUse(BOOL bUse) { m_iAxisUse = bUse; }
	int GetAxisUse() { return m_iAxisUse; }
	//	void SetOriginMethod() { m_IOriginMethod = GetParasOriginMethod(); }
	int GetOriginMethod() { return m_IOriginMethod; }
	void SetMm4Pulse(double dMm) { m_dMm4Pulse = dMm; }		// 1Pluse당 이동 거리값 설정	
	double GetMm4Pulse() { return m_dMm4Pulse; }
	void SetPulse4Mm(double dPulse) { m_dPulse4Mm = dPulse; }		// 1mm 이동 시 Plus값 설정
	double GetPulse4Mm() { return m_dPulse4Mm; }
	void SetMotorType(int nType) { m_iMotorType = nType; }
	int GetMotorType() { return m_iMotorType; }
	int GetHomingOrder() { return m_iHomingOrder; }
	void SetAxisNo(long lAxis) { m_lAxis = lAxis; }
	long GetAxisNo() { return m_lAxis; }
	BOOL SetServoLodRatio(int iType);
	BOOL GetServoLodRatio(double &dLoad);
	void SetNormVel(double dVel) { m_NormVel = dVel; }
	double GetNormVel() { return m_NormVel; }
	void SetNormAcc(double dAcc) { m_NormAcc = dAcc; }
	double GetNormAcc() { return m_NormAcc; }
	void SetNormDec(double dDec) { m_NormDec = dDec; }
	double GetNormDec() { return m_NormDec; }
	void SetPosSWLimit(double dLimit) { m_dPosSWLimit = dLimit; }
	double GetPosSWLimit() { return m_dPosSWLimit; }
	void SetNegSWLimit(double dLimit) { m_dNegSWLimit = dLimit; }
	double GetNegSWLimit() { return m_dNegSWLimit; }
	void SetJogVel(double dVel) { m_JogVel = dVel; }
	double GetJogVel() { return m_JogVel; }
	void SetJogAcc(double dAcc) { m_JogAcc = dAcc; }
	double GetJogAcc() { return m_JogAcc; }
	void SetJogDec(double dDec) { m_JogDec = dDec; }
	double GetJogDec() { return m_JogDec; }
	void SetMinJogVel(double dVel) { m_dMinJogVel = dVel; }
	double GetMinJogVel() { return m_dMinJogVel; }
	void SetMaxJogVel(double dVel) { m_dMaxJogVel = dVel; }
	double GetMaxJogVel() { return m_dMaxJogVel; }
	void SetMinHomeVel(double dVel) { m_dMinHomeVel = dVel; }
	double GetMinHomeVel() { return m_dMinHomeVel; }
	void SetMaxHomeVel(double dVel) { m_dMaxHomeVel = dVel; }
	double GetMaxHomeVel() { return m_dMaxHomeVel; }
	void SetMinJogAcc(double dAcc) { m_dMinJogAccTime = dAcc; }
	double GetMinJogAcc() { return m_dMinJogAccTime; }
	void SetMaxJogAcc(double dAcc) { m_dMaxJogAccTime = dAcc; }
	double GetMaxJogAcc() { return m_dMaxJogAccTime; }
	void SetMinHomeAcc(double dAcc) { m_dMinHomeAccTime = dAcc; }
	double GetMinHomeAcc() { return m_dMinHomeAccTime; }
	void SetMaxHomeAcc(double dAcc) { m_dMaxHomeAccTime = dAcc; }
	double GetMaxHomeAcc() { return m_dMaxHomeAccTime; }
	void SetMaxVel(double dVel) { m_dMaxVel = dVel; }
	double GetMaxVel() { return m_dMaxVel; }
	void SetMinVel(double dVel) { m_dMinVel = dVel; }
	double GetMinVel() { return m_dMinVel; }
	void SetMaxAcc(double dAcc) { m_dMaxAccTime = dAcc; }
	double GetMaxAcc() { return m_dMaxAccTime; }
	void SetMinAcc(double dAcc) { m_dMinAccTime = dAcc; }
	double GetMinAcc() { return m_dMinAccTime; }
	void SetOrgOffset(double dVel) { m_dOrgOffset = dVel; }
	double GetOrgOffset() { return m_dOrgOffset; }
	void SetAxisInitPos(double dVal) { m_dAxisInitPos = dVal; }
	double GetAxisInitPos() { return m_dAxisInitPos; }
	void SetAxisSafetyPos(double dVal) { m_dAxisSafetyPos = dVal; }
	double GetAxisSafetyPos() { return m_dAxisSafetyPos; }
	void SetHomeVel(double dVel) { m_HomeVel = dVel; }
	double GetHomeVel() { return m_HomeVel; }
	void SetHomeVel2(double dVel) { m_HomeVel2 = dVel; }
	double GetHomeVel2() { return m_HomeVel2; }
	void SetHomeVel3(double dVel) { m_HomeVel3 = dVel; }
	double GetHomeVel3() { return m_HomeVel3; }

	void SetHomeAcc(double dAcc) { m_HomeAcc = dAcc; }
	double GetHomeAcc() { return m_HomeAcc; }
	void SetHomeDec(double dDec) { m_HomeDec = dDec; }
	double GetHomeDec() { return m_HomeDec; }
	void SetHomeFineVel(double dVel) { m_HomeFineVel = dVel; }
	double GetHomeFineVel() { return m_HomeFineVel; }
	void SetHomeFineAcc(double dAcc) { m_HomeFineAcc = dAcc; }
	double GetHomeFineAcc() { return m_HomeFineAcc; }

	void SetTriggerPeriod(double nPos) { m_dTriggerPeriodPos = nPos; }
	double GetTriggerPeriod() { return m_dTriggerPeriodPos; }


	void GetStopRate(int &msec) { msec = m_lStopDec; }

	BOOL GetPosition(double &rPos, BOOL bEncoder = TRUE); //130228 JSPark
};
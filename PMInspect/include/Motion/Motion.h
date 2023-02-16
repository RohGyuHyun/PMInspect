#pragma once

#include "Axis.h"

#define MAX_AXIS_NUM				3

#define AXIS_0						0
#define AXIS_1						1
#define AXIS_2						2

class CMotion
{
public:
	CMotion(void);
	~CMotion(void);

protected:

public:
	CAxis *m_pAxis[MAX_AXIS_NUM];		// 0: 리니어 x, 1: 리니어 y, 2: THETA t
	
	CAxis* GetAxis(int axis_no) { return m_pAxis[axis_no]; }

	BOOL Initialize();		//라이브러리 초기화
	BOOL Release();			//라이브러리 해제

	void SetAxis();
	BOOL bAjinMotorParamLoadAll();
	BOOL bAjinMotorParamSaveAll();
	BOOL SetOriginMethod();
	BOOL AmpEnableAll();
	BOOL AmpDisableAll();

	BOOL AmpEnable(int axis_no) { return m_pAxis[axis_no]->AmpEnable(); }
	BOOL AmpDisable(int axis_no) { return m_pAxis[axis_no]->AmpDisable(); }

	void WriteOutput(int axis_no, long lBitNo, DWORD dwVal) { m_pAxis[axis_no]->WriteOutput(lBitNo, dwVal); }
	BOOL ReadInput(int axis_no, long lBitNo) { return m_pAxis[axis_no]->ReadInput(lBitNo);  }
	BOOL ReadOutput(int axis_no, long lBitNo) { return m_pAxis[axis_no]->ReadOutput(lBitNo); }
	BOOL AxisGetPosition(int axis_no, double &rPos, BOOL bEncoder = TRUE) { return m_pAxis[axis_no]->GetPosition(rPos, bEncoder); }
	BOOL AxisGetPosLimitLevel(int axis_no, long level); //{ return m_pAxis[axis_no]->GetPosLimitLevel(level); }
	BOOL AxisGetPosLimitSensor(int axis_no); //{ return m_pAxis[axis_no]->GetPosLimitSensor(); }
	BOOL AxisGetNegLimitLevel(int axis_no, long level); //{ return m_pAxis[axis_no]->GetNegLimitLevel(level); }
	BOOL AxisGetNegLimitSensor(int axis_no); //{ return m_pAxis[axis_no]->GetNegLimitSensor(); }
	//BOOL AxisGetHomeSensor(int axis_no) { return m_pMotion->m_pAxis[axis_no]->GetHomeSensor(); }
	BOOL AxisGetAmpFaultState(int axis_no) { return m_pAxis[axis_no]->GetAmpFaultState(); }



	BOOL SetAxisConfig(int axis_no);
	BOOL LoadAxisConfig();
	BOOL SaveAxisConfig();

	BOOL SetOriginMethod(int axis_no) { return m_pAxis[axis_no]->SetOriginMethod(); }
	BOOL StartOrigin(int axis_no) { return m_pAxis[axis_no]->StartOrigin(); }
	DWORD ReadOriginResult(int axis_no) { return m_pAxis[axis_no]->ReadOriginResult(); }
	BOOL SetPosition(int axis_no, double dPos) { return m_pAxis[axis_no]->SetPosition(dPos); }
	void SetOriginReturn(int axis_no, BOOL bState) { m_pAxis[axis_no]->SetOriginReturn(bState); }

};

/******************************************************************************
 *	
 *	(C) Copyright WDI 2010
 *	
 ******************************************************************************
 *
 *	FILE:		atf_ooa.h
 *
 *	PROJECT:	AFT Sensors
 *
 *	SUBPROJECT:	.
 *
 *	Description: OOA control commands in case it is connected to the atf sensor.
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          17/06/2016 Andrew L.        Initial version
 *****************************************************************************/

#pragma once

class CEpp;

class COoa 
{
private:
	CEpp& m_epp;					// epp object used for all communication
	short m_iOoa_objMove;			// motion objective used to execute all the ooa motions
	short m_iCtlDevice;				// device id used to control motion
	float m_fOoa_MaxSpeedMmS,m_fOoa_AccelerationMmSS;	// currently used parameters
	int m_iOoa_positionUs;			// current position
	int m_status;					// most recent status
	int m_iStep7mm;					// step/mm setting
	int m_iUstepping;				// microstepping setting

public:
	COoa(CEpp &epp) : m_epp(epp) {
		clean ();
	}
	~COoa() { clean (); }
	bool init ();
	void clean ();
	bool setAccelerationMmSS(float fAccelerationMmSS);
	bool getAccelerationMmSS(float *fAccelerationMmSS);
	bool setVelocityMmS(float fVelocityMmS);
	bool getVelocityMmS(float *fVelocityMmS);
	bool doHome(bool bWait);
	bool doHomeStateMachineAsync(int *pOoaStatus);
	bool isOK(void);
	bool getStatus(int *pOoaStatus);
	bool moveUm(float fMoveUm, bool bWait);
	bool moveToPositionUm(float fMoveUm, bool bWait);
	bool isMoveDoneAsync(int *pOoaStatus);
	bool getPositionUm(float *pfPositionUm);
	bool getPositionUStep(int *piPositionUStep);
};


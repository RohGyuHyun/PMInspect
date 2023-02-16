/******************************************************************************
 *	
 *	(C) Copyright WDI 2010
 *	
 ******************************************************************************
 *
 *	FILE:		atf_ooa.cpp
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
#include "stdafx.h"
#include "atf_ooa.h"
#include "epp.h"

#define RET_FALSE_ON_ERROR(expression) {if((expression) != ErrOK) return false;}
#define RET_BOOL(expression) {return (expression)==ErrOK ? true : false; }

bool COoa::init ()
{
	clean ();

	int iMv[4];
	iMv[0] = 128;	// identify first found stepper motor objective not associated with Z => OOA
	iMv[1] = 0;		// distance at 0, means stops current motion
	if (m_epp.WriteArray ((void *)&iMv, 'U', 96, DtDwordEnm, 2,0) != ErrOK) 
	{
		// error means no OOA, perhaps there is EOA,  
		iMv[0] = 130;	// identify first EOA element
		iMv[1] = 0;		// distance at 0, means stops current motion
		RET_FALSE_ON_ERROR(m_epp.WriteArray ((void *)&iMv, 'U', 96, DtDwordEnm, 2,0));
		m_iCtlDevice = 2;	// EEye
	}
	else m_iCtlDevice = 0;	// Stepper

	RET_FALSE_ON_ERROR(m_epp.ReadArray  ((void *)&iMv, 'U', 96, DtDwordEnm, 4));

	unsigned int iObjMove			= iMv[0];
	if (iObjMove>=8) return false;

	m_fOoa_MaxSpeedMmS		= 0.001F * iMv[2];	// default speed
	m_fOoa_AccelerationMmSS = 0.001F * iMv[3];	// default acceleration

	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void *)&m_iOoa_positionUs, 'U', 95,  DtDwordEnm, 1));	// read current position
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void *)&m_iStep7mm,		'U', 102, DtWordEnm,  1, iObjMove));	// speed 
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void *)&m_iUstepping,	    'U', 103, DtWordEnm,  1, iObjMove));	// accel

	// is stage direction progressing along X (not Z)
	// iMv[0] = 0;
	// RET_FALSE_ON_ERROR(m_epp.ReadArray ((void *)&iMv,	  'U', 57, DtDwordEnm, 1, iObjMove));	// accel
	// if (!(iMv[0]&0x1000)) return false;		// direction not marked as X stage

	// sanity check
	if (m_fOoa_MaxSpeedMmS<=0 || m_fOoa_MaxSpeedMmS>100) return false;
	if (m_fOoa_AccelerationMmSS<=0 || m_fOoa_AccelerationMmSS>1000) return false;
	if (m_iStep7mm<=0   || m_iStep7mm>500000)	return false;
	if (m_iUstepping<=0 || m_iUstepping>100000)	return false;

	m_iOoa_objMove			= iObjMove;	// all is ok
	return true;
}
void COoa::clean ()
{
	m_iOoa_objMove			= -1;
	m_fOoa_MaxSpeedMmS		=  0;
	m_fOoa_AccelerationMmSS =  0;
	m_iOoa_positionUs		=  0;
	m_status				=  0;
	m_iStep7mm				=  0;
	m_iUstepping			=  0;
	m_iCtlDevice			=  0;
}
bool COoa::setAccelerationMmSS(float fAccelerationMmSS)
{
	if (m_iOoa_objMove<0) return false;
	m_fOoa_AccelerationMmSS = fAccelerationMmSS;
	int _Accel = (int)(m_fOoa_AccelerationMmSS*1000);
	RET_BOOL(m_epp.WriteArray ((void*)&_Accel, 'U', 107, DtDwordEnm, 1, m_iOoa_objMove));	
}
bool COoa::getAccelerationMmSS(float *pfAccelerationMmSS)
{
	if (m_iOoa_objMove<0) return false;
	int _Accel = 0;
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void*)&_Accel, 'U', 107, DtDwordEnm, 1, m_iOoa_objMove));
	m_fOoa_AccelerationMmSS = (float)(_Accel)/1000.0f;
	if(pfAccelerationMmSS) *pfAccelerationMmSS = m_fOoa_AccelerationMmSS;
	return true;
}
bool COoa::setVelocityMmS(float fVelocityMmS)
{
	if (m_iOoa_objMove<0) return false;
	m_fOoa_MaxSpeedMmS = fVelocityMmS;
	int _Speed = (int)(m_fOoa_MaxSpeedMmS*1000);
	RET_BOOL(m_epp.WriteArray ((void*)&_Speed, 'U', 108, DtDwordEnm, 1, m_iOoa_objMove));	
}
bool COoa::getVelocityMmS(float *pfVelocityMmS)
{
	if (m_iOoa_objMove<0) return false;
	int _Speed = 0;
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void*)&_Speed, 'U', 108, DtDwordEnm, 1, m_iOoa_objMove));
	m_fOoa_MaxSpeedMmS = (float)(_Speed)/1000.0f;
	if(pfVelocityMmS) *pfVelocityMmS = m_fOoa_MaxSpeedMmS;
	return true;
}
bool COoa::getStatus(int *pOoaStatus)
{
	if (m_iOoa_objMove<0) return false;

	// atf hw status
	int hwStat = 0, swStat = 0;
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void*)&hwStat, 'U', 45, DtDwordEnm)); 
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void*)&swStat, 'U', 20, DtWordEnm)); 
	
	m_status	= (m_status&OoaStatusLastMoveMask)						|
				  ((HwMotionCWLimit&hwStat)  ? OoaStatusCWLimitON		:
				   (HwMotionCCWLimit&hwStat) ? OoaStatusCCWLimitON		:
				   (MsXYMotion&swStat)	    ? OoaStatusIsMoving			:
				   (m_status&OoaStatusHomed) ? OoaStatusHomed			: // carry over
				   !(HwMotionX&hwStat)	    ? OoaStatusIsInPosition		: 0);

	if (pOoaStatus) *pOoaStatus = m_status;
	return true;
}

bool COoa::doHome(bool bWait)
{
	if (m_iOoa_objMove<0) return false;

	RET_FALSE_ON_ERROR(m_epp.WriteArray ((void *)0, 'U', 98, DtDwordEnm, 0, 0));
	m_status &= ~OoaStatusHomed;
	m_iOoa_positionUs = 0;

	if (bWait) 
	{
		for (int i=0;i<100;i++) 
		{
			int iFlags = 0;
			RET_FALSE_ON_ERROR(m_epp.ReadArray (&iFlags, 't', 21, DtDwordEnm, 1, 0));
			if (iFlags & 0x8) 
			{
				Sleep(100);
				continue;	// homing is still engadged
			}
			m_status |= OoaStatusHomed;
			break;
		}
	}

	return true;
}
bool COoa::doHomeStateMachineAsync(int *pOoaStatus)
{
	if (m_iOoa_objMove<0) return false;

	int iFlags = 0;
	RET_FALSE_ON_ERROR(m_epp.ReadArray (&iFlags, 't', 21, DtDwordEnm, 1, 0));
	
	if (!(iFlags & 0x8)) {
		m_status |= OoaStatusHomed;
	}
	// else homing still take place
	
	return getStatus(pOoaStatus);	
}

bool COoa::isOK()
{
	if (m_iOoa_objMove<0) return false;
	return getStatus(0);	
}

bool COoa::moveUm (float fMoveUm, bool bWait)
{
	if (m_iOoa_objMove<0) return false;

	int iMoveUstep = (int)(fMoveUm * m_iStep7mm * m_iUstepping * 0.001f);

	// start motion
	int iMv[2];
	iMv[0] = m_iOoa_objMove;		// identify first found stepper motor objective
	iMv[1] = iMoveUstep;			// distance for a current motion
	RET_FALSE_ON_ERROR(m_epp.WriteArray ((void *)&iMv, 'U', 96, DtDwordEnm, 2,0));

	m_status &= ~OoaStatusLastMoveMask;
	m_status |=  OoaStatusLastMoveStarted;

	if (bWait) 
	{
		for (int i=0; i<100; i++)
		{
			getStatus(0);
			if (!(m_status&OoaStatusIsMoving)) 
			{
				m_status |= OoaStatusIsInPosition;
				m_iOoa_positionUs += iMoveUstep;
				return true;	// completed
			}
			Sleep(100);
		}
		m_status |= OoaStatusLastMoveTo;
		// timeout
	}

	return true;
}

bool COoa::moveToPositionUm(float fMoveUm, bool bWait)
{
	if (m_iOoa_objMove<0) return false;

	int iMoveUstep = (int)(fMoveUm * m_iStep7mm * m_iUstepping * 0.001f);
	iMoveUstep = iMoveUstep-m_iOoa_positionUs;

	// start motion
	int iMv[2];
	iMv[0] = m_iOoa_objMove;		// identify first found stepper motor objective
	iMv[1] = iMoveUstep;			// distance for a current motion
	RET_FALSE_ON_ERROR(m_epp.WriteArray ((void *)&iMv, 'U', 96, DtDwordEnm, 2,0));

	m_status &= ~OoaStatusLastMoveMask;
	m_status |=  OoaStatusLastMoveStarted;

	if (bWait) 
	{
		for (int i=0; i<100; i++)
		{
			getStatus(0);
			if (!(m_status&OoaStatusIsMoving)) 
			{
				m_status |= OoaStatusIsInPosition;
				m_iOoa_positionUs += iMoveUstep;
				return true;	// completed
			}
			Sleep(100);
		}
		m_status |= OoaStatusLastMoveTo;
		// timeout
	}

	return true;
}

bool COoa::isMoveDoneAsync(int *pOoaStatus)
{
	if (m_iOoa_objMove<0) return false;
	bool bret = getStatus(pOoaStatus);

	if (bret && !(m_status&OoaStatusIsMoving)) {
		m_status |= OoaStatusIsInPosition;
	}

	return bret;
}

bool COoa::getPositionUm(float *pfPositionUm)
{
	bool bret = getPositionUStep(0);
	if (bret) {
		if (pfPositionUm) *pfPositionUm = 1000.0f * m_iOoa_positionUs /(m_iStep7mm * m_iUstepping);
	}
	return bret;
}

bool COoa::getPositionUStep(int *piPositionUStep)
{
	if (m_iOoa_objMove<0) return false;
	RET_FALSE_ON_ERROR(m_epp.ReadArray ((void *)&m_iOoa_positionUs, 'U', 95,  DtDwordEnm, 1, 4*m_iCtlDevice));	// read current position
	if (piPositionUStep) *piPositionUStep = m_iOoa_positionUs;
	return true;
}

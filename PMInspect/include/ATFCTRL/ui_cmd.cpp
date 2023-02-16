/******************************************************************************
 *	
 *	(C) Copyright WDI 2011
 *	
 ******************************************************************************
 *
 *	FILE:		ui_cmd.cpp
 *
 *	PROJECT:	AFT6 Sensor
 *
 *	SUBPROJECT:	.
 *
 *	Description: Low level interface to the sensor
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *                     Andrew L.        Initial version
 *          03/05/2010 Chris O.         Added file headers
 *			01/03/2011 Chris O.         Merged with atf library
 *
 *****************************************************************************/

#include "stdafx.h"
#include <Windows.h>
#include "atf_def.h"
#include "atf_lib.h"
//#include "llc2.h"
#include "ui_cmd.h"
#include "epp.h"


// global variables shared with other files
//
int gs_sensor_type = ATFX;
int gs_tiny_avail = -1;
int gs_ABC_avail = -1;
char logMsg[512] = {'\0'};

// local global variables
//
static int gs_read_delay = 4;

int ReadAcqBlock5x (EnumWindowMode *pmode, u_short* laser)
{
	u_short buf[30];
	int err;

	// cleanup result buffer 
	memset(buf, 0, sizeof(buf));

	if (err = g_epp.ReadArray ((void*)buf, 't', 91, DtWordEnm, 30, 0))
	{
		return err;
	}

	// validate first byte contain what we expect
	if (buf[0]!=ScanlineHeaderId) 
	{
		return -1;
	}

	if(buf[2+FR_AcquisitionId] != 0)
	{
		return -2;
	}

	*pmode = (EnumWindowMode)(buf[2+2*FR_MaxNumEnm+FR_AcqElem_FrameId]&WINSWITCH_WINDOWVALID);
	if (*pmode==5) *pmode = eWM_Center5x;	// this is shifted central mode
	if (*pmode>3)  *pmode = eWM_Auto5x;		// something wrong 

	for (int i=0; i<3; i++) laser[i] = buf[2+2*FR_MaxNumEnm+FR_AcqElem_Laser0Id5x+i];

	return 0;
}


// get sensor position - how close it is to in-focus position
int ReadPosition(MeasureDataType* pdata) 
{ 
	int err = 0;
	int *pTmp = &(pdata->status);

	// obtain status
	err = ReadStatus (pTmp);
	if (err) return err;

	// read positon
	RsData rs;
	err = atf_ReadPositionPacked(&rs);
	if (err) return err;

	if (!rs.miv)
	{	// out of range
		pdata->position = 0;
	}
	else
	if (rs.position&0x0200) 
	{
		pdata->position = 0xfc00 | rs.position;
	}
	else
	{
		pdata->position = 0x03ff & rs.position;
	}

	return rs.invalid_data ? ErrInvalid : ErrOK;
}


// get sensor firmware execution flags
int ReadExecFlag (int *flags)
{
	return g_epp.ReadArray ((void*)flags, 'U', 42, DtDwordEnm, 1, 1);
}


int ReadStatus (int *pStatus) 
{
	return g_epp.ReadArray ((void*)pStatus, 't', 20, DtDwordEnm); 
}

// get scanline
int ReadScanline (EnumWindowMode *eMode,	// from 0 to 3
				  u_short* pframe,			// from 0 to 7
				  u_short* h_scanline, u_short* v_scanline , 
				  u_short* h_len, u_short* v_len, 
				  u_short* width)
{
#define MAX_BUFF_LOCAL 4000

	u_short buf[MAX_BUFF_LOCAL];
	int err = 0;
	int h_len_max = 0;
	int v_len_max = 0;

	h_len_max = *h_len;
    if (h_len_max > MAX_BUFF_LOCAL) 
        h_len_max = MAX_BUFF_LOCAL;

    v_len_max = *v_len;
    if (v_len_max > MAX_BUFF_LOCAL) 
        v_len_max = MAX_BUFF_LOCAL;

	if (err = g_epp.ReadArray ((void*)buf, 't', 91, DtWordEnm, -MAX_BUFF_LOCAL))
	{
		return err;
	}

	// validate first byte contain what we expect
	if (buf[0] != ScanlineHeaderId) 
	{
		return -1;
	}

	if(buf[2 + FR_AcquisitionId] != 0)
	{
		return -2;
	}

	if(buf[1]<6 || buf[1]>8)	// currently 6 is used, but in future might be more
	{
		return -3;
	}

	switch (gs_sensor_type)
	{
	case ATF5x:
		*eMode = (EnumWindowMode)(buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_FrameId] & WINSWITCH_WINDOWVALID);

		*width = buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_PeakArea0Id];
		*(width + 1) = buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_PeakArea1Id];
		*(width + 2) = buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_PeakArea2Id];

		*h_len = buf[2 + FR_MaxNumEnm + FR_HProjId];
		*v_len = buf[2 + FR_MaxNumEnm + FR_VProjId];

		memcpy(h_scanline, &buf[2 + 2 * FR_MaxNumEnm + buf[2 + FR_HProjId]], *h_len * sizeof(u_short));
		memcpy(v_scanline, &buf[2 + 2 * FR_MaxNumEnm + buf[2 + FR_VProjId]], *v_len * sizeof(u_short));

		break;

	default:
		int mode = buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_FrameId];
		int mode1 = mode & 0xf;
		int mode2 = (mode & 0xf0) >> 4;

		switch(mode2) 
		{
		case eWM_Far :
			*eMode = eWM_Far;
			*pframe = 0;
			break;
		case eWM_Near :
			*eMode = eWM_Near;
			*pframe = (mode1>=4 && mode1 <= 11) ? (mode1 - 4) : 0;
			break;
		case eWM_NearSingle:
			*eMode = eWM_NearSingle;
			*pframe = (mode1 >= 6 && mode1 <= 8) ? (mode1 - 6) : 0;
			break;
		case eWM_Sv :
			*eMode = eWM_Sv;
			*pframe = 0;
			break;
		default :
			*eMode = static_cast<EnumWindowMode>(mode2);
			*pframe = mode1;
			break;
		}

		*width = buf[2 + 2 * FR_MaxNumEnm + FR_AcqElem_WidthVPeakId];  // v width

		// if hw cog is provided, then we us this one instead of horizontal line
		if (*h_len = buf[2+FR_MaxNumEnm+FR_HwCogId] && *eMode != eWM_NearSingle)
		{
			if (*h_len>h_len_max) *h_len = h_len_max;
			memcpy(h_scanline, &buf[2+2*FR_MaxNumEnm+ buf[2+FR_HwCogId]], *h_len * sizeof(u_short));
		}
		else 
		{
			*h_len = buf[2+FR_MaxNumEnm+FR_HProjId];
			if (*h_len>h_len_max) *h_len = h_len_max;
			if (*h_len)memcpy(h_scanline, &buf[2+2*FR_MaxNumEnm+ buf[2+FR_HProjId]], *h_len * sizeof(u_short));
		}

		*v_len = buf[2+FR_MaxNumEnm+FR_VProjId];

		if (*v_len>v_len_max) *v_len = v_len_max;
		if (*v_len)memcpy(v_scanline, &buf[2+2*FR_MaxNumEnm+ buf[2+FR_VProjId]], *v_len * sizeof(u_short));

		break;
	}

	return 0;
}

// get currently set objective number
int ReadObjNum (MeasureDataType* pdata)
{
	return atf_ReadObjNum((int *)&pdata->objective);
}

// set objective with its parameters to be the currrent one
int WriteObjNum(MeasureDataType* pdata)
{
	int iTryNum = 0;
	int iRet = 0;

	// try to change objective up to three times in case of failure
	for (iTryNum = 0; iTryNum < 3; iTryNum++)
	{
		iRet = atf_WriteObjNum(pdata->objective);

		if (iRet == ErrOK)
		{
			break;
		}

		Sleep(100);
	}

	return iRet;
}

int WriteSurface(short iSurface)
{
	int iTryNum = 0;
	int iRet = 0;

	// try to change objective up to three times in case of failure
	for (iTryNum = 0; iTryNum < 3; iTryNum++)
	{
		iRet = atf_WriteSurface(iSurface);

		if (iRet == ErrOK)
		{
			break;
		}

		Sleep(100);
	}

	return iRet;
}


// get offset for current objective
int ReadOffset (MeasureDataType* pdata) 
{ 
	int error = 0;

	if (pdata->surface==0) 
	{
		int offset[24];
		error = g_epp.ReadArray ((void*)&offset, 'U', 51, DtDwordEnm, 24, pdata->objective*24);
		for (int i=0; i<24; i++) 
			pdata->offset[i] = (double)offset[i]/1024;
	}
	else if (pdata->surface<5) 
	{
		unsigned short offset[24];
		error = g_epp.ReadArray ((void*)&offset, 't', 191, DtWordEnm, 24, (pdata->objective*4+(pdata->surface-1))*24);
		for (int i=0; i<24; i++) 
			pdata->offset[i] = (double)offset[i]/16;
	}
	return error; 
}
// set offset for current objective
int WriteOffset (MeasureDataType* pdata) 
{ 
	int error = 0;

	if (pdata->surface==0) 
	{
		int offset[24];
		for (int i=0; i<24; i++) 
			offset[i] = (int)(pdata->offset[i]*1024);
		error = g_epp.WriteArray ((void*)&offset, 'U', 51, DtDwordEnm, 24, pdata->objective*24);
	}
	else if (pdata->surface<5) {
		unsigned short offset[24];
		for (int i=0; i<24; i++) 
			offset[i] = (int)(pdata->offset[i]*16);
		error = g_epp.WriteArray ((void*)&offset, 't', 191, DtWordEnm, 24, (pdata->objective*4+(pdata->surface-1))*24);
	}
	return error; 
}

// get in-focus range
int ReadInfoc (MeasureDataType* pdata) 
{ 
//	return g_epp.ReadArray ((void*)&pdata->infocus, 'U', 53, DtWordEnm, 1, pdata->objective);
	return atf_ReadInfocusRange(pdata->objective, (int *)&pdata->infocus);
}

// set in-focus range
int WriteInfoc (MeasureDataType* pdata) 
{ 
//	return g_epp.WriteArray ((void*)&pdata->infocus, 'U', 53, DtWordEnm, 1, pdata->objective); 
	return atf_WriteInfocusRange(pdata->objective, pdata->infocus);
}

// get slope value
int ReadUmout (double* Umout, int obj) 
{ 
	float fUmout = 0.0f;
	int iRet = 0;

	iRet = atf_ReadSlopeUmPerOut (obj, &fUmout);

	if (iRet == ErrOK)
	{
		*Umout = (double) fUmout;
	}

	return iRet; 
}

// set slope value
int WriteUmout (double Umout, int obj) 
{ 
	return atf_WriteSlopeUmPerOut (obj, (float) Umout);
}

// get gain P value
int ReadP (double* P, int obj) 
{ 
	float fP = 0.0f;
	int iRet = 0;

	iRet = atf_ReadP (obj, &fP);

	if (iRet == ErrOK)
	{
		*P = (double) fP;
	}

	return iRet;
}

// set gain P value
int WriteP (double P, int obj) 
{ 
	return atf_WriteP (obj, (float) P);
}

// get acceleration value
int ReadAccel (int* Accel, int obj)
{
	float fAccel = 0.0f;
	int iRet = 0;

	iRet = atf_ReadAccel (obj, &fAccel);
	if (iRet == ErrOK)
	{
		*Accel = (int) (fAccel + 0.5);
	}

	return iRet;
}

// set acceleration value
int WriteAccel (int Accel, int obj)
{
	return atf_WriteAccel (obj, (float) Accel);
}

// get speed value
int ReadSpeed (float* Speed, int obj)
{
	float fSpeed = 0.0f;
	int iRet = 0;

	iRet = atf_ReadSpeed (obj, &fSpeed);
	if (iRet == ErrOK)
	{
		*Speed = fSpeed;
	}

	return iRet;
}

// set speed value
int WriteSpeed (float Speed, int obj)
{
	return atf_WriteSpeed (obj, (float) Speed);
}

// get Z delta value
static enum 
{
	IsU115Unknow = 0,
	IsU115Present = 1,
	IsU115Absent = 2,
} U115_Mode = IsU115Unknow;

int ReadZDelta (int* pDelta, int obj)
{
	short dz = 0;
	int err = 0;
	*pDelta = 0;

	if (U115_Mode == IsU115Unknow) 
	{
		int bt = 0;
		int e1 = g_epp.ReadArray ((void*)&bt, 'U', 115, DtWordEnm, 1, 1);

		if (e1)   U115_Mode = IsU115Absent;
		if (!e1)  U115_Mode = IsU115Present;
	}

	switch (U115_Mode) 
	{
	case IsU115Present :
		err = g_epp.ReadArray ((void*)&dz, 'U', 115, DtWordEnm, 1, obj);
		*pDelta = dz;
		return err;

	case IsU115Absent :
		return  ErrOK;

	case IsU115Unknow:
	default:
		return ErrNoAccess;
	}
}

// set Z delta value
int WriteZDelta (int Delta, int obj)
{
	switch (U115_Mode) 
	{
	case IsU115Present :
		return g_epp.WriteArray ((void*)&Delta, 'U', 115, DtWordEnm, 1, obj);

	case IsU115Absent :
		return  ErrOK;

	case IsU115Unknow:
	default:
		return ErrNoAccess;
	}
}

// set max speed of Z change allowed in Regular AF
int WriteZMaxRate (int iRate, int obj)
{
	int iRet = 0;
	iRet = g_epp.WriteArray ((void*)&iRate, 'U', 111, DtWordEnm, 1, obj);
	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to write max Z rate change. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}
	return iRet;
}
int ReadZMaxRate (int* pRate, int obj)
{
	int iRet = 0;
	*pRate = 0;
	iRet = g_epp.ReadArray ((void*)pRate, 'U', 111, DtWordEnm, 1, obj);
	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to read max Z rate change. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}
	return iRet;
}


int WriteCvdAfParm (int numset, int option)
{
	short Parm[2] = {(short)numset, (short)option };
	return g_epp.WriteArray((void*)Parm, 'U', 59, DtWordEnm, 2, 0); 
}


int ReadCameraGains(unsigned char *pGains)
{
	int iRet = 0;

	iRet = g_epp.ReadArray((void*)pGains, 't', 69, DtByteEnm, 8);
	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to get camera gains. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}

	return iRet;
}


int WriteCameraGains(unsigned char *pGains)
{
	int iRet = 0;

	iRet = g_epp.WriteArray((void*)pGains, 't', 69, DtByteEnm, 8);
	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to set camera gains. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}

	return iRet;
}


int IsAutoOffEnabled(bool *bIsAutoOffEnabled) 
{
	int buf = 0;
	int iRet = 0;

	iRet = g_epp.ReadArray ((void*)&buf, 'U', 56, DtByteEnm, 1, 1);

	if (iRet != ErrOK)
	{
		*bIsAutoOffEnabled = false;
	}
	else
	{
		*bIsAutoOffEnabled = (buf&0x04) ? true : false;
	}

	return iRet;
}


int ReadLedData(u_short* iLedData)
{
	return g_epp.ReadArray((void *) iLedData,  'f', 50, DtWordEnm, 1, 11);
}


int WriteLedData(u_short* iLedData)
{
	return g_epp.WriteArray((void *) iLedData, 'f', 50, DtWordEnm, 1, 11);
}

// CM
int ReadCmGlassThickness (int* i)
{
	return g_epp.ReadArray ((void*)i, 'U', 155, DtDwordEnm);
}


int WriteCmGlassThickness (int i)
{
	return g_epp.WriteArray ((void*)&i, 'U', 155, DtDwordEnm);
}

static int s_err_count = 0;
int ReadCmSvAbsPos (int* pi)
{
	int err;
	*pi = 0;
	if (s_err_count>5) {
		// too many errors, most likely this version of firmware is not supporting this option
		return 0;
	}
	err = g_epp.ReadArray ((void*)pi, 't', 149, DtDwordEnm);
	if (err) s_err_count++;
	return err;
}

int ReadCmRetardation (int* pi, int iSurface)
{
	*pi = 0;
	return g_epp.ReadArray ((void*)pi, 'U', 211, DtWordEnm, 1, iSurface);
}
int WriteCmRetardation (int iRetardation, int iSurface)
{
	return g_epp.WriteArray ((void*)&iRetardation, 'U', 211, DtWordEnm, 1, iSurface);
}
CmRetarderEnum ReadCmRetarderStatus ()
{
	CmRetarderEnum status = CmRetarderStatusUnavailable;
	g_epp.ReadArray ((void*)&status, 't', 211, DtWordEnm);
	return status;
}
// CM AutoDetect
#define RETARDER_OPTION_AUTOCORRECT_AF     0x10   // keep correcting retardation angle during AF
#define RETARDER_OPTION_AUTODISCOVERY_WAIT 0x20   // wait for make0 or AF before doing retardation discovery
#define RETARDER_OPTION_MAKE0DISCOVERY     0x40   // discover retardation value during make0 phase        
int ReadCmAutoDetectConfigOptions (bool &bDoAutoDetectOnMake0, bool &bDoWaitAutoDetectToAf)
{
	int err;
	int option = 0;
	bDoAutoDetectOnMake0 = bDoWaitAutoDetectToAf = false;
	err = g_epp.ReadArray ((void*)&option, 'U', 212, DtWordEnm);
	if (!err) {
		bDoAutoDetectOnMake0  = (option&RETARDER_OPTION_MAKE0DISCOVERY) ? true : false;
		bDoWaitAutoDetectToAf = (option&RETARDER_OPTION_AUTODISCOVERY_WAIT) ? true : false;
	}
	return err;
}
int WriteCmAutoDetectConfigOptions(bool bDoAutoDetectOnMake0,  bool  bDoWaitAutoDetectToAf)
{
	int option = 0;

	int err = g_epp.ReadArray ((void*)&option, 'U', 212, DtWordEnm);
	if (err) return err;

	if (bDoAutoDetectOnMake0)	option |=  RETARDER_OPTION_MAKE0DISCOVERY;
	else						option &= ~RETARDER_OPTION_MAKE0DISCOVERY;
	if (bDoWaitAutoDetectToAf)	option |=  RETARDER_OPTION_AUTODISCOVERY_WAIT;
	else						option &= ~RETARDER_OPTION_AUTODISCOVERY_WAIT;

	return g_epp.WriteArray ((void*)&option, 'U', 212, DtWordEnm);
}
int ScheduleCmAutoDetectForAF()
{
	int code = CmAutoDetectStatusPending;
	return g_epp.WriteArray ((void*)&code, 'U', 213, DtWordEnm);
}
int StartCmAutoDetect() 
{
	int code = CmAutoDetectStatusDone;
	return g_epp.WriteArray ((void*)&code, 'U', 213, DtWordEnm);
}
int StopCmAutoDetect()
{
	int code = CmAutoDetectStop;
	return g_epp.WriteArray ((void*)&code, 'U', 213, DtWordEnm);
}
CmAutoDetectEnum ReadCmAutoDetectStatus() 
{
	CmAutoDetectEnum eadStatus = CmAutoDetectStatusFailed;
	g_epp.ReadArray ((void*)&eadStatus, 'U', 213, DtWordEnm);
	return eadStatus;
}
int GetCurrentMetric()
{
	int met = 0;
	g_epp.ReadArray ((void*)&met, 't', 213, DtWordEnm, 1, 4);
	return met;
}
void EnableMetric()
{
	int met = 2;
	g_epp.WriteArray ((void*)&met, 't', 213, DtWordEnm, 1, 4);
}
void DisableMetric()
{
	int met = 0;
	g_epp.WriteArray ((void*)&met, 't', 213, DtWordEnm, 1, 4);
}

// mfc see FpgaConfigFlagsEnm flags
static enum 
{
	IsU48Unknow = 0,
	IsU48Present = 1,
	IsU48Absent = 2,
} U48_Mode = IsU48Unknow;


int ReadMfcConfig (int* mfc_config)
{
	if (U48_Mode==IsU48Unknow) 
	{
		int err = atf_ReadMfcConfig (mfc_config);
		if (err) 
		{
			*mfc_config = 0;			// pretend nothing happen if call is not supported
			U48_Mode=IsU48Absent;
		}
		else 
		{
			U48_Mode=IsU48Present;
		}
	}
	else if (U48_Mode==IsU48Present) 
	{
		return atf_ReadMfcConfig (mfc_config);
	}
	else *mfc_config = 0;
	return ErrOK;
}


int WriteMfcUstep (u_short  Ustep)
{
	// MFC microstp is stored in 2^-n format
	int i;
	u_short _i;
	unsigned char curr;

	for (i=0; i<5; i++)
	{
		if (Ustep == (1<<i))
		{
			break;
		}
	}

	int err = g_epp.WriteArray ((void*)&i, 't', 171, DtByteEnm, 1, 1);
	if (err!=ErrOK) return err;
	err = g_epp.ReadArray ((void*)&curr, 't', 171, DtByteEnm);
	if (err!=ErrOK) return err;
	_i = 0x08 + (u_short) i + (curr <<4);
	return g_epp.WriteArray ((void*)&_i, 'f', 50, DtWordEnm, 1, 5);
}


int ReadMfcUstep (u_short* Ustep)
{
	// MFC microstp is stored in 2^-n format
	int err = 0;
	unsigned char _Ustep ;

	err = g_epp.ReadArray ((void*)&_Ustep, 't', 171, DtByteEnm, 1, 1);
	*Ustep = (u_short)(1<<_Ustep);
	return err;
}


void GetLLCCharDelay() 
{
	const char *pFunName = "GetLLCCharDelay";

	gs_read_delay = 6;			// 4 will work, but on a safe side
	int mfc_conf = 0;
	int iRet = 0;
	char pToRecv[32] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer

	ENTERFUN(pFunName);

	sprintf(pToSend, ";");
	iRet = atf_sendLlc(pToSend, pToRecv);
/*
	if (iRet != LlcErrOK)
	{
		sprintf(logMsg, "Error: Failed to set FPGA hardware MCM Link flag. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
		goto exitfun;
	}*/

	iRet = ReadMfcConfig (&mfc_conf);
	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to get MFC configuration. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
		goto exitfun;
	}

	if (mfc_conf&FpgaConfig_HW_Link_Mcm) 
	{
		gs_read_delay = 2;	// 1 ms for 19200 back and forth + 1 ms extra
	}
	else
	{
		iRet = g_epp.ReadArray ((void*)&gs_read_delay, 't', 173, DtWordEnm, 1, 3);
		if (iRet == ErrOK) 
		{
			if (gs_read_delay<2)  gs_read_delay = 2;
			if (gs_read_delay>32) gs_read_delay = 31;
			gs_read_delay += 2;
		}
		else
		{
			sprintf(logMsg,"Error: Failed to get LLC Read Delay. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
		}
	}

exitfun:
	sprintf(logMsg,"Info: LLC Read Delay set to %d.\n", gs_read_delay);
	LOGMSG(logMsg, CLogger::LogMid);
	EXITFUN(pFunName);
}

int ReadLLCComCorrOption (int* popt)
{
	*popt = 0;
	return g_epp.ReadArray ((void*)popt, 't', 172, DtByteEnm, 3); 
}

int WriteLLCComCorrOption (int opt)
{
	int err = g_epp.WriteArray ((void*)&opt, 't', 172, DtByteEnm, 3); 
	Sleep (1000);
	return err;
}

int BeginLLCErrorTracking()
{
	int iOption = 0;
	int err = g_epp.ReadArray ((void*)&iOption, 't', 172, DtByteEnm); 
	if (err != ErrOK) goto exitfun;	// not supported ??

	// reset statistics by disabling and enabling or other way around
	int errorEnable;
	if ((iOption & 0x1)==0) 
	{
		// is disabled already
		errorEnable = '1';	// enable
		err = g_epp.WriteArray ((void*)&errorEnable, 't', 172, DtByteEnm);
		if (err != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to reset statistics (enable phase). [ErrorNum=%d]\n", err);
			LOGMSG(logMsg, CLogger::LogMid);
			goto exitfun;
		}
		Sleep (500);		// disable
		err = g_epp.WriteArray ((void*)&iOption, 't', 172, DtByteEnm); 
		if (err != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to reset statistics (disable phase). [ErrorNum=%d]\n", err);
			LOGMSG(logMsg, CLogger::LogMid);
		}
	}
	else 
	{
		// is enabled
		errorEnable = '0';	// disable
		err = g_epp.WriteArray ((void*)&errorEnable, 't', 172, DtByteEnm); 
		if (err != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to reset statistics (disable phase). [ErrorNum=%d]\n", err);
			LOGMSG(logMsg, CLogger::LogMid);
			goto exitfun;
		}
		Sleep (500);		// enable
		err = g_epp.WriteArray ((void*)&iOption, 't', 172, DtByteEnm); 
		if (err != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to reset statistics (enable phase). [ErrorNum=%d]\n", err);
			LOGMSG(logMsg, CLogger::LogMid);
		}
	}

exitfun:
	if (err == ErrOK)
	{
		sprintf(logMsg,"Info: LLC Error Correction Begin {\n");
		LOGMSG(logMsg, CLogger::LogAll);
	}

	return err;
}


int EndLLCErrorTracking()
{
	u_short sErrorSettings[10];
	int err = g_epp.ReadArray ((void*)&sErrorSettings, 't', 173, DtWordEnm, 9); 
	if (err != ErrOK) goto exitfun; // not supported ??

	sprintf(logMsg,
			"Info: "
			"Config = %d; State = %d; "
			"Wr Del = %d; Rd Del = %d; "
			"Char counts: lost(%d), rpt(%d), ok(%d), extra(%d), dropped(%d)\n",
			sErrorSettings[0],sErrorSettings[1],sErrorSettings[2],sErrorSettings[3],sErrorSettings[4],
			sErrorSettings[5],sErrorSettings[6],sErrorSettings[7],sErrorSettings[8]);
	LOGMSG(logMsg, CLogger::LogAll);

	if (atf_getLogFileHandle() != NULL)
	{
		fflush(atf_getLogFileHandle());
	}

exitfun:
	if (err == ErrOK)
	{
		sprintf(logMsg,"Info: LLC Error Correction End {\n");
		LOGMSG(logMsg, CLogger::LogAll);
	}

	return err;
}

// Freeze
int WriteAoiFreezeZGlassThicknessPercent (int freeze) 
{
	return g_epp.WriteArray ((void*)&freeze, 'U', 113, DtWordEnm); 
}


int ReadAoiFreezeZGlassThicknessPercent (int* pfreeze) 
{
    *pfreeze = 0;
	return g_epp.ReadArray ((void*)pfreeze, 'U', 113, DtWordEnm); 
}

// Profilometer
int ReadProfCalibNumMeasure (short* num)
{
	return g_epp.ReadArray ((void*)num, 'U', 140, DtWordEnm, 2, 0);
}


int WriteProfCalibNumMeasure (short* num)
{
	return g_epp.WriteArray ((void*)num, 'U', 140, DtWordEnm, 2, 0);
}


int ReadProfCalibNumPoint (short* num)
{
	return g_epp.ReadArray ((void*)num, 'U', 141, DtWordEnm);
}


int WriteProfCalibNumPoint (short num)
{
	return g_epp.WriteArray ((void*)&num, 'U', 141, DtWordEnm);
}


int ReadProfCalibPulseDist (short* nanometer)
{
	return g_epp.ReadArray ((void*)nanometer, 'U', 142, DtWordEnm);
}


int WriteProfCalibPulseDist (short nanometer)
{
	return g_epp.WriteArray ((void*)&nanometer, 'U', 142, DtWordEnm);
}

int ProfCalibStart (void)
{
	short st=10;
	return g_epp.WriteArray ((void*)&st, 'U', 144, DtWordEnm);
}
int ProfStart (void)
{
	short st=1;
	return g_epp.WriteArray ((void*)&st, 'U', 144, DtWordEnm);
}


int ProfTerminate (void)
{
	short st=16;
	return g_epp.WriteArray ((void*)&st, 'U', 144, DtWordEnm);
}


int GetProfInfo (int *pstatus, int *pavgcnt, int *pcalibcnt)
{
	short tmp[3];
    int err = g_epp.ReadArray ((void*)tmp, 'U', 145, DtWordEnm, 3);
    if (err) {
        *pstatus = PRFERR_ERROR;
        return err;
    }
    *pstatus   = tmp[0];
    *pavgcnt   = tmp[1];
    *pcalibcnt = tmp[2];
    return err;
}


int GetProfStatus ()
{
	short status = PRFERR_ERROR;
    g_epp.ReadArray ((void*)&status, 'U', 145, DtWordEnm);
    return status;
}


int ReadProfData (int* data, int num)
{
	return g_epp.ReadArray ((void*)data, 'U', 146, DtDwordEnm, num);
}


int ReadProfCalibData (short* data, int which)
{
	return g_epp.ReadArray ((void*)data, 'U', 147, DtWordEnm, PROF_MAX_CALIB, which*PROF_MAX_CALIB);
}


// ATF6 specific commands
static enum 
{
	IsU72Unknow = 0,
	IsU72OnlyOne = 1,
	IsU72WindowSupported = 2,
	IsU72WindowNotSupported = 3,
} U72_Mode = IsU72Unknow;

int ReadLineWindowWidth (int obj, short* pwidth)
{
	int error = 0, width = 0;

	if (U72_Mode==IsU72Unknow) 
	{
		int bt = 0;
		int e1 = g_epp.ReadArray ((void*)&bt, 'U', 72, DtWordEnm, 1, 0);
		int e2 = g_epp.ReadArray ((void*)&bt, 'U', 72, DtWordEnm, 1, 1);
		int e3 = g_epp.ReadArray ((void*)&bt, 'U', 72, DtWordEnm, 1, 16);

		if (!e1 &&  e2 &&  e3)  U72_Mode = IsU72OnlyOne;
		if (!e1 && !e2 &&  e3)  U72_Mode = IsU72WindowNotSupported;
		if (!e1 && !e2 && !e3)  U72_Mode = IsU72WindowSupported;
	}

	for (int i=0;i<3;i++) pwidth[i]=0;

	switch (U72_Mode) 
	{
	case IsU72OnlyOne :
		error = g_epp.ReadArray ((void*)&width, 'U', 72, DtWordEnm, 1);
		for (int i=0;i<3;i++) 
		{
			pwidth[i]=(short)(width);
		}
		break;

	case IsU72WindowSupported :
		return g_epp.ReadArray ((void*)pwidth, 'U', 72, DtWordEnm, 3, obj*4);

	case IsU72WindowNotSupported :
		error = g_epp.ReadArray ((void*)&width, 'U', 72, DtWordEnm, 1, obj);
		for (int i=0;i<3;i++) 
		{
			pwidth[i]=(short)(width);
		}
		break;

	case IsU72Unknow:
	default:
		return ErrNoAccess;
	}
	return error;

}


int WriteLineWindowWidth (int obj, int window, short width)
{
	if (U72_Mode==IsU72Unknow) return ErrNoAccess;

	switch (U72_Mode) 
	{
	case IsU72OnlyOne :
		return g_epp.WriteArray ((void*)&width, 'U', 72, DtWordEnm);

	case IsU72WindowSupported :
		return g_epp.WriteArray ((void*)&width, 'U', 72, DtWordEnm, 1, obj*4+window);

	case IsU72WindowNotSupported :
		return g_epp.WriteArray ((void*)&width, 'U', 72, DtWordEnm, 1, obj);

	case IsU72Unknow:
	default:
		return ErrNoAccess;
	}
}


int ReadWindowIntegrationTimes (int obj, short* pit)
{
	return g_epp.ReadArray ((void*)pit, 't', 56, DtWordEnm, 3, obj*4);
}


int WriteWindowIntegrationTimes (int obj, int window, short it)
{
	return g_epp.WriteArray ((void*)&it, 't', 56, DtWordEnm, 1, obj*4+window);
}


int ReadWindowRowDelay (int obj, short* prd )
{ 
	return g_epp.ReadArray ((void*)prd, 't', 72, DtWordEnm, 3, obj*4);
}


int WriteWindowRowDelay (int obj, int window, short rd)
{
	return g_epp.WriteArray ((void*)&rd, 't', 72, DtWordEnm, 1, obj*4+window); 
}


static enum 
{
	Ist58Unknow = 0,
	Ist58WindowSupported = 1,
	Ist58WindowNotSupported = 2,
} t58_Mode = Ist58Unknow;


int ReadWindowBeheadingThresh (int obj, double* pbeheading)
{
	short error;
	short bt[3];

	if (t58_Mode==Ist58Unknow) 
	{

		int e1 = g_epp.ReadArray ((void*)&bt, 't', 58, DtDwordEnm, 1, 0);
		int e2 = g_epp.ReadArray ((void*)&bt, 't', 58, DtWordEnm, 1, 9);
		if (!e1 && e2)  t58_Mode = Ist58WindowNotSupported;
		if (e1 && !e2) t58_Mode = Ist58WindowSupported;
	}

	for (int i=0;i<3;i++) bt[i]=0;

	switch (t58_Mode) 
	{
	case Ist58WindowSupported :
		error = g_epp.ReadArray ((void*)&bt, 't', 58, DtWordEnm, 3, obj*4);
		for (int i=0;i<3;i++) {
			pbeheading[i] = (double)(bt[i])/1024;
		}
		break;
	case Ist58WindowNotSupported :
		error = g_epp.ReadArray ((void*)&bt, 't', 58, DtDwordEnm, 1, obj);
		for (int i=0;i<3;i++) {
			pbeheading[i] = (double)(bt[0])/1024;
		}
		break;
	case Ist58Unknow:
	default:
		return ErrNoAccess;
	}

	return error; 
}


int WriteWindowBeheadingThresh (int obj, int window, double beheading)
{
	short _beheading = (short)(beheading*1024);

	switch (t58_Mode) 
	{
	case Ist58WindowSupported :
		return g_epp.WriteArray ((void*)&_beheading, 't', 58, DtWordEnm, 1, obj*4+window); 
	case Ist58WindowNotSupported :
		return g_epp.WriteArray ((void*)&_beheading, 't', 58, DtDwordEnm, 1, obj); 
	case Ist58Unknow:
	default:
		return ErrNoAccess;
	}
}


int ReadWindowObjMode (int obj, short* mode) 
{
	return g_epp.ReadArray ((void*)mode, 't', 55, DtWordEnm, 1, obj*2+1); 
}


int WriteWindowObjMode (int obj, short mode) 
{
	return g_epp.WriteArray ((void*)&mode, 't', 55, DtWordEnm, 1, obj*2+1); 
}


static enum 
{
	u74_Unknown = 0,
	u74_Supported = 1,
	u74_NotSupported = 2,
} u74_Mode = u74_Unknown;
//EnumWindowMode static gs_mode = eWM_Default;
int ReadWindowCurrMode (EnumWindowMode *pMode) 
{
	int iRet = 0;
	EnumWindowMode mode = eWM_Error;

	if (u74_Mode == u74_Unknown || u74_Mode == u74_Supported)
	{
		iRet = g_epp.ReadArray ((void*)&mode, 'U', 74, DtWordEnm);

		if (iRet == ErrOK)
		{
			if (u74_Mode == u74_Unknown)
			{
				u74_Mode = u74_Supported;
			}
			goto exitfun;
		}
		else if (iRet == ErrOperFailed)
		{
			u74_Mode = u74_NotSupported;
		}
		else
		{
			goto exitfun;
		}
	}

	iRet = g_epp.ReadArray((void*)&mode, 'U', 56, DtByteEnm);
	if (iRet != ErrOK)
	{
		goto exitfun;
	}

	if (mode & 0x20)
	{	// in auto mode
		if (gs_sensor_type == ATF5x)
		{
			mode = eWM_Auto5x;
		}
		else
		{
			mode = eWM_Auto;
		}

		goto exitfun;
	}

	int tmp[2];
	mode = eWM_Error;

	iRet = g_epp.ReadArray((void*)&tmp, 't', 20, DtDwordEnm, 2);

	if (iRet != ErrOK)
	{
		goto exitfun;
	}

	if (tmp[0] & MsNearWindow)
	{	// in near mode
		if (gs_sensor_type == ATF5x)
		{
			tmp[1] &= _Ms7DotValidBits;
			
			if (tmp[1])
			{
				if (tmp[1] == _Ms7Dot3Left)
				{
					mode = eWM_Left5x;
				}
				else
				{
					mode = eWM_Right5x;
				}
			}
			else
			{
				mode = eWM_Center5x;
			}
		}
		else
		{
			mode = eWM_Near;
		}
	}
	else
	{	// in far mode
		if (gs_sensor_type == ATF5x)
		{
			mode = eWM_Far5x;
		}
		else
		{
			mode = eWM_Far;
		}
	}

exitfun:
	*pMode = mode;
	return iRet;
}


int WriteWindowCurrMode (EnumWindowMode mode) 
{
	int iRet = 0;

	if (u74_Mode == u74_Unknown || u74_Mode == u74_Supported)
	{
		iRet = g_epp.WriteArray ((void*)&mode, 'U', 74, DtWordEnm); 

		if (iRet == ErrOK)
		{
			if (u74_Mode == u74_Unknown)
			{
				u74_Mode = u74_Supported;
			}

			return iRet;
		}
		else if (iRet == ErrOperFailed)
		{
			u74_Mode = u74_NotSupported;
		}
		else
		{
			return iRet;
		}
	}

	switch(mode)
	{
	case eWM_Auto5x:
		return atf_EnableAutoWindowTransit();
	case eWM_Far5x:
		return atf_ForceFarMode();
	case eWM_Center5x:
	case eWM_Near:
		return atf_ForceNearMode();
	default:	// should never happen
		return ErrOperFailed;
	}

	return iRet;
}


int ReadProcOptions1 (short* popt1, int obj)
{
	return g_epp.ReadArray ((void*)popt1, 't', 55, DtWordEnm, 1, obj*2);
}


int SetProcOptions1 (int opt1, int opt1_mask, int obj)
{
    short opt1_curr;
    int err = ReadProcOptions1 (&opt1_curr, obj);
    if (err) return err;
    opt1_curr &= ~opt1_mask;
    opt1_curr |= (opt1&opt1_mask);
	int cnt=10;
	do {
		Sleep (50);
		err = g_epp.WriteArray ((void*)&opt1_curr, 't', 55, DtWordEnm, 1, obj*2);
	} while (err && --cnt>0);
	return err;
}


int ReadProcOptions2 (short* popt1, int obj)
{
	return g_epp.ReadArray ((void*)popt1, 't', 55, DtWordEnm, 1, obj*2+1);
}

static enum 
{
	IsU57Unknow = 0,
	IsU57ObjSupported = 1,
	IsU57ObjNotSupported = 2,
} U59_Mode = IsU57Unknow;

int SetProcOptions1Ext (int opt1, int opt1_mask, int obj)
{
    int opt1_curr;
	if (U59_Mode==IsU57ObjNotSupported) return 0; 
    int err = ReadProcOptions1Ext (&opt1_curr, obj);
    if (err) return err;
	if (U59_Mode==IsU57ObjNotSupported) return 0; 
    opt1_curr &= ~opt1_mask;
    opt1_curr |= (opt1&opt1_mask);
	err = g_epp.WriteArray ((void*)&opt1_curr, 'U', 57, DtDwordEnm, 1, obj);
	return err;
}
int ReadProcOptions1Ext (int* popt1, int obj)
{
	*popt1 = 0;
	if (U59_Mode==IsU57ObjNotSupported) return 0; 
	int err = g_epp.ReadArray ((void*)popt1, 'U', 57, DtDwordEnm, 1, obj);
	if (U59_Mode==IsU57Unknow) {
		U59_Mode = err ? IsU57ObjNotSupported : IsU57ObjSupported;
		err = 0;
	}
	return err;
}

int SetProcOptions2 (short opt1, short opt1_mask, int obj)
{
    short opt1_curr;
    int err = ReadProcOptions2 (&opt1_curr, obj);
    if (err) return err;
    opt1_curr &= ~opt1_mask;
    opt1_curr |= opt1;
	return g_epp.WriteArray ((void*)&opt1_curr, 't', 55, DtWordEnm, 1, obj*2+1);
}

int ReadProcOptions (unsigned short* popt, int obj)
{
	return g_epp.ReadArray ((void*)popt, 't', 55, DtWordEnm, 2, obj*2);
}
int WriteProcOptions (unsigned short* popt, int obj)
{
	return g_epp.WriteArray ((void*)popt, 't', 55, DtWordEnm, 2, obj*2);
}

int DisableLineProcessing()
{
    int iTmp = 0;
    int err = g_epp.ReadArray ((void*)&iTmp, 't', 57, DtDwordEnm);
	if (err!=ErrOK) return err;
    iTmp &= ~0x1000;
	err = g_epp.WriteArray ((void*)&iTmp, 't', 57, DtWordEnm);
    return err;
}


int EnableLineProcessing()
{
    int iTmp = 0;
    int err = g_epp.ReadArray ((void*)&iTmp, 't', 57, DtDwordEnm);
	if (err!=ErrOK) return err;
    iTmp |= 0x1000;
	err = g_epp.WriteArray ((void*)&iTmp, 't', 57, DtWordEnm);
    return err;
}


// depreciated, it is always 30
static enum 
{
	IsU52Unknow = 0,
	IsU52Present = 1,
	IsU52Absent = 2,
} U52_Mode = IsU52Unknow;

int ReadLinearRange (int *pLinearRange, int obj) 
{
	if (U52_Mode==IsU52Absent) 
	{
		*pLinearRange = 30;
		return ErrOK;
	}

    int err = atf_ReadLinearRange (obj, pLinearRange);
	if (err && U52_Mode==IsU52Unknow) 
	{
		U52_Mode=IsU52Absent;
		err = ErrOK;
		*pLinearRange = 30;
	}
	return err;
}


static enum 
{
	IsT129Unknown = 0,
	IsT129Present = 1,
	IsT129Missing = 2,
} T129_Mode = IsT129Unknown;

int WriteLinearPrediction (int fltLen, int predLen, int predAhead, int outShift, double dspeedShift) 
{
	int iRet = 0;
	short aoiFlt[8];

	memset(aoiFlt,0,sizeof(aoiFlt));

	switch (T129_Mode)
	{
		case IsT129Unknown:
		{
			iRet = g_epp.ReadArray((void*)aoiFlt, 't', 129, DtWordEnm, 7);
			if (iRet == ErrOK)
			{
				T129_Mode = IsT129Present;
			}
			else
			{
				T129_Mode = IsT129Missing;
				iRet = ErrOK;
				break;
			}
		}

		case IsT129Present:
		{
			aoiFlt[1] = fltLen;
			aoiFlt[5] = predLen;
			aoiFlt[3] = predAhead;
			aoiFlt[4] = outShift;
			aoiFlt[6] = (short)(dspeedShift * 1024);

			iRet = g_epp.WriteArray((void*)(&aoiFlt[1]), 't', 129, DtWordEnm, 6, 1);
			break;
		}

		case IsT129Missing:
		{
			iRet = ErrOK;
			break;
		}
	}

	return iRet;
}


int ReadLinearPrediction (int *pfltLen, int *ppredLen, int *ppredAhead, int *poutShift, double *pdspeedShift) 
{
	int iRet = 0;
	short aoiFlt[8];

	memset(aoiFlt,0,sizeof(aoiFlt));

	switch (T129_Mode)
	{
		case IsT129Unknown:
		{
			iRet = g_epp.ReadArray((void*)aoiFlt, 't', 129, DtWordEnm, 7);
			if (iRet == ErrOK)
			{
				T129_Mode = IsT129Present;
			}
			else
			{
				T129_Mode = IsT129Missing;
				iRet = ErrOK;
			}
			break;
		}

		case IsT129Present:
		{
			iRet = g_epp.ReadArray((void*)aoiFlt, 't', 129, DtWordEnm, 7);
			break;
		}

		case IsT129Missing:
		{
			iRet = ErrOK;
			break;
		}
	}

	*pfltLen	= aoiFlt[1];
	*ppredLen	= aoiFlt[5];
	*ppredAhead	= aoiFlt[3];
	*poutShift  = aoiFlt[4];
	*pdspeedShift = (double)(aoiFlt[6])/1024.0;

	return iRet;
}


int RecoverSensor() { return atf_RecoverSensor(); }


int ReadAoiFltOut (int *piFlt) 
{
    *piFlt = 0; // to 0 higher 16 its
	return g_epp.ReadArray ((void*)piFlt, 't', 129, DtWordEnm, 1, 4);
}


int WriteAoiFltOut (int iFlt)
{
	return g_epp.WriteArray ((void*)&iFlt, 't', 129, DtWordEnm, 1, 4); 
}


int ReadPAssymetry (double* pdAssym, int obj) 
{
    int iAssymetry = 1024;
	int eret = g_epp.ReadArray ((void*)&iAssymetry, 'U', 106, DtDwordEnm, 1, 16+obj);
    if (!eret) 
	{
        *pdAssym = ((double)iAssymetry)/1024; 
    }
    return eret;
}


int WritePAssymetry (double dAssym, int obj)
{
    int iAssymetry = (int)(dAssym*1024);
	return g_epp.WriteArray ((void*)&iAssymetry, 'U', 106, DtDwordEnm, 1, 16+obj); 
}


int ReadItrTime (int *pItrTime, bool bhwCog)
{
	int iAvg = 0, tCnt = 0;
	*pItrTime = 0;

	// do little averaging since call returns length of the last itr. Ignore results of 0
	// since they can be generated if requests come too soon
	for (int i=0; i<2; i++) 
	{
        int tmp = 0;
		int iRet;

		if (bhwCog) 
		{
			iRet = g_epp.ReadArray ((void*)&tmp, 't', 81, DtDwordEnm, 1, 3);
		}
		else 
		{
			iRet = g_epp.ReadArray ((void*)&tmp, 't', 80, DtDwordEnm, 1, 4);
		}
		if (iRet != ErrOK) return iRet;
		
		if (tmp) {	
			iAvg += tmp;
			tCnt++;
		}
		else Sleep(5);
    }

	if (tCnt) *pItrTime = iAvg/tCnt;
	return ErrOK;
}


/////////// Tracking Debuging //////////////////
int ReadZPositionBuffer (short* pBuff, int len)
{
	return g_epp.ReadArray ((void*)pBuff, 't', 112, DtWordEnm, len); 
}


int ReadPredictionBuffer (short* pBuff, int len)
{
	return g_epp.ReadArray ((void*)pBuff, 't', 113, DtWordEnm, len); 
}


int ReadSensorBuffer (short* pBuff, int len)
{
	return g_epp.ReadArray ((void*)pBuff, 't', 114, DtWordEnm, len); 
}


int ReadZStepBuffer (short* pBuff, int len)
{
	return g_epp.ReadArray ((void*)pBuff, 't', 110, DtWordEnm, len); 
}


int ReadEstimateBuffer (short* pBuff, int len)
{
	return g_epp.ReadArray ((void*)pBuff, 't', 110, DtWordEnm, len, 1024); 
}

int LimitAFSampleRun(short len)
{
	return g_epp.WriteArray ((void*)&len, 't', 111, DtWordEnm,1,1); 
}


int ReadLimitAFSampleRun(int *pLimit)
{
	short tmp;
    int err = g_epp.ReadArray ((void*)&tmp, 't', 111, DtWordEnm, 1, 1); 
//    return err ? 0 : tmp;
	if (err != ErrOK)
	{
		*pLimit = 0;
	}
	else
	{
		*pLimit = (int) tmp;
	}

	return err;
}

int ReadAnalogNearMode(bool *bAmode)
{
	int err = 0;
	int tmp = 0;

	err = g_epp.ReadArray ((void*)&tmp, 'U', 56, DtByteEnm, 1, 3);

	*bAmode = (tmp&1) ? true : false;

	return err;
}

int WriteAnalogNearMode (bool banalognearmode)
{
	int tmp = 0;
	int error = 0;

	error = g_epp.ReadArray ((void*)&tmp, 'U', 56, DtByteEnm, 1, 3);

	if(!error)
	{
		if(banalognearmode)
		{
			tmp |= 1;
		}
		else
		{
			tmp &= ~1;
		}

		error = g_epp.WriteArray ((void*)&tmp, 'U', 56, DtByteEnm, 1, 3);
	}

	return error;	
}

int ReadAnalogMove(int *status)
{
	int err = 0;

	err = g_epp.ReadArray ((void*)status, 'U', 56, DtByteEnm, 1, 1);

	*status &= 0x10;

	return err;
}


int WriteAnalogMove(int status)
{
	int buffer_int = 0;
	int error = 0;

	error = g_epp.ReadArray ((void*)&buffer_int, 'U', 56, DtByteEnm, 1, 1);

	if(!error)
	{
		if(status)
		{
			buffer_int |= 0x10;
		}

		else
		{
			buffer_int &= ~0x10;
		}

		error = g_epp.WriteArray ((void*)&buffer_int, 'U', 56, DtByteEnm, 1, 1);
	}

	return error;	
}

int ReadObjConf(int index_obj, int *status)
{
	int offset = 0;
	*status = 0;
	offset = index_obj << 1;
	return g_epp.ReadArray((void *)status, 't', 55, DtWordEnm, 1, offset);
}


int WriteObjConf(int index_obj, int status)
{
	int offset = 0;
	offset = index_obj << 1;
	return g_epp.WriteArray((void *)&status, 't', 55, DtWordEnm, 1, offset);
}


int ChangeObjConf (int index_obj, int flags, int mask)
{
	int buffer_int = 0;	

	g_epp.ReadArray((void *)&buffer_int, 't', 55, DtWordEnm, 1, index_obj<<1);

	buffer_int &= ~mask;
	buffer_int |= (flags&mask);

	return g_epp.WriteArray((void *)&buffer_int, 't', 55, DtWordEnm, 1, index_obj<<1);
}


int ReadHistograms (u_short** l_hist, u_short* lh_len, 
					u_short** r_hist, u_short* rh_len,
					PHistProcExtraDef* ppHistExtra)
{
	static u_short s_buf[1800];
	int err;

	if (err = g_epp.ReadArray ((void*)s_buf, 't', 91, DtWordEnm, -1800))	// read <0 - means read all
	{		
		return err;
	}
	PFpgaScanlineHeader psh = (PFpgaScanlineHeader)s_buf;

	// validate first byte contain what we expect
	if (psh->id != ScanlineHeaderId) return -1;

	// left
	*lh_len = psh->len[FR_LHistProjId];
	*l_hist = &psh->data[psh->beg[FR_LHistProjId]];

	// right
	*rh_len = psh->len[FR_RHistProjId];
	*r_hist = &psh->data[psh->beg[FR_RHistProjId]];

	// extra information
	PHistProcExtraDef pped = (PHistProcExtraDef)&psh->data[psh->beg[FR_ExtraDataId]];

	if (psh->len[FR_ExtraDataId]>=HistProcExtraRecLen/2
		&& pped->RecId==HistProcExtraRecId
		 && pped->RecLen>=HistProcExtraRecLen)
	{
		*ppHistExtra = pped;
	}
	else *ppHistExtra = 0;

	return ErrOK;
}

// ATF6 specific commands
static enum 
{
	Ist159Unknow = 0,
	Ist159OnlyOne = 1,
	Ist159ObjSupported = 2,
	Ist159Obj2Supported = 3,
} t159_Mode = Ist159Unknow;

int ReadCmThresh(int obj, short *pThreshTop, short *pThreshBottom) 
{
	int err;

	if (t159_Mode==Ist159Unknow) 
	{
		int bt = 0;
		int e1 = g_epp.ReadArray ((void*)&bt, 't', 159, DtDwordEnm, 1, 0);
		int e2 = g_epp.ReadArray ((void*)&bt, 't', 159, DtWordEnm,  1, 1);
		int e3 = g_epp.ReadArray ((void*)&bt, 't', 159, DtWordEnm,  1, 9);

		if (!e1 &&  e2 &&  e3)  t159_Mode = Ist159OnlyOne;
		if ( e1 && !e2 &&  e3)  t159_Mode = Ist159ObjSupported;
		if ( e1 && !e2 && !e3)  t159_Mode = Ist159Obj2Supported;
	}
	*pThreshTop = 0;
	*pThreshBottom = 0;

	switch(t159_Mode) {
	case Ist159OnlyOne : 
	{
		int iThreshTop = 0;
		err = g_epp.ReadArray ((void*)&iThreshTop, 't', 159, DtDwordEnm);
		*pThreshTop = iThreshTop;
		return err;
	}

	case Ist159ObjSupported :
		return g_epp.ReadArray ((void*)pThreshTop, 't', 159, DtWordEnm, 1, obj);	

	case Ist159Obj2Supported :
		err = g_epp.ReadArray ((void*)pThreshTop, 't', 159, DtWordEnm, 1, obj);	
		if (err!=ErrOK) return err;
		return g_epp.ReadArray ((void*)pThreshBottom, 't', 159, DtWordEnm, 1, obj+8);

	default:
	case Ist159Unknow :
		return ErrNoAccess;
	}
}

int WriteCmThresh(int obj, short threshTop, short threshBottom) 
{
	int err;

	switch(t159_Mode) {
	case Ist159OnlyOne :
	{
		int iThreshTop = threshTop;
		return g_epp.WriteArray ((void*)&iThreshTop, 't', 159, DtDwordEnm);	
	}

	case Ist159ObjSupported :
		return g_epp.WriteArray ((void*)&threshTop, 't', 159, DtWordEnm, 1, obj);	

	case Ist159Obj2Supported :
		err = g_epp.WriteArray ((void*)&threshTop, 't', 159, DtWordEnm, 1, obj);	
		if (err!=ErrOK) return err;
		return g_epp.WriteArray ((void*)&threshBottom, 't', 159, DtWordEnm, 1, obj+8);	

	default:
	case Ist159Unknow :
		return ErrNoAccess;
	}
}


static enum 
{
	Ist150Unknow = 0,
	Ist150Short = 1,
	Ist150Long = 2,
} t150_Mode = Ist150Unknow;


// the surface and objecive needs to be set
int ReadCmLpRatio(int *pRatio) 
{
	if (t150_Mode == Ist150Unknow) 
	{
		int bt = 0;
		int e1 = g_epp.ReadArray ((void*)pRatio, 't', 150, DtWordEnm, 1, 6);
		int e2 = g_epp.ReadArray ((void*)pRatio, 't', 150, DtWordEnm, 1, 14);

		if (!e1 &&  e2)  t150_Mode = Ist150Short;
		if (!e1 && !e2)  t150_Mode = Ist150Long;
	}

	*pRatio = 0;

	switch (t150_Mode) 
	{
	case Ist150Short :
		*pRatio = 100;
		return ErrOK;

	case Ist150Long :
		return g_epp.ReadArray ((void*)pRatio, 't', 150, DtWordEnm, 1, 7);

	default:
	case Ist150Unknow :
		return ErrNoAccess;
	}
}


int WriteCmLpRatio(int ratio) 
{
	switch (t150_Mode) 
	{
	case Ist150Short :
		return ErrOK;

	case Ist150Long :
		return g_epp.WriteArray ((void*)&ratio, 't', 150, DtWordEnm, 1, 7);	

	default:
	case Ist150Unknow :
		return ErrNoAccess;
	}
}


int ReadHistCalibration (int *piHCal)
{
	return g_epp.ReadArray ((void*)piHCal, 'U', 77, DtDwordEnm); 
}


int WriteHistCalibration (int iHCal)
{
	return g_epp.WriteArray ((void*)&iHCal, 'U', 77, DtDwordEnm); 
}


int WriteFactoryInit ()
{
	int tmp = 0;
	return g_epp.WriteArray((void*)&tmp, 'U', 70, DtDwordEnm);
}

int ReadSeparation (int* pseparation, int *pSeparationOption)
{
	int tmp[2];
	*pseparation = 0;
	*pSeparationOption = 0;
	int err = g_epp.ReadArray((void*)&tmp, 'U', 70, DtDwordEnm, 2);
	if (!err) {
		*pseparation = tmp[0];
		*pSeparationOption = tmp[1];
	}
	return err;
}

int ReadModel(int* model)
{
	return g_epp.ReadArray((void*)model, 'U', 56, DtByteEnm, 1, 2);
}

// returns 5 or 6 depending which version of sensor we are dealing with
// in case of error it will return -1. It will return 0 if version of the 
// firmware is unrecognizable (older then expected)
int ReadSensorType()
{
	int tmp = 0;
	int err = g_epp.ReadArray((void*)&tmp, 't', 21, DtDwordEnm);
	if (err) return -1;
	tmp >>= 20; tmp &= 0xf;
	return tmp==5 ? ATF5 : (tmp==6 ? ATF6 : (tmp==2 ? ATF2 : (tmp==7 ? ATF7 : (tmp==4 ? ATF4 : tmp))));
}


int ReadFpgaStrapCode(u_short* ver)
{
	return g_epp.ReadArray((void*)ver, 'f', 40, DtWordEnm, 1, 15);
}


bool IsSplitOptics()
{
	int speraration_flags = 0;
	int err = g_epp.ReadArray((void*)&speraration_flags, 'U', 70, DtDwordEnm, 1, 1);
	return !err && speraration_flags&512;
}

int ReadExtendedStatus (int &status, int &more_status)
{
	int tmp[2];
	int err = g_epp.ReadArray((void*)&tmp, 't', 20, DtDwordEnm, 2);
	status = tmp[0];
	more_status = tmp[1];
	return err;
}


int ReadCurrentWindow(EnumWindowMode *pWinMode) 
{
	short tmp[2] = {0, 0};
	int err = 0;

	err = g_epp.ReadArray((void*)&tmp, 't', 125, DtWordEnm);
	
	if (err == ErrOK)
	{
		*pWinMode = (EnumWindowMode)(tmp[0] & 0x00FF);
	}
	else
	{
		*pWinMode = eWM_Error;
	}

	return err;
}


int ReadABCStatus(short *pABCstat)
{
	return g_epp.ReadArray ((void*)pABCstat, 't', 200, DtWordEnm, 11, 0);
}


int WriteABCState(short iABCState, int iOffset)
{
	return g_epp.WriteArray ((void*)&iABCState, 't', 200, DtWordEnm, 1, iOffset); 
}


static enum 
{
	IsU201Unknown = 0,
	IsU201Present = 1,
	IsU201Missing = 2,
} U201_Mode = IsU201Unknown;

int ReadABCLaserPower(int iObj, unsigned short *pABClp)
{
	int iRet = 0;

	switch (U201_Mode)
	{
		case IsU201Unknown:
		{
			iRet = g_epp.ReadArray ((void*)pABClp, 'U', 201, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U201_Mode = IsU201Present;
			}
			else
			{
				U201_Mode = IsU201Missing;
				iRet = ErrOK;
			}
			break;
		}

		case IsU201Present:
		{
			iRet = g_epp.ReadArray ((void*)pABClp, 'U', 201, DtWordEnm, 1, iObj);
			break;
		}

		case IsU201Missing:
		{
			iRet = ErrOK;
			break;
		}
	}

	return iRet;
}

int WriteABCLaserPower(int iObj, unsigned short iABClp)
{
	int iRet = 0;
	unsigned short usABClp = 0;

	switch (U201_Mode)
	{
		case IsU201Unknown:
		{
			iRet = g_epp.ReadArray ((void*)&usABClp, 'U', 201, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U201_Mode = IsU201Present;
			}
			else
			{
				U201_Mode = IsU201Missing;
				iRet = ErrOK;
				break;
			}
		}

		case IsU201Present:
		{
			iRet = g_epp.WriteArray ((void*)&iABClp, 'U', 201, DtWordEnm, 1, iObj); 
			break;
		}
			
		case IsU201Missing:
		{
			iRet = ErrOK;
			break;
		}
	}			
			
	return iRet;
}


static enum 
{
	IsU202Unknown = 0,
	IsU202Present = 1,
	IsU202Missing = 2,
} U202_Mode = IsU202Unknown;

int ReadABCZDelta(int iObj, unsigned short *pABCzd)
{
	int iRet = 0;

	switch (U202_Mode)
	{
		case IsU202Unknown:
		{
			iRet = g_epp.ReadArray ((void*)pABCzd, 'U', 202, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U202_Mode = IsU202Present;
				gs_ABC_avail = 1;
			}
			else
			{
				U202_Mode = IsU202Missing;
				gs_ABC_avail = 0;
				iRet = ErrOK;
			}
			break;
		}

		case IsU202Present:
		{
			iRet = g_epp.ReadArray ((void*)pABCzd, 'U', 202, DtWordEnm, 1, iObj);
			break;
		}

		case IsU202Missing:
		{
			iRet = ErrOK;
			break;
		}
	}

	return iRet;
}

int WriteABCZDelta(int iObj, unsigned short iABCzd)
{
	int iRet = 0;
	unsigned short usABCzd = 0;

	switch (U202_Mode)
	{
		case IsU202Unknown:
		{
			iRet = g_epp.ReadArray ((void*)&usABCzd, 'U', 202, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U202_Mode = IsU202Present;
			}
			else
			{
				U202_Mode = IsU202Missing;
				iRet = ErrOK;
				break;
			}
		}

		case IsU202Present:
		{
			iRet = g_epp.WriteArray ((void*)&iABCzd, 'U', 202, DtWordEnm, 1, iObj);
			break;
		}
			
		case IsU202Missing:
		{
			iRet = ErrOK;
			break;
		}
	}			
			
	return iRet;
}


static enum 
{
	IsU205Unknown = 0,
	IsU205Present = 1,
	IsU205Missing = 2,
} U205_Mode = IsU205Unknown;

int ReadTinyZDelta(int iObj, short *pzd)
{
	int iRet = 0;

	switch (U205_Mode)
	{
		case IsU205Unknown:
		{
			iRet = g_epp.ReadArray((void*)pzd, 'U', 205, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U205_Mode = IsU205Present;
				gs_tiny_avail = 1;
			}
			else
			{
				U205_Mode = IsU205Missing;
				gs_tiny_avail = 0;
				iRet = ErrOK;
			}
			break;
		}

		case IsU205Present:
		{
			iRet = g_epp.ReadArray((void*)pzd, 'U', 205, DtWordEnm, 1, iObj);
			break;
		}

		case IsU205Missing:
		{
			iRet = ErrOK;
			break;
		}
	}

	return iRet;
}

int WriteTinyZDelta(int iObj, unsigned short izd)
{
	int iRet = 0;
	short pzd = 0;

	switch (U205_Mode)
	{
		case IsU205Unknown:
		{
			iRet = g_epp.ReadArray((void*)&pzd, 'U', 205, DtWordEnm, 1, iObj);
			if (iRet == ErrOK)
			{
				U205_Mode = IsU205Present;
			}
			else
			{
				U205_Mode = IsU205Missing;
				iRet = ErrOK;
				break;
			}
		}

		case IsU205Present:
		{
			iRet = g_epp.WriteArray ((void*)&izd, 'U', 205, DtWordEnm, 1, iObj);
			break;
		}
			
		case IsU205Missing:
		{
			iRet = ErrOK;
			break;
		}
	}			
			
	return iRet;
}


int ReadTestData(char *pcDataBuf)
{
	return g_epp.ReadArray ((void *)pcDataBuf, 't', 117, DtDwordEnm, 40, 0);
}

int WriteTestData(char *pcDataBuf)
{
	return g_epp.WriteArray ((void *)pcDataBuf, 't', 117, DtDwordEnm, 40, 0);
}

// Homing Support
int ReadHomingParameters (int *pTable8)
{
	return atf_ReadHomingZ_Parameters(pTable8);
}
int WriteHomingParameters (int *pTable8)
{
	return atf_WriteHomingZ_Parameters(pTable8);
}
int RunCurrentHoming (int *pTable8)
{
	return atf_RunHomingZ(pTable8);
}

// AF statistical timer suppport
int ReadAFTimer(float *pAFDurationLast)
{
	int tmp = 0;
	int iRet = g_epp.ReadArray ((void*)&tmp, 't', 25, DtWordEnm, 1, 1);
	if (!iRet) {
		*pAFDurationLast = (float)tmp;
		*pAFDurationLast /= 1000;
	}
	return iRet;
}
int ReadDuvTrackingStatistics(int *pOption, int *pStatus, int *pLength, int *pDisturbance, int *pDuration,
							  int *pInValidCnt, int *pValidCnt)
{
	short tmps[34];
	int iRet = g_epp.ReadArray ((void*)&tmps, 't', 216, DtWordEnm, 34, 0);
	if (iRet) return iRet;

	if (pOption) {
		*pOption = tmps[0];
	}
	if (pStatus) {
		*pStatus = tmps[1];
	}
	if (pInValidCnt) {
		*pInValidCnt = tmps[7];
	}
	if (pValidCnt) {
		*pValidCnt = tmps[8];
	}
	if (pLength) {
		*pLength = tmps[10];
	}
	if (pDisturbance) {
		*pDisturbance = tmps[27];
	}
	if (pDuration) {
		*pDuration = tmps[29];
	}

	return ErrOK;
}

// pre af jump access 
static enum 
{
	IsU119Unknown = 0,
	IsU119Present = 1,
	IsU119Missing = 2,
} U119_Mode = IsU119Unknown;

bool isReadPreAfJumpPresent() { return U119_Mode == IsU119Present; }
int ReadPreAfJump (int obj, short* pUStep)
{
	int iRet;
	*pUStep = 0;
	switch (U119_Mode)
	{
	case IsU119Unknown:
		iRet = g_epp.ReadArray ((void*)pUStep, 'U', 119, DtWordEnm, 1, obj); 
		if (iRet==ErrOK) U119_Mode = IsU119Present;
		else		     U119_Mode = IsU119Missing;
		iRet = ErrOK;
		break;
	case IsU119Present:
		iRet = g_epp.ReadArray ((void*)pUStep, 'U', 119, DtWordEnm, 1, obj); 
		break;
	case IsU119Missing:
		iRet = ErrOK;
		break;
	}
	return iRet;
}
int WritePreAfJump (int obj, short iUStep)
{
	int iRet;
	switch (U119_Mode)
	{
	case IsU119Unknown:
		iRet = g_epp.WriteArray ((void*)&iUStep, 'U', 119, DtWordEnm, 1, obj); 
		if (iRet==ErrOK) U119_Mode = IsU119Present;
		else		     U119_Mode = IsU119Missing;
		iRet = ErrOK;
		break;
	case IsU119Present:
		iRet = g_epp.WriteArray ((void*)&iUStep, 'U', 119, DtWordEnm, 1, obj); 
		break;
	case IsU119Missing:
		iRet = ErrOK;
		break;
	}
	return iRet;
}

// call needed to reinitialized statics if new connection is established
void ReinitAllUiCmdStatics() 
{
	gs_read_delay	= 4;
	gs_tiny_avail	= -1;
	gs_ABC_avail	= -1;
	U115_Mode		= IsU115Unknow;
	U72_Mode		= IsU72Unknow;
	t58_Mode		= Ist58Unknow;
	u74_Mode		= u74_Unknown;
	U52_Mode		= IsU52Unknow;
	t159_Mode		= Ist159Unknow;
	t150_Mode		= Ist150Unknow;
	U48_Mode		= IsU48Unknow;
	U205_Mode		= IsU205Unknown;
	U202_Mode		= IsU202Unknown;
	U201_Mode		= IsU201Unknown;
	T129_Mode		= IsT129Unknown;
	U119_Mode		= IsU119Unknown;

	s_err_count		= 0;
}

int ReadHwCog(bool *phwCog, int obj)
{
	int dw = 0;
	*phwCog = false;
	int err = g_epp.ReadArray ((void *)&dw, 'U', 57, DtDwordEnm, 1, obj);
	if (err==ErrOK && (dw&1)) *phwCog = true;
	return ErrOK;
}

int WriteHwCog(bool hwCog, int obj)
{
	int dw = 0;
	int err = g_epp.ReadArray ((void *)&dw, 'U', 57, DtDwordEnm, 1, obj);
	if (err == ErrOK) 
	{
		if (hwCog) dw |=  1;	// set hw cog
		else       dw &= ~1;	// clear hw cog
		err = g_epp.WriteArray ((void *)&dw, 'U', 57, DtDwordEnm, 1, obj);
	}
	else err = ErrOK;
	return err;
}

// get slope value
int ReadICoeff (float* pfICoeff, int obj) 
{ 
	int dw = 0;
	int err = g_epp.ReadArray ((void *)&dw, 'U', 104, DtDwordEnm, 1, obj);
	*pfICoeff = ((float)dw)/1024;
	return err; 
}

// set slope value
int WriteICoeff (float fICoeff, int obj) 
{
	int dw = fICoeff*1024;
	return g_epp.WriteArray ((void *)&dw, 'U', 104, DtDwordEnm, 1, obj);
}

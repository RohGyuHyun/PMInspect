#include "stdAfx.h"
#include "AtfCtrl.h"

CAtfCtrl::CAtfCtrl()
{
	m_bConnection = InitAFCModoule();
}


CAtfCtrl::~CAtfCtrl(void)
{
	CloseAFCModoule();
}

BOOL CAtfCtrl::SetAfOnOff(BOOL isOn)
{
	BOOL rslt = TRUE;
	if (!isOn)
	{
		if (!AFCAfOff())
		{
			rslt = FALSE;
		}

	}

	if (rslt && SetAFCLaserOnOff(isOn))
	{
		rslt = TRUE;
		Sleep(50);
	}
	else
	{
		rslt = FALSE;
	}

	if (isOn)
	{
		if (rslt && GetLaser())
		{
			if (!AFCAfOn(AF_MODE))
			{
				rslt = FALSE;
			}
			else
			{
				rslt = TRUE;
			}
		}
	}

	return rslt;
}

BOOL CAtfCtrl::AFCAfOff()
{
	BOOL rslt = TRUE;
	int iRet = 0;

	iRet = atf_AfStop();

	if (iRet != ErrOK)
	{
		rslt = FALSE;
		sprintf(logMsg, "Error: Failed to AF Stop [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}

	return rslt;
}

BOOL CAtfCtrl::AFCAfOn(int nAfModeType)
{
	BOOL rslt = TRUE;
	int iRet = 0;
	switch (nAfModeType)
	{
	case AF_MODE:
		iRet = atf_AFTrack();
		break;
	case AOI_MODE:
		iRet = atf_AFAoiTrack();
		break;
	}

	if (iRet != ErrOK)
	{
		rslt = FALSE;
		sprintf(logMsg, "Error: Failed to AF On [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}

	return rslt;
}

BOOL CAtfCtrl::GetInFocus()
{
	if (GetAFCState(AFC_STATE_IN_FOCUS) == 1)
	{
		m_bInFocus = TRUE;
	}
	else
	{
		m_bInFocus = FALSE;
	}

	return m_bInFocus;
}

BOOL CAtfCtrl::GetLaser()
{
	if (GetAFCState(AFC_STATE_ENABLE_LASER) == 1)
	{
		m_bLaser = TRUE;
	}
	else
	{
		m_bLaser = FALSE;
	}

	return m_bLaser;
}

BOOL CAtfCtrl::InitAFCModoule()
{
	return AFCConnect();
}

BOOL CAtfCtrl::CloseAFCModoule()
{
	BOOL rslt = FALSE;

	SetAfOnOff(FALSE);
	atf_CloseConnection();
	m_bConnection = FALSE;

	return rslt;
}

void CAtfCtrl::GetSensorInformation()
{
	int buffer_int = 0;
	int iMfcConfig = 0;
	unsigned int buffer_uint = 0;
	char buffer_string[256];
	char *p_buffer_string = buffer_string;

	int left = 0;
	int width = 0;
	int iRet = 0;
	int iErrCode = 0;

	// clear the flags at the begining
	m_flag_split_cm = m_flag_cm = m_flag_ao = 0;


	//	Firmware version.
	iRet = atf_ReadFirmwareVer(&buffer_int);

	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to get sensor firmware version. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}
	else
	{
		sprintf(m_FirmwareBuffer, "%d.%02x.%02d.%02d", (buffer_int >> 24), ((buffer_int >> 16) & 0xff), ((buffer_int >> 8) & 0xff), (buffer_int & 0xff));
		sprintf(m_FirmwareVer, "%d%02d%02d%02d", (buffer_int >> 24), ((buffer_int >> 16) & 0xff), ((buffer_int >> 8) & 0xff), (buffer_int & 0xff));

		sprintf(logMsg, "Info: Sensor Firmware Version = %s\n", m_FirmwareBuffer);
		LOGMSG(logMsg, CLogger::LogLow);
	}
	// get sensor type (i.e. ATF5,6,7)
	gs_sensor_type = ReadSensorType();	// if not 5 or 6 than determine sensor type from firmware version

	if (gs_sensor_type < 1)
	{
		// older firmware versions
		int ver = buffer_int >> 24;

		switch (ver)
		{
		case 5:
			if (((buffer_int >> 16) & 0xff)>0x0c) gs_sensor_type = ATF5y;	// ATF5 old single window, same as merged
			else								gs_sensor_type = ATF5x;	// ATF5 old version
			break;
		case 6:
			gs_sensor_type = ATF6;	// ATF6 old version
			break;
		default:
			gs_sensor_type = ATFX;	// unknown sensor type
			sprintf(logMsg, "Error: Failed to detect sensor type.\n");
			LOGMSG(logMsg, CLogger::LogLow);
		}
	}

	m_pSensorName = gs_sensor_type == ATF2 ? m_pATF2 :
		(gs_sensor_type == ATF4) ? m_pATF4 :
		(gs_sensor_type >= ATF5x && gs_sensor_type < ATF6) ? m_pATF5 :
		(gs_sensor_type == ATF6) ? m_pATF6 :
		(gs_sensor_type >= ATF7) ? m_pATF7 :
		m_pATFX;
	sprintf(logMsg, "Info: Sensor Type = %s\n", m_pSensorName);

	LOGMSG(logMsg, CLogger::LogLow);

	//	Sensor serial number.
	iRet = atf_ReadSerialNumber(&buffer_uint);

	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to get sensor serial number. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}
	else
	{
		sprintf(p_buffer_string, "00%c %04d",
			(gs_sensor_type == ATF2 ? '2' :
			(gs_sensor_type == ATF4 ? '4' :
				(gs_sensor_type >= ATF5x && gs_sensor_type < ATF6 ? '5' :
				(gs_sensor_type == ATF6 ? '6' :
					(gs_sensor_type >= ATF7 ? '7' : ATFX))))),
			buffer_uint);

		sprintf(logMsg, "Info: Sensor Serial Number = %s\n", p_buffer_string);
		LOGMSG(logMsg, CLogger::LogLow);

		sprintf(p_buffer_string, "%s S/N:", m_pSensorName);
	}

	iRet = ReadModel(&buffer_int);

	if (iRet != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to get sensor model info. [ErrorNum=%d]\n", iRet);
		LOGMSG(logMsg, CLogger::LogLow);
	}

	switch (buffer_int & 0xff)
	{
	case 0:
	case 2:
		iRet = ReadMfcConfig(&iMfcConfig);
		if (iRet != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to get MFC configuration to determine sensor model. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
			sprintf(p_buffer_string, "Unknown");
		}
		else
		{
			if (iMfcConfig & FpgaConfig_Analog)
			{
				sprintf(p_buffer_string, "Analog Output (SA)");
				m_flag_ao = 1;
			}
			else
			{
				sprintf(p_buffer_string, "SYS / AOI");
			}
		}

		m_flag_cm = 0;
		break;

	case 1:
		m_flag_split_cm = IsSplitOptics();
		sprintf(p_buffer_string, "Cell Module%s", m_flag_split_cm ? " - Split" : "");

		m_flag_cm = 1;
		break;
	}

	sprintf(logMsg, "Info: Sensor Model = %s\n", p_buffer_string);
	LOGMSG(logMsg, CLogger::LogLow);

	// read separation, distance between peaks
	//ReadSeparation (&m_separation, &m_separationOption);
}

BOOL CAtfCtrl::AFCConnect()
{
	int iPortConnect;
	int iPortSpeed = 9600;
	char *cPort = { "COM4" };
	int iRet = 0;
	MeasureDataType m_measure;
	int err = 0;
	int buffer_int = 0;
	int iErrCode = 0;
	short buffer_short = 0;
	unsigned short buffer_ushort = 0;
	double buffer_double = 0.0f;
	double temp3 = 0.;
	BOOL bRet = FALSE;

	iRet = atf_isSerialConnection();
	atf_CloseConnection();
	iPortConnect = atf_OpenConnection(cPort, iPortSpeed);
	if (iPortConnect)
	{
		return FALSE;
	}

	iRet = ::ReadPosition(&m_measure);

	iRet = RecoverSensor();
	if (0 == iRet)
	{
		sprintf(logMsg,
			"Error: Failed to connect to the sensor. [ComPortName=%s][BaudRate=%d][ErrorNum=%d]\n",
			cPort, iPortSpeed, iRet);
		LOGMSG(logMsg, CLogger::LogLow);
		atf_CloseConnection();
		return FALSE;
	}
	else
	{
		sprintf(logMsg, "Warning: Sensor connection has been restored.\n");
		LOGMSG(logMsg, CLogger::LogMid);
	}

	GetSensorInformation();

	if ((gs_sensor_type == ATF6 || gs_sensor_type == ATF5))
	{
		m_IsVideoAvailable = true;
	}
	/*err = atf_AfStop();
	if (err != ErrOK)
	{
		sprintf(logMsg, "Error: Failed to turn autofocus off. [ErrorNum=%d]\n", err);
		LOGMSG(logMsg, CLogger::LogLow);
		if (err != ErrOperFailed)
		{
			strcat(logMsg, "There is a problem with the COM port - re-start application.");
		}
		atf_CloseConnection();
		return FALSE;
	}*/

	int objective, nVal;
	float fVal;
	u_short sVal;
	atf_ReadObjNum(&objective);

	atf_ReadAccel(objective, &fVal);
	atf_ReadSpeed(objective, &fVal);
	atf_ReadP(objective, &fVal);
	atf_ReadStepPerMmConversion(&sVal);
	atf_ReadMicrostep(&sVal);
	atf_ReadSlopeUmPerOut(objective, &fVal);
	atf_ReadLinearRange(objective, &nVal);
	atf_ReadInfocusRange(objective, &nVal);
	atf_WriteInfocusRange(objective, 10);
	atf_WriteAccel(objective, 120);
	atf_WriteSpeed(objective, 8);

	nVal = 0;
	if (ATFHome() != ErrOK)
	{
		//return FALSE;
	}

	if (AFCLaserMode(TRUE) != ErrOK)
	{
		atf_CloseConnection();
		return FALSE;
	}

	if (GetLaser())
	{
		if (!SetAFCLaserOnOff(FALSE))
		{
			atf_CloseConnection();
			return FALSE;
		}
	}

	m_bConnection = TRUE;

	return TRUE;
}

BOOL CAtfCtrl::GetAFCState(int nStatusType)
{
	int iRet = 0;
	short status[1] = { 0 };
	BOOL rslt[16] = { FALSE, };
	Sleep(50);
	iRet = ::atf_ReadStatus(status);
	WORD wRslt;
	wRslt = status[0];
	WordToBit(wRslt, rslt);

	/*for (int i = 0; i < 16; i++)
	{
		rslt[i] = status[0] % 2;
		status[0] = status[0] / 2;
	}*/

	return rslt[nStatusType];
}

BOOL CAtfCtrl::GetAFCHWState(int nStatusType)
{
	int iRet = 0;
	int status[1] = { 0 };
	BOOL rslt[32] = { FALSE, };
	Sleep(50);
	iRet = ::atf_ReadHwStat(status);
	DWORD wRslt;
	wRslt = status[0];
	DWordToBit(wRslt, rslt);

	/*for (int i = 0; i < 16; i++)
	{
	rslt[i] = status[0] % 2;
	status[0] = status[0] / 2;
	}*/

	return rslt[nStatusType];
}

BOOL CAtfCtrl::AFCLaserOnOff(BOOL isOn)
{
	int iRet;
	BOOL rslt = TRUE;;
	if (isOn)
	{	//	Changed to on.
		iRet = atf_EnableLaser();
		if (iRet != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to turn laser on. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
			rslt = FALSE;
		}
	}
	else
	{	//	Changed to off.
		iRet = atf_DisableLaser();
		if (iRet != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to turn laser off. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
			rslt = FALSE;
		}
	}

	return rslt;
}

BOOL CAtfCtrl::AFCLaserMode(BOOL isAuto)
{
	int button = isAuto;
	u_short laser[15];
	int iRet = 0;

	if (button)
	{	// auto laser control
		iRet = atf_LaserTrackOn();
		if (iRet != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to turn the laser track on. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
		}
	}
	else
	{	// manual laser control
		iRet = atf_LaserTrackOff();
		if (iRet != ErrOK)
		{
			sprintf(logMsg, "Error: Failed to turn the laser track off. [ErrorNum=%d]\n", iRet);
			LOGMSG(logMsg, CLogger::LogLow);
		}
		else
		{
			if (laser[0] == 0)
			{
				// don't write 0, this disables laser. Often laser is so low that system is unable to report current value
				laser[0] = 1;
			}

			iRet = atf_WriteLaserPower(laser[0], 3);
			if (iRet != ErrOK)
			{
				sprintf(logMsg, "Error: Failed to set laser power. [ErrorNum=%d]\n", iRet);
				LOGMSG(logMsg, CLogger::LogLow);
			}
		}
	}

	return iRet;
}

void CAtfCtrl::WordToBit(WORD word, BOOL *bit)
{
	for (int i = 15; i > -1; i--)
	{
		bit[i] = (word >> i) & 0x00000001;
	}
}

void CAtfCtrl::DWordToBit(DWORD word, BOOL *bit)
{
	for (int i = 31; i > -1; i--)
	{
		bit[i] = (word >> i) & 0x00000001;
	}
}


BOOL CAtfCtrl::ATFHome()
{
	int nVal[8], nRet;
	BOOL bState[2];

	bState[0] = GetAFCHWState(16);
	bState[1] = GetAFCHWState(18);

	if (bState[0] || bState[1])
	{
		nRet = atf_ReadHomingZ_Parameters(nVal);

		nRet = atf_RunHomingZ(nVal);

		while (TRUE)
		{
			bState[0] = GetAFCHWState(11);

			if (!bState[0])
			{
				break;
			}
			Sleep(10);
		}

	}
	return nRet;
}
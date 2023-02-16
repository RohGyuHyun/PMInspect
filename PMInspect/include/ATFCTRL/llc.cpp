/******************************************************************************
 *	
 *	(C) Copyright WDI 2011
 *	
 ******************************************************************************
 *
 *	FILE:		llc.cpp
 *
 *	PROJECT:	AFT Sensor
 *
 *	SUBPROJECT:	.
 *
 *	Description: LLC interface
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          28/01/2011 Chris O.         Initial version
 *
 ******************************************************************************/

#include "stdafx.h"
#include "llc.h"

#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <math.h>


// error messages
const char *pLlcErrOK =							"LLC is operational [OK].";
const char *pLlcErrStatus =						"Failed to get LLC status. Check power supply connector.";
const char *pLlcErrSensorCom =					"There is no communication with sensor.\n"
												"Call recoverSensorConnection or recoverLlc function";
const char *pLlcErrMotorDisabled =				"Motor was not started yet or\n"
												"has been disabled by the command or\n"
												"by motor failure.";
const char *pLlcErrLimitSwitch =				"The limit switch has been activated.";
const char *pLlcErrPeakCurrExceeded =			"The peak current has been exceeded.\n"
												"Possible reasons are:\n" 
												"- drive malfunction\n"
												"- bad tuning of the current controller.";
const char *pLlcErrInhibit =					"Inhibit. The motor movement is temporarily inhibited.";
const char *pLlcErrSpeedTracking =				"Speed tracking error exceeded speed error limit.\n"
												"This may occur due to:\n"
												"- Bad tuning of the speed controller\n"
												"- Too tight a speed error tolerance\n"
												"- Inability of motor to accelerate to the required speed due to\n" 
												"  too low a line voltage or not a powerful enough motor\n";
const char *pLlcErrPositionTracking =			"Position tracking error exceeded position error limit.\n"
												"This may occur due to:\n"
												"- Bad tuning of the position or speed controller\n"
												"- Too tight a position error tolerance\n"
												"- Abnormal motor load, or reaching a mechanical limit\n";
const char *pLlcErrUnderVoltage =				"Under voltage. The power supply is shut down or\n"
												"it has too high an output impedance.";
const char *pLlcErrOverVoltage =				"Over voltage. The voltage of the power supply is too high, or\n"
												"the servo drive did not succeed in absorbing the kinetic energy while braking a load.\n"
												"A shunt resistor may be required.";
const char *pLlcErrShortCircuit =				"Short circuit. The motor or its wiring may be defective, or\n"
												"the drive is faulty.";
const char *pLlcErrOverheating =				"Temperature. Drive overheating.\n"
												"The environment is too hot, or lacks heat removal.\n"
												"There may be a large thermal resistance between\n"
												"the drive and its mounting.";
const char *pLlcErrElectricalZero =				"Failed to find the electrical zero of the motor in an attempt to\n"
												"start it with an incremental encoder.\n"
												"The reason may be that the applied motor current did not\n"
												"suffice for moving the motor from its position or\n"
												"there is wiring problem with UVW or encoder connectors.";
const char *pLlcErrSpeedLimit =					"Speed limit exceeded.\n"
												"This may occur due to\n"
												"bad tuning of the speed controller.";
const char *pLlcErrMotorStuck =					"Motor stuck - the motor is powered but is not moving.\n"
												"The reason may be that there is wiring problem with\n"
												"power supply, UVW or encoder connectors.";
const char *pLlcErrPositionlimit =				"Position limit exceeded.";
const char *pLlcErrStartMotor =					"Cannot start motor.";
const char *pLlcErrStackOverflow =				"Stack overflow - fatal exception. This may occur if the CPU was\n"
												"subject to a load that it could not handle. Such a situation can\n"
												"arise only due to a software bug in the drive.";
const char *pLlcErrCpuException =				"CPU exception - fatal exception. Something such as an attempt\n"
												"to divide in zero or another fatal firmware error has occurred.";

// LLC objective position offset file header
const char *pFileHeader = "LLC Objective Position Offsets";
// maximum acceptable error when moving to requested objective position
const int MaxInPositionRange = 50;
// default COM baudrate
const int DefaultBaudrate = 9600;
// maximum objective position offset in um = +/- 1 mm
const int MaxPositionOffset = 1000;

// LLC models
typedef struct
{
	char cLlcModel;						// available models
	float fObjPitch[MAX_LLC_OBJ_NUM];	// available pitches
} llcModel, *pLlcModel;
const llcModel llcModels[] = {{'C', {0.0f, 38.0f, 76.0f, 114.0f, 152.0f, 190.0f, 228.0f, 266.0f}},			// for objective number: 3, 4, 6, 8
							  {'D', {0.0f, 38.0f, 76.0f, 114.0f, 152.0f, 190.0f, 228.0f, 266.0f}},			// for objective number: 6 
							  {'E', {0.0f, 54.75f, 109.5f, 164.25f, 219.0f, 273.75f, 328.5f, 383.25f}},		// for objective number: 3 
							  {'F', {0.0f, 36.5f, 73.0f, 109.5f, 146.0f, 182.5f, 219.0f, 255.5f}},			// for objective number: 3
							  {'G', {0.0f, 36.5f, 73.0f, 109.5f, 146.0f, 182.5f, 219.0f, 255.5f}},			// for objective number: 4
							  {'H', {0.0f, 27.38f, 54.76f, 82.14f, 109.52f, 136.9f, 164.28f, 191.66f}},		// for objective number: 5
							  {'J', {0.0f, 41.0f, 82.0f, 123.0f, 164.0f, 205.0f, 246.0f, 287.0f}},			// for objective number: 4, 6, 8
							  {'K', {0.0f, 41.0f, 82.0f, 123.0f, 164.0f, 205.0f, 246.0f, 287.0f}},			// for objective number: 6
							  {'L', {0.0f, 38.0f, 71.5f, 109.5f, 147.5f, 185.5f, 223.5f, 261.5f}},			// for objective number: 4
							  {'M', {0.0f, 53.0f, 106.0f, 159.0f, 212.0f, 265.0f, 318.0f, 371.0f}},			// for objective number: 3
							  {'N', {0.0f, 53.0f, 106.0f, 164.0f, 222.0f, 280.0f, 338.0f, 396.0f}}			// for objective number: 4
};


/**************************************************************************************************
*
* Function	 :	CLlc::initializeLlc
*
* Description:	Initialize LLC class.
*				Call this function to initialize LLC before any other LLC function - this function 
*				MUST be called first.
*
* Input		 :	pOffsetFileDir - directory for objective position offsets file (if specified MUST
*								 already exist) or can be NULL
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK			 = LLC initialization successful
*				- LlcErrSensorCom    = serial communication failure
*				- LlcErrObjectiveNum = failed to get available number of LLC objectives 
*									   (initialization is still successfull with this error)
*				- LlcErrOffsetFile	 = failed to create objective position offset file  - make your
*									   application current directory writable otherwise you will
*									   not be able to specify objective position offsets 
*									   (initialization is still successfull with this error) 
*
* Return	 :	true = success; false = failure
*
* Notice	 :	After runnig this function the piErrCode has to be examinned. In case of 
*				"LlcErrObjectiveNum" error call "setObjectiveNum" function and set the objective
*				number manually depends on which LLC model you have. Set: 2 for LLC2, 4 for LLC4, 6
*				for LLC6 or 8 for LLC8. The serial communication has to be opened first before
*				calling any LLC functions.
*
**************************************************************************************************/
bool CLlc::initializeLlc(char *pOffsetFileDir, int *piErrCode)
{
	const char *pFunName = "CLlc::initializeLlc";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	int iErrCode = LlcErrOK;	// LLC error code
	int iTmp = 0;				// temp
	int iRetLoc = 0;			// return code for locally called funcitons
	int i = 0;					// index
	int j = 0;					// index
	int k = 0;					// index
	bool bRetLoc = false;		// return code for locally called functions
	bool bRet = false;			// function return value

	LLCENTERFUN(pFunName);

	// check sensor/LLC connectivity
	bRetLoc = recoverLlcSensorConnection(&iErrCode);
	if (!bRetLoc || iErrCode != LlcErrOK)
	{
		LLCLOGMSG("Error: Failed to establish LLC connection.\n", CLogger::LogLow);
		goto exitfun;
	}

	// get forcer serial number - ET[4]
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ET[4];");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: ET[4]. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	strcpy(m_cForcerSerNum, pToRecv);
	LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Forcer serial number = ", m_cForcerSerNum, ".\n");

	// get param indicating home detection implementation - ET[6]
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ET[6];");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: ET[6]. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	// evaluate LLC response
	if (atoi(pToRecv) != 0)
	{
		LLCLOGMSG("Info: ET[6] status - move to home position - implemented.\n", CLogger::LogLow);
		m_bIsHomeDetImpl = true;
	}
	else
	{
		LLCLOGMSG("Info: ET[6] status - move to home position - not implemented.\n", CLogger::LogLow);
		m_bIsHomeDetImpl = false;
	}

	// get LLC objective number
	for (i = 0; i < 4; i++)
	{
		ZeroMemory(pToRecv, sizeof(pToRecv));
		sprintf(pToSend, "ET[%d];", i + 8);
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s%d%s", "Error: Failed to get LLC parameter: ET[", i + 8, "]. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}

		iTmp = atoi(pToRecv);

		switch (i)
		{
		case 0:
		case 1:		// first two characters are: LL
			if (iTmp != 'L') 
			{
				i = 4;
				iErrCode = LlcErrObjectiveNum;
			}
			break;

		case 2:		// third character is: C, D, E, F, G, H, J or K - see llcModels definitions
			for (k = 0; k < sizeof(llcModels) / sizeof(llcModel); k++)
			{
				if (iTmp == (int)(llcModels[k].cLlcModel))
				{
					// get objective pitch based on model character
					for (j = 0; j < MAX_LLC_OBJ_NUM; j++)
					{
						m_fObjPitch[j] = llcModels[k].fObjPitch[j];
					}
					break;
				}
			}

			if (m_fObjPitch[1] == 0.0f)
			{
				i = 4;
				iErrCode = LlcErrObjectiveNum;
			}

			break;

		case 3:		// fourth character is: objective number
			// set number of objectives
			m_iObjNum = atoi(pToRecv) - '0';
			if (m_iObjNum < 3 || m_iObjNum > 8)
			{
				iErrCode = LlcErrObjectiveNum;
				m_iObjNum = 0;
			}
			else
			{
				LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: Number of available objectives = ", m_iObjNum, ".\n");
			}
/*
			if (m_iObjNum < 0) m_iObjNum = 0;
			if (m_iObjNum > 8) m_iObjNum = 8;
			
			// set objective separation value
			switch (m_iObjNum)
			{
			case 0:
				iErrCode = LlcErrObjectiveNum;
				break;
			case 2:
				// LLC with two objectives has 35 mm separation
				m_iObjPitch = 35;
				m_fObjPitch = 35.0f;
				break;
			default:
				// LLC with more than two objectives (4, 6 or 8) has 38 mm separation
				m_iObjPitch = 38;
				m_fObjPitch = 38.0f;
			}

			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: Objective separation pitch size (i) = ", m_iObjPitch, ".\n");
*/

			break;
		}
	}

	if (iErrCode == LlcErrObjectiveNum)
	{
		LLCLOGMSG("Error: Failed to get device model / objective pitch distance / number of available objectives.\n", CLogger::LogLow);
	}

	// create objective position offset file - if already exist it is treated as success
	if (createLlcOffsetFile(pOffsetFileDir) == false)
	{
		LLCLOGMSG("Error: Failed to create LLC offset file.\n", CLogger::LogLow);
		iErrCode = LlcErrOffsetFile;
	}

	if (iErrCode == LlcErrOK)
	{
		m_bIsLlcInit = true;
		bRet = true;
	}

exitfun:
	*piErrCode = iErrCode;

	if (bRet)
	{
		LLCLOGMSG("Info: LLC initialization successfull.\n", CLogger::LogLow);
	}
	else
	{
		LLCLOGMSG("Error: Failed to initialize LLC.\n", CLogger::LogLow);
	}

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setObjectiveNum
*
* Description:	Set a number of available objectives for the LLC.
*				Call this function to finish LLC initialization if automatic detection of available
*				objective number failed.
*
* Input		 :	iObjNum - number of objectives available in user LLC model. Set: 2 for LLC2, 4 for 
*						  LLC4, 6 for LLC6 or 8 for LLC8.
*
* Output	 :	void
*
* Return	 :	void
*
* Notice	 :	To ensure if there is any need to set objective number parameter, call
*				"getObjectiveNum" function after call to "initializeLlc". If "getObjectiveNum" 
*				returns zero than use this function as shown in Input section above.
*
**************************************************************************************************/
void CLlc::setObjectiveNum(int iObjNum) 
{
	// set number of objectives
	m_iObjNum = iObjNum;
	if (m_iObjNum < 3) m_iObjNum = 3;
	if (m_iObjNum > 8) m_iObjNum = 8;
/*
	// set objective separation value
	if (m_iObjNum == 2)
	{
		// LLC with two objectives has 35 mm separation
		m_iObjPitch = 35;
		m_fObjPitch = 35.0f;
	}
	else
	{
		// LLC with more than two objectives (4, 6 or 8) has 38 mm separation
		m_iObjPitch = 38;
		m_fObjPitch = 38.0f;
	}
	
	LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: Objective separation pitch size (i) = ", m_iObjPitch, ".\n");
*/
	LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: Number of available objectives = ", m_iObjNum, ".\n");
}


/**************************************************************************************************
*
* Function	 :	CLlc::getObjectiveNum
*
* Description:	Get a number of available objectives for the LLC.
*				Call this function to get a number of available objectives for the LLC.
*
* Input		 :	void
*
* Output	 :	void
*
* Return	 :	int - a number of available objectives for the LLC
*
* Notice	 :	Use this function after call to "initializeLlc" function. If it returns zero than
*				call "setObjectiveNum" function (as described in the function header).
*
**************************************************************************************************/
int CLlc::getObjectiveNum() 
{
	return m_iObjNum;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setObjectivePitch
*
* Description:	Set a distance between two LLC objectives [mm].
*				Call this function to finish LLC initialization if automatic detection of objective
*				pitch size failed.
*
* Input		 :	fObjPitch - a distance between two LLC objectives (depends on lens changer model - 
*				see model definition for: llcModels). 
*
* Output	 :	void
*
* Return	 :	void
*
* Notice	 :	To ensure if there is any need to set objective pitch parameter, call
*				"getObjectivePitch" function after call to "initializeLlc". If "getObjectivePitch" 
*				returns zero than use this function as shown in Input section above.
*
**************************************************************************************************/
void CLlc::setObjectivePitch(float fObjPitch[MAX_LLC_OBJ_NUM]) 
{
	for (int i = 0; i < MAX_LLC_OBJ_NUM; i++)
	{
		if (i < m_iObjNum)
		{
			m_fObjPitch[i] = fObjPitch[i];
		}
		else
		{
			m_fObjPitch[i] = 0.0f;
		}
	}

	LLCLOGMSG2(CLogger::LogLow, "%s%.2f%.2f%.2f%.2f%.2f%.2f%.2f%.2f%s", "Info: Objective separation pitch size (f) = ", 
								m_fObjPitch[0], m_fObjPitch[1], m_fObjPitch[2], m_fObjPitch[3], 
								m_fObjPitch[4], m_fObjPitch[5], m_fObjPitch[6], m_fObjPitch[7], ".\n");
}


/**************************************************************************************************
*
* Function	 :	CLlc::getObjectivePitch
*
* Description:	Get a distance between two LLC objectives [mm].
*				Call this function to get a distance between two LLC objectives.
*
* Input		 :	void
*
* Output	 :	void - a set of distances between first and the rest of LLC objectives
*
* Return	 :	void
*
* Notice	 :	Use this function after call to "initializeLlc" function. If it returns zeros than
*				call "setObjectivePitch" function (as described in the function header).
*
**************************************************************************************************/
void CLlc::getObjectivePitch(float fObjPitch[MAX_LLC_OBJ_NUM]) 
{
	for (int i = 0; i < MAX_LLC_OBJ_NUM; i++)
	{
		fObjPitch[i] = m_fObjPitch[i];
	}
}


/**************************************************************************************************
*
* Function	 :	CLlc::sendLlc
*
* Description:	Process LLC command.
*				Call this function when sending command to LLC. This function is also waiting for 
*				LLC response. Use this function when implementing specific task not covered by CLlc
*				class, otherwise use already ready to use library functions.
*
* Input		 :	pToSend - pointer to buffer containing LLC command
*
* Output	 :	pToRecv - pointer to buffer for LLC response
*
* Return	 :	int - LLC error code returned by this function
*				- LlcErrOK			= processing LLC command successful
*				- LlcErrCmndSyntax	= LLC command syntax error - command to long
*				- LlcErrSendFailure = failed to send LLC command - COM port problem
*				- LlcErrSensorCom	= failed to receive LLC response - timeout
*				- LlcErrRecvLen		= incorrect LLC response - response to long
*				- LlcErrRecvSyntax	= incorrect LLC response - missing termination character or 
*									  missing LLC command echo or mismatch betweeen sent command and
*									  its echo
*				- LlcErrWhistleEC	= LLC responded with error - see log file for error details and
*									  documentation for Whistle Error Code meaning
*
* Notice	 :	This function is a replacement for CommToLlc function available in earlier versions
*				of the ATF library. Use this function - if required - in new applications.
*
**************************************************************************************************/
int CLlc::sendLlc(char *pToSend, char *pToRecv)
{
	const char *pFunName = "CLlc::sendLlc";

	const int iMaxResponse = 64;			// max allowed LLC response length
	const int iMaxCmndLen = 48;				// max allowed LLC command length
	const int iReadDelay = 4;				// read delay coefficient

	char cRecvBuf[iMaxResponse] = {'\0'};	// receive buffer
	int iCmdLen = strlen(pToSend);			// command length
	int iRecvCnt = 0;						// received number of bytes
	int iRetLoc = 0;						// local return code
	int iRet = 0;							// function return code
	int i = 0;								// index
	int iRecvTimeout = 50;					// basic get response timeout (in numbers of loops)
	int iTmp = 0;							// temp value
	int iErrTimeoutCnt = 0;					// timeout error counter

	CEPPARR_DECLARE_CURRENT_OR_RETURN

	LLCENTERFUN(pFunName);

	// check command length
	if (iCmdLen >= iMaxCmndLen)
	{
		iRet = LlcErrCmndSyntax;

		LLCLOGMSG2(CLogger::LogLow, "%s%s%s%d%s", 
			       "Error: Failed to execute LLC command: \"", pToSend, 
				   "\" - command to long. [LlcErrorNum=", iRet, "]\n");

		goto exitfun;
	}

	// send command to LLC
	for (i = 0; i < 2; i++)
	{
		iRecvTimeout = 50;
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send command to LLC: \"", pToSend, "\".\n");

		// send command to sensor
		iRetLoc = pEpp->WriteArray ((void*)pToSend, 'f', 100, DtByteEnm, iCmdLen, 0);

		if (iRetLoc != ErrOK)
		{
			iRet = LlcErrSendFailure;
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send LLC command [EppErrorNum=", iRetLoc, "]\n");
			continue;
		}

		// reset receive variables
		ZeroMemory(cRecvBuf, sizeof(cRecvBuf));
		iRecvCnt = 0;

		// basic wait time for response
		Sleep((iCmdLen + 1) * iReadDelay);

		// wait a little bit longer for RS or LD command
		if (_stricmp(pToSend, "RS;") == 0 || _stricmp(pToSend, "LD;") == 0)
		{
			iRecvTimeout *= 2;
		}

		// get LLC response
		while (iRecvTimeout-- > 0)
		{
			iRetLoc = pEpp->ReadArray((void*)(cRecvBuf + iRecvCnt), 'f', 100, DtByteEnm, (iMaxResponse - 2) - iRecvCnt, 0);

			if (iRetLoc != ErrOK)
			{
				iRet = LlcErrSensorCom;

				if (iRetLoc == ErrOperFailed)
				{
					// received NAK but without LLC response
					LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC response - NAK. [EppErrorNum=", iRetLoc, "]\n");
					iErrTimeoutCnt = 0;
				}
				else if (iRetLoc == ErrTimeout)
				{
					// no response - timeout
					LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC response - TIMEOUT. [EppErrorNum=", iRetLoc, "]\n");
					iErrTimeoutCnt++;
					if (iErrTimeoutCnt >= 2)
					{
						LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC response - exit. [EppErrorNum=", iRetLoc, "]\n");
						goto exitfun;
					}
				}
				else if (iRetLoc == ErrChksum)
				{
					// checksum error
					LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC response - CHECKSUM. [EppErrorNum=", iRetLoc, "]\n");
					break;
				}
				else
				{
					// any of the above - should not happen
					LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC response. [EppErrorNum=", iRetLoc, "]\n");
					iErrTimeoutCnt = 0;
				}
			}
			else
			{
				iRecvCnt = strlen(cRecvBuf);

				if (iRecvCnt >= (iMaxResponse - 3)) 
				{
					// unexpected number of characters received
					LLCLOGMSG2(CLogger::LogLow, "%s%d%s", 
							   "Error: Failed to get LLC response - unexpected number of characters received [LlcResponseLen=",
							   iRecvCnt, "].\n");
					LLCLOGMSG2(CLogger::LogLow, "%s%32s %s%s",
							   "Info: Failed LLC response: \"", cRecvBuf,
							   (iRecvCnt > 32) ? "..." : "", "\".\n");

					iRet = LlcErrRecvLen;
					break;
				}

				// the response is terminated with ";"
				if (iRecvCnt > iCmdLen && cRecvBuf[iRecvCnt - 1] == ';') 
				{
					iRet = LlcErrOK;
					break; // last byte is terminator
				}
			}

			Sleep(10);
		}

		// check for receiving timeout
		if (iRecvTimeout < 0) 
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", 
					   "Error: Failed to get LLC response - no termination character [LlcResponseLen=", 
					   iRecvCnt, "].\n");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", 
					   "Info: Failed LLC response: \"",
					   (iRecvCnt > 0) ? cRecvBuf : "", "\".\n");

			iRet = LlcErrRecvSyntax;	// can't get termination ;

			// cleanout partial command
			LLCLOGMSG("Info: Send command to LLC: \";\".\n", CLogger::LogLow);

			iRetLoc = pEpp->WriteArray ((void*)";", 'f', 100, DtByteEnm, 1, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send LLC cleanout command. [EppErrorNum=", iRetLoc, "]\n");
			}

			Sleep(10);
			iRetLoc = pEpp->ReadArray((void*)(cRecvBuf), 'f', 100, DtByteEnm, iMaxResponse - 2, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get response to LLC cleanout command. [EppErrorNum=", iRetLoc, "]\n");
			}
//
			LLCLOGMSG("Info: Send command to LLC: \"WDI\".\n", CLogger::LogLow);

			iRetLoc = pEpp->WriteArray ((void*)"WDI", 'f', 100, DtByteEnm, 3, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send LLC debug command. [EppErrorNum=", iRetLoc, "]\n");
			}

			Sleep(10);
			iRetLoc = pEpp->ReadArray((void*)(cRecvBuf), 'f', 100, DtByteEnm, iMaxResponse - 2, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get response to LLC debug command. [EppErrorNum=", iRetLoc, "]\n");
			}
//
			LLCLOGMSG("Info: Send command to LLC: \";\".\n", CLogger::LogLow);

			iRetLoc = pEpp->WriteArray ((void*)";", 'f', 100, DtByteEnm, 1, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send LLC cleanout_2 command. [EppErrorNum=", iRetLoc, "]\n");
			}

			Sleep(10);
			iRetLoc = pEpp->ReadArray((void*)(cRecvBuf), 'f', 100, DtByteEnm, iMaxResponse - 2, 0);

			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get response to LLC cleanout_2 command. [EppErrorNum=", iRetLoc, "]\n");
			}

			continue;
		}
		else if (iRetLoc == ErrChksum)
		{
			// if last cmnd was relative movement setup, and current commnad is begin movement then exit
			if (_strnicmp(m_cLastCmnd, "PR=", 3) == 0 && _stricmp(pToSend, "BG;") == 0)
			{
				goto exitfun;
			}
			else
			{
				continue;
			}
		}

		// check for response length
		if (iRecvCnt <= iCmdLen) 
		{
			LLCLOGMSG("Error: Failed to receive LLC response - response to short.\n", CLogger::LogLow);
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", 
					   "Info: Failed LLC response: \"", 
					   (iRecvCnt > 0) ? cRecvBuf : "", "\".\n");

			iRet = LlcErrRecvSyntax;	// elmo always suppose to repeat what was send
			continue;
		}

		// verify that leading echo characters are the same
		if (_strnicmp(cRecvBuf, pToSend, iCmdLen) != 0)
		{
			LLCLOGMSG("Error: Failed to receive LLC response - leading echo characters mismatch.\n", CLogger::LogLow);
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s",
					   "Info: Failed LLC response: \"",
					   (iRecvCnt > 0) ? cRecvBuf : "", "\".\n");

			iRet = LlcErrRecvSyntax;	// some mismatch
			continue;
		}

		// verify that character before ; is not ?, in this case command was garbled
		if (cRecvBuf[iRecvCnt - 2] == '?') 
		{			
			iRet = LlcErrWhistleEC;
			iTmp = ((unsigned char)cRecvBuf[iRecvCnt - 3]);	// error as detected by whistle

			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to receive LLC response - garbled response.[LlcWhistleError=", iTmp, "]\n");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s",
					   "Info: Failed LLC response: \"",
					   (iRecvCnt > 0) ? cRecvBuf : "", "\".\n");

			if (iTmp == 2 || iTmp == 3 || iTmp == 5 || iTmp == 18 || iTmp == 19 || iTmp == 24 || iTmp == 42)
			{
				// those error codes indicate syntax problem
				continue;
			}

			// serious problem - repeating the command will not help - check EC error codes in Elmo manual
			goto exitfun;
		}

		// all is OK
		iRet = LlcErrOK;
		break;
	}

exitfun:
	if (iRet)	
	{
		*pToRecv = '\0';
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to process LLC command. [LlcErrorNum=", iRet, "]\n");
	}
	else 
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: LLC response: \"", cRecvBuf, "\".\n");
		strcpy (pToRecv, cRecvBuf + iCmdLen);
		pToRecv[strlen(pToRecv) - 1] = '\0';
	}
	
	// save last command
	if (iCmdLen < iMaxCmndLen)
	{
		strcpy(m_cLastCmnd, pToSend);
	}

	LLCEXITFUN(pFunName);
	return iRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcStatus
*
* Description:	Get LLC status.
*				Call this function to get current LLC serial communication and hardware status.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code indicating current LLC status
*				- LlcErrOK		      = LLC is in good operational state
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = failed to communicate with sensor, call 
*									    "recoverLlcSensorConnection" for an attempt to reinstate
*										serial communication
*				- LlcErrStatus	      = failed to get status register or motor failure register 
*										value from the sensor, retry this function again, if failed
*										with the same error code again call "recoverLlcHdwr"
*				- LlcErrLimitSwitch   = one of the limit switches is on
*				- LlcErrMotorDisabled = LLC motor is disabled
*				- LlcErrPeakCurrExceeded = hardware failure - peak current exceeded - to recover
*										call "recoverLlc" or "recoverLlcHdwr" function
*				- LlcErrInhibit		  =	hardware failure - inhibit - to recover	call "recoverLlc"
*										or "recoverLlcHdwr" function
*				- LlcErrSpeedTracking =	hardware failure - speed tracking error - to recover call 
*										"recoverLlc" or "recoverLlcHdwr" function
*				- LlcErrPositionTracking = hardware failure - position tracking error - to recover
*										call "recoverLlc" or "recoverLlcHdwr" function
*				- LlcErrUnderVoltage  =	hardware failure - under voltage error - examine LLC power
*										lines, restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrOverVoltage   =	hardware failure - over voltage error - examine LLC power
*										lines, restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrShortCircuit  =	hardware failure - short circuit - examin LLC wiring,
*										restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrOverheating   =	hardware failure - motor overheating - examine LLC hardware,
*										restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrElectricalZero = hardware failure - electrical zero not found - examine LLC
*										hardware, restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrSpeedLimit	  = hardware failure - speed limit exceeded, examine LLC 
*										parameters, restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrMotorStuck	  = hardware failure - motor not moving - examine LLC hardware,
*										restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrPositionlimit =	hardware failure - position limit exceeded - examine LLC
*										hardware and parameters, restart application, to recover 
*										"recoverLlc" or	"recoverLlcHdwr" function may be called
*				- LlcErrStartMotor	  = hardware failure - cannot start motor - examine LLC
*										hardware, restart application, to recover "recoverLlc" or
*										"recoverLlcHdwr" function may be called
*				- LlcErrStackOverflow =	firmware failure - stack overflow - restart LLC
*				- LlcErrCpuException  =	frimware failure - CPU exception- restart LLC
*				ppErrMsg - error message information (see top of this file for all error messages)
*				piStatReg - status register - received with "SR;" command (see relevant bit
*							description on the top of llc.h file)
*				piMtrFailureReg - motor failure register - received with "MF;" command (see
*								  relevant bit description on the top of llc.h file)
*
* Return	 :	true = success; false = failure
*
* Notice	 :	In case of error - especially hardware failures - examine error message returned
*				with this function to undertake proper way to fix the problem. It may be beneficial
*				to examine additionally status register and motor failure register if needed.
*
**************************************************************************************************/
bool CLlc::getLlcStatus(int *piErrCode, const char **ppErrMsg, int *piStatReg, int *piMtrFailureReg)
{
	const char *pFunName = "CLlc::getLlcStatus";

	Combuf_Char pToRecv[64] = {'\0'};	// LLC response buffer
	Combuf_Char pToSend[32] = {'\0'};	// LLC command buffer
	const char *pErrMsg = pLlcErrOK;	// LLC error message
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iMF = 0;				// motor failure register value
	int iErrCode = LlcErrOK;	// LLC error code
	bool bRet = true;			// function return value
	CEpp *pEpp = CEppArr::Epp();
	if (!pEpp) return false;

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check communication with sensor
	ZeroMemory(pToRecv, sizeof(pToRecv));

	sprintf((char*)pToSend, "ACv00000000A");
	LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
	iRetLoc = pEpp->sendCmnd(pToSend, pToRecv, 200, 2);
	if (iRetLoc != ErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to process LLC command. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		pErrMsg = pLlcErrSensorCom;
		goto exitfun;
	}

	LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");

	if (_strnicmp((char*)pToRecv, "AA", 2) != 0)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Received wrong response - expected: AA. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		pErrMsg = pLlcErrSensorCom;
		goto exitfun;
	}

	// check SR - status register
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf((char*)pToSend, "SR;");
	iRetLoc = sendLlc((char*)pToSend, (char*)pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrStatus;
		pErrMsg = pLlcErrStatus;
		bRet = false;
		goto exitfun;
	}

	iSR = atoi((char*)pToRecv);
	if ((iSR & LLC_SR_MF_FLAG) == 0)
	{
		// there is no motor failure - examine status register
		if (iSR & LLC_SR_SL_FLAG)
		{
			// limit switch turned on
			iErrCode = LlcErrLimitSwitch;
			pErrMsg = pLlcErrLimitSwitch;
			goto exitfun;
		}
		else if ((iSR & LLC_SR_MO_FLAG) == 0)
		{
			// motor disabled
			iErrCode = LlcErrMotorDisabled;
			pErrMsg = pLlcErrMotorDisabled;
			goto exitfun;
		}

		goto exitfun;
	}

	// check MF - motor failure register
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf((char*)pToSend, "MF;");
	iRetLoc = sendLlc((char*)pToSend, (char*)pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: MF. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrStatus;
		pErrMsg = pLlcErrStatus;
		bRet = false;
		goto exitfun;
	}

	iMF = atoi((char*)pToRecv);
	// examine motor failure register
	if (iMF & LLC_MF_PC_FLAG)
	{
		iErrCode = LlcErrPeakCurrExceeded;
		pErrMsg = pLlcErrPeakCurrExceeded;
		goto exitfun;
	}
	else if (iMF & LLC_MF_IN_FLAG)
	{
		iErrCode = LlcErrInhibit;
		pErrMsg = pLlcErrInhibit;
		goto exitfun;
	}
	else if (iMF & LLC_MF_ST_FLAG)
	{
		iErrCode = LlcErrSpeedTracking;
		pErrMsg = pLlcErrSpeedTracking;
		goto exitfun;
	}
	else if (iMF & LLC_MF_PS_FLAG)
	{
		iErrCode = LlcErrPositionTracking;
		pErrMsg = pLlcErrPositionTracking;
		goto exitfun;
	}
	else if (iMF & LLC_MF_DR_FLAG)
	{
		if (iMF & LLC_MF_DRUV_FLAG)
		{
			iErrCode = LlcErrUnderVoltage;
			pErrMsg = pLlcErrUnderVoltage;
		}
		else if (iMF & LLC_MF_DROV_FLAG)
		{
			iErrCode = LlcErrOverVoltage;
			pErrMsg = pLlcErrOverVoltage;
		}
		else if (iMF & LLC_MF_DRSC_FLAG)
		{
			iErrCode = LlcErrShortCircuit;
			pErrMsg = pLlcErrShortCircuit;
		}
		else
		{
			iErrCode = LlcErrOverheating;
			pErrMsg = pLlcErrOverheating;
		}

		goto exitfun;
	}
	else if (iMF & LLC_MF_EZ_FLAG)
	{
		iErrCode = LlcErrElectricalZero;
		pErrMsg = pLlcErrElectricalZero;
		goto exitfun;
	}
	else if (iMF & LLC_MF_SL_FLAG)
	{
		iErrCode = LlcErrSpeedLimit;
		pErrMsg = pLlcErrSpeedLimit;
		goto exitfun;
	}
	else if (iMF & LLC_MF_MS_FLAG)
	{
		iErrCode = LlcErrMotorStuck;
		pErrMsg = pLlcErrMotorStuck;
		goto exitfun;
	}
	else if (iMF & LLC_MF_PL_FLAG)
	{
		iErrCode = LlcErrPositionlimit;
		pErrMsg = pLlcErrPositionlimit;
		goto exitfun;
	}
	else if (iMF & LLC_MF_SM_FLAG)
	{
		iErrCode = LlcErrStartMotor;
		pErrMsg = pLlcErrStartMotor;
		goto exitfun;
	}
	else if (iMF & LLC_MF_SO_FLAG)
	{
		iErrCode = LlcErrStackOverflow;
		pErrMsg = pLlcErrStackOverflow;
		goto exitfun;
	}
	else if (iMF & LLC_MF_CE_FLAG)
	{
		iErrCode = LlcErrCpuException;
		pErrMsg = pLlcErrCpuException;
		goto exitfun;
	}

exitfun:
	LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: LLC status code = ", iErrCode, ".\n");

	*piErrCode = iErrCode;
	*ppErrMsg = pErrMsg;
	*piStatReg = iSR;
	*piMtrFailureReg = iMF;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::recoverLlc
*
* Description:	Recover LLC - serial communication to the sensor/LLC and  LLC hardware.
*				Call this function to recover from serial communication failure to the sensor as 
*				well as from LLC hardware failure.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = LLC recovery succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrStatus		  = failed to get status register
*				- LlcErrRecoveryFailed = LLC attempted recovery failed on status register
*				- error codes returned by "recoverLlcSensorConnection" function
*				- error codes returned by "recoverLlcHdwr" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function is a wrapper for "recoverLlcSensorConnection" and "recoverLlcHdwr"
*				functions combined into one. An additional call is made to evaluate status register
*				to ensure LLC good health state.
*
**************************************************************************************************/
bool CLlc::recoverLlc(int *piErrCode)
{
	const char *pFunName = "CLlc::recoverLlc";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iErrCode = LlcErrOK;	// LLC error code
	bool bRetLoc = false;		// return code for locally called functions
	bool bRet = false;			// function return value

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check sensor/LLC connectivity
	bRetLoc = recoverLlcSensorConnection(&iErrCode);
	if (!bRetLoc)
	{
		LLCLOGMSG("Error: Failed to establish LLC connection.\n", CLogger::LogLow);
		goto exitfun;
	}

	// recover from hardware error
	bRetLoc = recoverLlcHdwr(&iErrCode);
	if (!bRetLoc)
	{
		LLCLOGMSG("Error: Failed to recover LLC hardware.\n", CLogger::LogLow);
		goto exitfun;
	}

	// get SR - status register
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	iSR = atoi(pToRecv);

	if ((iSR & (LLC_SR_MO_FLAG | LLC_SR_MF_FLAG | LLC_SR_MS_FLAG | LLC_SR_SL_FLAG)) != LLC_SR_MO_FLAG)
	{
		// motor is disabled or motor failure flag is set or motion status is not cleared (i.e. not in position)
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to recover LLC hardware. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrRecoveryFailed;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::recoverLlcSensorConnection
*
* Description:	Restore communication with the sensor / LLC.
*				Call this function to test or to restore serial communication with the sensor / LLC.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = LLC serial communication recovery succeded
*				- LlcErrSensorCom	  = serial communication not working at all
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function is restoring communication with the sensor / LLC if at least the 
*				communication with the bootloader, Whistle or MFC is detected. The call to 
*				"getLlcStatus" function may be performed first before calling this function. The
*				function is also called from within "recoverLlc" function for serial communication 
*				and hardware recovery.
*
**************************************************************************************************/
bool CLlc::recoverLlcSensorConnection(int *piErrCode)
{
	const char *pFunName = "CLlc::recoverllcSensorConnection";

	// recovery finalizing steps mode 
	enum FinalizingSteps
	{
		FinalConnectionCheck = 0,
		SensorBufferCleanup,
		ExitWithFailure
	};

	Combuf_Char pToRecv[64] = {'\0'};	// LLC response buffer
	Combuf_Char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iErrCode = LlcErrOK;	// LLC error code
	int iLoopNum = 0;			// number of recovery loops
	DWORD dwCurrBaudrate = 0;	// current COM baudrate
	DWORD dwInitBaudrate = 0;	// initial baudrate
	int iFinalizingSteps = ExitWithFailure;	// recovery finalizing steps mode
	CEpp *pEpp = CEppArr::Epp();
	if (!pEpp) return false;

	LLCENTERFUN(pFunName);

	// save current baudrate
	pEpp->_GetCommBaudrate(&dwInitBaudrate);
	dwCurrBaudrate = dwInitBaudrate;

	// maximum three passes
	while(iLoopNum < 3)
	{
		// send testing string
		ZeroMemory(pToRecv, sizeof(pToRecv));

		sprintf((char*)pToSend, "ACv00000000A");
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
		iRetLoc = pEpp->sendCmnd(pToSend, pToRecv, 200);
		if (iRetLoc != ErrOK && iRetLoc != ErrTimeout)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send COM command. [LlcErrorNum=", iRetLoc, "]\n");
			goto exitfun;
		}

		if (iRetLoc == ErrTimeout)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to receive COM response. [LlcErrorNum=", iRetLoc, "]\n");
		}
		else
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");
		}

		if (_strnicmp((char*)pToRecv, "AA", 2) == 0)
		{
			// received sensor response
			if (dwInitBaudrate == dwCurrBaudrate)
			{
				// first pass successfull - perform just buffer cleanup
				iFinalizingSteps = SensorBufferCleanup;
			}
			else
			{
				// second/third pass successfull - perform all finalizing steps
				iFinalizingSteps = FinalConnectionCheck;
			}

			break;
		}
		else if (_strnicmp((char*)pToRecv, (char*)pToSend, strlen((char*)pToSend)) == 0)
		{
			// received Whistle response - so, switch first to MFC
			ZeroMemory(pToRecv, sizeof(pToRecv));
			sprintf((char*)pToSend, ";A");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
			iRetLoc = pEpp->sendCmnd(pToSend, pToRecv, 200, 5);
			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send COM command. [LlcErrorNum=", iRetLoc, "]\n");
				goto exitfun;
			}

			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");

			if (pToRecv[0] != ';' || _strnicmp((char*)pToRecv + 2, "?;K", 3) != 0)
			{
				LLCLOGMSG("Error: Failed to restore sensor communication - unexpected response\n", CLogger::LogLow);
				goto exitfun;
			}

			// enforce sensor connection
			sprintf((char*)pToSend, "\xb6\x49");
			LLCLOGMSG("Info: Send COM command: \"0xb6 0x49\"\n", CLogger::LogLow);
			iRetLoc = pEpp->sendCmnd(pToSend);
			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send COM command. [LlcErrorNum=", iRetLoc, "]\n");
				goto exitfun;
			}

			Sleep(100);
		}
		else if (_strnicmp((char*)pToRecv, (char*)pToSend, 3) == 0 && _strnicmp((char*)pToRecv + 3, (char*)pToSend + 3, 8) != 0 && pToRecv[strlen((char*)pToSend) - 1] == 'A')
		{
			// received boot loader response - so, reset it to restore sensor connection
			ZeroMemory(pToRecv, sizeof(pToRecv));
			sprintf((char*)pToSend, "Z");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
			iRetLoc = pEpp->sendCmnd(pToSend, pToRecv, 200, 1);
			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send COM command. [LlcErrorNum=", iRetLoc, "]\n");
				goto exitfun;
			}

			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");

			// check response
			if (pToRecv[0] != 'A')
			{
				goto exitfun;
			}

			// if the last used baudrate was not default one than change it to default one (because of sensor resetting)
			if (dwCurrBaudrate != DefaultBaudrate)
			{
				pEpp->_ChangeCommBaudrate((DWORD)DefaultBaudrate);
				dwCurrBaudrate = DefaultBaudrate;
			}

			// wait for 5 sec
			Sleep(5000);
			iFinalizingSteps = FinalConnectionCheck;

			break;
		}
		else
		{
			// no response or unrecognized response received - try again with default baudrate
			if (dwInitBaudrate != DefaultBaudrate)
			{
				// change baudrate to default one (9600)
				pEpp->_ChangeCommBaudrate((DWORD)DefaultBaudrate);
				dwCurrBaudrate = DefaultBaudrate;
				LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Info: Changed COM port baudrate to default: ", DefaultBaudrate, ".\n");
			}
			else
			{
				// total failure - giving up
				iFinalizingSteps = ExitWithFailure;
				break;
			}
		}

		iLoopNum++;
	}

	// reinstate initial baudrate
	if (dwInitBaudrate != dwCurrBaudrate && iFinalizingSteps != ExitWithFailure)
	{
		iRetLoc = pEpp->WriteArray((void*)&dwInitBaudrate, 'U', 25, DtDwordEnm);
		if (iRetLoc != ErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to change sensor baudrate. [LlcErrorNum=", iRetLoc, "]\n");
		}

		Sleep(300);
		pEpp->_ChangeCommBaudrate(dwInitBaudrate);
		Sleep(200);
	}

	// finalizing steps
	switch (iFinalizingSteps)
	{
	case FinalConnectionCheck:
		{
			// check finally if connecting to the sensor was successfull
			ZeroMemory(pToRecv, sizeof(pToRecv));
			
			sprintf((char*)pToSend, "ACv00000000A");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
			iRetLoc = pEpp->sendCmnd(pToSend, pToRecv, 200, 2);
			if (iRetLoc != ErrOK)
			{
				LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send COM command. [LlcErrorNum=", iRetLoc, "]\n");
				break;
			}

			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");

			// check response
			if (_strnicmp((char*)pToRecv, "AA", 2) != 0)
			{
				LLCLOGMSG("Error: Failed to restore sensor communication - unexpected response\n", CLogger::LogLow);
				break;
			}
		}

	case SensorBufferCleanup:
		{
			// cleanup sensor buffer
			ZeroMemory(pToRecv, sizeof(pToRecv));
			
			sprintf((char*)pToSend, ";");
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Send COM command: \"", pToSend, "\".\n");
			iRetLoc = sendLlc((char*)pToSend, (char*)pToRecv);
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Received COM response: \"", pToRecv, "\".\n");
			bRet = true;
		}
	}

exitfun:
	if (bRet)
	{
		LLCLOGMSG("Info: Checking/restoring COM port connection successfull.\n", CLogger::LogLow);
		iErrCode = LlcErrOK;
	}
	else
	{
		LLCLOGMSG("Error: Failed to check/restore COM port connection.\n", CLogger::LogLow);
		iErrCode = LlcErrSensorCom;
	}

	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}

/**************************************************************************************************
*
* Function	 :	CLlc::recoverLlcHdwr
*
* Description:	Recover from LLC hardware error.
*				Call this function to restore LLC hardware functionality in case of hardware error.
*				One of the recovery step is LLC homing what may take up to 10 seconds.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = LLC hardware recovery succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- error codes returned by "resetLlcController" function
*				- error codes returned by "homeLlc" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	Call to	"getLlcStatus" function may be performed first before calling this function
*				to ensure hardware failure. The function is also called from within "recoverLlc" 
*				function for serial communication and hardware recovery.
*
**************************************************************************************************/
bool CLlc::recoverLlcHdwr(int *piErrCode)
{
	const char *pFunName = "CLlc::recoverLlcHdwr";

	char pToSend[32] = {'\0'};	// LLC response buffer
	char pToRecv[64] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	bool bRetLoc = false;		// return code for locally called functions
	int iRetLoc = 0;			// return code for locally called funcitons
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// reset LLC controller
	bRetLoc = resetLlcController(&iErrCode);
	if (bRetLoc == false)
	{
		LLCLOGMSG("Error: Failed to reset LLC controller.\n", CLogger::LogLow);
		goto exitfun;
	}

	// home LLC
	bRetLoc = homeLlc(true, &iErrCode);
	if (bRetLoc == false)
	{
		LLCLOGMSG("Error: Failed to move LLC into home position.\n", CLogger::LogLow);
		goto exitfun;
	}

	bRet = true;

exitfun:
	if (bRet)
	{
		LLCLOGMSG("Info: Recovering LLC hardware successfull.\n", CLogger::LogLow);
	}
	else
	{
		LLCLOGMSG("Error: Failed to recover LLC hardware.\n", CLogger::LogLow);
	}

	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::resetLlcController
*
* Description:	Reset LLC controller.
*				Call this function to reset LLC controller, i.e.: disable the motor, perform soft
*				reset and load all non-volatile variables from the flash memory to the RAM and 
*				reset all volatile variables to their default values.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = LLC resetting controller succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom    = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	The function is also called from within "recoverLlcHdwr" function as a part of
*				hardware recovery.
*
**************************************************************************************************/
bool CLlc::resetLlcController(int *piErrCode)
{
	const char *pFunName = "CLlc::resetLlcController";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// stop motion - if any
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ST;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to enable LLC motor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// disable LLC motor
	sprintf(pToSend, "MO=0;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to disable LLC motor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// soft LLC reset
	sprintf(pToSend, "RS;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to soft LLC reset. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// load parameters from LLC flash
	sprintf(pToSend, "LD;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to load parameters from LLC flash. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);
	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::homeLlc
*
* Description:	Move LLC into home position.
*				Call this function to move LLC into home position. The function may exit 
*				immediatelly (bWaitForEnd = false) or wait until the home position is reached
*				(bWaitForEnd = true). When choosing first option call "isLlcInHomePos" function
*				periodically to check for the end of the homing procedure.
*
* Input		 :	bWaitForEnd - false = exit function immediatelly
*							  true  = exit function on the end of homing
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = homing LLC succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom    = serial communication failure
*				- LlcErrTimeout		 = failed to reach home position before timeout
*				- error codes returned by "isLlcInHomePos" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	The function is also called from within "recoverLlcHdwr" function as a part of
*				hardware recovery. The homing procedure may take up to 10 seconds. It is worthy to
*				mention that this function is enabling the LLC motor internally.
*
**************************************************************************************************/
bool CLlc::homeLlc(bool bWaitForEnd, int *piErrCode)
{
	const char *pFunName = "CLlc::homeLlc";

	const int HomeTimeout = 10000;	// total homing timeout in msec
	const int TimeStepSize = 100;	// checking current position frequency in msec

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRetLoc = false;		// return code for locally called functions
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iHomeTimeout = 0;		// homing timeout
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// start LLC homing procedure
	LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Starting LLC homing procedure. [bWaitForEnd=", bWaitForEnd ? "true" : "false", "]\n");
	sprintf(pToSend, "XQ##;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to send XQ## command for LLC homing procedure. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	Sleep(500);

	// check if wait for the end of homing procedure
	if (bWaitForEnd)
	{
		// wait for the end of homing procedure
		for (iHomeTimeout = 0; iHomeTimeout < HomeTimeout; iHomeTimeout += TimeStepSize)
		{
			bRetLoc = isLlcInHomePos(&iErrCode);
			if (bRetLoc || iErrCode != LlcErrOK) break;
			Sleep(TimeStepSize);
		}

		// check for homing results
		if (iHomeTimeout < HomeTimeout && iErrCode == LlcErrOK) 
		{
			// homing successfull
			LLCLOGMSG("Info: LLC homing procedure successfull.\n", CLogger::LogLow);
		}
		else if (iHomeTimeout >= HomeTimeout)
		{
			// homing timeout
			LLCLOGMSG("Error: Failed to move LLC into home position - timeout.\n", CLogger::LogLow);
			iErrCode = LlcErrTimeout;
			goto exitfun;
		}
		else
		{
			// homing error
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Failed to move LLC into home position - homing error. [LlcErrorNum=", iErrCode, "]\n");
			goto exitfun;
		}
	}

	iErrCode = LlcErrOK;
	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}

/**************************************************************************************************
*
* Function	 :	CLlc::isLlcInHomePos
*
* Description:	Check if LLC reached its home position.
*				Call this function to check if LLC reached its home position. The function should
*				be called periodically to determine homing status after calling "homeLlc" function
*				(assuming that "homeLlc" was called with "bWaitForEnd = false" argument, otherwise
*				there is no need to call this function).
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected (does not mean that it reached home 
*										position) 
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrHoming		  = failed to reach home position - incorrect final status
*				- LlcErrStatus		  = failed to reach home position - incorrect motion status or 
*									    motor status
*
* Return	 :	true = success (in home postion); false = failure (not in home position yet)
*
* Notice	 :	The function is also called from within "homeLlc" function if "bWaitForEnd = true"
*				argument is specified. The homing procedure may take up to 10 seconds.
*
**************************************************************************************************/
bool CLlc::isLlcInHomePos(int *piErrCode)
{
	const char *pFunName = "CLlc::isLlcInHome";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iErrCode = LlcErrOK;	// LLC error code
	int iSR = 0;				// status register value
	int iET5 = 0;				// ET[5] value
	int iET1 = 0;				// ET[1] value
	int iPX = 0;				// PX value (current position register)

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check which method of end of homing detection should be chosen 
	if (m_bIsHomeDetImpl)
	{
		// get ET[5] value
		sprintf(pToSend, "ET[5];");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: ET[5]. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}
		
		// check for current state of homing
		iET5 = atoi(pToRecv);
		if (iET5 == 1)
		{
			// LLC is in home position
			bRet = true;
		}
		else if (iET5 > 1024)
		{
			// LLC failed during homing
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to reach LLC home position - (ET[5] error). [LlcET5ErrorNum=", iET5, "]\n");
			iErrCode = LlcErrHoming;
		}
	}
	else
	{
		// get homing status
		// get SR register status
		sprintf(pToSend, "SR;");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}
		
		iSR = atoi(pToRecv);
		// is homing still running (PR=1)
		if ((iSR & LLC_SR_PR_FLAG) != 0)
		{
			// homing is still running
			goto exitfun;
		}

		// is motion status InPosition (MS=0)
		if ((iSR & LLC_SR_MS_FLAG) != 0)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to reach LLC home position - MS error. [LlcMsErrorNum=", (iSR & LLC_SR_MS_FLAG) >> 14, "]\n");
			iErrCode = LlcErrStatus;
			goto exitfun;
		}

		// is motor enabled
		if ((iSR & LLC_SR_MO_FLAG) == 0)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to reach LLC home position - motor disabled. [LlcMoErrorNum=", 0, "]\n");
			iErrCode = LlcErrStatus;
			goto exitfun;
		}

		// check "ET[1] - PX" condition
		// get ET[1] value
		sprintf(pToSend, "ET[1];");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: ET[1]. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}
		
		// check for current state of homing
		iET1 = atoi(pToRecv);

		// get PX value
		sprintf(pToSend, "PX;");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: PX. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}
		
		// check for current state of homing
		iPX = atoi(pToRecv);
		
		// check "ET[1] - PX" condition
		if (abs(iET1 - iPX) >= 2000)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to reach LLC position: ET[1] - PX. [LlcET1PxErrorNum=", abs(iET1 - iPX), "]\n");
			iErrCode = LlcErrHoming;
			goto exitfun;
		}

		bRet = true;
	}

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::createLlcOffsetFile
*
* Description:	Create LLC offset file if it doesn't exist yet.
*				It is is called by "initializeLlc" function. The file may be needed if the position
*				offset is needed to be taken into account when moving LLC into objective position
*				determined by lens separation distance. The default offset values for each 
*				objective position is zero.
*
* Input		 :	pOffsetFileDir - directory for objective position offsets file (if specified MUST
*								 already exist) or can be NULL
*
* Output	 :	void
*
* Return	 :	true = success; false = failure
*
* Notice	 :	The LLC offset file is created in the given directory or application current 
*				directory (if pOffsetFileDir = NULL). The file name is:
*				"OT.LLC.xxxxxxx" - where xxxxxxx is replaced by forcer number (distinct for every 
*				LLC). It contains the	position objective offset for each lens. Failure to create
*				this file is still allowing to run LLC functions without possibilities to set
*				objective position offsets.
*
**************************************************************************************************/
bool CLlc::createLlcOffsetFile(char *pOffsetFileDir)
{
	const char *pFunName = "CLlc::createLlcOffsetFile";

	char cCurrLine[64] = {'\0'};				// file read buffer
	char cOffsetFileName[MAX_PATH] = {'\0'};	// path to offset file
	struct _stat st;							// file stat structure
	FILE *pFile = NULL;							// file handle
	int i = 0;									// index
	bool bRet = false;							// function return value 

	LLCENTERFUN(pFunName);

	// get offset file directory
	if (pOffsetFileDir == NULL)
	{
		WCHAR m_strOffsetFilePath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, m_strOffsetFilePath);
		WideCharToMultiByte(CP_ACP, 0, m_strOffsetFilePath, -1, m_cOffsetFilePath, 100, NULL, NULL);
	}
	else
	{
		strcpy(m_cOffsetFilePath, pOffsetFileDir);
	}

	if (m_cOffsetFilePath[0] != '\0' && m_cOffsetFilePath[strlen(m_cOffsetFilePath) - 1] != '\\')
	{
		strcat(m_cOffsetFilePath, "\\");
	}

	// create offset file name
	sprintf(cOffsetFileName, "OT.LLC.%s", m_cForcerSerNum);
	strcat(m_cOffsetFilePath, cOffsetFileName);
	LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: LLC offset file name: ", m_cOffsetFilePath, "\n");

	// check if file already exist
	if (_stat(m_cOffsetFilePath, &st) == 0)
	{
		// the file already exist - open the file and check file header
		pFile = fopen(m_cOffsetFilePath, "r");
		if (pFile == NULL)
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to open LLC offset file: ", m_cOffsetFilePath, "\n");
			goto exitfun;
		}

        if (fgets(cCurrLine, sizeof(cCurrLine) - 1, pFile) == NULL)
        {
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to read LLC offset file: ", m_cOffsetFilePath, "\n");
			goto exitfun;
		}

		if (_strnicmp(cCurrLine, pFileHeader, strlen(pFileHeader)) != 0)
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed in contents of LLC offset file: ", m_cOffsetFilePath, "\n");
			goto exitfun;
		}
	}
	else
	{
		// the file doesn't exist yet - create it with all offsets set to zero
		pFile = fopen(m_cOffsetFilePath, "w");

		if (pFile == NULL)
		{
			LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to open LLC offset file: ", m_cOffsetFilePath, "\n");
			goto exitfun;
		}

		fprintf(pFile, "%s\n", pFileHeader);

		for (i = 0; i < 8; i++)
		{
			fprintf(pFile, "OBJ%d=0\n", i);
		}

		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Info: Created LLC offset file: ", m_cOffsetFilePath, "\n");
	}

	bRet = true;

exitfun:
	if (pFile != NULL)
	{
		fclose(pFile);
	}

	if (bRet == false)
	{
		m_cOffsetFilePath[0] = '\0';
	}
		
	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::saveLlcObjectivePositionOffset
*
* Description:	Save objective position offset into LLC offset file.
*				Call this function to save objective position offset for each objective. The
*				default value of all offsets are zero. The value of the offset is taken into
*				account	when moving LLC into any requested objective position.
*
* Input		 :	iObjectiveNumber - objective number (0 to 1 for LLC2, 0 to 3 for LLC4,
*													(0 to 5 for LLC6, 0 to 7 for LLC8)
*				iPositionOffset	- objective position offset [in um] for requested objective
*								  (max/min value: +/- 1000 um)
*
* Output	 :	void
*
* Return	 :	true = success; false = failure
*
* Notice	 :	The function will fail if "initializeLlc" was not able to create the file.
*
**************************************************************************************************/
bool CLlc::saveLlcObjectivePositionOffset(int iObjectiveNumber, int iPositionOffset)
{
	const char *pFunName = "CLlc::saveToLlcOffsetFile";

	FILE *pFile = NULL;						// file handle
	FILE *pFileTmp = NULL;					// file temp handle
	char cTmpFileName[MAX_PATH] = {'\0'};	// temp file name
	char cCurrLine[64] = {'\0'};			// read buffer
	char cReqParamName[32] = {'\0'};		// requested objective param name
	char cNewParamData[32] = {'\0'};		// requested objective param new value
	bool bRet = false;						// function return value

	LLCENTERFUN(pFunName);

	// check objective number
	if (iObjectiveNumber < 0 || iObjectiveNumber >= m_iObjNum)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set objective position offset - objective number not in range. [RequestedObjectiveNum=", iObjectiveNumber, "]\n");
		goto exitfun;
	}
	// check position offset range
	if (iPositionOffset < ((-1) * MaxPositionOffset) || iPositionOffset > MaxPositionOffset)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set objective position offset - position offset not in range. [RequestedPositionOffset=", iPositionOffset, "]\n");
		goto exitfun;
	}
	// check LLC offset file name
	if (strlen(m_cOffsetFilePath) == 0)
	{
		LLCLOGMSG("Error: Failed to get objective position offset - LLC offset file do not exist.\n", CLogger::LogLow);
		goto exitfun;
	}
	// open LLC offset file name
	pFile = fopen(m_cOffsetFilePath, "r");
	if (pFile == NULL)
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to open LLC offset file: ", m_cOffsetFilePath, "\n");
		goto exitfun;
	}
	// open temp LLC offset file name
	sprintf(cTmpFileName, "%s.tmp", m_cOffsetFilePath);
	pFileTmp = fopen(cTmpFileName, "w");
	if (pFileTmp == NULL)
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to open LLC offset file: ", cTmpFileName, "\n");
		goto exitfun;
	}
	// create requested parameter name
	sprintf(cReqParamName, "OBJ%d=", iObjectiveNumber);
	// read file and find requested parameter
	while (!feof(pFile))
	{
        // read next line from the file
		cCurrLine[0] = '\0';
        if (fgets(cCurrLine, sizeof(cCurrLine) - 1, pFile) == NULL)
        {
            // check for error
            if (ferror(pFile) != 0)
            {
				LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to read LLC offset file: ", m_cOffsetFilePath, "\n");
				goto exitfun;
            }

			if (strlen(cCurrLine) == 0)
			{
				break;
			}
		}
		// check for requested parameter
		if (_strnicmp(cCurrLine, cReqParamName, strlen(cReqParamName)) == 0)
		{
			// create new line
			sprintf(cNewParamData, "%s%d\n", cReqParamName, iPositionOffset);
			fputs(cNewParamData, pFileTmp);
			continue;
		}
		// copy line
		fputs(cCurrLine, pFileTmp);
	}

	fclose(pFile);
	pFile = NULL;
	fclose(pFileTmp);
	pFileTmp = NULL;

	remove(m_cOffsetFilePath);
	rename(cTmpFileName, m_cOffsetFilePath);

	bRet = true;

exitfun:
	if (pFile != NULL)
	{
		fclose(pFile);
	}
	if (pFileTmp != NULL)
	{
		fclose(pFileTmp);
	}

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcObjectivePositionOffset
*
* Description:	Get objective position offset value from LLC offset file.
*				Call this function to get objective position offset for any objective. The
*				default value of all offsets are zero. The value of the offset is taken into
*				account	when moving LLC into any requested objective position.
*
* Input		 :	iObjectiveNumber - objective number (0 to 1 for LLC2, 0 to 3 for LLC4,
*													(0 to 5 for LLC6, 0 to 7 for LLC8)
*
* Output	 :	piPositionOffset - objective position offset [in um] for requested objective
*
* Return	 :	true = success; false = failure
*
* Notice	 :	The function will fail if "initializeLlc" was not able to create the file.
*
**************************************************************************************************/
bool CLlc::getLlcObjectivePositionOffset(int iObjectiveNumber, int *piPositionOffset)
{
	const char *pFunName = "CLlc::getFromLlcOffsetFile";

	FILE *pFile = NULL;					// file handle
	char cCurrLine[64] = {'\0'};		// read buffer
	char cReqParamName[32] = {'\0'};	// requested objective param name
	int iPositionOffset = 0;			// objective position offset
	bool bRet = false;					// function return value
	bool bFirstLine = true;				// first line in the file validation flag

	LLCENTERFUN(pFunName);

	// check objective number
	if (iObjectiveNumber < 0 || iObjectiveNumber >= m_iObjNum)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get objective position offset - objective number not in range. [RequestedObjectiveNum=", iObjectiveNumber, "]\n");
		goto exitfun;
	}
	// check LLC offset file name
	if (strlen(m_cOffsetFilePath) == 0)
	{
		LLCLOGMSG("Error: Failed to get objective position offset - LLC offset file do not exist.\n", CLogger::LogLow);
		goto exitfun;
	}
	// open LLC offset file name
	pFile = fopen(m_cOffsetFilePath, "r");
	if (pFile == NULL)
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to open LLC offset file: ", m_cOffsetFilePath, "\n");
		goto exitfun;
	}
	// create requested parameter name
	sprintf(cReqParamName, "OBJ%d=", iObjectiveNumber);
	// read file and find requested parameter
	while (!feof(pFile))
	{
        // read next line from the file
        if (fgets(cCurrLine, sizeof(cCurrLine) - 1, pFile) == NULL)
        {
            // check for error
            if (ferror(pFile) != 0)
            {
				LLCLOGMSG2(CLogger::LogLow, "%s%s%s", "Error: Failed to read LLC offset file: ", m_cOffsetFilePath, "\n");
				goto exitfun;
            }
		}
		// skip empty lines and comments
		if (strlen(cCurrLine) == 0 || strcmp(cCurrLine, "\n") == 0 || 
			cCurrLine[0] == '/' || cCurrLine[0] == '-' || cCurrLine[0] == ';' || cCurrLine[0] == '*')
		{
			continue;
		}

		if (bFirstLine == true)
		{
			// validate file header
			bFirstLine = false;
			
			if (_strnicmp(cCurrLine, pFileHeader, strlen(pFileHeader)) == 0)
			{
				continue;
			}

			goto exitfun;
		}

		// check for requested objective position param name
		if (_strnicmp(cCurrLine, cReqParamName, strlen(cReqParamName)) != 0)
		{
			continue;
		}
		// get offset value
		iPositionOffset = atoi(cCurrLine + strlen(cReqParamName));
		LLCLOGMSG2(CLogger::LogLow, "%s%s%d%s", "Info: Objective position offset: ", cReqParamName, iPositionOffset, "\n");
		break;
	}

	bRet = true;

exitfun:
	*piPositionOffset = iPositionOffset;

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	LLCEXITFUN(pFunName);
	return bRet;
}

/**************************************************************************************************
*
* Function	 :	CLlc::moveLlcToObjective
*
* Description:	Move LLC to requested objective position.
*				Call this function to move LLC to requested objective position. The function may 
*				exit immediately (bWaitForEnd = false) or wait until the target objective position
*				is reached (bWaitForEnd = true). When choosing first option call 
*				"isLlcInTargetPosition" function periodically to check for the end of the moving
*				procedure.
*
* Input		 :	iObjectiveNumber - objective number (0 to 1 for LLC2, 0 to 3 for LLC4,
*													(0 to 5 for LLC6, 0 to 7 for LLC8)
*				bWaitForEnd - false = exit function immediately
*							  true  = exit function on the end of moving
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = moving LLC to objective position succeeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrArgRange	  = objective number not in range
*				- LlcErrSensorCom     = serial communication failure 
*				- LlcErrStatus		  = LLC status shows error preventing the movement
*				- LlcErrTimeout		  = failed to reach target position before timeout
*				- error codes returned by "isLlcInTargetPosition" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When using this function a value saved in the objective position offset file is
*				taken into account (offset default value is equal zero). The target objective 
*				position is	calculated using the following formula:
*				- for LLC2:		
*				  (ObjectiveNumber[=0 to 1] * ObjectivePositionPitch[=35000]) + ObjectivePositionOffset
*				- for LLC4/6/8:	
*				  (ObjectiveNumber[=0 to 7] * ObjectivePositionPitch[=38000]) + ObjectivePositionOffset 
*
**************************************************************************************************/
bool CLlc::moveLlcToObjective(int iObjectiveNumber, bool bWaitForEnd, int *piErrCode)
{
	return moveLlcToObjectiveInt(iObjectiveNumber, bWaitForEnd, false, piErrCode) ;
}

/**************************************************************************************************
*
* Function	 :	CLlc::moveLlcToObjective2
*
* Description:	Move LLC to requested objective position.
*				Call this function to move LLC to requested objective position. The function may 
*				exit immediately (bWaitForEnd = false) or wait until the target objective position
*				is reached (bWaitForEnd = true). When choosing first option call 
*				"isLlcInTargetPosition" function periodically to check for the end of the moving
*				procedure.
*				In contrary to moveLlcToObjective this function will not check LLC status before movement.
*
* Input		 :	iObjectiveNumber - objective number (0 to 1 for LLC2, 0 to 3 for LLC4,
*													(0 to 5 for LLC6, 0 to 7 for LLC8)
*				bWaitForEnd - false = exit function immediately
*							  true  = exit function on the end of moving
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = moving LLC to objective position succeeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrArgRange	  = objective number not in range
*				- LlcErrSensorCom     = serial communication failure 
*				- LlcErrStatus		  = LLC status shows error preventing the movement
*				- LlcErrTimeout		  = failed to reach target position before timeout
*				- error codes returned by "isLlcInTargetPosition" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When using this function a value saved in the objective position offset file is
*				taken into account (offset default value is equal zero). The target objective 
*				position is	calculated using the following formula:
*				- for LLC2:		
*				  (ObjectiveNumber[=0 to 1] * ObjectivePositionPitch[=35000]) + ObjectivePositionOffset
*				- for LLC4/6/8:	
*				  (ObjectiveNumber[=0 to 7] * ObjectivePositionPitch[=38000]) + ObjectivePositionOffset 
*
**************************************************************************************************/
bool CLlc::moveLlcToObjective2(int iObjectiveNumber, bool bWaitForEnd, int *piErrCode)
{
	return moveLlcToObjectiveInt(iObjectiveNumber, bWaitForEnd, true, piErrCode) ;
}

/**************************************************************************************************
*
* Function	 :	CLlc::moveLlcToObjectiveInt
*
* Description:	Move LLC to requested objective position.
*				Call this function to move LLC to requested objective position. The function may 
*				exit immediatelly (bWaitForEnd = false) or wait until the target objective position
*				is reached (bWaitForEnd = true). When choosing first option call 
*				"isLlcInTargetPosition" function periodically to check for the end of the moving
*				procedure.
*
* Input		 :	iObjectiveNumber - objective number (0 to 1 for LLC2, 0 to 3 for LLC4,
*													(0 to 5 for LLC6, 0 to 7 for LLC8)
*				bWaitForEnd - false = exit function immediately
*							  true  = exit function on the end of moving
*				bIgnoreInitStatus - false = check LLC status before move
*									true  = do not check LLC status before move
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = moving LLC to objective position succeeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrArgRange	  = objective number not in range
*				- LlcErrSensorCom     = serial communication failure 
*				- LlcErrStatus		  = LLC status shows error preventing the movement
*				- LlcErrTimeout		  = failed to reach target position before timeout
*				- error codes returned by "isLlcInTargetPosition" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When using this function a value saved in the objective position offset file is
*				taken into account (offset default value is equal zero). The target objective 
*				position is	calculated using the following formula:
*				- for LLC2:		
*				  (ObjectiveNumber[=0 to 1] * ObjectivePositionPitch[=35000]) + ObjectivePositionOffset
*				- for LLC4/6/8:	
*				  (ObjectiveNumber[=0 to 7] * ObjectivePositionPitch[=38000]) + ObjectivePositionOffset 
*
**************************************************************************************************/
bool CLlc::moveLlcToObjectiveInt(int iObjectiveNumber, bool bWaitForEnd, bool bIgnoreInitStatus, int *piErrCode)
{
	const char *pFunName = "CLlc::moveLlcToObjective";

	const int MoveTimeout = 2000;	// movement timeout in msec
	const int TimeStepSize = 100;	// checking current position frequency in msec

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	bool bRetLoc = false;		// return code for locally called functions
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iPositionOffset = 0;	// objective position offset
	int iObjectivePosition = 0;	// target objective position
	int iMoveTimeout = 0;		// moving timeout
//	int iObjPositionPitch = m_iObjPitch * ENC_PULSE_PER_MM;	// objective position pitch in motor "clicks"
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check objective number
	if (iObjectiveNumber < 0 || iObjectiveNumber >= m_iObjNum)
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%d%s", 
				   "Error: Failed to move LLC into target position - objective number not in range. [RequestedObjectiveNum=", 
				   iObjectiveNumber, "]\n");
		iErrCode = LlcErrArgRange;
		goto exitfun;
	}

	// get LLC status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		iErrCode = LlcErrSensorCom;
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		goto exitfun;
	}
	// evaluate LLC status
	iSR = atoi(pToRecv);

	if ( ! bIgnoreInitStatus )
	{
		if ((iSR & LLC_SR_MF_FLAG) != 0 || (iSR & LLC_SR_MO_FLAG) == 0 || 
			((iSR & LLC_SR_MS_FLAG) != 0 && ((iSR & LLC_SR_MS_FLAG) == 0 || (iSR & LLC_SR_SL_FLAG) == 0)))
		{
			// motor is off or motor failure flag is set or motion status is not cleared (i.e. in position)
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to move LLC to objective. [LlcSrStatus=", iSR, "]\n");
			iErrCode = LlcErrStatus;
			goto exitfun;
		}
	}

	// get objective position offset
	getLlcObjectivePositionOffset(iObjectiveNumber, &iPositionOffset);
	// calculate target position
//	iObjectivePosition = (iObjPositionPitch * iObjectiveNumber) + (iPositionOffset * ENC_PULSE_PER_MM / 1000);
	iObjectivePosition = (int)((m_fObjPitch[iObjectiveNumber] * (float)ENC_PULSE_PER_MM) + 0.5f) + (iPositionOffset * ENC_PULSE_PER_MM / 1000);

	// set LLC absolute position
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "PA=%d;", iObjectivePosition);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC parameter: PA. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	// begin LLC motion
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "BG;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to start LLC motion. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	// check if wait for the end of moving procedure
	if (bWaitForEnd)
	{
		// wait for the end of moving procedure
		for (iMoveTimeout = 0; iMoveTimeout < MoveTimeout; iMoveTimeout += TimeStepSize)
		{
			bRetLoc = isLlcInTargetPosition(&iErrCode);
			if (bRetLoc || iErrCode != LlcErrOK) break;
			Sleep(TimeStepSize);
		}

		// check for moving results
		if (iMoveTimeout < MoveTimeout && iErrCode == LlcErrOK) 
		{
			// moving successfull
			LLCLOGMSG("Info: LLC move to target position successfull.\n", CLogger::LogLow);
		}
		else if (iMoveTimeout >= MoveTimeout)
		{
			// move timeout
			LLCLOGMSG("Error: Failed to move LLC into target position - timeout.\n", CLogger::LogLow);
			iErrCode = LlcErrTimeout;
			goto exitfun;
		}
		else
		{
			// target error
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Failed to move LLC into home position - target error. [LlcErrorNum=", iErrCode, "]\n");
			goto exitfun;
		}
	}

	iErrCode = LlcErrOK;
	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::moveLlcToPosition
*
* Description:	Move LLC to requested target position.
*				Call this function to move LLC to requested target position. The function may 
*				exit immediatelly (bWaitForEnd = false) or wait until the requested target position
*				is reached (bWaitForEnd = true). When choosing first option call 
*				"isLlcInTargetPosition" function periodically to check for the end of the moving
*				procedure.
*
* Input		 :	iPosition - target position in um
*				bWaitForEnd - false = exit function immediatelly
*							  true  = exit function on the end of moving
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = moving LLC to requested target position succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrArgRange	  = target position value not in range
*				- LlcErrSensorCom    = serial communication failure
*				- LlcErrStatus		  = LLC status shows error preventing the movement
*				- LlcErrTimeout		 = failed to reach target position before timeout
*				- error codes returned by "isLlcInTargetPosition" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When using this function a value saved in the objective position offset file is
*				NOT taken into account. The target position has to be in the following range:
*				- for LLC2:
*				-MaxPositionOffset[=-1000] =< TargetPosition =< (MaxObjectiveNumber[=1] * ObjectivePositionPitch[=35000]) + MaxPositionOffset[=1000]
*				- for LLC4/6/8:	
*				-MaxPositionOffset[=-1000] =< TargetPosition =< (MaxObjectiveNumber[=3, 5 or 7] * ObjectivePositionPitch[=38000]) + MaxPositionOffset[=1000]
*
**************************************************************************************************/
bool CLlc::moveLlcToPosition(int iPosition, bool bWaitForEnd, int *piErrCode)
{
	const char *pFunName = "CLlc::moveLlcToPosition";

	const int MoveTimeout = 2000;	// movement timeout in msec
	const int TimeStepSize = 100;	// checking current position frequency in msec

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	bool bRetLoc = false;		// return code for locally called functions
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iMoveTimeout = 0;		// moving timeout
	int iErrCode = LlcErrOK;	// LLC error code
	int iPositionLoc = 0;		// target position

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check target position range
//	if (iPosition < ((-1) * MaxPositionOffset) || iPosition > (((m_iObjNum - 1) * m_iObjPitch * 1000) + MaxPositionOffset))
	if (iPosition < ((-1) * MaxPositionOffset) || iPosition > (((int)((m_fObjPitch[m_iObjNum - 1] * 1000.0f) + 0.5f)) + MaxPositionOffset))
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%d%s", 
				   "Error: Failed to move LLC into target position - position value not in range. [RequestedPosition=", 
				   iPosition, "]\n");
		iErrCode = LlcErrArgRange;
		goto exitfun;
	}

	// calculate target position in motor "clicks"
	iPositionLoc = iPosition * ENC_PULSE_PER_MM / 1000;

	// get LLC status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		iErrCode = LlcErrSensorCom;
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		goto exitfun;
	}
	// evaluate LLC status
	iSR = atoi(pToRecv);

	if ((iSR & LLC_SR_MF_FLAG) != 0 || (iSR & LLC_SR_MO_FLAG) == 0 || 
		((iSR & LLC_SR_MS_FLAG) != 0 && ((iSR & LLC_SR_MS_FLAG) == 0 || (iSR & LLC_SR_SL_FLAG) == 0)))
	{
		// motor is off or motor failure flag is set or motion status is not cleared (i.e. in position)
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to move LLC to objective. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	// set LLC absolute position
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "PA=%d;", iPositionLoc);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC parameter: PA. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	// begin LLC motion
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "BG;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to start LLC motion. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	// check if wait for the end of moving procedure
	if (bWaitForEnd)
	{
		// wait for the end of moving procedure
		for (iMoveTimeout = 0; iMoveTimeout < MoveTimeout; iMoveTimeout += TimeStepSize)
		{
			bRetLoc = isLlcInTargetPosition(&iErrCode);
			if (bRetLoc || iErrCode != LlcErrOK) break;
			Sleep(TimeStepSize);
		}

		// check for moving results
		if (iMoveTimeout < MoveTimeout && iErrCode == LlcErrOK) 
		{
			// moving successfull
			LLCLOGMSG("Info: LLC move to target position successfull.\n", CLogger::LogLow);
		}
		else if (iMoveTimeout >= MoveTimeout)
		{
			// move timeout
			LLCLOGMSG("Error: Failed to move LLC into target position - timeout.\n", CLogger::LogLow);
			iErrCode = LlcErrTimeout;
			goto exitfun;
		}
		else
		{
			// target error
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Failed to move LLC into home position - target error. [LlcErrorNum=", iErrCode, "]\n");
			goto exitfun;
		}
	}

	iErrCode = LlcErrOK;
	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::moveLLcRelativeDistance
*
* Description:	Move LLC by requested distance in relation to its current position.
*				Call this function to move LLC by requested distance in relation to its current
*				position. The function may exit immediatelly (bWaitForEnd = false) or wait until
*				the requested target position is reached (bWaitForEnd = true). When choosing first
*				option call	"isLlcInTargetPosition" function periodically to check for the end
*				of the moving procedure.
*
* Input		 :	iDistance - distance to move in um
*				bWaitForEnd - false = exit function immediatelly
*							  true  = exit function on the end of moving
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = moving LLC to requested target position succeded
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrArgRange	  = distance to move value not in range
*				- LlcErrSensorCom    = serial communication failure
*				- LlcErrStatus		  = LLC status shows error preventing the movement
*				- LlcErrTimeout		 = failed to reach target position before timeout
*				- error codes returned by "isLlcInTargetPosition" function
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When using this function a value saved in the objective position offset file is NOT
*				taken into account. The aim of this function is to allow small LLC movements in
*				relation to current position. The distance has to be in the following range:
*				- LLC2/4/6/8:
*				-MaxPositionOffset[=-1000] =< Distance =< MaxPositionOffset[=1000]
*
**************************************************************************************************/
bool CLlc::moveLLcRelativeDistance(int iDistance, bool bWaitForEnd, int *piErrCode)
{
	const char *pFunName = "CLlc::moveLLcRelativeDistance";

	const int MoveTimeout = 2000;	// movement timeout in msec
	const int TimeStepSize = 100;	// checking current position frequency in msec

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	bool bRetLoc = false;		// return code for locally called functions
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iMoveTimeout = 0;		// moving timeout
	int iErrCode = LlcErrOK;	// LLC error code
	int iDistanceLoc = 0;		// distance to move

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check target position range
	if (iDistance < ((-1) * MaxPositionOffset) || iDistance > MaxPositionOffset)
	{
		LLCLOGMSG2(CLogger::LogLow, "%s%d%s", 
				   "Error: Failed to move LLC into target position - distance value not in range. [RequestedDistance=", 
				   iDistance, "]\n");
		iErrCode = LlcErrArgRange;
		goto exitfun;
	}

	// calculate distance to move in motor "clicks"
	iDistanceLoc = iDistance * ENC_PULSE_PER_MM / 1000;

	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	// evaluate LLC status
	iSR = atoi(pToRecv);

	if ((iSR & LLC_SR_MF_FLAG) != 0 || (iSR & LLC_SR_MO_FLAG) == 0 || 
		((iSR & LLC_SR_MS_FLAG) != 0 && ((iSR & LLC_SR_MS_FLAG) == 0 || (iSR & LLC_SR_SL_FLAG) == 0)))
	{
		// motor is off or motor failure flag is set or motion status is not cleared (i.e. in position)
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to move LLC to objective. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	// set LLC absolute position
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "PR=%d;", iDistanceLoc);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC parameter: PA. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}
	// begin LLC motion
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "BG;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to start LLC motion. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	// check if wait for the end of moving procedure
	if (bWaitForEnd)
	{
		// wait for the end of moving procedure
		for (iMoveTimeout = 0; iMoveTimeout < MoveTimeout; iMoveTimeout += TimeStepSize)
		{
			bRetLoc = isLlcInTargetPosition(&iErrCode);
			if (bRetLoc || iErrCode != LlcErrOK) break;
			Sleep(TimeStepSize);
		}

		// check for moving results
		if (iMoveTimeout < MoveTimeout && iErrCode == LlcErrOK) 
		{
			// moving successfull
			LLCLOGMSG("Info: LLC move to target position successfull.\n", CLogger::LogLow);
		}
		else if (iMoveTimeout >= MoveTimeout)
		{
			// move timeout
			LLCLOGMSG("Error: Failed to move LLC into target position - timeout.\n", CLogger::LogLow);
			iErrCode = LlcErrTimeout;
			goto exitfun;
		}
		else
		{
			// target error
			LLCLOGMSG2(CLogger::LogLow, "%s%d%s", "Failed to move LLC into home position - target error. [LlcErrorNum=", iErrCode, "]\n");
			goto exitfun;
		}
	}

	iErrCode = LlcErrOK;
	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::isLlcInTargetPosition
*
* Description:	Check if LLC reached its requested target position.
*				Call this function to check if LLC reached its home position. The function should
*				be called periodically to determine movement status after calling 
*				"moveLlcToObjective", "moveLlcToPosition" or "moveLLcRelativeDistance" function
*				(assuming that these functions were called with "bWaitForEnd = false" argument, 
*				otherwise there is no need to call this function). The function returns if the 
*				target is reached within +/- MaxInPositionRange [=5um].
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected (does not mean that it reached target 
*										position) 
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrTargetPos	  = failed to reach target position - incorrect final status
*
* Return	 :	true = success (in target postion); false = failure (not in target position yet)
*
* Notice	 :	The function is also called from within "moveLlcToObjective", "moveLlcToPosition"
*				and "moveLLcRelativeDistance" functions if "bWaitForEnd = true"	argument is
*				specified.
*
**************************************************************************************************/
bool CLlc::isLlcInTargetPosition(int *piErrCode)
{
	const char *pFunName = "CLlc::isLlcInTargetPosition";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iPA = 0;				// PA value (target position register)
	int iPX = 0;				// PX value (current position register)
	int iMS = 0;				// motion status register value
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motion status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "MS;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC motion status. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	// evaluate motion status
	iMS = atoi(pToRecv);

	if (iMS == 0)
	{
		// status shows stop of movement - check final position
		// get requested absolute position
		ZeroMemory(pToRecv, sizeof(pToRecv));
		sprintf(pToSend, "PA;");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: PA. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}

		iPA = atoi(pToRecv);

		// get encoder position
		ZeroMemory(pToRecv, sizeof(pToRecv));
		sprintf(pToSend, "PX;");
		iRetLoc = sendLlc(pToSend, pToRecv);
		if (iRetLoc != LlcErrOK)
		{
			LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: PA. [LlcErrorNum=", iRetLoc, "]\n");
			iErrCode = LlcErrSensorCom;
			goto exitfun;
		}

		// evaluate target position
		iPX = atoi(pToRecv);

		if (iPX < (iPA - MaxInPositionRange) || iPX > (iPA + MaxInPositionRange))
		{
			iErrCode = LlcErrTargetPos;
		}

		bRet = true;
	}

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::enableLlcMotor
*
* Description:	Enable LLC motor power.
*				Call this function to enable LLC motor. To make LLC operational the "homeLlc" 
*				function has to be called too.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrStatus		  = failed to enable LLC motor
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function is also called internally by "homeLlc" function.
*
**************************************************************************************************/
bool CLlc::enableLlcMotor(int *piErrCode)
{
	const char *pFunName = "CLlc::enableLlcMotor";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// enable motor power
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "MO=1;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to enable LLC motor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(500);

	// check status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iSR = atoi(pToRecv);
	// check motor enable and motor failure flags
	if ((iSR & (LLC_SR_MO_FLAG | LLC_SR_MF_FLAG)) != LLC_SR_MO_FLAG)
	{
		// motor is off or motor failure
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to enable LLC motor. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::disableLlcMotor
*
* Description:	Disable LLC motor power.
*				Call this function to disable LLC motor. This action invalidates LLC home position.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrStatus		  = failed to disable LLC motor
*
* Return	 :	true = success; false = failure
*
* Notice	 :	When enabling LLC motor again the home procedure has to be run again.
*
**************************************************************************************************/
bool CLlc::disableLlcMotor(int *piErrCode)
{
	const char *pFunName = "CLlc::disableLlcMotor";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSR = 0;				// status register value
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// stop motion - if any
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ST;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to enable LLC motor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// disable motor power
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "MO=0;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to disable LLC motor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// check status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iSR = atoi(pToRecv);
	// check motor disable and motor failure flags
	if ((iSR & (LLC_SR_MO_FLAG | LLC_SR_MF_FLAG)) != 0)
	{
		// motor is on or motor failure
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to disable LLC motor. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}

/**************************************************************************************************
*
* Function	 :	CLlc::stopLlcMotion
*
* Description:	Stop LLC motion.
*				Call this function to stop LLC motion.
*
* Input		 :	void
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrStatus		  = failed to stop LLC motion
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to stop motor motion in emergency cases.
*
**************************************************************************************************/
bool CLlc::stopLlcMotion(int *piErrCode)
{
	const char *pFunName = "CLlc::stopLlcMotion";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iErrCode = LlcErrOK;	// LLC error code
	int iMS = 0;				// motion status register value

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// stop motion - if any
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ST;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to stop LLC motion. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	Sleep(100);

	// get motion status
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "MS;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC motion status. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	// evaluate motion status
	iMS = atoi(pToRecv);

	if (iMS == 2)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to stop LLC motion. [LlcMotionStatus=", iMS, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setLlcSpeed
*
* Description:	Set LLC motor speed.
*				Call this function to set LLC motor speed parameter. The speed range limits are as
*				follow:
*				- minimum speed = 1    mm/s
*				- maximum speed = 1000 mm/s
*
* Input		 :	iLlcSpeed - LLC motor speed value
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to modify factory set default speed value
*				(default speed = 900 mm/s).
*
**************************************************************************************************/
bool CLlc::setLlcSpeed(int iLlcSpeed, int *piErrCode)
{
	const char *pFunName = "CLlc::setLlcSpeed";

	const int MaxLlcSpeed = 1000;	// maximum allowable speed value
	const int MinLlcSpeed = 1;		// minimum allowable speed value

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSpeed = iLlcSpeed;		// LLC speed
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check parameter in range
	if (iLlcSpeed < MinLlcSpeed) 
	{
		iSpeed = MinLlcSpeed;
	}
	else if (iLlcSpeed > MaxLlcSpeed)
	{
		iSpeed = MaxLlcSpeed;
	}

	// set motor speed
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SP=%d;", iSpeed * ENC_PULSE_PER_MM);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC speed. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcSpeed
*
* Description:	Get LLC motor speed.
*				Call this function to get LLC motor speed parameter. The speed range limits are as
*				follow:
*				- minimum speed = 1    mm/s
*				- maximum speed = 1000 mm/s
*
* Input		 :	void
*
* Output	 :	piLlcSpeed - LLC motor speed value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get currently set LLC speed value parameter
*				(default speed = 900 mm/s).
*
**************************************************************************************************/
bool CLlc::getLlcSpeed(int *piLlcSpeed, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcSpeed";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called functions
	int iSpeed = 0;				// LLC speed
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor speed
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SP;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC speed. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iSpeed = (int)(atoi(pToRecv) / ENC_PULSE_PER_MM);
	bRet = true;

exitfun:
	*piLlcSpeed = iSpeed;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setLlcAcceleration
*
* Description:	Set LLC motor acceleration.
*				Call this function to set LLC motor acceleration parameter. The acceleration range
*				limits are as follow:
*				- minimum acceleration = 100  mm/s/s
*				- maximum acceleration = 8500 mm/s/s
*
* Input		 :	iLlcAcceleration - LLC motor acceleration value
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to modify factory default set acceleration value
*				(default acceleration = 6500 mm/s/s).
*
**************************************************************************************************/
bool CLlc::setLlcAcceleration(int iLlcAcceleration, int *piErrCode)
{
	const char *pFunName = "CLlc::setLlcAcceleration";

	const int MaxLlcAcceleration = 8500;	// maximum allowable acceleration value
	const int MinLlcAcceleration = 100;		// minimum allowable acceleration value

	char pToRecv[64] = {'\0'};				// LLC response buffer
	char pToSend[32] = {'\0'};				// LLC command buffer
	bool bRet = false;						// function return value
	int iRetLoc = 0;						// return code for locally called funcitons
	int iAcceleration = iLlcAcceleration;	// LLC acceleration
	int iErrCode = LlcErrOK;				// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check parameter in range
	if (iLlcAcceleration < MinLlcAcceleration) 
	{
		iAcceleration = MinLlcAcceleration;
	}
	else if (iLlcAcceleration > MaxLlcAcceleration)
	{
		iAcceleration = MaxLlcAcceleration;
	}

	// set motor acceleration
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "AC=%d;", iAcceleration * ENC_PULSE_PER_MM);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC acceleration. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcAcceleration
*
* Description:	Get LLC motor acceleration.
*				Call this function to get LLC motor acceleration parameter. The acceleration range
*				limits are as follow:
*				- minimum acceleration = 100  mm/s/s
*				- maximum acceleration = 8500 mm/s/s
*
* Input		 :	void
*
* Output	 :	piLlcAcceleration - LLC motor acceleration value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get currently set LLC acceleration value parameter
*				(default acceleration = 6500 mm/s/s).
*
**************************************************************************************************/
bool CLlc::getLlcAcceleration(int *piLlcAcceleration, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcAcceleration";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iAcceleration = 0;		// LLC acceleration
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor acceleration
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "AC;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC acceleration. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iAcceleration = (int)(atoi(pToRecv) / ENC_PULSE_PER_MM);
	bRet = true;

exitfun:
	*piLlcAcceleration = iAcceleration;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setLlcDeceleration
*
* Description:	Set LLC motor deceleration.
*				Call this function to set LLC motor deceleration parameter. The deceleration range
*				limits are as follow:
*				- minimum deceleration = 100  mm/s/s
*				- maximum deceleration = 8500 mm/s/s
*
* Input		 :	iLlcDeceleration - LLC motor deceleration value
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to modify factory default set deceleration value
*				(default deceleration = 7500 mm/s/s).
*
**************************************************************************************************/
bool CLlc::setLlcDeceleration(int iLlcDeceleration, int *piErrCode)
{
	const char *pFunName = "CLlc::setLlcDeceleration";

	const int MaxLlcDeceleration = 8500;	// maximum allowable deceleration value
	const int MinLlcDeceleration = 100;		// minimum allowable deceleration value

	char pToRecv[64] = {'\0'};				// LLC response buffer
	char pToSend[32] = {'\0'};				// LLC command buffer
	bool bRet = false;						// function return value
	int iRetLoc = 0;						// return code for locally called funcitons
	int iDeceleration = iLlcDeceleration;	// LLC deceleration
	int iErrCode = LlcErrOK;				// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check parameter in range
	if (iLlcDeceleration < MinLlcDeceleration) 
	{
		iDeceleration = MinLlcDeceleration;
	}
	else if (iLlcDeceleration > MaxLlcDeceleration)
	{
		iDeceleration = MaxLlcDeceleration;
	}

	// set motor deceleration
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "DC=%d;", iDeceleration * ENC_PULSE_PER_MM);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC deceleration. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;
	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcDeceleration
*
* Description:	Get LLC motor deceleration.
*				Call this function to get LLC motor deceleration parameter. The deceleration range
*				limits are as follow:
*				- minimum deceleration = 100  mm/s/s
*				- maximum deceleration = 8500 mm/s/s
*
* Input		 :	void
*
* Output	 :	piLlcDeceleration - LLC motor deceleration value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get currently set LLC deceleration value parameter
*				(default deceleration = 7500 mm/s/s).
*
**************************************************************************************************/
bool CLlc::getLlcDeceleration(int *piLlcDeceleration, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcDeceleration";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iDeceleration = 0;		// LLC deceleration
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor deceleration
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "DC;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC deceleration. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iDeceleration = (int)(atoi(pToRecv) / ENC_PULSE_PER_MM);
	bRet = true;

exitfun:
	*piLlcDeceleration = iDeceleration;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::setLlcSmoothFactor
*
* Description:	Set LLC motor smooth factor.
*				Call this function to set LLC motor smooth factor parameter. The smooth factor
*				range limits are as follow:
*				- minimum smooth factor = 0   ms
*				- maximum smooth factor = 100 ms
*				The price for smoothing is that the total time required for completing the motion 
*				increases.
*
* Input		 :	iLlcSmoothFactor - LLC motor smooth factor value
*
* Output	 :	piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*				- LlcErrStatus		  = the smooth factor can not be set - motor is enabled
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to modify factory set default smooth factor value
*				(default smooth factor = 10 ms). This parameter can be set only when LLC motor is
*				disabled.
*
**************************************************************************************************/
bool CLlc::setLlcSmoothFactor(int iLlcSmoothFactor, int *piErrCode)
{
	const char *pFunName = "CLlc::setLlcSmoothFactor";

	const int MaxLlcSmoothFactor = 100;	// maximum allowable smooth factor value
	const int MinLlcSmoothFactor = 0;	// minimum allowable smooth factor value

	char pToRecv[64] = {'\0'};				// LLC response buffer
	char pToSend[32] = {'\0'};				// LLC command buffer
	bool bRet = false;						// function return value
	int iRetLoc = 0;						// return code for locally called funcitons
	int iSmoothFactor = iLlcSmoothFactor;	// LLC smooth factor
	int iSR = 0;							// status register
	int iErrCode = LlcErrOK;				// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// check parameter in range
	if (iLlcSmoothFactor < MinLlcSmoothFactor) 
	{
		iSmoothFactor = MinLlcSmoothFactor;
	}
	else if (iLlcSmoothFactor > MaxLlcSmoothFactor)
	{
		iSmoothFactor = MaxLlcSmoothFactor;
	}

	// check status - MO must be 0
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SR;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC parameter: SR. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iSR = atoi(pToRecv);
	// check motor disable and motor failure flags
	if ((iSR & LLC_SR_MO_FLAG) != 0)
	{
		// motor is on
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC smooth factor - MO=1. [LlcSrStatus=", iSR, "]\n");
		iErrCode = LlcErrStatus;
		goto exitfun;
	}

	// set motor smooth factor
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SF=%d;", iSmoothFactor);
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to set LLC smooth factor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	bRet = true;

exitfun:
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcSmoothFactor
*
* Description:	Get LLC motor smooth factor.
*				Call this function to get LLC motor smooth factor parameter. The smooth factor
*				range limits are as follow:
*				- minimum smooth factor = 0   ms
*				- maximum smooth factor = 100 ms
*				The price for smoothing is that the total time required for completing the motion 
*				increases.
*
* Input		 :	void
*
* Output	 :	piLlcSmoothFactor - LLC motor smooth factor value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get currently set LLC smooth factor value parameter
*				(default smooth factor = 10 ms).
*
**************************************************************************************************/
bool CLlc::getLlcSmoothFactor(int *piLlcSmoothFactor, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcSmoothFactor";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called funcitons
	int iSmoothFactor = 0;		// LLC smooth factor
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor smooth factor
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "SF;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC smooth factor. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iSmoothFactor = atoi(pToRecv);
	bRet = true;

exitfun:
	*piLlcSmoothFactor = iSmoothFactor;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcVelocity
*
* Description:	Get LLC motor velocity.
*				Call this function to get LLC current motor velocity.
*
* Input		 :	void
*
* Output	 :	pfLlcVelocity - LLC motor speed value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get current motion velocity while motor is moving.
*
**************************************************************************************************/
bool CLlc::getLlcVelocity(float *pfLlcVelocity, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcVelocity";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called functions
	int iVelocity = 0;			// LLC motor velocity
	float fVelocity = 0.0;		// LLC motor velocity
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor current velocity
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "VX;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC velocity. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iVelocity = atoi(pToRecv);
	fVelocity = (float)iVelocity / (float)ENC_PULSE_PER_MM;
	bRet = true;

exitfun:
	*pfLlcVelocity = fVelocity;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcPosition
*
* Description:	Get LLC motor position.
*				Call this function to get LLC current motor position.
*
* Input		 :	void
*
* Output	 :	pfLlcPosition - LLC motor current position value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get current motor position while it is moving or it is
*				stationary.
*
**************************************************************************************************/
bool CLlc::getLlcPosition(float *pfLlcPosition, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcPosition";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called functions
	int iPosition = 0;			// LLC motor position
	float fPosition = 0.0;		// LLC motor position
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor smooth factor
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "PX;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC position. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	iPosition = atoi(pToRecv);
	fPosition = (float)iPosition / (float)ENC_PULSE_PER_MM;
	bRet = true;

exitfun:
	*pfLlcPosition = fPosition;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcMotorCurrent
*
* Description:	Get LLC motor current.
*				Call this function to get LLC motor active and reactive currents.
*
* Input		 :	void
*
* Output	 :	pfLlcMotorCurrent - LLC motor current position value
*				piErrCode - error code returned by this function
*				- LlcErrOK		      = no problem detected
*				- LlcErrNotInitialized = LLC is not initialized yet, call "initializeLlc" function
*										first
*				- LlcErrSensorCom     = serial communication failure
*
* Return	 :	true = success; false = failure
*
* Notice	 :	This function may be used to get motor current while it is moving or it is
*				stationary.
*
**************************************************************************************************/
bool CLlc::getLlcMotorCurrent(float *pfLlcMotorCurrent, int *piErrCode)
{
	const char *pFunName = "CLlc::getLlcMotorCurrent";

	char pToRecv[64] = {'\0'};	// LLC response buffer
	char pToSend[32] = {'\0'};	// LLC command buffer
	bool bRet = false;			// function return value
	int iRetLoc = 0;			// return code for locally called functions
	float fIdCurrent = 0.0;		// active current
	float fIqCurrent = 0.0;		// reactive current
	int iErrCode = LlcErrOK;	// LLC error code

	LLCENTERFUN(pFunName);

	// check if LLC is initialized
	if (m_bIsLlcInit == false || m_iObjNum == 0)
	{
		iErrCode = LlcErrNotInitialized;
		goto exitfun;
	}

	// get motor smooth factor
	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "ID;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC active current. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	fIdCurrent = fabs((float)atof(pToRecv));

	ZeroMemory(pToRecv, sizeof(pToRecv));
	sprintf(pToSend, "IQ;");
	iRetLoc = sendLlc(pToSend, pToRecv);
	if (iRetLoc != LlcErrOK)
	{
		LLCLOGMSG2(CLogger::LogAll, "%s%d%s", "Error: Failed to get LLC reactive current. [LlcErrorNum=", iRetLoc, "]\n");
		iErrCode = LlcErrSensorCom;
		goto exitfun;
	}

	fIqCurrent = fabs((float)atof(pToRecv));

	bRet = true;

exitfun:
	*pfLlcMotorCurrent = fIdCurrent + fIqCurrent;
	*piErrCode = iErrCode;

	LLCEXITFUN(pFunName);
	return bRet;
}

/**************************************************************************************************
*
* Function	 :	CLlc::saveToLog
*
* Description:	Save message in the log file.
*				Call this function to save message in the log file. This function is used only by
*				CLlc class.
*
* Input		 :	pLogMsg - pointer to message to be saved
*				iLogLevel - logging level (1 to 3)
*
* Output	 :	void
*
* Return	 :	void
*
* Notice	 :	The caller is responsible for allocating buffer for the message. 
*				Logging levels: 1 = save always.
*				e.g. saveToLog("Info: new baudrate=9600;\n", 1)
*
**************************************************************************************************/
void CLlc::saveToLog(const char *pLogMsg, CLogger::LogLevels iLogLevel)
{
	if (m_pLogger != NULL && pLogMsg != NULL)
	{
		m_pLogger->saveToLog(pLogMsg, iLogLevel);
	}
}

/**************************************************************************************************
*
* Function	 :	CLlc::saveToLog2
*
* Description:	Save message in the log file.
*				Call this function to save message in the log file. This function is used only by
*				CLlc class. This is another flavor of logging with variable-length argument list.
*
* Input		 :	iLogLevel - logging level (1 to 3)
*				pFormat - message formatting string
*				... - arguments according to formatting string
*
* Output	 :	void
*
* Return	 :	void
*
* Notice	 :	The function is allocating buffer for the message. 
*				Logging levels: 1 = save always.
*				e.g. saveToLog2(1, "%s%d%s", "Info: new baudrate=", 9600, " ;\n")
*
**************************************************************************************************/
void CLlc::saveToLog2(CLogger::LogLevels iLogLevel, char *pFormat, ...)
{
    va_list args;
    int iLen = 0;
    char *pLogMsg = NULL;

	if (m_pLogger != NULL && pFormat != NULL)
	{
		va_start(args, pFormat);
	    
		iLen = _vscprintf(pFormat, args) + 1;
		pLogMsg = (char*)malloc(iLen * sizeof(char));

		if (pLogMsg != NULL)
		{
			vsprintf(pLogMsg, pFormat, args);
			m_pLogger->saveToLog(pLogMsg, iLogLevel);
			free(pLogMsg);
		}
	}
}


/**************************************************************************************************
*
* Function	 :	CLlc::getLlcMotorCurrent
*
* Description:	Get LLC forcer serial number.
*				Call this function to get unique LLC forcer serial number.
*
* Input		 :	void
*
* Output	 :	see return value
*
* Return	 :	int - forcer serial number
*
* Notice	 :	This number is unique for each LLC/MFC pair system.
*
**************************************************************************************************/
int CLlc::getLlcForcerSerNum()
{
	return atoi(m_cForcerSerNum);
}
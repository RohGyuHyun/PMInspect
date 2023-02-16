/******************************************************************************
 *	
 *	(C) Copyright WDI 2012
 *	
 ******************************************************************************
 *
 *	FILE:		atf_lib_exp.h
 *
 *	PROJECT:	AFT Sensors
 *
 *	SUBPROJECT:	.
 *
 *	Description: Functions exported from the ATF DLL library
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          19/04/2012 Chris O.         Initial version 
 *                                      
 *****************************************************************************/

#pragma once

#include "atf_def.h"
#include <stdio.h>

#ifdef __cplusplus
	extern "C" {
#endif

#define DllExport __declspec(dllexport)

// ---------- ATF error info functions

// returns text name of most recent error
DllExport const char* ATF_GetErrorInfo(int iErr);

// ---------- ATF serial port functions

// establishing connection. Return 0 if OK, or something else in case of error
DllExport int ATF_OpenConnection(char *port, int speed);
DllExport void ATF_CloseConnection(void);
// to select which of the multiple com objects are used. New index is assigned
// with every ATF_OpenConnection and invalidated with ATF_CloseConnection.
// Index is a thread local. There is no need to use those functions if a thread is
// dedicated to communicate with a single com object. If thread is talking through 
// multiple com objects (multiple sensors) then ATF_SetComIdx needs to be done
// every time ccom object changes
DllExport int ATF_GetComIdx();
DllExport int ATF_SetComIdx(int idx);

// serial Com port baudrate
DllExport int ATF_ChangeCommBaudrate(DWORD dwBaudRate);
DllExport int ATF_GetCommBaudrate(DWORD *dwBaudrate);
DllExport BOOL ATF_isSerialConnection(void);

// read current sensor baudrate setting
DllExport int ATF_ReadBaud(int *pBR);
// writes new baudrate setting (need to follow up with change of port setting if communication is to continue) 
DllExport int ATF_WriteBaud(int br);

// ---------- ATF ping and recovery functions

// attempts to recover in case of communication loss
// verify what is running. Returns:
// 0 - no communnication could be established
// 1 - application found
// 2 - bootloader found, it was restarted back to application
DllExport int ATF_RecoverSensor(void);
// check if "A" is returned which indicate that software is working
DllExport int ATF_PingAck(void);
// resets sensor
DllExport int ATF_Reset(void);

// ---------- ATF low level communication functions

// direct access to read and write
DllExport int ATF_ReadArray(void *to, int bid, int pid, EppDataType tag, int num, int offset);
DllExport int ATF_ReadArray2(void **to, int bid, int pid, EppDataType tag, int num, int offset);
DllExport int ATF_WriteArray(void *from, int bid, int pid, EppDataType tag, int num, int offset);

// ---------- ATF part identification functions

// returns text corresponding to sensor id
DllExport const char* ATF_PartInfo(void);

DllExport int ATF_ReadSerialNumber(unsigned int *SensorSN);
DllExport int ATF_ReadFirmwareVer(int *ver);

// ---------- ATF one time initialization functions

// raed magnification assigned to particular objective
DllExport int ATF_ReadMagnification(int obj, short *pMag);
// assigns magnification to the objective. 
// This will cause to reinitialize all the parameters associated with objective
DllExport int ATF_WriteMagnification(int obj, short uMag);

// ---------- ATF objective configuration functions

// assign current z position to be best in focus
DllExport int ATF_Make0(void);

// save all parameters into the eprom memory
DllExport int ATF_SaveAll(void);
// read and set current objective number
DllExport int ATF_ReadObjNum(int *pObj);
DllExport int ATF_WriteObjNum(int iObj);

// read/write peak offset
DllExport int ATF_ReadPeakOffset (int obj, enum PeakOffsetWhich pow, int* peakArray);
DllExport int ATF_WritePeakOffset(int obj, enum PeakOffsetWhich pow, int* peakArray);

// read acceleration in mm/ss
DllExport int ATF_ReadAccel(int obj, float *pAccel);
DllExport int ATF_WriteAccel(int obj, float fAccel);
// read speed in mm/s
DllExport int ATF_ReadSpeed(int obj, float *pSpeed);
DllExport int ATF_WriteSpeed(int obj, float fSpeed);
// read proportional gain factor
DllExport int ATF_ReadP(int obj, float *pP);
DllExport int ATF_WriteP(int obj, float fP);

// reads/writes row delay parameter for specified objective and specified window
DllExport int ATF_ReadWindowRowDelay  (int obj, WindowEnm which, short* prd);
DllExport int ATF_WriteWindowRowDelay (int obj, WindowEnm which, short rd);

// read number of steps correponding to 1 mm of motion
DllExport int ATF_ReadStepPerMmConversion(unsigned short *puStepMm);
// write number of steps correponding to 1 mm of motion
DllExport int ATF_WriteStepPerMmConversion(unsigned short uStepMm);

// read number of microsteps assigned per each stepper motor step
DllExport int ATF_ReadMicrostep(unsigned short *puUstep);
// change number os microsteps 
DllExport int ATF_WriteMicrostep(unsigned short uUstep);

// read number of micrometers corresponding to unit on sensor output
DllExport int ATF_ReadSlopeUmPerOut(int obj, float *pfumout);
DllExport int ATF_WriteSlopeUmPerOut(int obj, float fumout);

// DUV jump dist
DllExport int ATF_WriteDuvJump(int obj, short sUStep);
DllExport int ATF_ReadDuvJump(int obj, short *pUStep);

// write number of units as reported by sensor position defining infocus range
DllExport int ATF_WriteInfocusRange(int obj, int iRange);
DllExport int ATF_ReadInfocusRange(int obj, int *pRange);

// prevent sensor from changing current window mode to the different one
DllExport int ATF_DisableAutoWindowTransit(void) ;
// alows automatic window selection. TYpically: near mode (7 segments) at focus and far mode if distance is
// greater then linear range
DllExport int ATF_EnableAutoWindowTransit(void);

// read number of imager pixels assigned to represent linear range
DllExport int ATF_ReadLinearRange(int obj, int *pLinearRange);
DllExport int ATF_WriteLinearRange(int obj, int iLinearRange);

// read and write surface
DllExport int ATF_ReadSurface(short *piSurf);
DllExport int ATF_WriteSurface(short iSurf);

// ---------- ATF auto focus tracking setup functions

// sets upper and lower limits (in abs position units) for motion executed during AF cycle.
// if adaptive_lim!=0 then upper and lower limits are adjusted up or down around 0 position if number
// of samples in a row falls within InFocusRange
DllExport int ATF_WriteMotionLimits(int upper_limit, int lower_limit, int adaptive_lim);

// sets mode in which laser is going to be turned off once focus is reached. 
// Next AF command is going to enable laser
DllExport int ATF_EnableAutoOff(void);
DllExport int ATF_DisableAutoOff(void);

// set far mode
DllExport int ATF_ForceFarMode(void);
// set near mode
DllExport int ATF_ForceNearMode(void);
// set center of the imager
DllExport int ATF_CenterFarWindow(void);
DllExport int ATF_ClearFarWindowCentering(void);

// ---------- ATF commands and tracking functions

// executes motion of move microsteps
DllExport int ATF_MoveZ(int move);
// executes motion of move micrometers
DllExport int ATF_MoveZum(int iMoveUm);
// reads most recent motion distance
DllExport int ATF_ReadLastMoveZ(int *pzMove);
// executes stop motor motion
DllExport int ATF_StopZMotor(void);
// executes motion of move microsteps followed by clear abs position
DllExport int ATF_MoveZ_ClearAbs(int move);

// reads position in microstepps updated by the sensor with every z motion
DllExport int ATF_ReadAbsZPos(int *absZ);
DllExport int ATF_WriteAbsZPos(int absZ);

// stops AF tracking
DllExport int ATF_AfStop(void);
// starts AF tracking
DllExport int ATF_AFTrack(void);
// start AF tracking first, once at focus continue on AOI tracking
DllExport int ATF_AFAoiTrack(void);
// start Video tracking
DllExport int ATF_AFVideoTrack(void);
// execute focus and disables, if DUV jump is defined will be taken
DllExport int ATF_AfDuvJump(void);
// start AOI tracking
DllExport int ATF_AFAoiOnlyTrack(void);

DllExport int ATF_DPAF();

// ---------- ATF status and results functions

// reads sensor position, typically in range +/- 512
DllExport int ATF_ReadPosition(float *pfpos);

// read position packed into single short along with some extra flags
DllExport int ATF_ReadPositionPacked(RsData *prs);

// reads hardware status of HwStatusFlagsEnm type (see description above)
DllExport int ATF_ReadHwStat(int *HwStat);

// read status flags of StatusFlagsEnm type (see description above)
DllExport int ATF_ReadStatus(short *pstatus);

// reads scanline.
// mode: one of the ...WINIDOW defined above
// h_scanline: points to a buffer of MAX_SCANLINE_LENGTH where horizontal scanline is to be copied to
// v_scanline: points to a buffer of MAX_SCANLINE_LENGTH where vertical scanline is to be copied to
// h_len: number of elements of h_scanline
// v_len: returns number of elements of v_scanline
// width: returns width of the v_scanline peak as detected by sensor
DllExport int ATF_ReadScanline(unsigned short *mode, 
							   unsigned short *h_scanline, unsigned short *v_scanline, 
							   unsigned short *h_len, unsigned short *v_len, 
							   unsigned short *width);

// read individual dot.line segment position
DllExport int ATF_Read7DotPosition(short *p7dots, int iLinearRange);

// read value reported on analog output
DllExport int ATF_ReadAnalogOut(short *pAnalog);

// get sensor error code
DllExport int ATF_ReadErrorCode(enum AtfCodesEnm *pcode);

// ---------- ATF laser control functions

// turns laser ON
DllExport int ATF_EnableLaser(void);
// turns laser OFF
DllExport int ATF_DisableLaser(void);
// enable automated laser power control
DllExport int ATF_LaserTrackOn(void);
// enable manual laser power control
DllExport int ATF_LaserTrackOff(void);
// read current laser power. Value from 0-1023.
// ATF5
// dotIdx:	0-2 - write laser power to current window
//          4 - far mode laser power (3,5 unused) 
//          6-8 - near mode left window laser power
//          9-11 - near mode center window laser power (10 is central dot)
//          12-14 - near mode right window laser power 
// ATF6
// dotIdx:	0 - write laser power to current window (1,2 - unused)
//          4 - far mode laser power (3,5 unused) 
//			7-13 - near mode 7 dot laser power (typically they are the same, 10 is central dot)
//          6,14 - unused
DllExport int ATF_ReadLaserPower(unsigned short *plaser, int iByteNumToRead, int dotIdx);
// set current laser power
DllExport int ATF_WriteLaserPower(unsigned short laser, int iByteNumToRead, int dotIdx);


// equivalent to enabling sync signal on input DIO
DllExport int ATF_EnableSync(void);
// equivalent to disabling sync signal on input DIO
DllExport int ATF_DisableSync(void);

// ---------- ATF LED control functions

// there are up to 3 led (0,1,2)
DllExport int ATF_ReadLedCurrent(int channel, unsigned short *pCurrent);
DllExport int ATF_WriteLedCurrent(int channel, unsigned short uCurrent);
DllExport int ATF_ReadLedPwm(int channel, unsigned short *pPWM);
DllExport int ATF_WriteLedPwm(int channel, unsigned short uPWM);
DllExport int ATF_ReadLedPwmStartupVal(int iChannel, unsigned short *puPWM);
DllExport int ATF_WriteLedPwmStartupVal(int iChannel, unsigned short uPWM);

// ---------- ATF MFC related functions

DllExport int ATF_ReadMfcConfig(int *mfc_config);
// read 5 bytes used to configure motor.
DllExport int ATF_ReadMfcMotorParm(unsigned char *motor_parm);
DllExport int ATF_WriteMfcMotorParm(unsigned char *motor_parm);

// ---------- ATF CM functions
DllExport BOOL ATF_IsCmAvailable();
DllExport int ATF_ReadCmGlassThickness (int* piThicknessUstep);
DllExport int ATF_WriteCmGlassThickness (int iThicknessUstep);
DllExport int ATF_ReadCmRetarder (float *pfRetardation_percent, int iSurface);
DllExport int ATF_WriteCmRetarder (float fRetardation_percent, int iSurface);
DllExport int ATF_ReadCmRetarderAutoDetectConfigOptions (BOOL *pbDoAutoDetectOnMake0, BOOL *pbDoWaitAutoDetectToAf);
DllExport int ATF_WriteCmRetarderAutoDetectConfigOptions(BOOL bDoAutoDetectOnMake0,  BOOL  bDoWaitAutoDetectToAf);
DllExport CmAutoDetectEnum ATF_ReadCmRetarderAutoDetectStatus();
DllExport int ATF_ScheduleCmRetarderAutoDetectForAF();
DllExport int ATF_StartCmRetarderAutoDetect();
DllExport int ATF_StopCmRetarderAutoDetect();

// ---------- ATF logger functions

DllExport BOOL ATF_openLogFile(const char *pLogFilePath, const char *pMode);
DllExport BOOL ATF_closeLogFile(void);
DllExport void ATF_setLogLevel(int iLogLevel);
DllExport FILE *ATF_getLogFileHandle(void);
DllExport void ATF_saveToLog(char *pLogMsg, int iLogLevel);
DllExport void ATF_saveToLog2( int iLogLevel, char *pFormat, ...);


// ---------- ATF LLC related functions

DllExport BOOL ATF_initializeLlc(char *pOffsetFileDir, int *piErrCode);
DllExport int ATF_sendLlc(char *toSend, char *toRecv);
DllExport BOOL ATF_getLlcStatus(int *piErrCode, const char **ppErrMsg, int *piStatReg, int *piMtrFailureReg);
DllExport BOOL ATF_recoverLlc(int *piErrCode);
DllExport BOOL ATF_recoverLlcSensorConnection(int *piErrCode);
DllExport BOOL ATF_recoverLlcHdwr(int *piErrCode);
DllExport BOOL ATF_resetLlcController(int *piErrCode);
DllExport BOOL ATF_homeLlc(BOOL bWaitForEnd, int *piErrCode);
DllExport BOOL ATF_isLlcInHomePos(int *piErrCode);
DllExport void ATF_setObjectiveNum(int iObjNum);
DllExport int ATF_getObjectiveNum(void);
DllExport int ATF_getLlcForcerSerNum(void);
DllExport BOOL ATF_saveLlcObjectivePositionOffset(int iObjectiveNumber, int iOffset);
DllExport BOOL ATF_getLlcObjectivePositionOffset(int iObjectiveNumber, int *piPositionOffset);
DllExport BOOL ATF_moveLlcToObjective(int iObjectiveNumber, BOOL bWaitForEnd, int *piErrCode);
DllExport BOOL ATF_moveLlcToObjective2(int iObjectiveNumber, BOOL bWaitForEnd, int *piErrCode);
DllExport BOOL ATF_moveLlcToPosition(int iPosition, BOOL bWaitForEnd, int *piErrCode);
DllExport BOOL ATF_moveLLcRelativeDistance(int iDistance, BOOL bWaitForEnd, int *piErrCode);
DllExport BOOL ATF_isLlcInTargetPosition(int *piErrCode);
DllExport BOOL ATF_enableLlcMotor(int *piErrCode);
DllExport BOOL ATF_disableLlcMotor(int *piErrCode);
DllExport BOOL ATF_stopLlcMotion(int *piErrCode);
DllExport BOOL ATF_setLlcSpeed(int iLlcSpeed, int *piErrCode);
DllExport BOOL ATF_getLlcSpeed(int *piLlcSpeed, int *piErrCode);
DllExport BOOL ATF_setLlcAcceleration(int iLlcAcceleration, int *piErrCode);
DllExport BOOL ATF_getLlcAcceleration(int *piLlcAcceleration, int *piErrCode);
DllExport BOOL ATF_setLlcDeceleration(int iLlcDeceleration, int *piErrCode);
DllExport BOOL ATF_getLlcDeceleration(int *piLlcDeceleration, int *piErrCode);
DllExport BOOL ATF_setLlcSmoothFactor(int iLlcSmoothFactor, int *piErrCode);
DllExport BOOL ATF_getLlcSmoothFactor(int *piLlcSmoothFactor, int *piErrCode);
DllExport BOOL ATF_getLlcVelocity(float *pfLlcVelocity, int *piErrCode);
DllExport BOOL ATF_getLlcPosition(float *pfLlcPosition, int *piErrCode);
DllExport BOOL ATF_getLlcMotorCurrent(float *pfLlcMotorCurrent, int *piErrCode);
DllExport int ATF_CommToLlc(char* to_send, char* to_read);

// ---------- ATF homing Z functions
DllExport int ATF_ReadHomingZ_Parameters(int *pHomingZ_Parameters);
DllExport int ATF_WriteHomingZ_Parameters(int *pHomingZ_Parameters);
DllExport int ATF_RunHomingZ(int *pHomingZ_Parameters);
DllExport int ATF_IsInHomePosition(BOOL *pbInHome);

// ---------- ATF jogging Z functions
DllExport int ATF_StartJoggingZ(int iSpeed);

DllExport int ATF_ReadStrobePulseTime(int iLed, int *pStrobeTime); 
DllExport int ATF_WriteStrobePulseTime(int iLed, int iStrobeTime); 
DllExport int ATF_ReadStrobeIntervalTime(int *pStrobeIntervalUs); 
DllExport int ATF_WriteStrobeIntervalTime(int iStrobeIntervalUs); 
DllExport int ATF_Strobe(int iLed, int iNum, int iMaxError);
DllExport int ATF_StrobeCount(int *pNumStb, int *pNumStbRequested, bool *pbStrobeActive);
DllExport int ATF_ReadConfigExt (int iObj, int *pcfg);
DllExport int ATF_SetConfigExt (int iObj, int mask, int cfg);
DllExport int ATF_ReadMoveObjNum (int *pObj, int *pMoveObj);
DllExport int ATF_WriteMoveObjNum(int iObj, int iMoveObj);
DllExport int ATF_ReadAbsZPosStage (enum StageSelectorEnum ess, int* pAbsZ, int *pMoveObj) ;
DllExport int ATF_ReadAoiFollowUp (int iObj, int *pObjFollowUp);
DllExport int ATF_WriteAoiFollowUp (int iObj, int iObjFollowUp);

//sensor measurement
DllExport int ATF_EnableMeasurement(bool bEnable);
DllExport int ATF_WriteMeasInterval(unsigned short iMeasIntervalUS=800); 
DllExport int ATF_ReadMeasInterval(unsigned short* iMeasInterval);
DllExport int ATF_ReadMeasurementStatus(short *piStatus, short *piDataPoints);
DllExport int ATF_ReadMeasurementBuffer(double *pData, short *piDatalen);
DllExport int ATF_ReadCurrentDataPoint(double *pdData);
DllExport int ATF_ReadMeasTimeArray(double* pTimeStampArray);

DllExport int ATF_EnableMpaf (bool bEnable);
DllExport int ATF_MpafStable (bool bEnable);

// OOA --------------------------------------------------------------------------------------
DllExport bool ATF_OOA_Init ();
DllExport void ATF_OOA_Close ();
DllExport bool ATF_OOA_SetAccelerationMmSS(float fAccelerationMmSS);
DllExport bool ATF_OOA_GetAccelerationMmSS(float *fAccelerationMmSS);
DllExport bool ATF_OOA_SetVelocityMmS(float fVelocityMmS);
DllExport bool ATF_OOA_GetVelocityMmS(float *fVelocityMmS);
DllExport bool ATF_OOA_DoHome(bool bWait);
DllExport bool ATF_OOA_DoHomeStateMachineAsync(int *pOoaStatus);
DllExport bool ATF_OOA_isOK(void);
DllExport bool ATF_OOA_GetStatus(int *pOoaStatus);
DllExport bool ATF_OOA_MoveUm(float fMoveUm, bool bWait);
DllExport bool ATF_OOA_MoveToPositionUm(float fMoveUm, bool bWait);
DllExport bool ATF_OOA_IsMoveDoneAsync(int *pOoaStatus);
DllExport bool ATF_OOA_GetPositionUm(float *pfPositionUm);
DllExport bool ATF_OOA_GetPositionUStep(int *piPositionUStep);

// OOA --------------------------------------------------------------------------------------
DllExport int ATF_WindowModeSet(int obj, WindowEnm windowMode);
DllExport int ATF_WindowModeGet(int obj, WindowEnm *pWindowMode);
DllExport int ATF_ProcessSet(int obj, ProcessEnm processEnm);
DllExport int ATF_ProcessGet(int obj, ProcessEnm *pProcessEnm);

#ifdef __cplusplus
	}
#endif
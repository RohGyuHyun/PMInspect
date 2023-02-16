/******************************************************************************
 *	
 *	(C) Copyright WDI 2011
 *	
 ******************************************************************************
 *
 *	FILE:		ui_cmd.h
 *
 *	PROJECT:	AFT Sensor
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

#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#include <windows.h>
#include "common_AF.h"

enum SensorType
{
	// keep them in the order of advancing functionality
	ATF2 = 200,	// ATF2 LV
	ATF4 = 400, // ATF4
	ATFX = 'X',	// what's the hell is this one ? :-) = unknown
	ATF5x= 500,	// ATF5 before merger
	ATF5y= 550,	// ATF5 single window
	ATF5 = 570,	// ATF5 after merger
	ATF6 = 600,	// ATF6 before and after merger
	ATF7 = 700,	// ATF7 
};

extern int gs_sensor_type;
extern int gs_tiny_avail;
extern int gs_ABC_avail;
extern char logMsg[512];

// logging macros
#define ENTERFUN(funName) atf_saveToLog2(CLogger::LogLow, "%s%s", funName, " - entered\n");
#define EXITFUN(funName) atf_saveToLog2(CLogger::LogLow, "%s%s", funName, " - exited\n");
#define LOGMSG(pLogMsg, LogLev) atf_saveToLog(pLogMsg, LogLev);
#define LOGMSG2(LogLev, format, ...) atf_saveToLog2(LogLev, format, __VA_ARGS__);

// ---------------------------------------------------------------------------------
// histogram extra processing record
// 
#define FR_ExtraDataId			5
#define HistProcExtraRecId      0xf2c4
#define HistProcExtraRecLen     64
#define NUM_DOTS				3

typedef struct _HistProcExtra
{
    u_short RecId;                      // unique id identifying record
    u_short RecLen;                     // record length, serving as a record version as well
    u_short iCogRef[NUM_DOTS];          // beginning of hist offset
    u_short iHistLenLeft,iHistLenRight; // length of left and right combined histograms
    u_short iCntLeft,iCntRight;         // total pixel sum left and right
    u_short iAllSumLeft[NUM_DOTS];      // all pixel sum of the left 3 histograms
    u_short iAllSumRight[NUM_DOTS];     // all pixel sum of the right 3 histograms
    u_short iCogLeft[NUM_DOTS];         // pixel sum of the left 3 histograms
    u_short iCogRight[NUM_DOTS];        // pixel sum of the right 3 histograms
      short pfRelRatio;                 // left vs. right ratio
    u_short iWindow, iBlind;            // size of the window including blind zone
	u_short iExtraData[8];				// extra processing data, may change
}
HistProcExtraDef, *PHistProcExtraDef;

enum StatusFlagsHigherBitsEnm 
{
	// higher 16 bits
	MsSwDetectSyncMode      = 0x00010000,   // exec: software sync detected
	MsActivateLeds          = 0x00020000,   // param, exec    
	MsNoParamChanges        = 0x00040000,   // exec: changing parameters is temporarly disabled
	MsEnableItrFpga         = 0x00080000,   // param: enable itr, exec: isr enabled

	MsImageAcqMode          = 0x00100000,   // exec: image acquisition mode
	MsEngSecureMode         = 0x00200000,   // exec: allows access to secure commands    
	MsSurfValidationMode    = 0x00400000,   // exec: executing surface validation
	MsProfilometer          = 0x00800000,   // exec: executing profilometer mode

	Ms7DotNoOutliers        = 0x00000000,   // exec: select peaks w/o outliers
	Ms7DotLeastMotion       = 0x01000000,   // exec: select peaks that pass screening
	Ms7DotAvgAll            = 0x02000000,   // exec: select average out of 3 dots        
	Ms7DotMedian            = 0x03000000,   // exec: select median out of 3 dots
	Ms7Dot1                 = 0x04000000,   // exec: force dot 1
	Ms7Dot2                 = 0x05000000,   // exec: force dot 2
	Ms7Dot3                 = 0x06000000,   // exec: force dot 3
	Ms7DotCentral           = 0x07000000,   // exec: force dot 4
	Ms7Dot5                 = 0x08000000,   // exec: force dot 5
	Ms7Dot6                 = 0x09000000,   // exec: force dot 6
	Ms7Dot7                 = 0x0a000000,   // exec: force dot 7
	Ms7DotStable            = 0x0b000000,   // exec: use dots that proved to be most stable    
	Ms7DotHigh              = 0x0c000000,   // exec: use more reflective dots preferentially
	Ms7DotRecent            = 0x0d000000,   // exec: use mst recent dot    
	Ms7DotLeft              = 0x0e000000,   // exec: use all left dots (1,2,3)
	Ms7DotRight             = 0x0f000000,   // exec: use all right dots (5,6,7)    
	Ms7DotSelectValidBits   = 0x0f000000,   // exec: 4 bits selects dot combination mode    

    MsSoftLaserTracking     = 0x10000000,   // exec: laser tracked by software
	MsWindowAutoTransit     = 0x20000000,   // exec: continous transition
	MsFixedStepTracking     = 0x40000000,   // exec: limit tracking to small steps, that can be executed in one cycle 
};

enum LedStatusFlgsEnm
{
	LedStatus_Shorted	= 0x0001,
	LedStatus_HwErr_Fl	= 0x0001,
	LedStatus_OK		= 0x0002,
	LedStatus_Calib_Fl	= 0x0002,
	LedStatus_Open		= 0x0004,
	LedStatus_Intrcp_Fl	= 0x0004,
	LedStatus_Defective	= 0x0008,
	LedStatus_Slope_Fl	= 0x0008,
	LedStatus_On		= 0x0010,
	LedStatus_Unused	= 0x0020,
	LedStatus_Lamp		= 0x0040,
	LedStatus_HwError	= 0x0080,
	LedStatus_OK_Fl		= 0x0080,
	LedStatus_Valid		= 0x8000
};

// scanline header
#define ScanlineHeaderId    0xfead

#define FR_MaxNumEnm      6         // number of bits used by FR_....Enm
#define FR_HProjId        0
#define FR_VProjId        1         // this is measuring scanline
#define FR_AcquisitionId  2         // this contains elements below
#define FR_LHistProjId    3         // histograms are reported in stead of projections
#define FR_RHistProjId    4
#define FR_HwCogId        5         // cog hw position

// elements found in acquisition array
#define FR_AcqElem_LaserId				0   // elements found in acquisition array
#define FR_AcqElem_MaxHPeakIdxId		1   // where max V peak is found (in pixels from the beginning of scanline)
#define FR_AcqElem_MaxVPeakIdxId		2   // where max H peak is found (...)
#define FR_AcqElem_Laser0Id5x           0   // relative laser power from 0-1023 for peak 0
#define FR_AcqElem_Laser1Id5x           1   // ・ 1
#define FR_AcqElem_Laser2Id5x           2   // ・ 2
#define FR_AcqElem_IntegrationTimeId	3
#define FR_AcqElem_4RowFloorId			4
#define FR_AcqElem_MapPeakDotId			5
#define FR_AcqElem_WidthHPeakId			6   // width for V peak (in pixels * 10)
#define FR_AcqElem_WidthVPeakId			7   // width for H peak (in pixels * 10)
#define FR_AcqElem_MaxPeakDot0Id5x      5   // max peak as detected by HW for peak 0 (0-1023)
#define FR_AcqElem_MaxPeakDot1Id5x      6
#define FR_AcqElem_MaxPeakDot2Id5x      7
#define FR_AcqElem_FrameId				8

// FR_AcqElem_FrameId bits are partition here
#define WINSWITCH_DOTWINDOW_VALID   0x000f    // where dotwindow window information is supposed to be found
#define WINSWITCH_WINDOWVALID		0x000f    // where window information is supposed to be found
#define WINSWITCH_WINDOW_VALID      0x00f0    // where window information is supposed to be found
#define SURFRACE_VALID              0x0100    // indicate surface validation  
#define IMAGE_VALID                 0x0200    // indicate image presence
#define WINSWITCH_VALID             0x1000    // this frame is deemed to be valid (0 is invalid)
#define WINSWITCH_SYNC              0x2000    // indicates that frame had been resulting from dynamic switching
#define WINSWITCH_CASERR            0x4000    // cas error reported
#define WINSWITCH_ISRERR            0x8000    // isr error reported

#define FR_AcqElem_LapMaxId				 9
#define FR_AcqElem_LapMinId             10
#define FR_AcqElem_ZAbsPosition         11    // absolute position recorded at the time frame has been captured
#define FR_AcqElem_PeakArea0Id          12	// Peak Area (Width H *Width V) for peak 0
#define FR_AcqElem_PeakArea1Id          13  // ・ 1
#define FR_AcqElem_PeakArea2Id          14  // ・ 2

typedef struct _FpgaScanlineHeader 
{
	u_short   id;                       // identifier (ScanlineHeaderId)
	u_short   num;                      // table size (=FR_MaxNumEnm)
	u_short   beg [FR_MaxNumEnm];       // beggining index of where block is placed in data
	u_short   len [FR_MaxNumEnm];       // number of elements of data used up by block
	u_short   data[1];                  // block of data follows immediately here 
} FpgaScanlineHeader,*PFpgaScanlineHeader;

int ReadPosition(MeasureDataType* pdata);
int ReadStatus (int *pStatus); 

int WriteCvdAfParm (int numset, int option);

int ReadExecFlag (int *flags);

#define GetCurrWindowName(w) ((w)==eWM_Far ? "Far":((w)==eWM_Near ? "Near7":((w)==eWM_NearSingle ? "Near":((w)==eWM_Sv ? "CM":"??"))))
int ReadAcqBlock5x (EnumWindowMode *pmode, u_short* laser);

int ReadScanline (EnumWindowMode* eMode, u_short* frame, u_short* h_scanline, u_short* v_scanline, u_short* h_len, u_short* v_len, u_short* area);
int ReadHistograms (u_short** l_hist, u_short* lh_len, u_short** r_hist, u_short* rh_len, PHistProcExtraDef* ppHistExtra);

int ReadObjNum (MeasureDataType* pdata);
int WriteObjNum(MeasureDataType* pdata);
int WriteSurface(short iSurface);

int WriteOffset (MeasureDataType* pdata); 
int ReadOffset (MeasureDataType* pdata) ;

#define SURFACEOFFSETS_PER_OBJ	96
unsigned short *ReadAllObjSurfaceOffsets (int obj);
int WriteAllObjSurfaceOffsets (unsigned short *pOffsets, int obj);

int WriteInfoc (MeasureDataType* pdata) ;
int ReadInfoc (MeasureDataType* pdata) ;

int WriteUmout (double Umout, int obj) ;
int ReadUmout (double* Umout, int obj) ;

int WriteP (double P, int obj) ;
int ReadP (double* P, int obj) ;

int WriteAccel (int Accel, int obj);
int ReadAccel (int* Accel, int obj);

int WriteSpeed (float Speed, int obj);
int ReadSpeed (float* Speed, int obj);

int WriteZDelta (int Delta, int obj);
int ReadZDelta (int* pDelta, int obj);

int WriteZMaxRate (int iRate, int obj);
int ReadZMaxRate (int* pRate, int obj);

int WriteCvdAfParm (int numset, int option);

int ReadCameraGains(unsigned char *pGains);
int WriteCameraGains(unsigned char *pGains);

int ReadWindowObjMode (int obj, short* mode) ;
int WriteWindowObjMode (int obj, short mode) ;

int ReadWindowCurrMode (EnumWindowMode *pMode);
int WriteWindowCurrMode (EnumWindowMode mode) ;
int ReadCurrentWindow(EnumWindowMode *pWinMode);

int IsAutoOffEnabled(bool *bIsAutoOffEnabled);

// LED
int ReadLedData(u_short* iLedData);
int WriteLedData(u_short* iLedData);

// CM
int ReadCmGlassThickness (int* i);
int WriteCmGlassThickness (int i);
int ReadCmSvAbsPos (int* pi);
int ReadCmThresh(int obj, short *pThreshTop, short *pThreshBottom); 
int WriteCmThresh(int obj, short threshTop, short threshBottom); 
int ReadCmLpRatio(int *pRatio); 
int WriteCmLpRatio(int ratio); 
int ReadCmRetardation (int* pi, int iSurface);
int WriteCmRetardation (int iRetardation, int iSurface);
//typedef enum 
//{
//	CmRetarderStatusUnavailable = 0,
//	CmRetarderStatusCalibrating = 1,
//	CmRetarderStatusActive = 2,
//	CmRetarderStatusInactive = 3,
//} CmRetarderEnum;
CmRetarderEnum ReadCmRetarderStatus ();

// CM auto detection
int ReadCmAutoDetectConfigOptions (bool &bDoAutoDetectOnMake0, bool &bDoWaitAutoDetectToAf);
int WriteCmAutoDetectConfigOptions(bool bDoAutoDetectOnMake0,  bool  bDoWaitAutoDetectToAf);

//typedef enum 
//{
//	CmAutoDetectStatusDone		= 0,
//	CmAutoDetectStop		    = 1,
//	CmAutoDetectStatusInitial	= 4,
//	CmAutoDetectStatusPending	= 11,
//	CmAutoDetectStatusFailed	= 10,
//	CmAutoDetectStatusBusy		= 13,
//	// other codes indicate error
//} CmAutoDetectEnum;
CmAutoDetectEnum ReadCmAutoDetectStatus();
int ScheduleCmAutoDetectForAF();
int StartCmAutoDetect();
int StopCmAutoDetect();
int GetCurrentMetric();
void EnableMetric();
void DisableMetric();


// AOI
int WriteAoiLim (double d);
int ReadAoiLim (double* d);

// mfc
int ReadMfcConfig (int* mfc_config);
int WriteMfcUstep (u_short  Ustep);
int ReadMfcUstep (u_short* Ustep);

// LLC
int CommToLlc (char* to_send, char* to_read);
extern int gs_llcErrorInARow;		// counts number of llc errors in a row

int BeginLLCErrorTracking();
int EndLLCErrorTracking();
void GetLLCCharDelay();
int ReadLLCComCorrOption (int *opt);
int WriteLLCComCorrOption (int opt);

// profilometer
int ReadProfCalibNumMeasure (short* num);
int WriteProfCalibNumMeasure (short* num);

int ReadProfCalibNumPoint (short* num);
int WriteProfCalibNumPoint (short num);

int ReadProfCalibPulseDist (short* nanometer);
int WriteProfCalibPulseDist (short nanometer);

int ProfCalibStart (void);
int ProfStart (void);
int ProfTerminate (void);

#define PRFERR_OK                  0
#define PRFERR_WRONG_STATE         1
#define PRFERR_BUSY                2            // operation has started, not finished yet
#define PRFERR_NOCALIB             3
#define PRFERR_CALIBREJECT         4
// #define PRFERR_DATACOLLECTING      5            // unused
#define PRFERR_BADSPOT             6
// #define PRFERR_CALIBCOMPLETED      7
#define PRFERR_PARAMERROR          8
#define PRFERR_ERROR               9            // serious error
#define PRFERR_TERMINATED          10           // operation is finished, not reported yet
#define PRFERR_ALL                 0x00ff
#define ExtractPrfStatus(prfStat)   ((prfStat)&PRFERR_ALL)
#define PRFOPT_CALINVALID           0x0000
#define PRFOPT_CALIBVALID           0x1000    
#define PRFOPT_ALL                  0xf000    
#define GetPrfOpt(prfStat)         ((prfStat)&PRFOPT_ALL)
int GetProfInfo (int *pstatus, int *pavgcnt, int *pcalibcnt);


#define PROF_MAX_DATA   200
#define PROF_MAX_CALIB   40
int ReadProfData (int* data, int num);
int ReadProfCalibData (short* data, int which);

#define OBJCONF_TOP_CV				0x0002
#define OBJCONF_BOTTOM_CV			0x0001
#define OBJCONF_SURFACEVALIDATION   0x0010
#define OBJCONF_SV_3SURFACE_RECON   0x0020
#define OBJCONF_SV_TRUSTSAMEOBJ     0x0100      // reuse same Sv safe level from the same objective CM
#define OBJCONF_SV_TRUSTDIFFEOBJ    0x0400      // reuse same Sv safe level from other objectives
#define OBJCONF_SV_FLAGS			(OBJCONF_BOTTOM_CV|OBJCONF_TOP_CV|OBJCONF_SURFACEVALIDATION|OBJCONF_SV_3SURFACE_RECON|OBJCONF_SV_TRUSTSAMEOBJ|OBJCONF_SV_TRUSTDIFFEOBJ)
int ReadObjConf(int index_obj, int *status);
int WriteObjConf(int index_obj, int status);
int ChangeObjConf (int index_obj, int flags, int mask);

// ATF6
int ReadLineWindowWidth (int obj, short* pwidth);
int WriteLineWindowWidth (int obj, int window, short width);
int ReadWindowIntegrationTimes (int obj, short* pit);
int WriteWindowIntegrationTimes (int obj, int window, short it);
int ReadWindowRowDelay (int obj, short* prd );
int WriteWindowRowDelay (int obj, int window, short rd);
int ReadWindowBeheadingThresh (int obj, double* pbeheading);
int WriteWindowBeheadingThresh (int obj, int window, double beheading);

//
#define OBJCONF_BOTTOM_SV           0x0001      // bottom surface, despite glass sign
#define OBJCONF_TOP_SV              0x0002      // top surface, despite glass sign
#define OBJCONF_PEAKEDGE            0x0004      // enable edge processing of the peak
#define OBJCONF_ZTRACK_FIXEDLASER   0x0008      // in MsFixedStepTracking don't change laser power
#define OBJCONF_SURFACEVALIDATION   0x0010      // enable surface validation
#define OBJCONF_SV_3SURFACE_RECON   0x0020      // activate 3 surface recognition
#define OBJCONF_LAPLACESEARCH		0x0040      // use laplacian search to follow surface recognition
#define OBJCONF_FILTERPEAKS         0x0080      // use peak filtration to remove/minimize second surface influence
#define OBJCONF_SV_TRUSTSAMEOBJ     0x0100      // reuse same Sv safe level from the same objective CM
#define OBJCONF_ZTRACKAUTO_FREEZE   0x0200      // in MsAutoTrackStop mode don't move if reflectivity changes drastically
#define OBJCONF_SV_TRUSTDIFFEOBJ    0x0400      // reuse same Sv safe level from other objectives
#define OBJCONF_ZTRACKAUTO_ENDJUMP  0x0800      // in MsAutoTrackStop mode after arriving in focus, need to execute jump
#define OBJCONF_TINY				0x0800      // in conjunction with OBJCONF_ONLYFARMODE sets short near mode
#define OBJCONF_ONLYFARMODE         0x1000      // don't switch into near mode
#define OBJCONF_FARMODEHWCOG_BOTTOM 0x2000      // use HwCog Bottom mode (need ONLYFARMODE to be set)
#define OBJCONF_FARMODEHWCOG_TOP    0x4000      // use HwCog Top mode (need ONLYFARMODE to be set)
#define OBJCONF_DUV_ACQUISITION     0x8000      // acquire duv data

int ReadProcOptions1 (short* opt1, int obj);
int SetProcOptions1  (int opt1, int opt1_mask, int obj);
int ReadProcOptions1Ext (int* opt1, int obj);
int SetProcOptions1Ext  (int opt1, int opt1_mask, int obj);

#define	Prc2_7DotNoOutliers        0x0000	// exec: select peaks w/o outliers
#define	Prc2_7DotLeastMotion       0x0100   // exec: select peaks that pass screening
#define	Prc2_7DotAvgAll            0x0200   // exec: select average out of 3 dots        
#define	Prc2_7DotMedian            0x0300   // exec: select median out of 3 dots
#define	Prc2_7Dot1                 0x0400   // exec: force dot 1
#define	Prc2_7Dot2                 0x0500   // exec: force dot 2
#define	Prc2_7Dot3                 0x0600   // exec: force dot 3
#define	Prc2_7DotCentral           0x0700   // exec: force dot 4
#define	Prc2_7Dot5                 0x0800   // exec: force dot 5
#define	Prc2_7Dot6                 0x0900   // exec: force dot 6
#define	Prc2_7Dot7                 0x0a00   // exec: force dot 7
#define	Prc2_7DotConsistent        0x0b00   // exec: use dots that proved to be most stable    
#define	Prc2_7DotBottom            0x0c00   // exec: use lower dots
#define	Prc2_7DotRecent            0x0d00   // exec: use mst recent dot    
#define	Prc2_7DotLeft              0x0e00   // exec: use all left dots (1,2,3)
#define	Prc2_7DotRight             0x0f00   // exec: use all right dots (5,6,7)    
#define	Prc2_7DotSelectValidBits   0x0f00   // exec: 4 bits selects dot combination mode
// ATF5 before merger
#define _Ms7Dot3Center			   0x0000   // exec: force to use only 3 center dots
#define _Ms7DotAuto                0x0800   // exec: allow dynamic selection of 7 dots    
#define _Ms7Dot3Left               0x1000   // exec: force to use only 3 left dots    
#define _Ms7Dot3Right              0x1800   // exec: force to use only 3 right dots
#define _Ms7DotValidBits           0x1800   // exec: valid bits
int ReadProcOptions2 (short* opt2, int obj);
int SetProcOptions2  (short opt2, short opt2_mask, int obj);

// read and write both options
int ReadProcOptions (unsigned short* popt, int obj);
int WriteProcOptions (unsigned short* popt, int obj);

// Alignment
int DisableLineProcessing();
int EnableLineProcessing();
int ReadAvgOptions (short* popt1, int obj);
int ReadLinearRange (int *pLinearRange, int obj);
int ReadLinearPrediction (int *pfltLen, int *ppredLen, int *ppredAhead, int *poutShift,double *pdspeedShift) ;
int WriteLinearPrediction (int fltLen, int predLen, int predAhead, int outShift, double dspeedShift);

int RecoverSensor();

int ReadAoiFltOut (int *piFlt);
int WriteAoiFltOut (int iFlt);

int ReadZPositionBuffer (short* pBuff, int len);
int ReadPredictionBuffer (short* pBuff, int len);
int ReadSensorBuffer (short* pBuff, int len);
int ReadZStepBuffer (short* pBuff, int len);
int ReadEstimateBuffer (short* pBuff, int len);
int LimitAFSampleRun(short len);
int ReadLimitAFSampleRun(int *pLimit);
int ReadItrTime (int *pItrTime, bool bhwCog);
int ReadPAssymetry (double *pdAssym, int obj);
int WritePAssymetry (double dAssym, int obj);
int WriteAoiFreezeZGlassThicknessPercent (int freeze);
int ReadAoiFreezeZGlassThicknessPercent (int* pfreeze);

int ReadAnalogMove(int *status);
int WriteAnalogMove(int status);
int ReadAnalogNearMode(bool *bAmode);
int WriteAnalogNearMode(bool baanalognearmode);

// CM Opt
enum CmOpts
{
	CmOptDisabled          = 0,
	CmOptTopValidation     = 16,
	CmOptBottomValidation  = -16,
	CmOptTop3SurfaceRec    = 48,
	CmOptBottom3SurfaceRec = -48
};

const char * const strCmOpts[] = {"Disabled",
								  "Top Validation",
								  "Bottom Validation",
								  "Bottom 3 Surface Rec",
								  "Top 3 Surface Rec"};

int ReadHistCalibration (int *piHCal);
int WriteHistCalibration (int iHCal);
int WriteFactoryInit ();
int ReadSensorType();
int ReadModel(int* model);
int ReadFpgaStrapCode(u_short* ver);
bool IsSplitOptics();
int ReadSeparation (int* pseparation, int *pSeparationOption);

// extended status
enum 
{
	MmsSet0                 = 0x00000001,   // exec: executing set0 option

	MmsFixedStepFrozen      = 0x00000010,   // exec: fixed step tracking decided to reeze
	MmsMotionLimited        = 0x00000020,   // exec: motion limits has been reached
	MmsExternalMotion       = 0x00000040,   // exec: external motion is activated
	MmsStartFixedStepInitDelayed = 0x080,   // exec: need to initialize AOI tracking with AOI data

	MmsTrackable            = 0x00000100,   // exec: can be used for tracking
	MmsStartFixedStepAtEnd  = 0x00000200,   // exec: instruct to follow up with fixed step tracking
	MmsZMotionLimitApply    = 0x00000400,   // exec: limit tracking motion to be within g_MainExec.iMotionLimit bounds
	MmsAutoTrackStop        = 0x00000800,   // exec: clear AF tracking when in range is reached

	MmsJumpRequested        = 0x00100000,   // Jump at the end of AF cycle has been requested
	MmsZscan                = 0x00200000,   // exec: Zscan in progress
	MmsTopZscan             = 0x00400000,   // exec: Zscan tracking on top
	MmsMidZscan             = 0x00800000,   // exec: Zscan tracking on medium

	MmsSls                  = 0x01000000,   // exec: software limit switches are ON
	MmsSlsHoming            = 0x02000000,   // exec: Homing being executed
};

int ReadExtendedStatus (int &status, int &more_status);
void ReinitAllUiCmdStatics();

// ABC
int ReadABCStatus(short *pABCstat);
int WriteABCState(short iABCState, int iOffset);
int ReadABCLaserPower(int iObj, unsigned short *pABClp);
int WriteABCLaserPower(int iObj, unsigned short iABClp);
int ReadABCZDelta(int iObj, unsigned short *pABCzd);
int WriteABCZDelta(int iObj, unsigned short iABCzd);
int ReadTinyZDelta(int iObj, short *pzd);
int WriteTinyZDelta(int iObj, unsigned short izd);

// Test
int ReadTestData(char *pcDataBuf);
int WriteTestData(char *pcDataBuf);

// Homing
int ReadHomingParameters (int *pTable8);
int WriteHomingParameters (int *pTable8);
int RunCurrentHoming (int *pTable8);

// AF statistical timer suppport
int ReadAFTimer(float *pAFDurationLast);
int ReadDuvTrackingStatistics(int *pOptoins, int *pStatus, int *pLength, int *pDisturbance, int *pDuration,
							  int *pInValidCnt, int *pValidCnt);
// pre af jump access 
int ReadPreAfJump (int obj, short* pUStep);
int WritePreAfJump (int obj, short iUStep);
bool isReadPreAfJumpPresent();

// hw cog
int ReadHwCog(bool *phwCog, int obj);
int WriteHwCog(bool hwCog, int obj);

// I coeff
int ReadICoeff (float* pfICoeff, int obj);
int WriteICoeff (float fICoeff, int obj);

#ifdef __cplusplus
    }
#endif

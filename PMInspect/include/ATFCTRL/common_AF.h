/******************************************************************************
 *	
 *	(C) Copyright WDI 2011
 *	
 ******************************************************************************
 *
 *	FILE:		common.h
 *
 *	PROJECT:	AFT Sensor
 *
 *	SUBPROJECT:	.
 *
 *	Description: Common header file.
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          01/03/2011 Chris O.         Initial version
 *          
 *****************************************************************************/

#pragma once

#ifdef __cplusplus

#include <windows.h>

typedef struct
{
	u_int objective;		// current objective
	u_int surface;			// current surface
	u_int mag;				// current magnification
	double offset[24];
	u_int infocus;
	u_int linear;
	u_int proc_range;
	short position;
	u_int errCode;
	int status;
} MeasureDataType;

typedef enum 
{
	eWM_Far			= 0,
	eWM_Near		= 1,
	eWM_NearSingle	= 2,
	eWM_Sv			= 3,
	eWM_Auto		= 0x1000,
	eWM_Default		= 0xFFFE,
	eWM_Error		= 0xFFFF,
	// ATF5 before merge
	eWM_Auto5x		= 0x1000,
	eWM_Far5x		= 0,
	eWM_Left5x		= 1,
	eWM_Center5x	= 2,
	eWM_Right5x		= 3,
} EnumWindowMode;

#endif // __cplusplus

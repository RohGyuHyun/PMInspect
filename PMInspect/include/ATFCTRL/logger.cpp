/******************************************************************************
 *	
 *	(C) Copyright WDI 2010
 *	
 ******************************************************************************
 *
 *	FILE:		logger.cpp
 *
 *	PROJECT:	AFT Sensor
 *
 *	SUBPROJECT:	.
 *
 *	Description: Logging mechanism
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          15/10/2010 Chris O.         Initial version
 *          15/11/2010 Chris O.         Added synchronization objects for
 *                                      multithreaded applications
 *
 ******************************************************************************/

#include "stdafx.h"
#include <time.h>
#include <sys\timeb.h>

#include "logger.h"

bool CLogger::openLogFile(const char *pLogFilePath, const char *pMode)
{
	m_pFile = NULL;

	if (pLogFilePath == NULL || pMode == NULL)
	{
		return false;
	}

	m_pFile = fopen (pLogFilePath, pMode);

	if (m_pFile)
	{
		return true;
	}

	return false;
}

bool CLogger::closeLogFile()
{
	int iRet = 0;

	if (m_pFile != NULL)
	{
		iRet = fclose(m_pFile);
		m_pFile = NULL;
	}

	return (iRet == 0 ? true : false);
}

void CLogger::setLogLevel(LogLevels iLogLevel)
{
	m_iLogLevel = iLogLevel;
}

CLogger::LogLevels CLogger::getLogLevel(void)
{
	return m_iLogLevel;
}

FILE *CLogger::getLogFileHandle(void)
{
	return m_pLogger ? NULL : m_pFile ;
}

void CLogger::saveToLog(char const *strMsg, LogLevels iLogLevel)
{
	if ( m_pLogger == NULL )
	{
		_timeb currTime;
		tm *pCurrTm;
		char strCurrTime[32];

		// check arguments
		if (m_pFile == NULL || strMsg == NULL) return;

		if (iLogLevel <= m_iLogLevel)
		{
			__try 
			{
				EnterCriticalSection(&m_cs);

				// get current time
				_ftime(&currTime);
				// format current time
				pCurrTm = localtime(&currTime.time);
				sprintf(strCurrTime, "[%4d-%02d-%02d %02d:%02d:%02d:%03d] : ", 
						1900 + pCurrTm->tm_year, pCurrTm->tm_mon + 1, pCurrTm->tm_mday, 
						pCurrTm->tm_hour, pCurrTm->tm_min, pCurrTm->tm_sec, currTime.millitm);
				// save current time
				fprintf(m_pFile, strCurrTime);
				fprintf(m_pFile, strMsg);
			}
			__finally 
			{
				LeaveCriticalSection(&m_cs);
			}
		}
	}
	else
	{
		NSLogSubsystem::ILogSubsystem::EVENT_SEVERITY	Severity[] =
		{
			NSLogSubsystem::ILogSubsystem::EVENT_SEVERITY ( -1 ),	// LogOff
			NSLogSubsystem::ILogSubsystem::ES_TRACE,				// LogLow
			NSLogSubsystem::ILogSubsystem::ES_WARNING,				// LogMid
			NSLogSubsystem::ILogSubsystem::ES_ERROR					// LogAll
		};

		m_pLogger -> CreateRecord ( "Legacy", Severity[iLogLevel], strMsg, true, NULL ) ;
	}
}

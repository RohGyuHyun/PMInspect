#include "StdAfx.h"
#include "IO.h"

CIO::CIO(void)
{

}


CIO::~CIO(void)
{
}

//본 프로젝트에서는 사용하지 않는다.
BOOL CIO::AXTLibInit()
{
	BOOL bRet(TRUE);
	// Initialize library 
	if (!AxlIsOpened())
	{
		DWORD Code = AxlOpenNoReset(7);
		if (Code == AXT_RT_SUCCESS)
			TRACE("Library is initialized .\n");
		else
		{
			AfxMessageBox(_T("I/O Failed initialization."));
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CIO::AXTDIOInit()
{
	// Inspect if DIO module exsits

	DWORD Code, dwStatus;

	Code = AxdInfoIsDIOModule(&dwStatus);
	if (dwStatus == STATUS_EXIST)
	{
		TRACE("DIO module exists.\n");
		long IModuleCounts;
		Code = AxdInfoGetModuleCount(&IModuleCounts);

		if (Code == AXT_RT_SUCCESS)
			TRACE("Number of DIO module: %d\n", IModuleCounts);
		else
			TRACE("AxdInfoGetModuleCount() : ERROR code Ox%x\n", Code);

		//long IInputCounts, IOutputCounts;		
		long IBoardNo, IModulePos;
		DWORD dwModuleID;
		CString strData;


		for (int ModuleNo = 0; ModuleNo < IModuleCounts; ModuleNo++)
		{
			//AxdInfoGetInputCount(ModuleNo, &IInputCounts);
			//AxdInfoGetOutputCount(ModuleNo, &IOutputCounts);

			if (AxdInfoGetModule(ModuleNo, &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
			{
				switch (dwModuleID)
				{
				case AXT_SIO_DI32:
					strData.Format(_T("AXT_SIO_DI32"));
					break;
				case AXT_SIO_DO32P:
					strData.Format(_T("AXT_SIO_DO32P"));
					break;
				default:

					break;
				}

				m_DIOInfo[ModuleNo].m_dwModuleID = dwModuleID;
				m_DIOInfo[ModuleNo].m_IBoardNo = IBoardNo;
				m_DIOInfo[ModuleNo].m_IModulePos = IModulePos;
				//m_DIOInfo[ModuleNo].m_strModuleName = strData;
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Module not exist."));
		return FALSE;
	}

	return TRUE;
}


BOOL CIO::AXTLibClose()
{
	BOOL bRet(TRUE);

	bRet = AxlClose();

	if (bRet)
		TRACE("Library is Close OK!! .\n");
	else
		TRACE("Failed AXL Library Close!!.");

	return bRet;
}

BOOL CIO::ReadDO(int nBit)
{
	DWORD dwValue;
	BOOL rslt;

	AxdoReadOutport(nBit, &dwValue);

	if (dwValue)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;

}

BOOL CIO::ReadDI(int nBit)
{
	// 이 함수를 쓰자 
	DWORD dwValue;
	BOOL rslt;

	AxdiReadInport(nBit, &dwValue);

	if (dwValue)
		rslt = TRUE;
	else
		rslt = FALSE;

	return rslt;
}

void CIO::WriteDO(int nBit, BOOL bVal)
{
	DWORD dwValue;

	dwValue = bVal;

	AxdoWriteOutport(nBit, dwValue);
}
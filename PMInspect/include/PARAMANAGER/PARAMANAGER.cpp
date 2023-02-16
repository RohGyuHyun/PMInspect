#include "stdafx.h"
#include "PARAMANAGER.h"
#include <AFXPRIV.H>

PARAMANAGER::PARAMANAGER()
{
	Release();
}


PARAMANAGER::~PARAMANAGER()
{
	Release();
}

BOOL	PARAMANAGER::PushElement(LPCTSTR strSection, LPCTSTR strKey, PARAMANAGER_VARTYPE Type, VOID* pValue, LPCTSTR strDefault)
{
	if (strSection == NULL || strKey == NULL || pValue == NULL)
		return	FALSE;

	if (_tcslen(strSection) == 0 || _tcslen(strKey) == 0)
		return	FALSE;

	//	이미 존재하면 FALSE 반환
	if (FindElement(strSection, strKey, Type) >= 0)
		return	FALSE;

	PARAMANAGER_ELEMENT	Element;

	Element.m_strSection = strSection;
	Element.m_strKey = strKey;
	Element.m_pLinkedData = pValue;
	Element.m_ValueType = Type;
	Element.m_strDefaultValue = strDefault;

	m_qElement.push_back(Element);

	return	TRUE;
}

BOOL	PARAMANAGER::Sort()
{
	return	FALSE;
}

BOOL	PARAMANAGER::LinkDataToString(P_PARAMANAGER_ELEMENT pPara, CString &strResult)
{
	if (pPara == NULL)
		return	FALSE;

	switch (pPara->m_ValueType)
	{
		case	PARAMANAGER_VARTYPE_LONGLONG	:	strResult.Format(_T("%d"), *((LONGLONG*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_ULONGLONG	:	strResult.Format(_T("%d"), *((ULONGLONG*)pPara->m_pLinkedData));	break;
		case	PARAMANAGER_VARTYPE_LONG		:	strResult.Format(_T("%d"), *((LONG*)pPara->m_pLinkedData));			break;
		case	PARAMANAGER_VARTYPE_ULONG		:	strResult.Format(_T("%d"), *((ULONG*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_SHORT		:	strResult.Format(_T("%d"), *((SHORT*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_USHORT		:	strResult.Format(_T("%d"), *((USHORT*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_CHAR		:	strResult.Format(_T("%d"), *((CHAR*)pPara->m_pLinkedData));			break;
		case	PARAMANAGER_VARTYPE_UCHAR		:	strResult.Format(_T("%d"), *((UCHAR*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_DOUBLE		:	strResult.Format(_T("%f"), *((DOUBLE*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_FLOAT		:	strResult.Format(_T("%f"), *((FLOAT*)pPara->m_pLinkedData));		break;
		case	PARAMANAGER_VARTYPE_INT			:	strResult.Format(_T("%d"), *((INT*)pPara->m_pLinkedData));			break;
		case	PARAMANAGER_VARTYPE_CSTRING		:	strResult = *(CString*)pPara->m_pLinkedData;						break;
		case	PARAMANAGER_VARTYPE_MBSTRING	:	strResult = CA2CT((char*)pPara->m_pLinkedData);						break;
		default:	return	FALSE;
	}

	return	TRUE;
}

BOOL	PARAMANAGER::CopyOrgToLink(P_PARAMANAGER_ELEMENT pPara)
{
	if (pPara == NULL)
		return	FALSE;

	switch (pPara->m_ValueType)
	{
		case	PARAMANAGER_VARTYPE_LONGLONG	:	*((LONGLONG*)pPara->m_pLinkedData)	=	_tcstoll(pPara->m_strOriginalValue, NULL, 10)			;	break;
		case	PARAMANAGER_VARTYPE_ULONGLONG	:	*((ULONGLONG*)pPara->m_pLinkedData)	=	_tcstoull(pPara->m_strOriginalValue, NULL, 10)			;	break;
		case	PARAMANAGER_VARTYPE_LONG		:	*((LONG*)pPara->m_pLinkedData)		=	_tcstol(pPara->m_strOriginalValue, NULL, 10)			;	break;
		case	PARAMANAGER_VARTYPE_ULONG		:	*((ULONG*)pPara->m_pLinkedData)		=	_tcstoul(pPara->m_strOriginalValue, NULL, 10)			;	break;
		case	PARAMANAGER_VARTYPE_SHORT		:	*((SHORT*)pPara->m_pLinkedData)		=	(SHORT)_tcstol(pPara->m_strOriginalValue, NULL, 10)		;	break;
		case	PARAMANAGER_VARTYPE_USHORT		:	*((USHORT*)pPara->m_pLinkedData)	=	(USHORT)_tcstol(pPara->m_strOriginalValue, NULL, 10)	;	break;
		case	PARAMANAGER_VARTYPE_CHAR		:	*((CHAR*)pPara->m_pLinkedData)		=	(CHAR)_tcstol(pPara->m_strOriginalValue, NULL, 10)		;	break;
		case	PARAMANAGER_VARTYPE_UCHAR		:	*((UCHAR*)pPara->m_pLinkedData)		=	(UCHAR)_tcstol(pPara->m_strOriginalValue, NULL, 10)		;	break;
		case	PARAMANAGER_VARTYPE_DOUBLE		:	*((DOUBLE*)pPara->m_pLinkedData)	=	_tcstod(pPara->m_strOriginalValue, NULL)				;	break;
		case	PARAMANAGER_VARTYPE_FLOAT		:	*((FLOAT*)pPara->m_pLinkedData)		=	_tcstof(pPara->m_strOriginalValue, NULL)				;	break;
		case	PARAMANAGER_VARTYPE_INT			:	*((INT*)pPara->m_pLinkedData)		=	(INT)_tcstol(pPara->m_strOriginalValue, NULL, 10)		;	break;
		case	PARAMANAGER_VARTYPE_CSTRING		:	*(CString*)pPara->m_pLinkedData		=	pPara->m_strOriginalValue								;	break;
		case	PARAMANAGER_VARTYPE_MBSTRING	:	strcpy(((char*)pPara->m_pLinkedData), CT2CA(pPara->m_strOriginalValue))							;	break;
		default:	return	FALSE;
	}

	return	TRUE;
}

LONG	PARAMANAGER::FindElement(LPCTSTR strSection, LPCTSTR strKey, PARAMANAGER_VARTYPE Type)
{
	if (strSection == NULL || strKey == NULL)
		return	-1;

	if (_tcslen(strSection) == 0 || _tcslen(strKey) == 0)
		return	-1;

	LONG	Ret = -1;

	for (int i = 0; i < m_qElement.size(); i++)
	{
		if (m_qElement[i].m_strSection.Compare(strSection) == 0 &&
			m_qElement[i].m_strKey.Compare(strKey) == 0 &&
			m_qElement[i].m_ValueType == Type)
		{
			Ret = i;
			break;
		}
	}

	return	Ret	;
}


BOOL	PARAMANAGER::Create()
{
	Release();

	return	TRUE;
}

VOID	PARAMANAGER::Release()
{
	m_strPath.Empty();
	m_strFileName.Empty();

	for (int i = 0; i < m_qElement.size(); i++)
	{
		m_qElement[i].m_strSection.Empty();
		m_qElement[i].m_strKey.Empty();
		m_qElement[i].m_strOriginalValue.Empty();
	}

	m_qElement.clear();
}

BOOL	PARAMANAGER::SetPath(LPCTSTR strPath, LPCTSTR strFileName)
{
	if (strPath == NULL || strFileName == NULL)
		return	FALSE;

	if (_tcslen(strPath) == 0 || _tcslen(strFileName) == 0)
		return	FALSE;

	m_strPath.Empty();
	m_strFileName.Empty();

	m_strPath = strPath;
	m_strFileName = strFileName;

	return	TRUE;
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONGLONG *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_LONGLONG, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONGLONG *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_ULONGLONG, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONG *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_LONG, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONG *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_ULONG, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, SHORT *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_SHORT, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, USHORT *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_USHORT, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, CHAR *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_CHAR, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, UCHAR *pValue, LPCTSTR strDef/*0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_UCHAR, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, DOUBLE *pValue, LPCTSTR strDef/*0.0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_DOUBLE, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, FLOAT *pValue, LPCTSTR strDef/*0.0*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_FLOAT, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, INT *pValue, LPCTSTR strDef/*"0"*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_INT, pValue, strDef);
}

BOOL	PARAMANAGER::LinkPara(LPCTSTR strSection, LPCTSTR strKey, CString *pValue, LPCTSTR strDef/*""*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_CSTRING, pValue, strDef);
}

BOOL	PARAMANAGER::LinkParaMBS(LPCTSTR strSection, LPCTSTR strKey, CHAR* pValue, LPCTSTR strDef/*""*/)
{
	return	PushElement(strSection, strKey, PARAMANAGER_VARTYPE_MBSTRING, pValue, strDef);
}

BOOL	PARAMANAGER::LoadPara()
{
	if (m_strPath.GetLength() == 0 || m_strFileName.GetLength() == 0)
		return	FALSE;

	CString	strPath = m_strPath + _T("\\") + m_strFileName;
	TCHAR	strTemp[512];

	Sort();

	for (int i = 0; i < m_qElement.size(); i++)
	{
		GetPrivateProfileString(m_qElement[i].m_strSection,
			m_qElement[i].m_strKey + PARAMANAGER_STR_VARTYPE[m_qElement[i].m_ValueType],
			m_qElement[i].m_strDefaultValue,
			strTemp,
			512,
			strPath);

		m_qElement[i].m_strOriginalValue = strTemp;
		CopyOrgToLink(&m_qElement[i]);
	}

	return	TRUE;
}

BOOL	PARAMANAGER::SavePara()
{
	if (m_strPath.GetLength() == 0 || m_strFileName.GetLength() == 0)
		return	FALSE;

	CString	strPath = m_strPath + _T("\\") + m_strFileName;
	CString	strValue;

	Sort();

	for (int i = 0; i < m_qElement.size(); i++)
	{
		LinkDataToString(&m_qElement[i], strValue);

		if (m_qElement[i].m_strOriginalValue.Compare(strValue) != 0)
		{
			//	Write Log Diff
		}

		WritePrivateProfileString(m_qElement[i].m_strSection,
			m_qElement[i].m_strKey + PARAMANAGER_STR_VARTYPE[m_qElement[i].m_ValueType],
			strValue,
			strPath);
	}

	return	TRUE;
}

BOOL	PARAMANAGER::CallBack()
{
	for (int i = 0; i < m_qElement.size(); i++)
	{
		CopyOrgToLink(&m_qElement[i]);
	}

	return	TRUE;
}


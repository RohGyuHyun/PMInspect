#ifndef	__PARAMANAGER_H__
#define	__PARAMANAGER_H__

#include<deque>

using namespace	std;

#define	PARAMANAGER_NUM_VARTYPE	13

const	enum PARAMANAGER_VARTYPE
{
	PARAMANAGER_VARTYPE_LONGLONG,
	PARAMANAGER_VARTYPE_ULONGLONG,
	PARAMANAGER_VARTYPE_LONG,
	PARAMANAGER_VARTYPE_ULONG,
	PARAMANAGER_VARTYPE_SHORT,
	PARAMANAGER_VARTYPE_USHORT,
	PARAMANAGER_VARTYPE_CHAR,
	PARAMANAGER_VARTYPE_UCHAR,
	PARAMANAGER_VARTYPE_DOUBLE,
	PARAMANAGER_VARTYPE_FLOAT,
	PARAMANAGER_VARTYPE_INT,
	PARAMANAGER_VARTYPE_CSTRING,
	PARAMANAGER_VARTYPE_MBSTRING	//	동적생성 배열버퍼(다중포인터) 지원 안함, 버퍼 오버플로우의 위험이 있어 가능한 사용 안하는 것이 좋음
};

static	LPCTSTR	PARAMANAGER_STR_VARTYPE[PARAMANAGER_NUM_VARTYPE]	=
{
	_T("_SLL"),
	_T("_ULL"),
	_T("_SLN"),
	_T("_ULN"),
	_T("_SSH"),
	_T("_USH"),
	_T("_SBT"),
	_T("_UBT"),
	_T("_DBL"),
	_T("_FLT"),
	_T("_INT"),
	_T("_STR"),
	_T("_MBS")
};

typedef	struct PARAMANAGER_ELEMENT
{
	CString				m_strSection;
	CString				m_strKey;
	CString				m_strOriginalValue;
	VOID*				m_pLinkedData;
	CString				m_strDefaultValue;
	PARAMANAGER_VARTYPE	m_ValueType;

}PARAMANAGER_ELEMENT, *P_PARAMANAGER_ELEMENT;

typedef	class PARAMANAGER
{
private:
	CString	m_strPath;
	CString	m_strFileName;

	deque<PARAMANAGER_ELEMENT>	m_qElement;

	BOOL	PushElement(LPCTSTR strSection, LPCTSTR strKey, PARAMANAGER_VARTYPE Type, VOID* pValue, LPCTSTR strDefault);
	BOOL	Sort();	//	섹션과 키 스트링으로 정렬
	BOOL	LinkDataToString(P_PARAMANAGER_ELEMENT pPara, CString &strResult);	//	pPara->m_pLinkedData 값을 strResult에 스트링으로 포맷
	BOOL	CopyOrgToLink(P_PARAMANAGER_ELEMENT pPara);	//	pPara->m_strOriginalValue를 m_pLinkedData에 데이터로 변환하여 저장
	LONG	FindElement(LPCTSTR strSection, LPCTSTR strKey, PARAMANAGER_VARTYPE Type);	//	m_qElement에서 섹션, 키, 자료형이 동일한 원소 인덱스를 찾아 반환(없으면 -1 반환)

public:
	PARAMANAGER();
	virtual ~PARAMANAGER();

	BOOL	Create();
	VOID	Release();
	BOOL	SetPath(LPCTSTR strPath, LPCTSTR strFileName);

	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONGLONG *pValue, LPCTSTR strDef = _T("0"));	//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONGLONG *pValue, LPCTSTR strDef = _T("0"));	//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONG *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONG *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, SHORT *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, USHORT *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, CHAR *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, UCHAR *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, DOUBLE *pValue, LPCTSTR strDef = _T("0.0"));	//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, FLOAT *pValue, LPCTSTR strDef = _T("0.0"));	//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, CString *pValue, LPCTSTR strDef = _T(""));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, INT *pValue, LPCTSTR strDef = _T("0"));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함		//21080824 add By
	BOOL	LinkParaMBS(LPCTSTR strSection, LPCTSTR strKey, CHAR *pValue, LPCTSTR strDef = _T(""));		//	Create 후 타 작업 전 반드시 파라미터들을 링크 시켜 주어야 함		//21080824 add By

	BOOL	LoadPara();	//	링크된 섹션과 키값을 기준으로 로드
	BOOL	SavePara();	//	링크된 섹션과 키값을 기준으로 저장

	BOOL	CallBack();	//	m_strOriginalValue값을 m_pLinkedData에 변환 복사

}PARAMANAGER, *P_PARAMANAGER;

#endif
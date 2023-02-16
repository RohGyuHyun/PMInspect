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
	PARAMANAGER_VARTYPE_MBSTRING	//	�������� �迭����(����������) ���� ����, ���� �����÷ο��� ������ �־� ������ ��� ���ϴ� ���� ����
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
	BOOL	Sort();	//	���ǰ� Ű ��Ʈ������ ����
	BOOL	LinkDataToString(P_PARAMANAGER_ELEMENT pPara, CString &strResult);	//	pPara->m_pLinkedData ���� strResult�� ��Ʈ������ ����
	BOOL	CopyOrgToLink(P_PARAMANAGER_ELEMENT pPara);	//	pPara->m_strOriginalValue�� m_pLinkedData�� �����ͷ� ��ȯ�Ͽ� ����
	LONG	FindElement(LPCTSTR strSection, LPCTSTR strKey, PARAMANAGER_VARTYPE Type);	//	m_qElement���� ����, Ű, �ڷ����� ������ ���� �ε����� ã�� ��ȯ(������ -1 ��ȯ)

public:
	PARAMANAGER();
	virtual ~PARAMANAGER();

	BOOL	Create();
	VOID	Release();
	BOOL	SetPath(LPCTSTR strPath, LPCTSTR strFileName);

	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONGLONG *pValue, LPCTSTR strDef = _T("0"));	//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONGLONG *pValue, LPCTSTR strDef = _T("0"));	//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, LONG *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, ULONG *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, SHORT *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, USHORT *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, CHAR *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, UCHAR *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, DOUBLE *pValue, LPCTSTR strDef = _T("0.0"));	//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, FLOAT *pValue, LPCTSTR strDef = _T("0.0"));	//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, CString *pValue, LPCTSTR strDef = _T(""));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��
	BOOL	LinkPara(LPCTSTR strSection, LPCTSTR strKey, INT *pValue, LPCTSTR strDef = _T("0"));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��		//21080824 add By
	BOOL	LinkParaMBS(LPCTSTR strSection, LPCTSTR strKey, CHAR *pValue, LPCTSTR strDef = _T(""));		//	Create �� Ÿ �۾� �� �ݵ�� �Ķ���͵��� ��ũ ���� �־�� ��		//21080824 add By

	BOOL	LoadPara();	//	��ũ�� ���ǰ� Ű���� �������� �ε�
	BOOL	SavePara();	//	��ũ�� ���ǰ� Ű���� �������� ����

	BOOL	CallBack();	//	m_strOriginalValue���� m_pLinkedData�� ��ȯ ����

}PARAMANAGER, *P_PARAMANAGER;

#endif
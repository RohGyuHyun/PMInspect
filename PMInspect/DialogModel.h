#pragma once
#include "afxcmn.h"


// CDialogModel 대화 상자입니다.

#define MAX_MOD_NAME					100
#define MAX_MOD_DESC					100

#define MOD_MODEL_PATH					("D:\\DATABASE\\MODEL")
#define MOD_PATH						("DATABASE")
#define MOD_MODEL_REGI   				("SOFTWARE\\PMISYSTEM\\MODEL")

typedef struct Model
{
	char szName[MAX_MOD_NAME];
	char szDesc[MAX_MOD_DESC];
	char szPath[MAX_PATH];
}typeModel;

class CDialogModel : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogModel)

public:
	CDialogModel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogModel();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MODEL };
#endif

protected:
	CString m_edit_strName;
	CString m_edit_strDescription;
	CListCtrl m_listctrl_model;
	
	BOOL DeleteDirectory(CString lpDirPath);
	
	BOOL AddList(typeModel *mod);
	BOOL SelectList(int idx, BOOL on_off);
	BOOL SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, CString lpintValue);
	BOOL GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue);
	int RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey);
	void RemoveFileSRecurse(LPCTSTR pstr, long period);

	int m_iDlgType;
	int m_iModelIdx;
	char m_szMainPath[MAX_PATH];
	char m_szMainRegi[MAX_PATH];
	typeModel m_CurrentModel;

public:
	void Open();
	void SetModelPath(CString path);

	char *GetName() { return m_CurrentModel.szName; }
	char *GetDesc() { return m_CurrentModel.szDesc; }
	char *GetPath() { return m_CurrentModel.szPath; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void ClickBtnenhctrlModelNew();
	void ClickBtnenhctrlModelSelect();
	void ClickBtnenhctrlModelDelete();
	void ClickBtnenhctrlModelOk();
	
	afx_msg void OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult);
};

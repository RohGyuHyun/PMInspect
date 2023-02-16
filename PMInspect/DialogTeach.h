#pragma once
#include "SerialComm\SerialPort.h"
#include "DialogTeachAlign.h"
#include "afxcmn.h"
#include "afxwin.h"


#define MAX_INSP_DOUBLE_PARA_CNT		28

#define MAX_LIGHT_SOURCE_CNT			2		// 0:main 1:align
#define LIGHT_SOURCE_MAIN				0
#define LIGHT_SOURCE_ALIGN				1

// CDialogTeach 대화 상자입니다.

class CDialogTeach : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogTeach)

public:
	CDialogTeach(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogTeach();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TEACH};
#endif
public:
	void SetMainLight(int nVal);


public:
	int				m_edit_nRectXIdx;
	int				m_edit_nRectYIdx;

	double			m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT];

	BYTE m_edit_byMainLightVal;

	CSliderCtrl m_slider_main_light;

	CSerialPort* m_pSerialComm;
	CDialogTeachAlign*	m_pDlgTeachAlign;

	void UpdatePara(BOOL isUpdate);
	void WritePacket(int iType, BYTE byVal);

	//20180914 ngh
	void RectValDisplay(int x_idx, int y_idx);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlRectXNumLeft();
	void ClickBtnenhctrlRectXNumRight();
	void ClickBtnenhctrlRectYNumLeft();
	void ClickBtnenhctrlRectYNumRight();
	void ClickBtnenhctrlSetupApply();
	void ClickBtnenhctrlSetupTest();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	void ClickBtnenhctrlSetupAlignTest2();
	BOOL m_chk_InspImgSave;
	BOOL m_chk_InspFilterView;
	BOOL m_chk_InspImageView;
	CComboBox m_combo_PMType;
	BOOL m_chk_AutoInspRectTeaching;
	BOOL m_chk_bInspection;
	void ClickBtnenhctrlSetupTeachingArea();
};

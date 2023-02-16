
// PMInspectDlg.h : 헤더 파일
//

#pragma once

#include "IMGALIGN\IMGALIGN.h"
#include "DialogDispMain.h"
#include "DialogTeach.h"
#include "DialogModel.h"
#include "DialogSystem.h"
#include "DialogMotion.h"
#include "DialogAlarm.h"
#include "DialogResultView.h"

#include "MsgBox\MsgBox.h"
#include "afxcmn.h"


#define MOT_PROCESS_MAX_NUM					1
#define MOT_STATUS_MAX_NUM					3
#define MOT_STATUS_SUB_MAX_NUM				2000
#define USER_MSG_MOT						WM_USER+21

#define MAX_ALIGN_NUM						2

//20180910 by 디파인 추가 
#define TIMEOUT_SAFE_SENSOR					10
#define TIMEOUT_VAC_SENSOR					500
#define TIMEOUT_MAIN_INSPECTION				40000
#define TIMEOUT_ALIGN_INSPECTION			2000
#define TIMEOUT_ALIGN_GRAB					3000
#define TIMEOUT_IN_MOTION					600000
#define TIMEOUT_START_WAITING				60000
#define TIMEOUT_INPUT_BUTTON				700
#define TIMEOUT_LINESCAN_GRAB				500000

//
#define ALARM_CODE_EMO				"Err000"
#define ALARM_CODE_READY_TIMEOUT	"Err001"
#define ALARM_CODE_STOP				"Err007"
#define ALARM_CODE_VACCUM			"Err008"
#define ALARM_CODE_SOL				"Err010"
#define ALARM_CODE_START			"Err011"
#define ALARM_CODE_INMOTION			"Err012"
#define ALARM_CODE_INSPECTION		"Err013"
#define ALARM_CODE_GRAB				"Err014"


#define OP_STATUS_STOP				_T("STOP")
#define OP_STATUS_AUTO				_T("AUTO RUN")

//
#define MAIN_DIALOG					1000
#define SETUP_DIALOG				1001
#define SYSTEM_DIALOG				1002
#define MODEL_DIALOG				1003
#define MOTION_DIALOG				1016
#define EXIT_DIALOG					1017

#define TIMER_DATE					0
#define TIMER_COUNT					1

#define MAX_TAP_NUM					6
#define LIST_MAX_NUM				50
//Motion Process Status///////////////////////////////////////////////////////
typedef struct MotionProcessStatus
{
	//Timeout
	double m_dTimeoutS;//시작 시간
	double m_dTimeoutE;//종료 시간(현재)
	double m_dTimeout;//지정된 시간

	//Status문자(에러 또는 정상종료시 출력할 문자열)
	char m_cNGStatus[MAX_CHAR];
	char m_cNGCode[MAX_CHAR];
	char m_iTypeStatus[MAX_CHAR];

	int m_iNGSeqIdx;

}typeMotionProcessSts;


//Process Result///////////////////////////////////////////////////////
typedef struct ProcessResult
{
	int m_iFilmRemoveSeq;

}typeProcessResult;

//Motion Sequence///////////////////////////////////////////////////////
typedef struct MotionSeq
{
	//Sequence
	int m_iSubSeqIdx;
	int m_iDispSubSeqIdx;
	int m_iNextSubSeqIdx;

	int m_iSubRoutineIdx;

	//Flag
	BOOL m_bAutoStart;
	BOOL m_bAlarm;

	//버튼중심의 동작상태
	BOOL m_bOpSts;

	//정보
	int m_iDeviceIdx;
}typeMotionSeq;

//Motion flag///////////////////////////////////////////////////////
typedef struct MotionFlag
{
	BOOL m_bProcessThread;
	BOOL m_bAutoStart;
	BOOL m_bFirstStart;
	BOOL m_bAlarm;
}typeMotionFlag;


class CMotionData
{
public:

	CMotionData();
	virtual ~CMotionData();

	//Motion process status
	typeMotionProcessSts m_ProSts[MOT_STATUS_MAX_NUM][MOT_STATUS_SUB_MAX_NUM];

	//Motion sequence
	typeMotionSeq m_Seq[MOT_STATUS_MAX_NUM];

	void ResetSeq();
};


// CPMInspectDlg 대화 상자
class CPMInspectDlg : public CDialogEx
{
// 생성입니다.
public:
	CPMInspectDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CPMInspectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PMINSPECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
private:
	//Motion Flag
	typeMotionFlag m_Flag;
	//Process result
	//typeProcessResult m_Rslt;

	CMotionData m_MDat[MOT_PROCESS_MAX_NUM];
	CWinThread   *m_pThread;
	UINT static   MotionThread(LPVOID pParam);

	void MotionThread();
	int MotionProcess(int prc_idx, int seq_idx);
	int StopModeOperation(int *prc_idx, int *seq_idx);
	//void SetLotInit();
	void SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx);
	void SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx, int next_seq_sub_idx, int type);
	void SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx, int next_seq_sub_idx, int type, int device_idx);
	int CheckTimeout(int prc_idx, int seq_idx, int seq_sub_idx, char* status_type);
	int CheckDelay(int prc_idx, int seq_idx, int delay);

//protected:
public:
	P_IMGALIGN					m_pDispAlign;
	CDialogDispMain*			m_pDlgDispMain;
	CDialogTeach*				m_pDlgTeach;
	CDialogModel*				m_pDlgModel;
	CDialogSystem*				m_pDlgSystem;
	CDialogMotion*				m_pDlgMotion;
	CDialogAlarm*				m_pDlgAlarm;
	CDialogResultView*			m_pDlgResult;
	
	CBtnenhctrl m_btnDate;
	CBtnenhctrl m_btnTap[6];
	CBtnenhctrl m_btnStart;
	CBtnenhctrl m_btnStop;
	CBtnenhctrl m_btnReset;
	CBtnenhctrl m_btnJudgement;
	CBtnenhctrl m_btnModelName;

	int m_nCountTotal;
	int m_nCountGood;
	int m_nCountNg;
	CBtnenhctrl m_btnCountTotal;
	CBtnenhctrl m_btnCountGood;
	CBtnenhctrl m_btnCountNg;
	CBtnenhctrl m_btnOpStatus;
	CBtnenhctrl m_btnVacStatus;
	
	CListCtrl m_ctrl_listInspect;
	CListCtrl m_ctrl_listSystem;

	int m_nTapID;
	BOOL m_bFlagInspect;

	BOOL InitDialog();
	BOOL ShowDialog();
	int HandleAllMessage();

	int AutoStart();
	void AutoStop();
	int AutoReset();
	void MakeDefaultDir();
	void SetCount();
	void SetChangeModel();

public:
	void Display();
	void SystemListDisplay(BOOL save, CString data);
	void InspectListDisplay(BOOL save, CString data);
	void SetOpStatus(CString str);

	char* GetCurrentModelPath() { return m_pDlgModel->GetPath();	}
	char* GetCurrentModelName() { return m_pDlgModel->GetName(); }
	void SetFirstStart(BOOL state) { m_Flag.m_bFirstStart = state; }

	LRESULT OnMotMessage(WPARAM para0, LPARAM para1);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void OnClickAutoRun();
	void ClickBtnenhctrlButtonReset();
	void ClickBtnenhctrlCountReset();
};

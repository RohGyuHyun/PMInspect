#pragma once
//COLOR
// Color Sample
const COLORREF BACKGROUND_COLOR = RGB(230, 230, 230);
const COLORREF CLOUDBLUE = RGB(128, 184, 223);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LTGRAY = RGB(192, 192, 192);
const COLORREF LTYELLOW = RGB(255, 255, 128);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF DKBLUE = RGB(0, 0, 220);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);
const COLORREF BASIC = RGB(212, 208, 200);
const COLORREF LTSKYBLUE = RGB(128, 255, 255);
const COLORREF LTVIOLET = RGB(157, 157, 255);
const COLORREF DATA_ITEM_CHANGE_BACKGROUND_COLOR = CYAN;
const COLORREF DATA_ITEM_CHANGE_TEXT_COLOR = BLACK;
const COLORREF DATA_ITEM_TEXT_COLOR = BLACK;
const COLORREF DATA_ITEM_BACKGROUND_COLOR = CYAN;

#define SYSTEM_LOG			0
#define TACTTIME_LOG		1
#define VISION_LOG			2

#define MAX_CHAR			512
#define MAX_BUF				2

#define DATA_BASIC_PATH				_T("d:\\Database")
#define DATA_SYSTEM_PATH			_T("d:\\Database\\system")
#define DATA_MODEL_PATH				_T("d:\\Database\\model")

#define SYSYEM_LOG_PATH				_T("D:\\PMInsLog\\SystemReport")
#define TACTTIME_LOG_PATH			_T("D:\\PMInsLog\\TactTimeReport")
#define VISION_LOG_PATH				_T("D:\\PMInsLog\\InspReport")
#define VISION_IMAGE_PATH			_T("D:\\PMInsLog\\InspImage")

extern "C" __declspec(dllexport) BOOL FileDelete(CString FilePath, CString FileName);
extern "C" __declspec(dllexport) BOOL FileSearch(CString FilePath, CString FileName);
extern "C" __declspec(dllexport) BOOL MakeDirectories(CString path);


/*
#define MARK_RES_CNT			2

typedef struct AlignData
{
	int m_nAlignLight;
	double m_dAlignMarkSize;
	double m_dAlignMarkRes[MARK_RES_CNT];
	double m_dAlignPixelSize;

	//패턴영역
	RECT m_InsRect;
	RECT m_PatRect;

	Hlong m_PatIDAlign;
	BOOL m_bCreatPatIDAlign;
}typeAlignData;
*/

typedef struct AlignResult
{
	double m_dFindPosX_pixel;					//찾은 mark중심
	double m_dFindPosY_pixel;
	double m_dFindPosA_pixel;					//찾은 mark각도(사용않함)
	double m_dFindPosX_mm;						//찾은 mark중심(mm)
	double m_dFindPosY_mm;
	double m_dTeachPosX_mm;						//티칭 mark중심(mm)
	double m_dTeachPosY_mm;

	double m_dFindScore;						//찾은 score
	double m_dFindNum;							//찾은 개수(1이여야 찾은것임)

	double m_dFindDiffX_pixel;					//찾은 mark - 티칭(pixel)		
	double m_dFindDiffY_pixel;
	double m_dFindDiffCentX_pixel;				//찾은 mark - 카메라중심(pixel)
	double m_dFindDiffCentY_pixel;
	double m_dFindDiffX_mm;						//찾은 mark - 티칭(mm)
	double m_dFindDiffY_mm;
	double m_dFindDiffCentX_mm;					//찾은 mark - 카메라중심(mm)
	double m_dFindDiffCentY_mm;

	double m_dRobotPosX_afteralign;									//Align후 위치 X
	double m_dRobotPosY_afteralign;									//Align후 위치 Y
	double m_dRobotPosT_afteralign;									//Align후 위치 T

	double m_dRobotPosX_afteroffset;								//Align후 offset적용 위치 X
	double m_dRobotPosY_afteroffset;								//Align후 offset적용 위치 Y
	double m_dRobotPosT_afteroffset;								//Align후 offset적용 위치 T

																	//좌표계 형성///////////////////////////////////////////////////////////////////////////////////////////
																	//Camera 좌표계 중심 위치
	double m_dCoordCntX;
	double m_dCoordCntY;

	//mark0에서부터 Mark1까지 X, Y거리
	double m_dDistInterMarkX;
	double m_dDistInterMarkY;

	//Aling0번 카메라의 중심이 좌표계 중심임.
	double m_dCoordAng_teach;										//티칭당시 mark0, mark1각도
	double m_dCoordAng_rslt;										//결과 mark0, mark1각도
	double m_dCoordX_rslt;
	double m_dCoordY_rslt;
	double m_dCoordAng_diff;										//결과 - 티칭
	double m_dCoordX_diff;
	double m_dCoordY_diff;

	double m_dCoordMarkCntX_teach;				//티칭당시 mark0, mark1 중심		
	double m_dCoordMarkCntY_teach;
	double m_dCoordMarkCntX_rslt;				//결과 mark0, mark1 중심		
	double m_dCoordMarkCntY_rslt;

	double m_dCoordMarkCntX_rslt_afterRot;		//결과 mark0, mark1 중심을 회전한 결과
	double m_dCoordMarkCntY_rslt_afterRot;

	double m_dCoordShiftX;
	double m_dCoordShiftY;
}typeAlignResult;

void DoEvents();
void Delay(long ms, BOOL bEvent);

BOOL WriteLog(CString strMsg, int nKey);
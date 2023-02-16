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

	//���Ͽ���
	RECT m_InsRect;
	RECT m_PatRect;

	Hlong m_PatIDAlign;
	BOOL m_bCreatPatIDAlign;
}typeAlignData;
*/

typedef struct AlignResult
{
	double m_dFindPosX_pixel;					//ã�� mark�߽�
	double m_dFindPosY_pixel;
	double m_dFindPosA_pixel;					//ã�� mark����(������)
	double m_dFindPosX_mm;						//ã�� mark�߽�(mm)
	double m_dFindPosY_mm;
	double m_dTeachPosX_mm;						//ƼĪ mark�߽�(mm)
	double m_dTeachPosY_mm;

	double m_dFindScore;						//ã�� score
	double m_dFindNum;							//ã�� ����(1�̿��� ã������)

	double m_dFindDiffX_pixel;					//ã�� mark - ƼĪ(pixel)		
	double m_dFindDiffY_pixel;
	double m_dFindDiffCentX_pixel;				//ã�� mark - ī�޶��߽�(pixel)
	double m_dFindDiffCentY_pixel;
	double m_dFindDiffX_mm;						//ã�� mark - ƼĪ(mm)
	double m_dFindDiffY_mm;
	double m_dFindDiffCentX_mm;					//ã�� mark - ī�޶��߽�(mm)
	double m_dFindDiffCentY_mm;

	double m_dRobotPosX_afteralign;									//Align�� ��ġ X
	double m_dRobotPosY_afteralign;									//Align�� ��ġ Y
	double m_dRobotPosT_afteralign;									//Align�� ��ġ T

	double m_dRobotPosX_afteroffset;								//Align�� offset���� ��ġ X
	double m_dRobotPosY_afteroffset;								//Align�� offset���� ��ġ Y
	double m_dRobotPosT_afteroffset;								//Align�� offset���� ��ġ T

																	//��ǥ�� ����///////////////////////////////////////////////////////////////////////////////////////////
																	//Camera ��ǥ�� �߽� ��ġ
	double m_dCoordCntX;
	double m_dCoordCntY;

	//mark0�������� Mark1���� X, Y�Ÿ�
	double m_dDistInterMarkX;
	double m_dDistInterMarkY;

	//Aling0�� ī�޶��� �߽��� ��ǥ�� �߽���.
	double m_dCoordAng_teach;										//ƼĪ��� mark0, mark1����
	double m_dCoordAng_rslt;										//��� mark0, mark1����
	double m_dCoordX_rslt;
	double m_dCoordY_rslt;
	double m_dCoordAng_diff;										//��� - ƼĪ
	double m_dCoordX_diff;
	double m_dCoordY_diff;

	double m_dCoordMarkCntX_teach;				//ƼĪ��� mark0, mark1 �߽�		
	double m_dCoordMarkCntY_teach;
	double m_dCoordMarkCntX_rslt;				//��� mark0, mark1 �߽�		
	double m_dCoordMarkCntY_rslt;

	double m_dCoordMarkCntX_rslt_afterRot;		//��� mark0, mark1 �߽��� ȸ���� ���
	double m_dCoordMarkCntY_rslt_afterRot;

	double m_dCoordShiftX;
	double m_dCoordShiftY;
}typeAlignResult;

void DoEvents();
void Delay(long ms, BOOL bEvent);

BOOL WriteLog(CString strMsg, int nKey);
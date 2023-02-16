// _FuncModuleBasic.h : main header file for the _FUNCMODULEBASIC DLL
//

#if !defined(AFX__FUNCMODULEBASIC_H__354C2BE2_D63A_4425_8B03_4A94985EDE08__INCLUDED_)
#define AFX__FUNCMODULEBASIC_H__354C2BE2_D63A_4425_8B03_4A94985EDE08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifdef _DLLEXPORT_BASIC
#define DLLAPI_BASIC extern "C" __declspec(dllexport)
#else
#define DLLAPI_BASIC extern "C" __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int CALLBACK sLock(UINT FuncCode, UINT Pass1, UINT Pass2, UINT InPut1, UINT InPut2, UINT *Retrun1, UINT *Return2);
#ifdef __cplusplus
};
#endif

#include <math.h>

//Common
#define DAT_STR_LENG								512
//PI
#define MATHFUNC_PI									3.14159265358979323846

/////////////////////////////////////////////////////////////////////////////
// C_FuncModuleBasicApp
// See _FuncModuleBasic.cpp for the implementation of this class
//

class C_FuncModuleBasicApp : public CWinApp
{
public:
	C_FuncModuleBasicApp();

private:

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C_FuncModuleBasicApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(C_FuncModuleBasicApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////
//General////////////////////////////////////////
/////////////////////////////////////////////////
//
//

extern "C" __declspec(dllexport) BOOL CopyFolder(CString strFromFolder, CString strToFolder);
/// 파일을 한개 씩 지우기
extern "C" __declspec(dllexport) void RemoveFileRecurse(LPCTSTR pstr, long period);
/// 설정한 디렉토리에 있는 파일을 한번에 모두 지우기
extern "C" __declspec(dllexport) void RemoveCheckDirectory(LPCTSTR pstr_f, long period_f);
/// 디렉토리를 체크하고 위에 2개의 함수를 이용하여 파일 지우기
extern "C" __declspec(dllexport) void RemoveFileSRecurse(LPCTSTR pstr, long period);
//Registry
extern "C" __declspec(dllexport) BOOL GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lplongValue, long length);
extern "C" __declspec(dllexport) BOOL SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, LPCTSTR lplongValue);

//Get current time
extern "C" __declspec(dllexport) void GetTime(char* str);

//Make directory
extern "C" __declspec(dllexport) void MakeDirectories(CString path);

//Send current status
extern "C" __declspec(dllexport) void SendStatus(HWND hnd, unsigned long msg, char *status, char* rslt);

//Get path
extern "C" __declspec(dllexport) void SplitPath(LPCTSTR fullPath, CString& drive, CString& path, CString& name, CString& ext);

//Log
extern "C" __declspec(dllexport) BOOL WriteLog(CString path, CString file, CString bak_file, CString err, long limit_line);
extern "C" __declspec(dllexport) BOOL WriteLogWithTimeFileName(CString path, CString file, CString err, long limit_file);

/////////////////////////////////////////////////
//Math///////////////////////////////////////////
/////////////////////////////////////////////////
//
//

//Theta 의 중심위치
extern "C" __declspec(dllexport) void CenterOfTheta(double cam_on_ic_x, double cam_on_ic_y,		
					  double cam_on_theta_x, double cam_on_theta_y,	
					  double *center_theta_x, double *center_theta_y);
//선분이 이루는 사잇각
extern "C" __declspec(dllexport) void AngBetween(double ax, double ay, double bx, double by,		
					 double ax2, double ay2, double bx2, double by2,
					 double *ang);
//사잇각 만큼 회전변환
extern "C" __declspec(dllexport) void RotatedPt(double x, double y,				
				  double theta_x, double theta_y,	
				  double ang,						
				  double *rx, double *ry);
//기준으로 부터의 거리
extern "C" __declspec(dllexport) void DistXY(double rx, double ry,				
				  double ax, double ay,
				  double *distX, double *distY);

//두점의 거리
extern "C" __declspec(dllexport) void DistPP(double sx, double sy,					
								  double ex, double ey,					
								  double *dist);							

/////////////////////////////////////////////////
//Vision Algorithm///////////////////////////////
/////////////////////////////////////////////////
//
//
#define MAX_LABEL 10000

//Blob parameter
typedef struct Blob
{
	int m_iCX;
	int m_iCY;
	long m_lArea;
	int m_iLabelIdx;
}typeBlob;

//칼라분석
extern "C" __declspec(dllexport) void RGBtoXYZ(double R, double G, double B, double *X, double *Y, double*Z);
extern "C" __declspec(dllexport) void XYZtoLUV(double X, double Y, double Z, double *L, double *U, double*V);
extern "C" __declspec(dllexport) void XYZtoLAB(double X, double Y, double Z, double *L, double *A, double *B);
extern "C" __declspec(dllexport) double delta_E(double L0, double A0, double B0, double L1, double A1, double B1);
extern "C" __declspec(dllexport) double delta_E_RGB(double R0, double G0, double B0, double R1, double G1, double B1);

//이미지 빼기
extern "C" __declspec(dllexport) void SubtractImage(long width, long height, long pitch, BYTE *by_source0, BYTE *by_source1, BYTE *by_source2);
//티칭편차
extern "C" __declspec(dllexport) void DiffTeachImage(long width, long height, long pitch, BYTE *by_source0, BYTE *by_source1, BYTE *by_source2);
//평균 이미지(3x3) 
extern "C" __declspec(dllexport) void MeanImage(long width, long height, long pitch, BYTE *by_source0, BYTE *by_source1);
//평균 이미지(5x5)
extern "C" __declspec(dllexport) void MeanImage2(long width, long height, long pitch, BYTE *by_source0, BYTE *by_source1);
//마스크 이미지 만들기
extern "C" __declspec(dllexport) void MaskingImage(RECT rect, long width, long height, long pitch, int var, BYTE *by_source0, BYTE *by_source1);
//이진화 이미지
extern "C" __declspec(dllexport) void ThresholdImage(long width, long height, long pitch, BYTE *by_source_mask, BYTE *by_source_compen, BYTE *by_source0, BYTE *by_source1);
//라벨링 이미지(blob)
extern "C" __declspec(dllexport) int LabelingImage(long width, long height, long pitch, BYTE *by_source0, int *by_source1);
//Blob Feature
extern "C" __declspec(dllexport) int BlobFeatureImage(RECT rect, long width, long height, long pitch, int size, int *by_source0, int *by_source1, typeBlob *blob);
//필요없는 라인 없에기
//얇은선 제거 _______________|__________________  ==> _|_
extern "C" __declspec(dllexport) void RemoveThinLine(RECT rect, BYTE *by_source0, BYTE *by_source1, long pitch);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX__FUNCMODULEBASIC_H__354C2BE2_D63A_4425_8B03_4A94985EDE08__INCLUDED_)

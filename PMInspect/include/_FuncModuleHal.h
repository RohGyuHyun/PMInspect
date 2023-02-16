// _FuncModuleHal.h : main header file for the _FUNCMODULEHAL DLL
//

#if !defined(AFX__FUNCMODULEHAL_H__797BB2AE_2F00_4787_8EE5_6218973ADE93__INCLUDED_)
#define AFX__FUNCMODULEHAL_H__797BB2AE_2F00_4787_8EE5_6218973ADE93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifdef _DLLEXPORT_HAL
#define DLLAPI_HAL extern "C" __declspec(dllimport)
#else
#define DLLAPI_HAL extern "C" __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int CALLBACK sLock(UINT FuncCode, UINT Pass1, UINT Pass2, UINT InPut1, UINT InPut2, UINT *Retrun1, UINT *Return2);
#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int CALLBACK sLock(UINT FuncCode, UINT Pass1, UINT Pass2, UINT InPut1, UINT InPut2, UINT *Retrun1, UINT *Return2);
#ifdef __cplusplus
};
#endif

//Model type
#define VISIONHALFUNC_MOD_RECT1_NODRAW				0
#define VISIONHALFUNC_MOD_RECT1						1
#define VISIONHALFUNC_MOD_RECT2						2
#define VISIONHALFUNC_MOD_RAND						3
#define VISIONHALFUNC_MOD_CIRCLE					4
#define VISIONHALFUNC_MOD_FITCIRCLE					5

//license check type
#define VISIONHALFUNC_LICENSE_TYPE_LAN				"lmhostid -ether"
#define VISIONHALFUNC_LICENSE_TYPE_DONGLE			"lmhostid -flexid"

#define VISIONHALFUNC_CAL_UNIT						"mm"
#define VISIONHALFUNC_MIN_PROCESS_SIZE				-32700
#define VISIONHALFUNC_MAX_PROCESS_SIZE				32700

#define VISIONHALFUNC_ZOOM_RESET					0
#define VISIONHALFUNC_ZOOM_IN						1
#define VISIONHALFUNC_ZOOM_OUT						2

//Circle
typedef struct CIRCLE
{
	Hlong cx;
	Hlong cy;
	Hlong r;
}typeCIRCLE;

//Rectangle2
typedef struct RECT2
{
	Hlong cx;
	Hlong cy;
	Hlong hwidth;
	Hlong hheight;
	double ang;
}typeRECT2;

//Point(double)
typedef struct DPOINT
{
	double x;
	double y;
}typeDPOINT;


//Pattern parameter(mark)
typedef struct PatPara
{
	//Region type
	int m_shType;

	//Searching(pattern) parameter
	double m_AngleStart;
	double m_AngleExtend;
	double m_AngleStep;
	char  m_Optimize[50];
	char  m_Matric[50];
	char  m_SupPix[50];
	double m_Overlap;

	double m_MatchNum;
	double m_Greed;
	double m_ScaleMin;
	double m_ScaleMax;
	double m_ScaleStep;

}typePatPara;

//Pattern parameter(edge)
typedef struct EdgePara
{
	//Edge parameter
	char  m_Transition[50];
	char  m_Select[50];
	char  m_Interpolation[50];
}typeEdgePara;


/////////////////////////////////////////////////////////////////////////////
// C_FuncModuleHalApp
// See _FuncModuleHal.cpp for the implementation of this class
//

class C_FuncModuleHalApp : public CWinApp
{
public:
	C_FuncModuleHalApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C_FuncModuleHalApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(C_FuncModuleHalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////
//Function///////////////////////////////////////
/////////////////////////////////////////////////
//
//

//License check//////////////////////////////
//
//
extern "C" __declspec(dllexport) BOOL Hal_License_LanNum(LPSTR type, CString* out);
//
//
/////////////////////////////////////////////

//Shape pattern matching/////////////////////
//
//
//Halcon : model teaching
//(RECT)
extern "C" __declspec(dllexport) BOOL Hal_ModelTeachingRect1(Hlong win, Hobject* img, RECT* mod_region, Hlong img_width, Hlong img_hight,
					   typePatPara para, 
	                   int thresh, int minthresh, 
					   double* x, double* y, Hobject* mod_img, Hlong* mod_id);

//(RECT2)
extern "C" __declspec(dllexport) BOOL Hal_ModelTeachingRect2(Hlong win, Hobject* img, typeRECT2* mod_region, Hlong img_width, Hlong img_hight,
	    				typePatPara para, int thresh, int minthresh, 
						double* x, double* y, Hobject* mod_img, Hlong* mod_id);


//(CIRCLE)
extern "C" __declspec(dllexport) BOOL Hal_ModelTeachingCircle(Hlong win, Hobject* img, typeCIRCLE* mod_region, Hlong img_width, Hlong img_hight,
						typePatPara para, int thresh, int minthresh, 
						double* x, double* y, Hobject* mod_img, Hlong* mod_id);

//Halcon : model finding 
//(RECT)
extern "C" __declspec(dllexport) BOOL Hal_ShapePatternFind(Hlong win, Hobject* img, Hlong mod_id, typePatPara para, 
					  RECT mod_rgn_size, Hlong img_width, Hlong img_hight,
					  double cx, double cy, double score,
					  Hobject* mod_rgn, Hobject* shapes, double* rx, double* ry, double* ra, double*rs, double*rscore, Hlong*rn);


//Halcon : find shape patterns
//(RECT)
extern "C" __declspec(dllexport) BOOL Hal_ShapePatternFindsRect(Hlong win, Hobject* img, Hlong* mod_id, BOOL* mod_use, int mod_num, typePatPara para,
						   RECT mod_rgn_size, Hlong img_width, Hlong img_hight,
						   double cx, double cy, double score, 
						   Hobject* mod_rgn, Hobject* shapes, double* rx, double* ry, double* ra, double*rs, double*rscore, Hlong*rn);

//(RECT2)
extern "C" __declspec(dllexport) BOOL Hal_ShapePatternFindRect2(Hlong win, Hobject* img, Hlong mod_id, typePatPara para,
					   typeRECT2 mod_rgn_size, Hlong img_width, Hlong img_hight,
					   double cx, double cy, double score, 
				       Hobject* mod_rgn, Hobject* shapes, double* rx, double* ry, double* ra, double*rs, double*rscore, Hlong*rn);
//(CIRCLE)
extern "C" __declspec(dllexport) BOOL Hal_ShapePatternFindCirc(Hlong win, Hobject* img, Hlong mod_id, typePatPara para,
					   typeCIRCLE mod_rgn_size, Hlong img_width, Hlong img_hight,
					   double cx, double cy, double score, 
				       Hobject* mod_rgn, Hobject* shapes, double* rx, double* ry, double* ra, double*rs, double* rscore, Hlong*rn);

//Halcon : model finding display
extern "C" __declspec(dllexport) void Hal_ShapePatternFindDisp(Hlong win, Hobject mod_rgn, Hobject shapes, double rx, double ry, double teach_x, double teach_y);
//
//
/////////////////////////////////////////////

//Edge///////////////////////////////////////
//
//
//Halcon : pair edge measuring
extern "C" __declspec(dllexport) BOOL Hal_PairEdgeMeasure(Hlong win, Hobject* img, typeEdgePara para,
					     typeRECT2 mod_rgn_size, Hlong img_width, Hlong img_height, double thresh, double sigma,  int edge_max_num,
					     double* rfx, double* rfy, double* rsx, double* rsy, double* gap, double* width, Hlong*rn);

//Halcon : measure single vertical edge
extern "C" __declspec(dllexport) BOOL Hal_EdgeMeasureVertical(Hlong win, Hobject* img, typeEdgePara para,
						     RECT mod_rgn_size, Hlong img_width, Hlong img_height, double thresh, double sigma, int edge_max_num,
						     double* rx, double* ry, int*rn);
//RECT2
extern "C" __declspec(dllexport) BOOL Hal_EdgeMeasure(Hlong win, Hobject* img, typeEdgePara para,
					 typeRECT2 mod_rgn_size, Hlong img_width, Hlong img_height, 
					 double thresh, double sigma, int edge_max_num,
					 double* rx, double* ry, int*rn);

//Halcon : measure edge pos-get diameter
//RECT2
extern "C" __declspec(dllexport) BOOL Hal_EdgeMeasureGetDiameter(Hlong win, Hobject* img, typeEdgePara para,
							             typeRECT2 mod_rgn_size, Hlong img_width, Hlong img_height, 
							             double thresh, double sigma,
						                 double* rsx, double* rsy, double* rex, double* rey, int*rn);

//Halcon : measure pair edge display
extern "C" __declspec(dllexport) void Hal_PairEdgeMeasureDisp(Hlong win, typeRECT2 mod_rgn_size, double* rfx, double* rfy, 
	     				  double* rsx, double* rsy, double* gap, double* width, Hlong rn);

//Halcon : measure sigle vertical edge display
extern "C" __declspec(dllexport) void Hal_EdgeMeasureVerticalDisp(Hlong win, RECT mod_rgn_size, double* rx, double* ry, Hlong rn);

//Halcon : measure edge display
extern "C" __declspec(dllexport) void Hal_EdgeMeasureDisp(Hlong win, typeRECT2 mod_rgn_size, double* rx, double* ry, Hlong rn);

//Halcon : measure edge-get diameter display
extern "C" __declspec(dllexport) void Hal_EdgeMeasureGetDiameterDisp(Hlong win, typeRECT2 mod_rgn_size, double rsx, double rsy, double rex, double rey, Hlong rn);
//
//
/////////////////////////////////////////////

//Calibration////////////////////////////////
//
//
//Make cal tab image
extern "C" __declspec(dllexport) BOOL Hal_MakeCalTabImagebyPosition(Hlong win, Hobject* image, Hlong img_width, Hlong img_hight, 
											double* x, double* y, 
											double real_pit_x, double real_pit_y, double* real_rad,
											int x_num, int y_num);

//Camera calibration(get camra pixel center, real center, pose, parameter)
extern "C" __declspec(dllexport) BOOL Hal_CameraCalibration(Hlong win, Hobject img, CString path, HTuple* campara, HTuple* campose, 
							double* cam_center_pixelx, double* cam_center_pixely, double* cam_center_realx, double* cam_center_realy, 
							double* px_size, double* py_size);
//Camera calibration test
extern "C" __declspec(dllexport) BOOL Hal_CameraCalibrationTest(Hlong win, Hobject img, HTuple* campara, HTuple* campose);
//Get real points
extern "C" __declspec(dllexport) BOOL Hal_PointstoWorldCal(HTuple param, HTuple pose, double x, double y, double* rx, double* ry);
//Make calibration file
extern "C" __declspec(dllexport) BOOL Hal_MakeCalibrationFile(CString path, double pitx, double pity, int cntx, int cnty, double dia);
//Make calibration file
extern "C" __declspec(dllexport) BOOL Hal_MakeCalibrationFile2(CString path_file, double* realx, double* realy, int cntx, int cnty, double dia);
//
//
/////////////////////////////////////////////

//Alignment//////////////////////////////////
//
//

//마크의 위치를 로보트 좌표의 위치로 변환 
extern "C" __declspec(dllexport) void Hal_AlignMarkPosToRoboPos(HTuple cam_para, HTuple cam_pos, 
							 double mark_pos_pixel_x, double mark_pos_pixel_y,
							 double cam_cent_real_x, double cam_cent_real_y,
							 double robo_pos_real_x, double robo_pos_real_y,
							 double robo_pos_offset_real_x, double robo_pos_offset_real_y,
							 double* mark_robo_pos_real_x, double* mark_robo_pos_real_y);
//
//
/////////////////////////////////////////////

//Region/////////////////////////////////////////
//
//
//Make regions
//(CIRCLE)
extern "C" __declspec(dllexport) BOOL Hal_MakeRegions(Hlong win,  HTuple color, typeCIRCLE circle, Hlong cx, Hlong cy, int num, Hobject* regions);
//
//
/////////////////////////////////////////////

//Image processing///////////////////////////////
//
//
//(Scale)
extern "C" __declspec(dllexport) BOOL Hal_ScaleImage(Hobject image,  Hobject* scaled_image, Hlong img_width, Hlong img_hight, int bright, int contrast);
//
//
/////////////////////////////////////////////

//Image//////////////////////////////////////
//
//
//Open halcon image
extern "C" __declspec(dllexport) BOOL Hal_OpenHalImage(Hobject* img, CString path_name, BOOL draw);
extern "C" __declspec(dllexport) BOOL Hal_SaveHalImage(Hobject* img, CString path_name, CString kind);
//
//
/////////////////////////////////////////////

/////////////////////////////////////////////////
//Script/////////////////////////////////////////
/////////////////////////////////////////////////
//
//
//포리코리아 2006.04/////////////////////////////
//
//
//Find center
extern "C" __declspec(dllexport) BOOL HalScript_Audit_FindCenter(Hlong win, double pixel_size, double cntx_mm, double cnty_mm,
					    double standardx, double standardy, 
						double *cntx_pixel, double *cnty_pixel, double *radius_pixel);

//Find measure position
extern "C" __declspec(dllexport) BOOL HalScript_Audit_FindMeasurePos(Hlong win, int type, double cntx, double cnty,
								   double foundx_type0, double foundy_type0, double foundx_type1, double foundy_type1, 
								   double measure_dist, double *measure_posx, double *measure_posy);
//
//
/////////////////////////////////////////////

//두성테크 2007.04///////////////////////////
//
//
//Empty region(등록시점) : 밝은 영역추출(밝기 값 이용).
extern "C" __declspec(dllexport) BOOL HalScript_Dome_GetEmpty(Hobject* reduced_img, 
							 Hlong min_thrs, Hlong min_area,
							 Hobject* rslt_conn_rgn, Hobject* rslt_union_rgn, Hobject* rslt_union_rgn_compl);

//Base region : 어두운 영역추출(밝기 값 이용).
//(등록시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_GetBase(Hlong win, Hobject* img, Hobject* rgn, 
	                        Hobject* dome_rgn,
							Hlong max_thrs, Hlong min_area,
							double spc_clos,
							BOOL not_use_back_light,
							Hobject* rslt_base_conn_rgn, Hobject* rslt_base_union_rgn, 
							Hobject* rslt_spc_conn_rgn, Hobject* rslt_spc_union_rgn);

//Dome region #1(등록시점) : Dome영역 중에서 안쪽 어두운 영역 부분.
extern "C" __declspec(dllexport) BOOL HalScript_Dome_GetDomeIn(Hlong win, Hobject* reduced_img, 
						  int dome_thrs, int space_thrs, Hlong min_area, Hlong max_area, double dome_open, double space_sens,
						  BOOL not_use_back_light,
						  Hobject* rslt_conn_rgn, Hobject* rslt_conn_cvx_rgn, Hobject* rslt_union_cvx_rgn, Hlong* rslt_num);
//쉬트영역 이물 검사하기(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectParticleInSheet(Hlong win, Hobject* img, Hobject* rgn,
										  Hobject* taught_sheet_conn_rgn,
										  Hlong base_max_thrs, Hlong spc_min_area, 
										  double clos, double spc_dila,
										  int sheet_thrs, int offset_thrs_light, int offset_thrs_dark, Hlong min_area, Hlong mean_mask, Hlong except_mask,
										  int select_pola, 
										  double part_width, double part_height,
										  BOOL not_use_back_light,
										  Hobject* rslt_spc_ins_conn_rgn, Hobject* rslt_spc_ins_union_rgn,
										  Hobject* rslt_base_ins_conn_rgn, Hobject* rslt_base_ins_union_rgn,
										  Hobject* rslt_base_particle_conn_rgn, Hlong* rslt_num);

//SPACE영역 이물 검사하기(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectParticleInSpace(Hlong win, Hobject* img, Hobject* rgn, Hobject* taught_sheet_conn_rgn,
										   Hobject* runtime_dome_ins_conn_rgn, Hobject* runtime_spc_ins_union_rgn,
										   int spc_particle_thrs, Hlong spc_prticel_min_area,
										   Hlong part_width, Hlong part_height,
										   double spc_eros,
										   BOOL not_use_back_light,
										   Hobject* rslt_spc_ins_union_rgn, Hobject* rslt_spc_particle_conn_rgn, Hlong* rslt_num);

//Dome영역 추출(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectDomeNum(Hlong win, Hobject* img, Hobject* rgn,  Hobject* taught_dome_conn_rgn,
								   int dome_thrs, int space_thrs, Hlong dome_min_area, Hlong dome_max_area, double dome_area_ratio_tol,
								   double dome_open, double space_sens, BOOL only_miss, BOOL check_area, BOOL not_use_back_light,
								   Hobject* rslt_dome_conn_rgn, Hlong* rslt_num);

//Dome영역 이물검사 하기(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectParticleInDome(Hlong win, Hobject* img, Hobject* rgn, Hobject* runtime_dome_conn_rgn,
										  int dome_part_offset_thrs, int dome_part_offset_thrs_light,
										  Hlong dome_part_min_area, 
										  Hlong mean_mask, Hlong except_mask,
										  double dome_eros,
										  int select_pola,
										  Hlong part_width,
										  Hlong part_height,
										  Hobject* rslt_dome_union_ins_rgn,
										  Hobject* rslt_dome_particle_conn_rgn, Hlong* rslt_num);

//역Dome 검사(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectReverseDome(Hlong win, Hobject* img, Hobject* runtime_dome_conn_rgn,
									   int diff_intst,
	                                   double dome_eros,
									   Hobject* rslt_dome_up_dn_union_rgn,
									   HTuple* rslt_dome_up_area, HTuple* rslt_dome_dn_area,
									   HTuple* rslt_dome_rvrs_sts, Hlong* rslt_rvrs_num);
//편심 검사(검사시점)
extern "C" __declspec(dllexport) BOOL HalScript_Dome_InspectDomePos(Hlong win, Hobject* img, Hlong img_width, Hlong img_height,
								   Hobject* runtime_dome_conn_rgn,
								   typeEdgePara para, int measure_num, int measure_width,
								   int measure_thrs, double measure_alpha,
								   double min_dist,
								   Hobject* rslt_pos_msre_union_rgn, 
								   HTuple* rslt_msre_pos, Hlong* rslt_num, Hlong* rslt_dome_num, 
								   HTuple* rslt_msre_ng_pos, Hlong* rslt_ng_num, 
								   Hobject* rslt_ng_dome_conn_rgn,
								   Hlong* rslt_dome_ng_num);
//
//
/////////////////////////////////////////////

//Intensity scan/////////////////////////////
//
//
extern "C" __declspec(dllexport) int IntensityScan1(RECT ins_rgn, Hobject *source_image, Hobject *candiate_image, Hobject *result_image, float thresh, int start, int end, int comp_type, BOOL var_scan, float thresh_ratio);
extern "C" __declspec(dllexport) int IntensityScan0(Hlong win, RECT ins_rgn, Hobject *source_image, Hobject *candiate_image, int start, int end, int candidate_thresh, float contour_thresh);
//
//
/////////////////////////////////////////////

/////////////////////////////////////////////
//ANI/AVI for STECO 2010
//
extern "C" __declspec(dllexport) BOOL Hal_ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, 
										int thresh, int high_thresh, int tolerance, int broken_thresh, 
										Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region,
										Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_ChipOut(Hlong win, Hobject *image, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_ChipPot(Hlong win, Hobject *image, RECT ins_region, Hlong pot_area, int thresh, int under_tol, int over_tol, Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region,
										Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region,
										Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_PotPart(Hlong win, Hobject *image, Hobject *crack_image, Hobject chip_ins_region, Hobject pot_ins_region, Hobject sr_region,
				 			 double offset_ox, double offset_oy, 
							 double offset_ox2, double offset_oy2, 
							 double offset_rx, double offset_ry,
							 double offset_rx2, double offset_ry2,
							 int part_thresh, int high_thresh, int tolerance, int range_tolerance, int black_pot_tol, int black_thresh,
							 Hlong *rslt_num, Hobject *rslt_region, Hlong *range_rslt_num, Hobject *range_rslt_region, Hobject *recog_ins_rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height, Hlong recog_dist,
							 Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr);

extern "C" __declspec(dllexport) BOOL Hal_DnLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   double* line_width_avg, 
						   Hlong *rslt_num, Hobject *rslt_region,  
						   Hlong *rslt_num2, Hobject *rslt_region2,
						   Hobject *rslt_dn_region,
						   Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_UpLead(Hlong win, Hobject *image, Hobject *image2,
							RECT ins_region,	Hobject not_sr_region, 
							Hobject up_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int thresh, double factor, int tolerance, 
						   int thresh_dirt, int tolerance_dirt,
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hlong *rslt_num2, Hobject *rslt_region2, 						   
						   Hobject *rslt_up_region,
						   Hlong min_area, Hlong min_width, Hlong min_height);

extern "C" __declspec(dllexport) BOOL Hal_FilmPF(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int high_thresh, int tolerance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr); 
//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
extern "C" __declspec(dllexport) BOOL Hal_FilmMold(Hlong win, Hobject *align_image, Hobject *mold_image, Hobject *ink_image, Hobject *user_image, Hobject *scratch_image,
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
							  Hlong var_id, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int abs_thresh, int var_thresh, int mold_fact, int tolerance, int tolerance_light, int user_thresh, int user_tol,
							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region,
							  Hlong min_area, Hlong min_width, Hlong min_height,
							  Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr);

//
//
/////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX__FUNCMODULEHAL_H__797BB2AE_2F00_4787_8EE5_6218973ADE93__INCLUDED_)

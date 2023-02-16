#pragma once

///////////////////////////////////////////////////////////////
//IO



#define MAX_DIO_MODULE_NUM	4

/*
#define DIO_INPUT_INDEX_1	0
#define DIO_OUTPUT_INDEX_1	1
#define DIO_INPUT_INDEX_2	2
#define DIO_OUTPUT_INDEX_2	3

//INPUT
#define SAFETY_KEY_AUTO_CHECK												0
#define GPS_POWER_FAILURE_ALARM_CHECK							2
#define SERVO_MC_ON_OFF_CHECK											3
#define FRONT_LEFT_DOOR_CHECK											4
#define FRONT_RIGHT_DOOR_CHECK											5
#define REAR_LEFT_DOOR_CHECK												6
#define REAR_RIGHT_DOOR_CHECK											7	
#define OP_EMS_CHECK																8
#define LIGHT_CURTAIN_DETECT_CHECK									9
#define LIGHT_CURTAIN_MUTING_START_CHECK_AUTO				10				//명칭변경  2016.04.07
#define LIGHT_CURTAIN_MUTING_END_CHECK							11				//신규 2016.03.29
#define LD_PNP_CYLINDER_UP_CHECK										12	
#define LD_PNP_CYLINDER_DN_CHECK										13	
#define LIGHT_CURTAIN_MUTING_START_CHECK_MANUAL			15				//신규  2016.03.30
#define START_SWITCH_1															32
#define READY_SWITCH_1															33
#define RESET_SWITCH																34
#define START_SWITCH_2															35
#define READY_SWITCH_2															36
#define MAIN_AIR_CHECK																37
#define LD_PNP_VAC_ON_CHECK												47
#define FILM_JIG_GLASS_CHECK													48
#define FILM_JIG_VAC_ON_CHECK												49
#define FILM_JIG_CYLINDER_OUT_CHECK									50				//자리변경 2016.04.07
#define FILM_JIG_CYLINDER_IN_CHECK										51				//자리변경 2016.04.07 
#define FILM_JIG_GLASS_CLAMP_CYLINDER_BACK_CHECK			52				//자리변경 2016.04.07
#define FILM_JIG_GLASS_CLAMP_CYLINDER_FEED_CHECK			53				//자리변경 2016.04.07
#define FILM_JIG_GLASS_CLAMP_CYLINDER_UNCLAMP_CHECK	54				//자리변경 2016.04.07
#define FILM_JIG_GLASS_CLAMP_CYLINDER_CLAMP_CHECK		55				//자리변경 2016.04.07
#define FILM_JIG_TILTING_CYLINDER_TILT_UP_CHECK				56
#define FILM_JIG_TILTING_CYLINDER_TILT_DN_CHECK				57	
#define FILM_JIG_VAC_PLATE_CYLINDER_UP_CHECK					58
#define FILM_JIG_VAC_PLATE_CYLINDER_DN_CHECK					59
#define GLASS_JIG_VAC_ON_CHECK											60
#define GLASS_JIG_CYLINDER_OUT_CHECK								61				//자리변경 2016.04.07
#define GLASS_JIG_CYLINDER_IN_CHECK									62				//자리변경 2016.04.07
#define TRAY_SHUTTLE_GLASS_VAC_ON_CHECK						63

//OUTPUT
#define TOWER_LAMP_RED															0
#define TOWER_LAMP_YELLOW													1
#define TOWER_LAMP_GREEN														2
#define BUZZER_ERROR_SIGNAL												3
#define SAFETY_KEY_LOCK_ON_OFF											4
#define SAFETY_DOOR_UNLOCK													5
#define LIGHT_CURTAIN_MUTING_ON											6
#define LD_PNP_VAC_ON_SOL														8
#define LD_PNP_VAC_OFF_SOL													9
#define LD_PNP_CYLINDER_DN_SOL											10				 //자리변경 2016.04.07	
#define LD_PNP_CYLINDER_UP_SOL											11				//자리변경  2016.04.07
#define START_SWITCH_LAMP_1													32
#define READY_SWITCH_LAMP_1													33
#define RESET_SWITCH_LAMP														34
#define START_SWITCH_LAMP_2													35
#define READY_SWITCH_LAMP_2													36
#define FILM_JIG_VAC_ON_SOL													40
#define FILM_JIG_VAC_OFF_SOL													41
#define FILM_JIG_CYLINDER_IN_SOL											42
#define FILM_JIG_CYLINDER_OUT_SOL										43
#define FILM_JIG_GLASS_CLAMP_CYLINDER_FEED_SOL								44
#define FILM_JIG_GLASS_CLAMP_CYLINDER_BACK_SOL				45
#define FILM_JIG_GLASS_CLAMP_CYLINDER_CLAMP_SOL			46
#define FILM_JIG_GLASS_CLAMP_CYLINDER_UNCLAMP_SOL		47
#define FILM_JIG_TILTING_CYLINDER_TILT_UP_SOL						48
#define FILM_JIG_TILTING_CYLINDER_TILT_DN_SOL						49
#define FILM_JIG_VAC_PLATE_CYLINDER_UP_SOL						50
#define FILM_JIG_VAC_PLATE_CYLINDER_DN_SOL						51
#define GLASS_JIG_VAC_ON_SOL												52
#define GLASS_JIG_VAC_OFF_SOL												53
//#define GLASS_JIG_CYLINDER_IN_SOL										54
//#define GLASS_JIG_CYLINDER_OUT_SOL									55
#define TRAY_SHUTTLE_GLASS_VAC_ON_SOL							56
#define TRAY_SHUTTLE_GLASS_VAC_OFF_SOL							57
//IO END
////////////////////////////////////////////////////////////////////////
*/


typedef struct DIOINFO
{
	long	m_IBoardNo;
	long	m_IModulePos;
	DWORD	m_dwModuleID;
	CString m_strModuleName;

}typeDIOInfo;


class CIO
{
public:
	CIO(void);
	~CIO(void);

protected:
	typeDIOInfo m_DIOInfo[MAX_DIO_MODULE_NUM];

public:
	BOOL AXTDIOInit();
	BOOL AXTLibInit();
	BOOL AXTLibClose();

	void WriteDO(int nBit, BOOL bVal);
	BOOL ReadDO(int nBit);
	BOOL ReadDI(int nBit);
};
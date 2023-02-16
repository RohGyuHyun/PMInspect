#pragma once
#include "atf_def.h"
#include "atf_lib.h"
#include "common_AF.h"
#include "ui_cmd.h"

#define AFC_STATE_HW_OK								0
#define AFC_STATE_SW_OK								1
#define AFC_STATE_XY_MOTION							2
#define AFC_STATE_Z_MOTION							3
#define AFC_STATE_ENABLE_LASER						4
#define AFC_STATE_LASER_DISABLED_MODE				5
#define AFC_STATE_ENABLED_SYNC_SENSITIVITY			6
#define AFC_STATE_SYNC_MODE							7
#define AFC_STATE_EXECUTE_AF						8
#define AFC_STATE_LASER_TRACKING					9
#define AFC_STATE_Z_TRACKING						10
#define AFC_STATE_NEAR_WINDOW						11
#define AFC_STATE_MIV								12
#define AFC_STATE_IN_FOCUS							13
#define AFC_STATE_FOCUS_REGINING					14
#define AFC_STATE_CACHED_DATA						15

#define AF_MODE			0
#define AOI_MODE		1

class CAtfCtrl
{
public:
	CAtfCtrl();
	virtual ~CAtfCtrl(void);

	BOOL SetAfOnOff(BOOL isOn);
	BOOL GetInFocus();
	BOOL GetLaser();
	BOOL SetAFCLaserOnOff(BOOL isOn) { return AFCLaserOnOff(isOn); };
	BOOL GetAFCState(int nStatusType);
	BOOL GetAFCHWState(int nStatusType);
	BOOL GetConnect() { return m_bConnection; };
	BOOL CloseAFCModoule();
	BOOL AFCConnect();
	BOOL ATFHome();
private:
	BOOL InitAFCModoule();
	

	
	void GetSensorInformation();

	BOOL AFCLaserOnOff(BOOL isOn);
	BOOL AFCLaserMode(BOOL isAuto);
	BOOL AFCAfOn(int nAfModeType);
	BOOL AFCAfOff();
	void WordToBit(WORD word, BOOL *bit);
	void DWordToBit(DWORD word, BOOL *bit);
protected:
	int m_nComPort;
	DWORD m_dwBaud;
	BOOL m_bConnection;
	BOOL m_bLaser;
	BOOL m_bAFOnOff;
	BOOL m_bInFocus;

	int m_iErrOk;
	short m_mag;
	int m_infocus;
	float m_acc;
	unsigned short m_fullstep;
	unsigned short m_microstep;
	float m_speed;
	float m_slope;
	float m_p;

	int m_flag_split_cm;
	int m_flag_ao;
	int m_flag_cm;
	int m_mfc_conf;

	char m_FirmwareBuffer[64];
	char m_FirmwareVer[64];

	const char *m_pSensorName;

	const char *m_pATF2;
	const char *m_pATF4;
	const char *m_pATF5;
	const char *m_patf6;
	const char *m_pATF6;
	const char *m_pATF7;
	const char *m_pATFX;

	bool m_IsVideoAvailable;

	float m_step_per_mm;

	bool m_bIsLlcInit;
};
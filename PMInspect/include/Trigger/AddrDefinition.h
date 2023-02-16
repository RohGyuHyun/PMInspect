




enum
{
	// Trigger Version
	TRIGER_VERSION_READ = 0xFFC,	// 트리거 보드의 프로그램 버젼 읽기

	// Line Scan Camera Parameters for X Axis
	////////////////////// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX /////////////////////////////////////////////
 	X_ENCODER_COUNTER			= 0x000,	// (Addr) X Axis Up/Down Counter - Read

	X_ENCODER_COUNTER_SET	    = 0x108,	// (Addr) X Axis Up/Down Counter Set
	X_ENCODER_COUNTER_SET_VALUE	= 0x10C,	// (Addr) X 카운터에 특정값을 입력한다.



	XL_TRIGGER_COUNTER			= 0x010,	// (Addr) X Axis Line Trigger1 Counter - Read
	XL_TRIGGER_COUNTER2			= 0x014,	// (Addr) X Axis Line Trigger2 Counter - Read
	XL_TRIGGER_COUNTER3			= 0x018,	// (Addr) X Axis Line Trigger3 Counter - Read
	XL_TRIGGER_COUNTER4			= 0x01C,	// (Addr) X Axis Line Trigger4 Counter - Read

	X_COUNTER_RESET				= 0x100,	// (Addr) X Axis Counter Clear : Up/Down Counter, Line Trigger Counter
	                                        //        Data(0) : 0 -> 1 -> 0 : Reset Encoder Up/Down Counter
											//        Data(1) : 0 -> 1 -> 0 : Reset Trigger Counter1
											//        Data(2) : 0 -> 1 -> 0 : Reset Trigger Counter2
											//        Data(3) : 0 -> 1 -> 0 : Reset Trigger Counter3
											//        Data(4) : 0 -> 1 -> 0 : Reset Trigger Counter4

	XL_SET_TRG_ENABLE			= 0x104,	// (Addr) XAxis Line Trigger, Trigger Out Enable
											//        Data(0) : bit(0) = 1 : CW Trigger Out Enable
											//				    bit(0) = 0 : CW Trigger Out Disable
											//        Data(1) : bit(1) = 1 : CCW Trigger Out Enable
											//				    bit(1) = 0 : CCW Trigger Out Disable 

	XL_SET_TRG_CYCLE			= 0x110,	// (Addr) X Axis Line Trigger1, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 
	XL_SET_TRG_CYCLE2			= 0x114,	// (Addr) X Axis Line Trigger2, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 
	XL_SET_TRG_CYCLE3			= 0x118,	// (Addr) X Axis Line Trigger3, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 
	XL_SET_TRG_CYCLE4			= 0x11C,	// (Addr) X Axis Line Trigger4, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 

	XL_SET_TRG_ON_TIME			= 0x120,	// (Addr) X Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	XL_SET_TRG_ON_TIME2			= 0x124,	// (Addr) X Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	XL_SET_TRG_ON_TIME3			= 0x128,	// (Addr) X Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	XL_SET_TRG_ON_TIME4			= 0x12C,	// (Addr) X Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec

	XL_SET_TRG_NUMBER			= 0x130,	// (Addr) X Axis Line Trigger, Number of Trigger1 32bit : 0 ~ 4294967295
	XL_SET_TRG_NUMBER2			= 0x134,	// (Addr) X Axis Line Trigger, Number of Trigger2 32bit : 0 ~ 4294967295 
	XL_SET_TRG_NUMBER3			= 0x138,	// (Addr) X Axis Line Trigger, Number of Trigger3 32bit : 0 ~ 4294967295 
	XL_SET_TRG_NUMBER4			= 0x13C,	// (Addr) X Axis Line Trigger, Number of Trigger4 32bit : 0 ~ 4294967295 
	
	XL_SET_TRG1_START_POS		= 0x140,	// (Addr) X Axis Line Trigger, Camera1 Trigger Start Position
	XL_SET_TRG1_END_POS			= 0x144,	// (Addr) X Axis Line Trigger, Camera1 Trigger End Position
	XL_SET_TRG2_START_POS		= 0x148,	// (Addr) X Axis Line Trigger, Camera2 Trigger Start Position
	XL_SET_TRG2_END_POS			= 0x14C,	// (Addr) X Axis Line Trigger, Camera2 Trigger End Position
	XL_SET_TRG3_START_POS		= 0x150,	// (Addr) X Axis Line Trigger, Camera3 Trigger Start Position
	XL_SET_TRG3_END_POS			= 0x154,	// (Addr) X Axis Line Trigger, Camera3 Trigger End Position
	XL_SET_TRG4_START_POS		= 0x158,	// (Addr) X Axis Line Trigger, Camera4 Trigger Start Position
	XL_SET_TRG4_END_POS			= 0x15C,	// (Addr) X Axis Line Trigger, Camera4 Trigger End Position



	XL_SET_LIVE_FREQUENCY1		= 0x180,	// (Addr) X Axis Line Trigger, Live 1 Frequency and Live On / Off
											//		  Data(15 ~ 0) 8bit : Trigger Live Frequency : 10Hz ~ 100KHz Frequency Variable
											//        Data(16) = 1 : Trigger Line Live On
											//		  Data(16) = 0 : Trigger Line Live Off

	XL_SET_LIVE_FREQUENCY2		= 0x184,	// (Addr) X Axis Line Trigger, Live 2 Frequency and Live On / Off
	XL_SET_LIVE_FREQUENCY3		= 0x188,	// (Addr) X Axis Line Trigger, Live 2 Frequency and Live On / Off
	XL_SET_LIVE_FREQUENCY4		= 0x18C,	// (Addr) X Axis Line Trigger, Live 2 Frequency and Live On / Off


// Line Scan Camera Parameters for Y Axis
////////////////////// YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY /////////////////////////////////////////////

	ENCODER_SHARE				= 0x904,    // (Addr) 1개의 엔코더로 X축, Y 축 동시에 사용
	                                        // Data = 0 : 2개의 엔코더를 각각 사용
											// Data = 1 : 1개의 엔코더로 X축, Y축 동시 사용

	Y_ENCODER_COUNTER			= 0x400,	// (Addr) Y Axis Up/Down Counter - Read

	Y_ENCODER_COUNTER_SET		= 0x508,	// (Addr) Y Axis Up/Down Counter Set
	Y_ENCODER_COUNTER_SET_VALUE	= 0x50C,	// (Addr) Y 카운터에 특정값을 입력한다.

	YL_TRIGGER_COUNTER			= 0x410,	// (Addr) Y Axis Line Trigger1 Counter - Read
	YL_TRIGGER_COUNTER2			= 0x414,	// (Addr) Y Axis Line Trigger2 Counter - Read
	YL_TRIGGER_COUNTER3			= 0x418,	// (Addr) Y Axis Line Trigger3 Counter - Read
	YL_TRIGGER_COUNTER4			= 0x41C,	// (Addr) Y Axis Line Trigger4 Counter - Read



	Y_COUNTER_RESET				= 0x500,	// (Addr) Y Axis Counter Clear : Up/Down Counter, Line Trigger Counter, Area Trigger Counter
											//        Data(0) : 0 -> 1 -> 0 : Reset Encoder Up/Down Counter
											//        Data(1) : 0 -> 1 -> 0 : Reset Trigger Counter1
											//        Data(2) : 0 -> 1 -> 0 : Reset Trigger Counter2
											//        Data(3) : 0 -> 1 -> 0 : Reset Trigger Counter3
											//        Data(4) : 0 -> 1 -> 0 : Reset Trigger Counter4

	YL_SET_TRG_ENABLE			= 0x504,	// (Addr) Y Axis Line Trigger, Trigger Out Enable
											//        Data(0) : bit(0) = 1 : CW Trigger Out Enable
											//				    bit(0) = 0 : CW Trigger Out Disable
											//        Data(1) : bit(1) = 1 : CCW Trigger Out Enable
											//				    bit(1) = 0 : CCW Trigger Out Disable 

	YL_SET_TRG_CYCLE			= 0x510,	// (Addr) Y Axis Line Trigger, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 세팅 가능
	YL_SET_TRG_CYCLE2			= 0x514,	// (Addr) Y Axis Line Trigger, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 세팅 가능
	YL_SET_TRG_CYCLE3			= 0x518,	// (Addr) Y Axis Line Trigger, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 세팅 가능
	YL_SET_TRG_CYCLE4			= 0x51C,	// (Addr) Y Axis Line Trigger, Cycle 16bit :   1 ~ 1048575 Encoder Pulse 세팅 가능

	YL_SET_TRG_ON_TIME			= 0x520,	// (Addr) Y Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	YL_SET_TRG_ON_TIME2			= 0x524,	// (Addr) Y Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	YL_SET_TRG_ON_TIME3			= 0x528,	// (Addr) Y Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec
	YL_SET_TRG_ON_TIME4			= 0x52C,	// (Addr) Y Axis Line Trigger, On Time 16bit : 1 ~ 65535 usec

	YL_SET_TRG_NUMBER			= 0x530,	// (Addr) Y Axis Line Trigger, Number of Trigger 32bit : 0 ~ 4294967295
	YL_SET_TRG_NUMBER2			= 0x534,	// (Addr) Y Axis Line Trigger, Number of Trigger 32bit : 0 ~ 4294967295 
	YL_SET_TRG_NUMBER3			= 0x538,	// (Addr) Y Axis Line Trigger, Number of Trigger 32bit : 0 ~ 4294967295 
	YL_SET_TRG_NUMBER4			= 0x53C,	// (Addr) Y Axis Line Trigger, Number of Trigger 32bit : 0 ~ 4294967295 

	YL_SET_TRG1_START_POS		= 0x540,	// (Addr) Y Axis Line Trigger, Camera1 Start Position
	YL_SET_TRG1_END_POS			= 0x544,	// (Addr) Y Axis Line Trigger, Camera2 End Position
	YL_SET_TRG2_START_POS		= 0x548,	// (Addr) Y Axis Line Trigger, Camera3 Start Position
	YL_SET_TRG2_END_POS			= 0x54C,	// (Addr) Y Axis Line Trigger, Camera3 End Position
	YL_SET_TRG3_START_POS		= 0x550,	// (Addr) Y Axis Line Trigger, Camera4 Start Position
	YL_SET_TRG3_END_POS			= 0x554,	// (Addr) Y Axis Line Trigger, Camera4 End Position
	YL_SET_TRG4_START_POS		= 0x558,	// (Addr) Y Axis Line Trigger, Camera5 Start Position
	YL_SET_TRG4_END_POS			= 0x55C,	// (Addr) Y Axis Line Trigger, Camera5 End Position


	YL_SET_LIVE_FREQUENCY1		= 0x580,	// (Addr) Y Axis Line Trigger, Live Frequency 1 and Live On / Off
											//		  Data(15 ~ 0) 16bit : Trigger Live Frequency : 10Hz ~ 100KHz Frequency Variable
											//        Data(16) = 1 : Trigger Line Live On
											//		  Data(16) = 0 : Trigger Line Live Off
	YL_SET_LIVE_FREQUENCY2		= 0x584,	// (Addr) Y Axis Line Trigger, Live Frequency 2 and Live On / Off
	YL_SET_LIVE_FREQUENCY3		= 0x588,	// (Addr) Y Axis Line Trigger, Live Frequency 3 and Live On / Off
	YL_SET_LIVE_FREQUENCY4		= 0x58C,	// (Addr) Y Axis Line Trigger, Live Frequency 4 and Live On / Off





/////// 파라메터 읽기
// Trigger Parameter Read
XL_SET_TRG_CYCLE1_READ = 0x020,	// 트리거 사이클(분주비) 읽기
XL_SET_TRG_CYCLE2_READ = 0x024,
XL_SET_TRG_CYCLE3_READ = 0x028,
XL_SET_TRG_CYCLE4_READ = 0x02C,

XL_SET_TRG_NUMBER1_READ = 0x030,	// 트리거 출력 수량 읽기
XL_SET_TRG_NUMBER2_READ = 0x034,
XL_SET_TRG_NUMBER3_READ = 0x038,
XL_SET_TRG_NUMBER4_READ = 0x03C,

XL_SET_TRG1_START_POS_READ = 0x040, // 트리거 스타트 / 엔드 읽기
XL_SET_TRG1_END_POS_READ = 0x044,
XL_SET_TRG2_START_POS_READ = 0x048,
XL_SET_TRG2_END_POS_READ = 0x04C,
XL_SET_TRG3_START_POS_READ = 0x050,
XL_SET_TRG3_END_POS_READ = 0x054,
XL_SET_TRG4_START_POS_READ = 0x058,
XL_SET_TRG4_END_POS_READ = 0x05C,



YL_SET_TRG_CYCLE1_READ = 0x080,	// 트리거 사이클(분주비) 읽기
YL_SET_TRG_CYCLE2_READ = 0x084,
YL_SET_TRG_CYCLE3_READ = 0x088,
YL_SET_TRG_CYCLE4_READ = 0x08C,

YL_SET_TRG_NUMBER1_READ = 0x090,	// 트리거 출력 수량 읽기
YL_SET_TRG_NUMBER2_READ = 0x094,
YL_SET_TRG_NUMBER3_READ = 0x098,
YL_SET_TRG_NUMBER4_READ = 0x09C,

YL_SET_TRG1_START_POS_READ = 0x0A0, // 트리거 스타트 / 엔드 읽기
YL_SET_TRG1_END_POS_READ = 0x0A4,
YL_SET_TRG2_START_POS_READ = 0x0A8,
YL_SET_TRG2_END_POS_READ = 0x0AC,
YL_SET_TRG3_START_POS_READ = 0x0B0,
YL_SET_TRG3_END_POS_READ = 0x0B4,
YL_SET_TRG4_START_POS_READ = 0x0B8,
YL_SET_TRG4_END_POS_READ = 0x0BC,
};
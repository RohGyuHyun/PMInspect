/******************************************************************************
 *	
 *	(C) Copyright WDI 2010
 *	
 ******************************************************************************
 *
 *	FILE:		epp.cpp
 *
 *	PROJECT:	AFT Sensors
 *
 *	SUBPROJECT:	.
 *
 *	Description: Embedded part configuration protocol.
 *               Protocol design for accessing configuration of the part.
 *	
 ******************************************************************************
 *	
 *	Change Activity
 *	Defect  Date       Developer        Description
 *	Number  DD/MM/YYYY Name
 *	======= ========== ================ =======================================
 *          21/09/2009 Andrew L.        Initial version
 *          02/11/2010 Chris O.           
 *          15/11/2010 Chris O.         Added synchronization objects for
 *                                      multithreaded applications
 *                                     
 *****************************************************************************/

#include "stdafx.h"
#include "epp.h"
#include "atf_lib.h"

#define NO_LOCAL_TEST

CEpp::EppVer_Mode CEpp::s_m_ev = CEpp::IsEppVerUnknow;
extern CLogger g_log;

// CEppArr global initializers
CEpp CEppArr::gs_epp_arr[MAX_EPP_NUM] = 
		{ CEpp(&g_log), CEpp(&g_log), CEpp(&g_log), CEpp(&g_log), 
		  CEpp(&g_log), CEpp(&g_log), CEpp(&g_log), CEpp(&g_log) };
bool CEppArr::gs_epp_allocated_arr[MAX_EPP_NUM] = 
		{ false, false, false, false, 
		  false, false, false, false };
int CEppArr::gst_epp_index = INVALID_IDX;
CWCriticalSection CEppArr::gs_cs = CWCriticalSection();

//*----------------------------------------------------------------------------
// called to open next available port from the pool
int CEppArr::Open (const char *port_arg, int rate_arg)
{
	__try
	{
		gs_cs.Lock();
		for (int i=0; i<MAX_EPP_NUM; i++) 
		{
			if (!gs_epp_allocated_arr[i]) 
			{
				CEpp* pepp = gs_epp_arr+i;
				// pepp->setLogger(&g_log);
				int iRet = pepp->Open(port_arg,rate_arg);
				if (iRet == ErrOK) 
				{
					// port was opened OK
					gst_epp_index = i;
					gs_epp_allocated_arr[i] = true;
					return ErrOK;
				}
			}
		}
	}
	__finally
	{
		gs_cs.Unlock();
	}
	return ErrNoresources;
}
//*----------------------------------------------------------------------------
// called to close current epp object and free it in the pool
void CEppArr::Close()
{
	__try
	{
		gs_cs.Lock();

		if (gst_epp_index>=0 
			&& gst_epp_index<MAX_EPP_NUM
			 && gs_epp_allocated_arr[gst_epp_index])
		{
			// closing valid port
			CEpp* pepp = gs_epp_arr+gst_epp_index;
			pepp->Close();
			gs_epp_allocated_arr[gst_epp_index] = false;
			gst_epp_index = INVALID_IDX;
		}
	}
	__finally
	{
		gs_cs.Unlock();
	}
}
//*----------------------------------------------------------------------------
#pragma pack(1) 
// transport layer protocol
struct TransportBinaryPacket
{
	TransportBinaryPacket() 
	: m_cmd(CEpp::TransportCodingBinaryLittleEndian)
	, m_len(0)
	, m_chksum(0) 
	{}

	void setCheksum()
	{
		int chksum = 0;
		char *p = (char*)this;
		for (int i=0;i<m_len+3;i++) chksum += *p++; 
		m_chksum = ((u_char)(-chksum))&0xff;
	}

	bool isChksum ()
	{
		int chksum = 0;
		char *p = (char*)this;
		for (int i=0;i<m_len+3;i++) chksum += *p++; 
		return (chksum&0xff)==0;
	}

	u_char m_cmd;		// processing command
	u_short m_len;	// number of bytes
	u_char m_chksum;	// cheksum
};

typedef struct TransportBinaryPacket *TransportBinaryPacketPtr;

//*----------------------------------------------------------------------------
ErrNamesTab const char* CEpp::errName (EppErrorCodes err) 
{
	return err>=0 && err<sizeof(g_eppErrorNames)/sizeof(*g_eppErrorNames) ?
		g_eppErrorNames[err] : "Invalid Err Code";
}
//*----------------------------------------------------------------------------
EppErrorCodes CEpp::_ReadArray (void *to, int bid, int pid, EppDataType _tag, int num, int offset)
{
#ifndef NO_LOCAL_TEST
	return ErrOK;
#endif
	char logMsg[256];
	char tmpMsg[64];
	EppErrorCodes iRet = ErrOK;
	DWORD dwRet = 0;
	int iLocRet = 0;
	char c = '\0';
	DWORD byte_header = 0;
	DWORD byte_data = 0;

	if (!p_commBuff) return ErrNoresources;	// allocating buffer must have failed

	__try
	{
		EnterCriticalSection(&m_cs);

		TransportBinaryPacketPtr ptb = (TransportBinaryPacketPtr)p_commBuff;
		*ptb = TransportBinaryPacket();

		Combuf_Char *p = (Combuf_Char*)(ptb+1);
		*p++ = SessionCmdRead;
		*p++ = bid;
		*p++ = pid;
		Combuf_Char *ptag = p++;
		int tag = _tag;

		// negative num indicates request for all characters (same as 0) but also specifies the
		// maximum buffer size for the source
		int max_src_size = (num > 0) ? num : ((num < 0) ? -num : (sizeof(Combuf_Char)*MAX_COM_BUFFER_SIZE) / DtTypeSize(_tag));
		max_src_size *= DtTypeSize(_tag);

		if (num < 0) num = 0;

		if (num>1 || !num) 
		{
			tag |= DtArrayEnm;
			*((unsigned short*)p) = num;
			p += 2;
		}
		if (offset) 
		{
			tag |= DtOffsetEnm;
			*((unsigned short*)p) = offset;
			p += 2;
		}

		*ptag = tag | ((QcEnumAllParamInfo & tag) == 0 ? QcValueCurrent : 0);

		int byte_len = (int)((Combuf_Char*)p-p_commBuff);
		ptb->m_len = byte_len-3;

		// calculate cheksum
		ptb->setCheksum();

		// send data request out
		EPPLOGMSG2(CLogger::LogLow, "%s%c %d%s%d%s%x%s%d%s",
				   "Info: Send command to sensor: [", bid, pid,
				   "] (offset=", offset,
				   "; tag=0x", _tag,
				   "; num=", num, ";).\n");

		getNbrOfBytes();	// clear status before sending
		dwRet = sendArray ((const char*)(p_commBuff), byte_len);

		if (m_pLogger != NULL) 
		{
			int printlen = byte_len > 32 ? 32 : byte_len;
			sprintf(logMsg, "Sr-> ");
			for (int i=0; i<printlen; i++) 
			{
				sprintf(tmpMsg,"%02x ", p_commBuff[i]&0xff);
				strcat(logMsg, tmpMsg);
			}
			if (printlen<byte_len) 
			{
				strcat(logMsg, " ...");
			}
			strcat(logMsg, "\n");
			EPPLOGMSG2(CLogger::LogLow, "%s", logMsg);
		}

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s", "Error: Failed to send a command to the sensor. [LastError=", dwRet, "]\n");
			iRet = ErrOperFailed;
			goto exitfun;
		}

		// check the response
		dwRet = getChar(&c);
		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s", "Error: Failed to receive a response from the sensor. [LastError=", dwRet, "]\n");
			iRet = ErrOperFailed;
			goto exitfun;
		}
		switch (c) 
		{
		case TransportAck :
			sprintf(logMsg," <-R 41[A] ");
			break;

		case TransportNak :
			// things failed
			EPPLOGMSG(" <-R 4e[N]\n", CLogger::LogLow);
			// things failed
			iRet = ErrOperFailed;
			goto exitfun;
		default :
			EPPLOGMSG("TIMEOUT\n", CLogger::LogLow);
			iRet = ErrTimeout;
			goto exitfun;
		}

		// we got ack, now we can read data
		dwRet = getArray ((char*)ptb, sizeof(TransportBinaryPacket), &byte_header);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogAll, "%s%d%s", 
					   "Error: Failed to receive a response (header) from the sensor. [LastError=", 
					   dwRet, "]\n");
			iRet = ErrOperFailed;
			goto exitfun;
		}

		if (m_pLogger != NULL) 
		{
			int printlen = byte_header;
			char *p = (char*)ptb;

			tmpMsg[0] = '\0';
			for (int i=0; i<printlen; i++) 
			{
				sprintf(tmpMsg,"%02x ", (*p++)&0xff);
				strcat(logMsg, tmpMsg);
			}
			if (byte_header!=sizeof(TransportBinaryPacket)) 
			{
				strcat(logMsg, "\n");
			}
		}
		if (byte_header!=sizeof(TransportBinaryPacket)) 
		{
			iRet = ErrTimeout;
			goto exitfun;
		}
		
		byte_len = ptb->m_len-1;
		if (byte_len>0) 
		{
			// read remaining bytes
			if (byte_len>=(sizeof(Combuf_Char)*MAX_COM_BUFFER_SIZE)-sizeof(TransportBinaryPacket))
			{
				iRet = ErrNoresources;
				goto exitfun;
			}

			dwRet = getArray ((char*)(ptb+1), byte_len, &byte_data);

			if (dwRet)
			{
				EPPLOGMSG2(CLogger::LogAll, "%s%d%s",
						   "Error: Failed to receive a response (data) from the sensor. [LastError=",
						   dwRet, "]\n");
				iRet = ErrOperFailed;
				goto exitfun;
			}

			if (m_pLogger != NULL)
			{
				int printlen = byte_data>32 ? 32 : byte_data;
				char *p = (char*)(ptb+1);

				tmpMsg[0] = '\0';
				for (int i=0; i<printlen; i++) 
				{
					sprintf(tmpMsg,"%02x ", (*p++)&0xff);
					strcat(logMsg, tmpMsg);
				}
				if (printlen<(int)byte_data) 
				{
					strcat(logMsg, " ...");
				}
				strcat(logMsg, "\n");
				EPPLOGMSG2(CLogger::LogLow, "%s", logMsg);
		}

			if (byte_data!=byte_len) 
			{
				iRet = ErrTimeout;
				goto exitfun;
			}
		}

		// what we received is bad
		if (!ptb->isChksum()) 
		{
			iRet = ErrChksum;
			goto exitfun;
		}
		
		// data doesn't look good
		p = (unsigned char*)(ptb+1);
		if (*p++!=SessionCmdReadResponse ||
			 *p++!=bid ||
			  *p++!=pid )
		{
			iRet = ErrSntaxError;
			goto exitfun;
		}

		// tags must agree
		int receive_tag = *p++;
		if (DtGetDataType(receive_tag)!= DtGetDataType(tag))
		{
			 iRet = ErrSntaxError;
			 goto exitfun;
		}
		// num must agree
		int receive_num = 1;
		if (receive_tag&DtArrayEnm) {
			receive_num = *(u_short*)p;
			p+=2;
		}
		if (num && receive_num>num)
		{
			 iRet = ErrSntaxError;
			 goto exitfun;
		}
		// tag is the only one
		if (!(receive_tag&DtLastDataEnm))
		{
			 iRet = ErrSntaxError;
			 goto exitfun;
		}

		// check if have enough data
		int data_size = DtTypeSize(tag) * receive_num;
		if (byte_len+(int)(sizeof(TransportBinaryPacket))-((int)((Combuf_Char*)p-p_commBuff))<data_size)
		{
			iRet = ErrOutOfBound;
			goto exitfun;
		}

		// return data
		memcpy (to, p, (max_src_size < data_size) ? max_src_size : data_size);
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

exitfun:
	return iRet;
}
//*----------------------------------------------------------------------------
EppErrorCodes CEpp::_WriteArray (void *from, int bid, int pid, EppDataType _tag, int num, int offset)
{
#ifndef NO_LOCAL_TEST
	return ErrOK;
#endif
	char logMsg[256];
	char tmpMsg[64];
	EppErrorCodes iRet = ErrOK;
	DWORD dwRet = 0;
	char c = '\0';

	if (!p_commBuff) return ErrNoresources;	// allocating buffer must have failed

	__try
	{
		EnterCriticalSection(&m_cs);

		TransportBinaryPacketPtr ptb = (TransportBinaryPacketPtr)p_commBuff;
		*ptb = TransportBinaryPacket();

		Combuf_Char *p = (Combuf_Char*)(ptb+1);
		*p++ = SessionCmdWrite;
		*p++ = bid;
		*p++ = pid;
		Combuf_Char *ptag = p++;
		int tag = _tag;

		if (num>1) {
			tag |= DtArrayEnm;
			*((unsigned short*)p) = num;
			p += 2;
		}
		if (offset) {
			tag |= DtOffsetEnm;
			*((unsigned short*)p) = offset;
			p += 2;
		}
		*ptag = tag|QcValueCurrent;

		// add data
		int data_size = DtTypeSize(tag) * num;
		if (p+data_size-p_commBuff>(sizeof(Combuf_Char)*MAX_COM_BUFFER_SIZE))
		{
			iRet = ErrNoresources;
			goto exitfun;
		}

		// copy into buffer
		if (data_size) {
			memcpy (p,(char*)(from),data_size);
			p+=data_size;
		}

		// set ptb
		int byte_len = (int)(p-p_commBuff);
		ptb->m_len = byte_len - 3;
		ptb->setCheksum();

		// send data request out
		EPPLOGMSG2(CLogger::LogLow, "%s%c %d%s%d%s%x%s%d%s",
				   "Info: Send command to sensor: [", bid, pid,
				   "] (offset=", offset,
				   "; tag=0x", _tag,
				   "; num=", num, ";).\n");

		getNbrOfBytes();	// clear status before sending
		dwRet = sendArray ((const char*)(p_commBuff), byte_len);

		if (m_pLogger != NULL)
		{
			int printlen = byte_len>32 ? 32 : byte_len;
			sprintf(logMsg, "Sw-> ");
			for (int i=0; i<printlen; i++) 
			{
				sprintf(tmpMsg, "%02x ", p_commBuff[i]&0xff);
				strcat(logMsg, tmpMsg);
			}
			if (printlen<byte_len) 
			{
				strcat(logMsg, " ...");
			}
			strcat(logMsg, "\n");
			EPPLOGMSG2(CLogger::LogLow, "%s", logMsg);
		}

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogAll, "%s%d%s",
					   "Error: Failed to send a command to the sensor. [LastError=",
					   dwRet, "]\n");
			iRet = ErrOperFailed;
			goto exitfun;
		}

		// chek the response
		dwRet = getChar(&c);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to receive a response from the sensor. [LastError=",
					   dwRet, "]\n");
			iRet = ErrOperFailed;
			goto exitfun;
		}

		switch (c) 
		{
		case TransportAck :
			EPPLOGMSG(" <-R 41[A]\n", CLogger::LogLow);
			break;

		case TransportNak :
			EPPLOGMSG(" <-R 4e[N]\n", CLogger::LogLow);
			// things failed
			iRet = ErrOperFailed;
			break;

		default :
			EPPLOGMSG("TiMEOUT\n", CLogger::LogLow);
			iRet = ErrTimeout;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

exitfun:
	return iRet;
}
//*----------------------------------------------------------------------------
// ping sensor with ACK
int CEpp::PingAck()
{
	char const *pFunName = "CEpp::PingAck";

	char tmpArray[16];
	DWORD iNumBytes = 0;
	DWORD dwRet = 0;
	int iRet = ErrOK;

 	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		getNbrOfBytes ();	// clear queues
		dwRet = sendArray ("A", 1);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to send ACK command to the sensor. [LastError=",
					   dwRet, "]\n");
			iRet = ErrInvalid;
			goto exitfun;
		}

		Sleep (50);
		dwRet = getArray (tmpArray, 1, &iNumBytes);

		if (dwRet || iNumBytes == 0)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to get ACK response from the sensor. [LastError=",
					   dwRet, "]\n");
			iRet = ErrTimeout;
			goto exitfun;
		}

		if (tmpArray[0] != 'A')	iRet = ErrInvalid;
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

exitfun:
	EPPEXITFUN(pFunName);
	return iRet;
}
//*----------------------------------------------------------------------------
// reset sensor
int CEpp::Reset()
{
	char const *pFunName = "CEpp::ResetSensor";

	int iRet = 0;
	int iTimeout = 10;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		if (isSerialConnection())
		{
			// sensor reset can be done at baudrate=4800 only
			iRet = atf_WriteBaud(4800);

			if (iRet != ErrOK)
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
						   "Error: Failed to change sensor COM port baudrate. [ErrorNum=",
						   iRet, "]\n");
				goto exitfun;
			}
			Sleep(100);

			iRet = _ChangeCommBaudrate(4800);

			if (iRet != ErrOK)
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
						   "Error: Failed to change COM port baudrate. [ErrorNum=",
						   iRet, "]\n");
				goto exitfun;
			}
			Sleep(100);

			// send reset character
			iRet = sendArray ("Z", 1);

			if (iRet != ErrOK)
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s", 
						   "Error: Failed to reset sensor. [LastError=",
						   iRet, "]\n");
			}

			Sleep(100);

			// change baudrate to sensor default
			iRet = _ChangeCommBaudrate(9600);

			if (iRet != ErrOK)
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
						   "Error: Failed to change COM port baudrate. [ErrorNum=",
						   iRet, "]\n");
				goto exitfun;
			}
			
			Sleep(5000);
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

exitfun:
	EPPEXITFUN(pFunName);
	return 0;

}
//*----------------------------------------------------------------------------
// resynchronize/recover sensor from failure
int CEpp::RecoverSensor()
{
	const char *pFunName = "CEpp::RecoverSensor";

	char logMsg[128];
	char tmpArray[16];
	int iTryCnt = 0;
	DWORD iNumBytes = 0;
	DWORD dwRet = 0;
	int iRet = 0;
	DWORD i = 0;
	char tmpRsp[16];

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		// close and reopen port first
		dwRet = reconnect ();
		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to reconnect to port. [LastError=",
					   dwRet, "]\n");
			goto exitfun;
		}

try_begin:
		// check for basic communication
		Sleep (50);
		getNbrOfBytes ();	// clear queues
		dwRet = sendArray ("AAA", 3);

		EPPLOGMSG("Info: Send command to sensor: \"AAA\"\n", CLogger::LogLow);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to send a command to the sensor. [LastError=",
					   dwRet, "]\n");
			goto exitfun;
		}

		Sleep (50);

		dwRet = getArray (tmpArray, 3, &iNumBytes);

		sprintf(logMsg, "Info: Received response from sensor:");
		for (i = 0; i < iNumBytes; i++)
		{
			sprintf(tmpRsp, " 0x%02x[%c]", tmpArray[i], (tmpArray[i] > 0x1f && tmpArray[i] < 0x7f) ? tmpArray[i] : 0x0);
			strcat(logMsg, tmpRsp);
		}
		strcat(logMsg, "\n");
		EPPLOGMSG2(CLogger::LogLow, "%s", logMsg);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to receive a response from the sensor. [LastError=",
					   dwRet, "]\n");
			goto exitfun;
		}

		switch(tmpArray[iNumBytes-1]) 
		{
		case 'A' :
			// OK got it going
			break;
		case 'K' :
		case 'X' :
			// MCM is there, get out of it
			dwRet = sendArray ("\xb6\x49", 2);

			EPPLOGMSG("Info: Send command to sensor: \"0xb6 0x49\"\n", CLogger::LogLow);

			if (dwRet)
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
						   "Error: Failed to send a command to the sensor. [LastError=",
						   dwRet, "]\n");
				goto exitfun;
			}

			if (++iTryCnt>2) 
			{
				EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
						   "Error: Failed to recover sensor. [TryCount=",
						   iTryCnt, "]\n");
				goto exitfun;	// can't get something useful
			}

			goto try_begin;
		default :
			goto exitfun;		// cant establish any meaningfull communication. Need to disconnect and try again
		}

		// see what is running on the other end
		getNbrOfBytes ();	// clear queues
		dwRet = sendArray ("ACi00000000A", 12);

		EPPLOGMSG("Info: Send command to sensor: \"ACi00000000A\"\n", CLogger::LogLow);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to send a command to the sensor. [LastError=",
					   dwRet, "]\n");
			goto exitfun;
		}

		Sleep (50);

		dwRet = getArray (tmpArray, 12, &iNumBytes);

		sprintf(logMsg, "Info: Received response from sensor:");
		for (i = 0; i < iNumBytes; i++)
		{
			sprintf(tmpRsp, " 0x%02x[%c]", tmpArray[i], (tmpArray[i] > 0x1f && tmpArray[i] < 0x7f) ? tmpArray[i] : 0x0);
			strcat(logMsg, tmpRsp);
		}
		strcat(logMsg, "\n");
		EPPLOGMSG2(CLogger::LogLow, "%s", logMsg);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to receive a response from the sensor. [LastError=",
					   dwRet, "]\n");
			goto exitfun;
		}

		getNbrOfBytes ();	// clear queues

		if (iNumBytes==2) 
		{
			// we have application running
			if (tmpArray[0]=='A' && tmpArray[1]=='A') 
			{
				iRet = 1;
				goto exitfun;
			}
		}
		else if (iNumBytes==12) 
		{
			// we have boot loader running
			if (tmpArray[0]=='A' 
				&& tmpArray[11]=='A'
				 && strncmp("00000000",tmpArray+3,8))		// respond can't be same 00000000, this would imply something wrong
			{
				// OK we have found boot loader, reset out of it
				dwRet = sendArray ("Z", 1);
				EPPLOGMSG("Info: Send reset command to sensor: \"Z\"\n", CLogger::LogLow);

				if (dwRet)
				{
					EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
							  "Error: Failed to send a reset command to the sensor. [LastError=",
							  dwRet, "]\n");
					goto exitfun;
				}

				Sleep (3000);
				iRet = 2;
			}
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	// something not quite right
exitfun:
	EPPEXITFUN(pFunName);
	return iRet;    
}

// change serial port timeout
EppErrorCodes CEpp::_ChangeCommTimeouts(DWORD dwTimeout)
{
	const char *pFunName = "CEpp::_ChangeCommTimeouts";

	DWORD dwRet = 0;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		dwRet = ChangeCommTimeouts(dwTimeout);

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed in ChangeCommTimeouts. [LastError=",
					   dwRet, "]\n");
			dwRet = ErrNoAccess;
		}
		else
		{
			dwRet = ErrOK;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	EPPEXITFUN(pFunName);
	return (EppErrorCodes)dwRet;
}

// reset serial port timeouts to default values
EppErrorCodes CEpp::_ResetCommTimeouts()
{
	const char *pFunName = "CEpp::_ResetCommTimeouts";

	DWORD dwRet = 0;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		dwRet = ResetCommTimeouts();
		
		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed in ResetCommTimeouts. [LastError=",
					   dwRet, "]\n");
			dwRet = ErrNoAccess;
		}
		else
		{
			dwRet = ErrOK;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	EPPEXITFUN(pFunName);
	return (EppErrorCodes)dwRet;
}

// set serial port new baudrate
EppErrorCodes CEpp::_ChangeCommBaudrate(DWORD dwBaudRate)
{
	const char *pFunName = "CEpp::_ChangeCommBaudrate";

	DWORD dwRet = 0;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		dwRet = ChangeCommBaudrate(dwBaudRate);	
		
		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed in ChangeCommBaudrate. [LastError=",
					   dwRet, "]\n");
			dwRet = ErrNoAccess;
		}
		else
		{
			dwRet = ErrOK;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	EPPEXITFUN(pFunName);
	return (EppErrorCodes)dwRet;
}

// get current serial port baudrate
EppErrorCodes CEpp::_GetCommBaudrate(DWORD *dwBaudrate)
{
	char const *pFunName = "CEpp::_GetCommBaudrate";

	DWORD dwRet = 0;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		dwRet = GetCommBaudrate(dwBaudrate);	
		
		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed in GetCommBaudrate. [LastError=",
						dwRet, "]\n");
			dwRet = ErrNoAccess;
		}
		else
		{
			dwRet = ErrOK;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	EPPEXITFUN(pFunName);
	return (EppErrorCodes)dwRet;
}

// save message in the log file
void CEpp::saveToLog(const char *pLogMsg, CLogger::LogLevels iLogLevel)
{
	if (m_pLogger != NULL && pLogMsg != NULL)
	{
		m_pLogger->saveToLog(pLogMsg, iLogLevel);
	}
}

// save message in the log file - another flavor of logging variable-length argument list
void CEpp::saveToLog2(CLogger::LogLevels iLogLevel, char *pFormat, ...)
{
    va_list args;
    int iLen = 0;
    char *pLogMsg = NULL;

	if (m_pLogger != NULL && pFormat != NULL)
	{
		va_start(args, pFormat);
	    
		iLen = _vscprintf(pFormat, args) + 1;
		pLogMsg = (char*)malloc(iLen * sizeof(char));

		if (pLogMsg != NULL)
		{
			vsprintf(pLogMsg, pFormat, args);
			m_pLogger->saveToLog(pLogMsg, iLogLevel);
			free(pLogMsg);
		}
	}
}

// format and save in the log file low level error
void CEpp::PrintfError (void* locale, bool bReading, EppErrorCodes err, int bid, int pid, EppDataType tag, int num, int offset)
{
    char *pTagName = "";
	char buff[128] = {' '};

    switch (tag&DtDataType)
	{
    case DtNoDataEnm:
        pTagName = ",Cmd";
        buff[0] = 0;
        break;
    case DtByteEnm:
        pTagName = ",Byte";
        break;
    case DtWordEnm:
        pTagName = ",Word";
        break;
    case DtDwordEnm:
        pTagName = ",Dword";
        break;
    }

	if (buff[0] != 0)
	{
        if (num <= 1)
            sprintf_s(buff, sizeof(buff), "[%d]", offset);
        else
            sprintf_s(buff, sizeof(buff), "[%d-%d]", offset, num-1);
	}

	EPPLOGMSG2(CLogger::LogLow, "%s%s%s%s %c%d%s%s%s",
		"Error: Cmd error ", CEpp::errName(err),
			   " on ", bReading ? "reading" : "writing",
			   bid, pid, buff, pTagName, "\n");
}

// open serial port, communication
int CEpp::Open (const char *port_arg, int rate_arg) 
{
	char const *pFunName = "CEpp::Open";

	int iRet = 0;

	EPPENTERFUN(pFunName);

	// in case was connected earlier
	disconnect();

	if (!p_commBuff) 
	{
		if (!(p_commBuff = (Combuf_Char*)malloc(sizeof(Combuf_Char)*MAX_COM_BUFFER_SIZE))) 
		{
			EPPLOGMSG("Error: Failed to allocate mem buffer.\n", CLogger::LogLow);
			iRet = ErrNoresources;
			goto exitfun;
		}
	}

	if (port_arg == NULL)
	{
		EPPLOGMSG("Error: Failed to open Com port - invalid name.\n", CLogger::LogLow);
		iRet = ErrInvalid;
		goto exitfun;
	}

	if (rate_arg <= 0)
	{
		EPPLOGMSG("Error: Failed to open Com port - invalid baudrate.\n", CLogger::LogLow);
		iRet = ErrInvalid;
		goto exitfun;
	}	

	iRet = connect (port_arg, rate_arg, spNONE);

	if (iRet != 0)
	{
		EPPLOGMSG2(CLogger::LogLow, "%s%d%s%s%s%d%s",
				   "Error: Failed to open Com port. [ErrorNum=", iRet,
				   "; ComPort=", port_arg,
				   "; BaudRate=", rate_arg, "]\n");
	}
	else
	{
		EPPLOGMSG2(CLogger::LogLow, "%s%s%s%d%s",
				   "Info: Com port opened. [ComPort=", port_arg,
				   "; BaudRate=", rate_arg, "]\n");
	}

exitfun:
	EPPEXITFUN(pFunName);
	return iRet;
}

// reuse port that is already opened
void CEpp::Attach (HANDLE h) 
{
	char const *pFunName = "CEpp::Attach";

	EPPENTERFUN(pFunName);

	if (serial_handle != INVALID_HANDLE_VALUE && serial_handle != h) 
	{
		CloseHandle(serial_handle);
	}

	if (!p_commBuff) 
	{
		if (!(p_commBuff = (Combuf_Char*)malloc(sizeof(Combuf_Char)*MAX_COM_BUFFER_SIZE))) {
			EPPLOGMSG("Error: Failed to allocate mem buffer.\n", CLogger::LogLow);
			goto exit_fun;
		}
	}

	serial_handle = h;

exit_fun:
	EPPEXITFUN(pFunName);
}

// close serial port 
void CEpp::Close () 
{
	char const *pFunName = "CEpp::Close";

	EPPENTERFUN(pFunName);
	
	if (p_commBuff) 
	{
		free(p_commBuff);
		p_commBuff = NULL;
	}

	disconnect();
	EPPEXITFUN(pFunName);
}

// leave handle opened, assuming someone else needs it
void CEpp::Detach () 
{
	char const *pFunName = "CEpp::Detach";

	EPPENTERFUN(pFunName);
	serial_handle = INVALID_HANDLE_VALUE;
	EPPEXITFUN(pFunName);
}

// send and receive response on COM port
EppErrorCodes CEpp::sendCmnd(Combuf_Char *pToSend, Combuf_Char *pToRecv, int iTimeout, int iExpByteNum)
{
	char const *pFunName = "CEpp::sendCmnd";

	DWORD dwRet = 0;
	DWORD iRecvByteNum = 0;
	int iExpByteNumLoc = iExpByteNum;
	EppErrorCodes iRet = ErrOK;

	EPPENTERFUN(pFunName);

	__try
	{
		EnterCriticalSection(&m_cs);

		if (iTimeout > 0)
		{
			_ChangeCommTimeouts((DWORD) iTimeout);
		}

		getNbrOfBytes();	// clear queues
		dwRet = sendArray ((const char*)(pToSend), strlen((const char*)(pToSend)));

		if (dwRet)
		{
			EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
					   "Error: Failed to send COM command. [LastError=",
					   dwRet, "]\n");
			iRet = ErrInvalid;
		}
		else
		{
			if (pToRecv)
			{
				if (iExpByteNumLoc == 0)
				{
					iExpByteNumLoc = strlen((const char*)pToSend);
				}

				dwRet = getArray((char*)pToRecv, iExpByteNumLoc, &iRecvByteNum);

				if (dwRet || iRecvByteNum == 0)
				{
					EPPLOGMSG2(CLogger::LogLow, "%s%d%s",
							   "Error: Failed to get COM response. [LastError=",
							   dwRet, "]\n");
					iRet = ErrTimeout;
				}
			}
		}

		if (iTimeout > 0)
		{
			_ResetCommTimeouts();
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	EPPEXITFUN(pFunName);
	return iRet;
}

bool CEpp::EppVerCheck() 
{
	if (s_m_ev==IsEppVerCheck)  return true;
	if (s_m_ev!=IsEppVerUnknow) return s_m_ev==IsEppVerSupport;

	int bid=0,pid=0;
	EppErrorCodes err = ErrOK;

	s_m_ev = IsEppVerCheck;
	s_m_ev = ReadMostRecentRetcode (&bid, &pid, &err)==ErrOK ? IsEppVerSupport : IsEppVerBasic;

	return s_m_ev==IsEppVerSupport;
}

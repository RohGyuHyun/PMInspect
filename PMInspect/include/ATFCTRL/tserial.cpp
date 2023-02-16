
#include "stdafx.h"

/* ------------------------------------------------------------------------ --
--                                                                          --
--                        PC serial port connection object                  --
--                           for non-event-driven programs                  --
--                                                                          --
--                                                                          --
--                                                                          --
--  Copyright @ 2001          Thierry Schneider                             --
--                            thierry@tetraedre.com                         --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  Filename : tserial.cpp                                                  --
--  Author   : Thierry Schneider                                            --
--  Created  : April 4th 2000                                               --
--  Modified : April 8th 2001                                               --
--  Plateform: Windows 95, 98, NT, 2000 (Win32)                             --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  This software is given without any warranty. It can be distributed      --
--  free of charge as long as this header remains, unchanged.               --
--                                                                          --
-- ------------------------------------------------------------------------ */


// modified by Chris O. Nov-02-2010


/* ---------------------------------------------------------------------- */
// #define STRICT

#include "Windows.h"
#include "tserial.h"

/* -------------------------------------------------------------------- */
/* -------------------------    Tserial   ----------------------------- */
/* -------------------------------------------------------------------- */
Tserial::Tserial()
{
    parityMode         = spNONE;
    port[0]            = 0;
    rate               = 0;
    serial_handle      = INVALID_HANDLE_VALUE;
	dwReadTotalTimeout = 0;
}

/* -------------------------------------------------------------------- */
/* --------------------------    ~Tserial     ------------------------- */
/* -------------------------------------------------------------------- */
Tserial::~Tserial()
{
    if (serial_handle!=INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;
}
/* -------------------------------------------------------------------- */
/* --------------------------    disconnect   ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::disconnect(void)
{
	DWORD dwRet = 0; 
	if (serial_handle!=INVALID_HANDLE_VALUE)
	{
		SetCommTimeouts(serial_handle, &ctoOrg);
		SetCommState(serial_handle, &dcbOrg);
		SetCommMask(serial_handle, cmOrg);

		if (!CloseHandle(serial_handle))
		{
			dwRet = GetLastError();
		}

		Sleep(500);
	}

    serial_handle = INVALID_HANDLE_VALUE;

	return dwRet;
}
/* -------------------------------------------------------------------- */
/* --------------------------    connect      ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::connect (const char *port_arg, int rate_arg, serial_parity parity_arg)
{
    DWORD erreur;
    DCB  dcb;
	COMSTAT cs;
	DWORD errors = 0;
    COMMTIMEOUTS cto = {RxIntrvlTimeout, RxTotalTimeoutMultip, RxTotalTimeoutConst, TxTotalTimeoutMultip, TxTotalTimeoutConst};

    /* --------------------------------------------- */
    if (serial_handle != INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;

    erreur = 0;

    if (port_arg!=0)
    {
        strncpy(port, port_arg, iMaxPortName);
        rate      = rate_arg;
//      parityMode= parity_arg;
        memset(&dcb, 0, sizeof(dcb));

        /* -------------------------------------------------------------------- */
        // set DCB to configure the serial port
        dcb.DCBlength       = sizeof(dcb);                   
        
        /* ---------- Serial Port Config ------- */
		BuildCommDCB(_T("9600,n,8,1"), &dcb);
		dcb.BaudRate = rate;

        /* -------------------------------------------------------------------- */
		CString strPort;
		strPort.Format(_T("%S"), port);
        serial_handle = CreateFile(strPort, GENERIC_READ | GENERIC_WRITE,
								   0, NULL, OPEN_EXISTING, NULL, NULL);
        // opening serial port

        if (serial_handle != INVALID_HANDLE_VALUE)
        {
			GetCommTimeouts(serial_handle, &ctoOrg);
			GetCommState(serial_handle, &dcbOrg);
			GetCommMask(serial_handle, &cmOrg);

			// clear errors
			if (!ClearCommError(serial_handle, &errors, &cs))
			{
				erreur = 32;
				goto exitfun;
			}
			// clear buffers
            if(!PurgeComm(serial_handle, PURGE_TXCLEAR | PURGE_RXCLEAR))
			{
				erreur = 64;
				goto exitfun;
			}
            // set DCB
            if(!SetCommState(serial_handle,&dcb))
			{
				erreur = 4;
			}
             // set timeouts
            if(!SetCommTimeouts(serial_handle,&cto))
			{
				erreur = 2;
				goto exitfun;
			}
			dwReadTotalTimeout = cto.ReadTotalTimeoutConstant;

			// set mask
			if(!SetCommMask(serial_handle, 0))
			{
				erreur = 1;
				goto exitfun;
			}       
		}
        else
		{
			erreur = 8;
		}
    }
    else
	{
		erreur = 16;
	}


    /* --------------------------------------------- */

exitfun:	
	if (erreur != 0)
    {
        erreur = GetLastError();
		CloseHandle(serial_handle);
        serial_handle = INVALID_HANDLE_VALUE;
    }

    return erreur;
}


/* -------------------------------------------------------------------- */
/* --------------------------    sendChar     ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::sendChar(char data)
{
    return sendArray(&data, 1);
}

/* -------------------------------------------------------------------- */
/* --------------------------    sendArray    ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::sendArray(const char *buffer, int len)
{
    DWORD dwRet = 0;
	unsigned long result;

    if (serial_handle != INVALID_HANDLE_VALUE)
	{
        if (!WriteFile(serial_handle, buffer, len, &result, NULL))
		{
			dwRet = GetLastError();
		}
		else
		{
			if (len != result)
			{
				dwRet = 0xFFFF;
			}
		}
	}

	return dwRet;
}

/* -------------------------------------------------------------------- */
/* --------------------------    getChar      ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::getChar(char *c)
{
	DWORD iReadNum = 0;

    return getArray(c, 1, &iReadNum);
}

/* -------------------------------------------------------------------- */
/* --------------------------    getArray     ------------------------- */
/* -------------------------------------------------------------------- */
DWORD Tserial::getArray (char *buffer, int len, DWORD *iReadNum)
{
    unsigned long read_nbr = 0;
	int count=0;
    DWORD dwRet = 0;

    if(serial_handle!=INVALID_HANDLE_VALUE )
    {
	    if (!ReadFile(serial_handle, buffer, len, &read_nbr, NULL))
		{
			dwRet = GetLastError();
		}
    }

	*iReadNum = read_nbr;
    return dwRet;
}
/* -------------------------------------------------------------------- */
/* --------------------------    getNbrOfBytes ------------------------ */
/* -------------------------------------------------------------------- */
int Tserial::getNbrOfBytes    (void)
{
    struct _COMSTAT status;
    int             n;
    unsigned long   etat;

    n = 0;

    if (serial_handle != INVALID_HANDLE_VALUE)
    {
        ClearCommError(serial_handle, &etat, &status);
        n = status.cbInQue;
		PurgeComm (serial_handle,PURGE_TXCLEAR|PURGE_RXCLEAR);
    }

    return(n);
}


// for some operations the longer com port timeout is needed
DWORD Tserial::ChangeCommTimeouts(DWORD dwTimeout)
{
	COMMTIMEOUTS cto = {RxIntrvlTimeout, 
						RxTotalTimeoutMultip, 
						RxTotalTimeoutConst, 
						TxTotalTimeoutMultip, 
						TxTotalTimeoutConst};
	DWORD dwReadTotalTimeout = 0;
	DWORD dwRet = 0;

	if (serial_handle != INVALID_HANDLE_VALUE)
	{
		if (GetCommTimeouts(serial_handle, &cto) == 0)
		{
			// failed to get com port timeouts
			dwRet = GetLastError();
			goto exitfun;
		}

		cto.ReadTotalTimeoutConstant = dwTimeout;
		if (SetCommTimeouts(serial_handle, &cto) == 0)
		{
			// failed to set new com port timeout
			dwRet = GetLastError();
			goto exitfun;
		}
		dwReadTotalTimeout = cto.ReadTotalTimeoutConstant;
	}
	else
	{
		// com port not created yet
		dwRet = 0xFFFF;
	}

exitfun:
	return dwRet;
}

// resetting serial port timeouts to default values
DWORD Tserial::ResetCommTimeouts()
{
	COMMTIMEOUTS cto = {RxIntrvlTimeout, 
						RxTotalTimeoutMultip, 
						RxTotalTimeoutConst, 
						TxTotalTimeoutMultip, 
						TxTotalTimeoutConst};
	DWORD dwRet = 0;

	if (serial_handle != INVALID_HANDLE_VALUE)
	{
		if (SetCommTimeouts(serial_handle, &cto) == 0)
		{
			// failed to set new com port timeout
			dwRet = GetLastError();
		}
		dwReadTotalTimeout = cto.ReadTotalTimeoutConstant;
	}
	else
	{
		// com port not created yet
		dwRet = 0xFFFF;
	}

	return dwRet;
}

// setting serial port new baudrate
DWORD Tserial::ChangeCommBaudrate(DWORD dwBaudRate)
{
	DCB dcb;
	DWORD dwRet = 0;

	if (serial_handle != INVALID_HANDLE_VALUE)
	{
		if(GetCommState(serial_handle, &dcb) == 0)
		{
			dwRet = GetLastError();
			goto exitfun;
		}

		dcb.BaudRate = dwBaudRate;
		
		if(SetCommState(serial_handle, &dcb) == 0)
		{
			dwRet = GetLastError();
			goto exitfun;
		}

		// assume new baudrate
		rate = dwBaudRate;
	}
	else
	{
		// com port not created yet
		dwRet = 0xFFFF;
	}
	
exitfun:
	return dwRet;
}

// getting current serial port baudrate
DWORD Tserial::GetCommBaudrate(DWORD *dwBaudrate)
{
	DCB dcb;
	DWORD dwRet = 0;

	if (serial_handle != INVALID_HANDLE_VALUE)
	{
		if(GetCommState(serial_handle, &dcb) == 0)
		{
			dwRet = GetLastError();
			goto exitfun;
		}

		*dwBaudrate = dcb.BaudRate;
	}
	else
	{
		// com port not created yet
		dwRet = 0xFFFF;
	}

exitfun:
	return dwRet;
}

// close and reopen a port with the same parameters. Might be helpful when dealing 
// with bad device driver
DWORD Tserial::reconnect ()
{
	DWORD dwRet;
	if (serial_handle == INVALID_HANDLE_VALUE) return 0;

	disconnect();

	if ((dwRet = connect (port, rate, parityMode))!= 0)
		return dwRet;

	return ChangeCommTimeouts(dwReadTotalTimeout);
}


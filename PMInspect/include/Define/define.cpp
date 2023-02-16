#include "stdafx.h"
//#include "afxwin.h"
#include "define.h"

void DoEvents()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

void Delay(long ms, BOOL bEvent)
{
	double s, e;
	double Ms = (double)ms;
	s = (double)GetCurrentTime();
	do
	{
		if (bEvent)
			DoEvents();

		Sleep(1);
		e = (double)GetCurrentTime();
	} while ((e - s) < Ms);
}


BOOL FileDelete(CString FilePath, CString FileName)
{
	CFileFind ff;
	CString strTarget = FilePath;
	CStringArray strFileArray;
	BOOL rslt = FALSE;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				continue;
			}
			if(ff.GetFileName() == FileName)
			{
				DeleteFile(Filename);
				break;
			}
		}
	}
	ff.Close();
	
	return rslt;
}


BOOL FileSearch(CString FilePath, CString FileName)
{
	CFileFind ff;
	CString strTarget = FilePath;
	CStringArray strFileArray;
	BOOL rslt = FALSE;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				continue;
			}
			if(ff.GetFileName() == FileName)
			{
				rslt = TRUE;
				break;
			}
		}
	}
	ff.Close();
	
	return rslt;
}

BOOL MakeDirectories(CString path)
{
	wchar_t tmp_path[MAX_PATH];
	wchar_t sub[MAX_PATH];
	long size;
	size = sizeof(path);

	long index = 0;

	DWORD	Attr;

	wsprintf(tmp_path, _T("%s"), (LPCTSTR)path);
	memset(sub, NULL, sizeof(sub));

	while(1)
	{
		if(tmp_path[index] == _T('\\'))
		{
			Attr = GetFileAttributes(sub);
			if (Attr == -1 || (Attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!CreateDirectory(sub, NULL))
				{
					AfxMessageBox(_T("Error : Create Failed(_MakeDirectories)!"));
					return	FALSE;
				}
			}
		}

		sub[index] = tmp_path[index];

		if(tmp_path[index] == _T('\0'))
		{
			Attr = GetFileAttributes(sub);
			if (Attr == -1 || (Attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!CreateDirectory(sub, NULL))
				{
					AfxMessageBox(_T("Error : Create Failed(_MakeDirectories)!"));
					return	FALSE;
				}
			}
			break;
		}

		//base_index++;
		index++;

		if( (index > MAX_PATH) )
		{
			AfxMessageBox(_T("Error : index overflow(_MakeDirectories)!"));
			return	FALSE;
		}
	}

	return	TRUE;
}

BOOL WriteLog(CString strMsg, int nKey)
{
	FILE*	stream;
	char	strToFile[MAX_PATH];
	long	result, line;
	
	char	szTime[MAX_CHAR];
	memset(szTime, NULL, sizeof(szTime));
	
	int limit_line = 500000;
	CString path, log, strKey, bak_file, strTopName;
	char buffer[MAX_CHAR];
	char strFile[MAX_CHAR];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	//CTime NowTime = CTime::GetCurrentTime();
	//strCurtime = NowTime.Format(_T("%Y")) + "\\" + NowTime.Format(_T("%m")) + "\\" + NowTime.Format(_T("%d"));

	switch (nKey)
	{
	case SYSTEM_LOG:
		strKey.Format(_T("%s\\%04d\\%02d\\%02d"), SYSYEM_LOG_PATH, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
		strTopName.Format(_T("DATE,TIME,MESSAGE"));
		break;
	case TACTTIME_LOG:
		strKey.Format(_T("%s\\%04d\\%02d\\%02d"), TACTTIME_LOG_PATH, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
		strTopName.Format(_T("DATE,TIME,TOTAL"));
		break;
	case VISION_LOG:
		strKey.Format(_T("%s\\%04d\\%02d\\%02d"), VISION_LOG_PATH, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
		strTopName.Format(_T("DATE,TIME,MESSAGE"));
		break;
	default:

		break;
	}

	MakeDirectories(strKey);

	//log.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d:%03d,%s"), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpSystemTime.wMilliseconds, strMsg);
	log.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d,%s"), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, strMsg);

	sprintf(strFile, "%S\\Log.csv", strKey);

	if ((stream = _fsopen(strFile, "a+", _SH_DENYNO)) == NULL)
	{
		HANDLE fd = CreateFile((LPCWSTR)(LPCSTR)strFile,
			GENERIC_READ | GENERIC_WRITE,
			//GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		stream = _fsopen(strFile, "a+", _SH_DENYNO);
		if (stream == NULL)
		{
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
	result = fseek(stream, 0L, SEEK_SET);
	if (result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while (fgets(buffer, MAX_CHAR, stream))
	{
		line++;
		if (1 < line)
		{
			break;
		}
	}

	if (0 == line)
		log = strTopName;


	result = fseek(stream, 0L, SEEK_END);
	if (result)
	{
		fclose(stream);
		return FALSE;
	}

	sprintf_s(szTime, log.GetLength() + 1, "%S", log);

	fputs(szTime, stream);
	fputs("\n", stream);

	result = fseek(stream, 0L, SEEK_SET);
	if (result)
	{
		fclose(stream);
		return FALSE;
	}

	if (0 == line)
		WriteLog(strMsg, nKey);

	return TRUE;
}
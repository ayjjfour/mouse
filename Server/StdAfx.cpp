// stdafx.cpp : source file that includes just the standard includes
//	ServerSite.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CString AfxGetApplicationDir()
{
	TCHAR sPath[MAX_PATH] = _T("");
	::GetModuleFileName(NULL ,sPath ,MAX_PATH);
	*(_tcsrchr(sPath ,_T('\\')) + 1) = _T('\0');

	return sPath;
}

CString Format(CString szFormat, ...)
{
	TCHAR pcBuffer[1024];

	va_list args;
	va_start(args, szFormat);
	{
		_vstprintf_s(pcBuffer, szFormat, args);
	}
	va_end(args);

	return pcBuffer;
}
#include "stdafx.h"
#include "TraceOut.h"

//CChatClient * g_pChatter;
ITrace * g_pChatter;

HRESULT Trace(LPCSTR lpszFormat, ...)
{
#ifndef _NO_TRACE
	if (g_pChatter==NULL) return E_FAIL;

	va_list args;
	va_start(args, lpszFormat);

	int nBuf;char szBuffer[2048];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
	if(nBuf < 0)
		return E_FAIL;

	// was there an error? was the expanded string too long?
	int lenofbuf=strlen(szBuffer);
	if(lenofbuf >= 2048 - 1)
		szBuffer[2048] = 0;
	else {
		//	add the log time to the string
		char timebuf[255];
		wsprintf(timebuf, ". log time: %s", LPCTSTR(GetTime()));
		lstrcpyn(szBuffer+lenofbuf, timebuf, 2048-lenofbuf-strlen(timebuf));
	}
	g_pChatter->ThreadSafeShowTrace(szBuffer);

	va_end(args);

	return S_OK;

#else	//_NO_TRACE
	return E_FAIL;
#endif	//_NO_TRACE
}

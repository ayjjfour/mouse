#ifndef __TRACE_H__
#define __TRACE_H__

#include "TableServer.h"

//extern CChatClient * g_pChatter;

extern ITrace * g_pChatter;

//#define Trace g_pChatter->ThreadSafeDebugPrint
/*
inline CString GetTime()
{
	CTime time;
	CString str;
	time = CTime::GetCurrentTime();
	str.Format("%d.%d.%d, %d:%d:%d",time.GetYear(),time.GetMonth(),time.GetDay(),
		time.GetHour(),time.GetMinute(),time.GetSecond());
	return str;
}
*/
HRESULT Trace(LPCSTR lpszFormat, ...);

#define Trace_L1 Trace
#define Trace_L2 Trace
#define Trace_L3 Trace

#ifdef _SRV_TRACE_LEVEL_1
#else
	#ifdef _SRV_TRACE_LEVEL_2
#undef Trace_L1
#define Trace_L1 (NULL)
	#else
		#ifdef _SRV_TRACE_LEVEL_3
		#undef Trace_L1
		#define Trace_L1 (NULL)
		#undef Trace_L2
		#define Trace_L2 (NULL)
		#endif	// _TRACE_LEVEL_3
	#endif	// _TRACE_LEVEL_2
#endif	//_TRACE_LEVEL_1

#define LOGD(f, ...) Trace(LEVEL_DEBUG, f, __VA_ARGS__)
#define LOGI(f, ...) Trace(LEVEL_INFO, f, __VA_ARGS__)
#define LOGW(f, ...) Trace(LEVEL_WARN, f, __VA_ARGS__)
#define LOGE(f, ...) Trace(LEVEL_ERROR, f, __VA_ARGS__)
#define LOGA(f, ...) Trace(LEVEL_ALARM, f, __VA_ARGS__)
#define LOGF(f, ...) Trace(LEVEL_FATAL, f, __VA_ARGS__)

#endif	//__TRACE_H__

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FC01905F_46CD_4BE1_86E2_F4FDF99AFF6F__INCLUDED_)
#define AFX_STDAFX_H__FC01905F_46CD_4BE1_86E2_F4FDF99AFF6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _CLIENT
#undef _CLIENT
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include <algorithm>
#include <string>
#include <stdint.h>
#include "Trace.h"
#include "MT19937.h"

extern CString AfxGetApplicationDir();

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
extern CString Format(CString szFormat, ...);

#endif // !defined(AFX_STDAFX_H__FC01905F_46CD_4BE1_86E2_F4FDF99AFF6F__INCLUDED_)

#define WILD_MONSTER_HP
//锤耗儿金币日志
//#define LOG_TRACE


//自动锁
class AutoLock
{
public:
	AutoLock(CRITICAL_SECTION& lock)
	{
		m_pLock = &lock;

		EnterCriticalSection(m_pLock);
	}

	~AutoLock()
	{
		LeaveCriticalSection(m_pLock);
	}
private:
	LPCRITICAL_SECTION m_pLock;
};
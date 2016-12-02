// Server.h : main header file for the SERVER DLL
//

#if !defined(AFX_SERVER_H__4FFC6555_B6AC_4F3B_B6FF_1DD1614C8B12__INCLUDED_)
#define AFX_SERVER_H__4FFC6555_B6AC_4F3B_B6FF_1DD1614C8B12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CServerApp
// See Server.cpp for the implementation of this class
//

class CServerApp : public CWinApp
{
public:
	CServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVER_H__4FFC6555_B6AC_4F3B_B6FF_1DD1614C8B12__INCLUDED_)

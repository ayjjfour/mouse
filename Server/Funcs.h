#ifndef _FUNCS_H
#define _FUNCS_H
#include "common.h"

extern void	Delay(DWORD ms);	// wait for some ms
extern int	DoEvent(HWND hWnd,BOOL bNote = TRUE);	// release CPU time for other processes
extern int	DoEventWait(DWORD dwWaitTime, HWND hWnd);
extern void	ShowLastError();	// show system error message
//class CChatClient;
//extern void	DebugTrace(CChatClient *pChat,LPCTSTR lpszFormat, ...);
extern HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,HPALETTE FAR* lphPalette);
extern HINSTANCE GotoURL(LPCTSTR url, int showcmd);
extern void SetTopWindow(CWnd *pWnd);

extern HRESULT CreateShortCut(LPCSTR pszShortcutFile, LPCSTR pszLink, LPSTR pszDesc,LPCSTR pszWorkingDir);
extern BOOL GetIPFromIPString(char *szName,char ip[4]);
extern BOOL ExitIGameMainWnd(CWnd *pMainWnd);

struct CWindowsVersion
{
	enum WINDOWS_VERSION
	{
		osWin95 = 0,osWin98 = 1,osWinNT = 2,osWin2K = 3
	};
	CWindowsVersion()
	{
		m_ver = osWin98;

		OSVERSIONINFO osvi;
		memset(&osvi,0,sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);
		switch (osvi.dwPlatformId)
		{
		  case VER_PLATFORM_WIN32_NT:
			 if ( osvi.dwMajorVersion <= 4 )
				m_ver = osWinNT;
			 if ( osvi.dwMajorVersion == 5 )
				m_ver = osWin2K;
			 break;
		  case VER_PLATFORM_WIN32_WINDOWS:
			 if ((osvi.dwMajorVersion > 4) || ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)))
				 m_ver = osWin98;
			 else 
				 m_ver = osWin95;
			 break;
		}
	}
	WINDOWS_VERSION GetVer(){ return m_ver;	}
	BOOL IsWindows95(){	return m_ver == osWin95;}
	BOOL IsWindows98(){ return m_ver == osWin98;}
	BOOL IsWindowsNT(){ return m_ver == osWinNT;}
	BOOL IsWindows2K(){	return m_ver == osWin2K;}
private:
	WINDOWS_VERSION m_ver;
};

#endif

// HTMLChat.cpp : Implementation of CNewFrameApp and DLL registration.

#include "stdafx.h"
#include "HTMLChat.h"
#include "htmlEncode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CHTMLChat::create(HWND hParent, RECT &rc)
{
	if (!m_hChatterDll) {
		m_hChatterDll =::LoadLibraryEx("htmlChat.dll", NULL, 0);
		if (!m_hChatterDll) {
			TRACE("can't load htmlChat.dll");
			ASSERT(FALSE);
			return FALSE;
		}
	}

	typedef BOOL (*typecreateChatter)(HWND, RECT &, IHTMLChat ** );

	typecreateChatter createChatter =(typecreateChatter)GetProcAddress(m_hChatterDll, "createChatter");
	if (createChatter==NULL) {
		TRACE("can't locate createChatter");
		return FALSE;
	}

	if (m_pChat) m_pChat->Delete();
	
	if (SUCCEEDED(createChatter(hParent, rc, &m_pChat))) {
		m_hWnd =m_pChat->getWnd();
		m_bNeedDestoryChat =TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CHTMLChat::attach(IHTMLChat * pChatter)
{
	if (!pChatter) return E_INVALIDARG;

	if (m_pChat) m_pChat->Delete();

	m_pChat =pChatter;
	m_hWnd =m_pChat->getWnd();

	return TRUE;
}

void CHTMLChat::formatString(string & strOut, LPCSTR szMsg, char * szType, COLORREF color, 
					int nTextHeight, DWORD dwStyle, 
					BOOL bLn, BOOL bShowTime)
{
	ASSERT(szMsg);

	strOut ="<span ";
	if (szType) {
		strOut+="class='";
		strOut+=szType;
		strOut+="'";
	}

	string modifyStr;
	if (color!=TC_DEFAULT) {
		char colorBuf[255];
		wsprintf(colorBuf, "color=#%02X%02X%02X; ", GetRValue(color), GetGValue(color), GetBValue(color));

		modifyStr += colorBuf;
	}
	if (nTextHeight!=TH_DEFAULT) {
		char fontBuf[255];
		wsprintf(fontBuf, "font-size=%dpt; ", nTextHeight);

		modifyStr +=fontBuf;
	}
	if (dwStyle!=TS_DEFAULT) {
		char styleBuf[255];
		if (dwStyle&CFE_BOLD) {
			strcpy(styleBuf+strlen(styleBuf), "font-weight : bold; ");
		}
		if (dwStyle&CFE_ITALIC) {
			strcpy(styleBuf+strlen(styleBuf), "font-style: italic; ");
		}
		if (dwStyle&CFE_UNDERLINE || dwStyle&CFE_LINK) {
			strcpy(styleBuf+strlen(styleBuf), "text-decoration : underline; ");
		}
		if (dwStyle&CFE_STRIKEOUT) {
			strcpy(styleBuf+strlen(styleBuf), "text-decoration : line-through; ");
		}
		if (dwStyle&CFE_PROTECTED) {
		}

		modifyStr +=styleBuf;
	}
	
	if (modifyStr.length()!=0) {
		strOut+=" style='";
		strOut +=modifyStr;
		strOut+="'";
	}

	strOut +=">";

	strOut+=szMsg;

	if (bShowTime) {
		SYSTEMTIME sysTime;
		::GetSystemTime(&sysTime);

		char timeBuf[255];
		wsprintf(timeBuf, ", %d-%d-%d %d:%d:%d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

		strOut +=timeBuf;
	}

	strOut+="</span>";
}

BOOL CHTMLChat::ShowTrace(LPCSTR szMsg, COLORREF color,
	int nTextHeight,DWORD dwStyle)
{
	ASSERT(m_pChat);
	
	string buf;

	formatString(buf, szMsg, "TRACETEXT", color, nTextHeight, dwStyle);

	if (m_pChat) m_pChat->addString(buf.c_str());

	return TRUE;
}

// show a string without '\r\n'
// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
BOOL CHTMLChat::ThreadSafeShowTrace(LPCSTR szMsg, COLORREF color,
	int nTextHeight,DWORD dwStyle)
{
	string buf;

	formatString(buf, szMsg, "TRACETEXT", color, nTextHeight, dwStyle);

	safeStoreMsg(buf);

	return TRUE;
}

//********************************  0.2版  ***********************************
// 直接调试信息显示
// 说明: 不带换行回车符("\r\n"), dwStyle可选值: CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,
//	CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
BOOL CHTMLChat::ShowTrace2(LPCSTR szMsg, COLORREF color, 
						int nTextHeight, DWORD dwStyle, 
						BOOL bLn, BOOL bShowTime)
{
//	ASSERT(m_pChat);
	if (!m_pChat) {
		TRACE(szMsg);
		return TRUE;
	}

	string buf;

	formatString(buf, szMsg, "TRACETEXT", color, nTextHeight, dwStyle, bLn, bShowTime);

	if (m_pChat) m_pChat->addString(buf.c_str());

	return TRUE;
}

// 线程安全调试信息显示(通过消息机制实现)
// 说明同上
BOOL CHTMLChat::ThreadSafeShowTrace2(LPCSTR szMsg, COLORREF color, 
						int nTextHeight, DWORD dwStyle, 
						BOOL bLn, BOOL bShowTime)
{
	string buf;

	formatString(buf, szMsg, "TRACETEXT", color, nTextHeight, dwStyle, bLn, bShowTime);

	safeStoreMsg(buf);

	return TRUE;
}

BOOL CHTMLChat::ThreadSafeGetTraceMsg()
{
	if (!m_pChat) return FALSE;

	if (m_mtxChat) {
		DWORD dwWaitResult = WaitForSingleObject(m_mtxChat, INFINITE);
		if (dwWaitResult==WAIT_OBJECT_0) {
			showMsg();
			::ReleaseMutex(m_mtxChat);
		}
		return TRUE;
	}

	showMsg();

	return TRUE;
}

void CHTMLChat::safeStoreMsg(string &str)
{
	if (!m_mtxChat) {
		m_mtxChat =::CreateMutex(NULL, FALSE, NULL);
		ASSERT(m_mtxChat);
	}
	
	DWORD dwWaitResult = WaitForSingleObject(m_mtxChat, INFINITE);
	if (dwWaitResult==WAIT_OBJECT_0) {
		if(m_cache.size() > MAX_THREAD_MESSAGE)
		{
			TRACE("=>CHTMLClient::safeStoreMsg() : message buf overflow\n");
		}
		else m_cache.push_front(str);

		::ReleaseMutex(m_mtxChat);

		if (m_cache.size()==1 && m_hThreadSafeNotifyWnd) ::PostMessage(m_hThreadSafeNotifyWnd, WM_THREAD_TRACE_EX, 0, 0);
	}
}

void CHTMLChat::showMsg()
{
	ASSERT(m_pChat);

	while (m_cache.size()>0) {
		_str_it it =m_cache.begin();
		if (m_pChat) m_pChat->addString((*it).c_str());

		m_cache.pop_front();
	}
}

void CHTMLChat::leftTrim(char *str)
{
	ASSERT(str);

	if (!str || strlen(str)==0) return;

	unsigned int i = 0;

	for (i = 0; i < strlen(str); i++) if (str[i]!=' ') break;

	memmove(str, &str[i], strlen(str));
}

void CHTMLChat::rightTrim(char *str)
{
	ASSERT(str);

	if (!str || strlen(str)==0) return;

	for (int i=strlen(str)-1; i>=0; i--) 
		if (str[i]!=' ') {
			str[i+1] ='\0';
			return;
		}
}

void CHTMLChat::trimString(char *str)
{
	ASSERT(str);

	if (!str || strlen(str)==0) return;

	rightTrim(str);
	leftTrim(str);
}


void CHTMLChat::parseChat(char *str)
{
	ASSERT(str);

	trimString(str);
}

void CHTMLChat::TRACELN(LPCSTR lpszFormat, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;char szBuffer[1024];

	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 8, lpszFormat, args);
	if(nBuf < 0)
		return;

	// was there an error? was the expanded string too long?
	strcat(szBuffer,"\r\n");
	if(strlen(szBuffer) >= 500)
		szBuffer[500] = 0;
	TRACE(szBuffer);
	va_end(args);
#endif	
}

BOOL CHTMLChat::ShowChat2V(LPCSTR szSpeaker,LPCSTR szTalkWith,COLORREF color,LPCSTR szMsg,int nTextHeight,DWORD dwStyle)
{
	ASSERT(szMsg);
	if(szMsg == NULL || szMsg[0] == 0)
		return FALSE;

//	ASSERT(m_pChat);
	if (!m_pChat) return FALSE;
	
	if(strlen(szMsg) >= MAX_USER_CHAT_MSG_LEN)
	{
		TRACE("CHTMLChat::ShowChat() : message is too long\n");
		return FALSE;
	}

	char temp[MAX_USER_CHAT_MSG_LEN*2];
	wsprintf(temp, "<font color=darkgreen>%s", htmlEncode(szSpeaker));
	if (strlen(szTalkWith)!=0) {
		wsprintf(temp+strlen(temp), "对%s说: </font>", htmlEncode(szTalkWith));
	}
	else {
		strcat(temp, "说: </font>");
	}
	
	char txtClrBuf[255];
	wsprintf(txtClrBuf, "<font color=#%02X%02X%02X>", GetRValue(color), GetGValue(color), GetBValue(color));
	strcat(temp, txtClrBuf);
	strcat(temp, szMsg);
	strcat(temp, "</font>");

	ChatPrint(temp);

	return TRUE;	
}

BOOL CHTMLChat::ShowManagerMsg(LPCSTR szMsg,int nTextHeight,DWORD dwStyle)
{
//	ASSERT(m_pChat);
	if (!m_pChat) {
		TRACE(szMsg);
		return TRUE;
	}
	
	string out;

	formatString(out, szMsg, "MANAGETEXT", TC_DEFAULT, nTextHeight, dwStyle);

	if (m_pChat) {
		m_pChat->addString(out.c_str());
		return TRUE;
	}

	return FALSE;
}

BOOL CHTMLChat::ChatPrint(LPCSTR szMsg,int nTextHeight,DWORD dwStyle)
{
//	ASSERT(m_pChat);
	if (!m_pChat) {
		TRACE(szMsg);
		return TRUE;
	}

	char buf[MAX_USER_CHAT_MSG_LEN];
	lstrcpyn(buf, szMsg, sizeof(buf));
	trimString(buf);

	string out;
	formatString(out, buf, "CHATTEXT", TC_DEFAULT, nTextHeight, dwStyle);

	if (m_pChat) {
		m_pChat->addString(out.c_str());
		return TRUE;
	}

	return FALSE;
}

BOOL CHTMLChat::ChatPrint(LPCSTR szMsg, COLORREF color,int nTextHeight,DWORD dwStyle)
{
//	ASSERT(m_pChat);
	if (!m_pChat) {
		TRACE(szMsg);
		return TRUE;
	}

	char buf[MAX_USER_CHAT_MSG_LEN];
	lstrcpyn(buf, szMsg, sizeof(buf));
	trimString(buf);

	string out;
	formatString(out, buf, "CHATTEXT", color, nTextHeight, dwStyle);

	if (m_pChat) {
		m_pChat->addString(out.c_str());
		return TRUE;
	}

	return FALSE;
}

// 用于显示调试信息
#define MAX_DEBUG_MSG_LEN	2048
void CHTMLChat::DebugPrint(LPCSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;char szBuffer[MAX_DEBUG_MSG_LEN];

	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 8, lpszFormat, args);
	if(nBuf < 0)
		return;

	// was there an error? was the expanded string too long?
	if(strlen(szBuffer) >= MAX_DEBUG_MSG_LEN - 1)
		szBuffer[MAX_DEBUG_MSG_LEN - 1] = 0;
	ShowManagerMsg(szBuffer);
	strcat(szBuffer,"\r\n");
	if(strlen(szBuffer) >= 500)
		szBuffer[500] = 0;
	TRACE(szBuffer);
	va_end(args);
}

// 功能：线程安全的调试信息显示
// 实现：将消息加入队列
void CHTMLChat::ThreadSafeDebugPrint(LPCSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;char szBuffer[MAX_DEBUG_MSG_LEN];

	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 8, lpszFormat, args);
	if(nBuf < 0)
		return;

	// was there an error? was the expanded string too long?
	if(strlen(szBuffer) >= MAX_DEBUG_MSG_LEN - 1)
		szBuffer[MAX_DEBUG_MSG_LEN - 1] = 0;

	string out;

	formatString(out, szBuffer, "MANAGETEXT");
	safeStoreMsg(out);

	strcat(szBuffer,"\r\n");
	if(strlen(szBuffer) >= 500)
		szBuffer[500] = 0;
	TRACE(szBuffer);
	va_end(args);
}

// 用于显示调试信息和系统信息
BOOL CHTMLChat::ShowSystemMsg(LPCSTR szMsg,int nTextHeight,DWORD dwStyle)
{
	ASSERT(szMsg);
	if(szMsg == NULL)
		return FALSE;
	if(szMsg[0] == 0)
		return FALSE;

	ASSERT(szMsg);

	char buf[MAX_USER_CHAT_MSG_LEN];
	lstrcpyn(buf, szMsg, sizeof(buf));
	trimString(buf);

	string out;
	formatString(out, buf, "SYSTEMTEXT", TC_DEFAULT, nTextHeight, dwStyle);

	if (m_pChat) {
		m_pChat->addString(out.c_str());
		return TRUE;
	}

	return FALSE;
}

// 按指定颜色显示消息
BOOL CHTMLChat::ShowChat(LPCSTR szSpeaker,LPCSTR szTalkWith,COLORREF color,LPCSTR szMsg,int nTextHeight,DWORD dwStyle)
{
	ASSERT(szMsg);
	if(szMsg == NULL || szMsg[0] == 0)
		return FALSE;

//	ASSERT(m_pChat);
	if (!m_pChat) return FALSE;
	
	if(strlen(szMsg) >= MAX_USER_CHAT_MSG_LEN)
	{
		TRACE("CHTMLChat::ShowChat() : message is too long\n");
		return FALSE;
	}

	char temp[MAX_USER_CHAT_MSG_LEN];
	wsprintf(temp, "<font color=darkgreen>%s", htmlEncode(szSpeaker));
	if (strlen(szTalkWith)!=0) {
		wsprintf(temp+strlen(temp), "对%s说: </font>", htmlEncode(szTalkWith));
	}
	else {
		strcat(temp, "说: </font>");
	}
	strcat(temp, szMsg);

	ChatPrint(temp);

	return TRUE;
}

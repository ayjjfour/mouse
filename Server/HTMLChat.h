// HTMLChat.h: Definition of the CHTMLChat class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLCHAT_H__16B8D693_16F0_4152_8421_0250018707D5__INCLUDED_)
#define AFX_HTMLCHAT_H__16B8D693_16F0_4152_8421_0250018707D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "utility.h"
#include "browserCtrl.h"
#include "ITrace.h"

/////////////////////////////////////////////////////////////////////////////
// CHTMLChat

#pragma warning(push)
#pragma warning(disable: 4786)
#include <list>
#include <string>


using namespace std;

#ifdef _DEBUG
	#define MAX_CHATTER_MESSAGE_LEN	(60 * 1024)
#else
	#define MAX_CHATTER_MESSAGE_LEN	(32 * 1024)
#endif

#define MAX_USER_CHAT_MSG_LEN		1024	// 用户最长可以显示这么长的串

#define MAX_CHAT_MSG_LEN	255

class CHTMLChat : public ITrace
{
public:		//ITrace
	//********************************  0.1版  ***********************************
	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ShowTrace(LPCSTR szMsg, COLORREF color,
		int nTextHeight = 9,DWORD dwStyle = 0);

	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ThreadSafeShowTrace(LPCSTR szMsg, COLORREF color,
		int nTextHeight = 9,DWORD dwStyle = 0);
	//********************************  0.1版  ***********************************




	//********************************  0.2版  ***********************************
	// 直接调试信息显示
	// 说明: 不带换行回车符("\r\n"), dwStyle可选值: CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,
	//	CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn =	FALSE, BOOL bShowTime = FALSE);
	// 线程安全调试信息显示(通过消息机制实现)
	// 说明同上
	virtual BOOL ThreadSafeShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// 线程安全获取消息队列中的消息并显示出来
	virtual BOOL ThreadSafeGetTraceMsg();
	// 获取内部创建的窗口句柄
	virtual HWND GetTraceWindow() {
		if (!m_pChat) return NULL;

		return m_pChat->getWnd();
	};
	// 释放(包括自带窗口的销毁)
	virtual BOOL Release() {
		//this class will never release itself
		return TRUE;
	};
	//********************************  0.2版  ***********************************

	//	old chat client
public:
	void TRACELN(LPCSTR lpszFormat, ...);
	BOOL SetDefaultFormat(int nTextHeight,COLORREF color) {
		// in HTML chat, format was defined in HTML file
		return FALSE;
	};
	BOOL ShowChat2V(LPCSTR szSpeaker,LPCSTR szTalkWith,COLORREF color,LPCSTR szMsg,int nTextHeight = 9,DWORD dwStyle = 0);
	BOOL ShowManagerMsg(LPCSTR szMsg,int nTextHeight = TH_DEFAULT,DWORD dwStyle = TS_DEFAULT);

	BOOL ChatPrint(LPCSTR szMsg,int nTextHeight = TH_DEFAULT,DWORD dwStyle = TS_DEFAULT);
	BOOL ChatPrint(LPCSTR szMsg, COLORREF color,int nTextHeight = TH_DEFAULT,DWORD dwStyle = TS_DEFAULT);

	// method
	void SetMyName(LPCSTR szMyName) {
		ASSERT(szMyName);

		lstrcpyn(m_szMyName, szMyName, sizeof(m_szMyName));
	}

	// 设置chat窗口的背景颜色
	void SetBkColor(COLORREF color) {
		ASSERT(m_pChat);

		if (m_pChat) m_pChat->SetBgColor(color);
	}

	//	设置chat窗口的背景图
	void SetBackground(char * filename) {
		ASSERT(m_pChat);

		if (m_pChat) m_pChat->SetBackground(filename);
	}

	BOOL ShowSystemMsg(LPCSTR szMsg,int nTextHeight = TH_DEFAULT,DWORD dwStyle = TS_DEFAULT);		// 用于显示调试信息和系统信息
	BOOL ShowChat(LPCSTR szSpeaker,LPCSTR szTalkWith,COLORREF color,LPCSTR szMsg,int nTextHeight = TH_DEFAULT,DWORD dwStyle = TS_DEFAULT);	// 按指定颜色显示消息
	// 表情指令
	BOOL GetEmotion(LPCSTR szSpeaker,LPCSTR szEmotionCmd,char *lpszReturnMsg) {
		return FALSE;
	}
	BOOL IsLocalMessage() {
		return m_bIsLocalMessage;
	}

	void EmptyChatter() {
		ASSERT(m_pChat);

		if (m_pChat) m_pChat->Clear();
	}

	// 线程安全显示
	void SetThreadSafeNotifyWnd(HWND hNotifyWnd) {
		ASSERT(hNotifyWnd);
		m_hThreadSafeNotifyWnd =hNotifyWnd;
	}
	void ThreadSafeDebugPrint(LPCSTR lpszFormat, ...);

	void DebugPrint(LPCSTR lpszFormat, ...);

	// 设置是否自动滚屏
	void SetAutoScroll(BOOL bAutoScroll = TRUE)
	{
		m_bAutoScroll = bAutoScroll;
		ASSERT(m_pChat);
		if (m_pChat) m_pChat->AutoScroll(bAutoScroll);
	}
	BOOL GetAutoScroll()
	{
		return m_bAutoScroll;
	}

public:
	HWND	m_hWnd;

private:
	BOOL			m_bAutoScroll;
	HWND			m_hThreadSafeNotifyWnd;

	char			m_szMyName[MAX_NAME_LEN];	// 当前用户名

	HANDLE			m_mtxChat;						// 用于在广场时多个客户端同时向chat窗口发字符串
	list<string> m_cache;		//thread mag cache
	typedef list<string>::iterator _str_it;

	BOOL			m_bIsLocalMessage;			// 每次调用GetEmotion后，保留当前消息是否仅本地显示(如出错信息)


public:
	CHTMLChat() {
		m_hChatterDll =NULL;
		m_pChat =NULL;
		m_mtxChat =NULL;

		m_bAutoScroll =TRUE;
		m_hThreadSafeNotifyWnd =NULL;

		m_bIsLocalMessage =FALSE;
		m_szMyName[0] =0;
		
		m_hWnd =NULL;

		m_bNeedDestoryChat =FALSE;
	}
	virtual ~CHTMLChat() {
#ifdef _DEBUG
		if (m_pChat && m_bNeedDestoryChat) {
			m_pChat->Delete();
			m_pChat =NULL;
		}
		SAFE_FREELIB(m_hChatterDll);
		if (m_mtxChat) ::CloseHandle(m_mtxChat);
#else
		try {
			if (m_pChat && m_bNeedDestoryChat) {
				m_pChat->Delete();
				m_pChat =NULL;
			}
			SAFE_FREELIB(m_hChatterDll);
			if (m_mtxChat) ::CloseHandle(m_mtxChat);
		} catch(...) {
		}
#endif
	}

public:
	BOOL create(HWND hParent, RECT &rc);
	BOOL attach(IHTMLChat * pChatter);

	void inputHandler(MSG * pMsg) {
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam!=VK_RETURN) {
			if (::IsWindow(m_hWnd)) 
				::SendMessage(m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
		}
	}
	IHTMLChat * m_pChat;
	BOOL	m_bNeedDestoryChat;			//只有当chat时自己创建的时候才删除

private:
	void parseChat(char * str);
	void rightTrim(char *str);
	void leftTrim(char * str);
	void trimString(char * str);
	void showMsg();
	void safeStoreMsg(string & str);
	void formatString(string &strOut, LPCSTR szMsg,  char * szType =NULL, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, BOOL bLn = FALSE, BOOL bShowTime = FALSE);

	HMODULE	m_hChatterDll;
};

#pragma warning(pop)
#endif // !defined(AFX_HTMLCHAT_H__16B8D693_16F0_4152_8421_0250018707D5__INCLUDED_)

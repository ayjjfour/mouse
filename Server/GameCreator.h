#ifndef __GAMECREATE_H__
#define __GAMECREATE_H__

#ifdef _CLIENT

#include "site_i.h"
#include "browserCtrl.h"

#include "HTMLChat.h"


interface IClientFrame;
extern IClientFrame * g_pFrame;

#ifndef _FRAME_
extern CHTMLChat g_TraceHelper;
#endif	//!_FRAME_
extern CHTMLChat * g_pChat;

#ifndef _FRAME_
#ifdef _DEBUG
#define Trace g_TraceHelper.DebugPrint
#else 
#define Trace (void)0;
#endif
#endif //!_FRAME_

//	added by hhh 20050422
//	增加了自动缩放的功能
#define WM_RECALCLAYOUT	800
struct RECALCLAYOUTINFO
{
	RECT rcFrameWnd;	//框架窗体的大小
	RECT rcGameWnd;		//建议游戏窗口的大小
	RECT rcControlWnd;	//右边的窗口的大小
};

//	modified by hhh 20030709
//	将 GameDll.h, bowserCtrl.h合并了

interface __declspec(novtable) IClientFrame
{
	virtual void SetTopWindow() PURE;
	virtual void Quit() PURE;
	virtual BOOL IsValidWindow() PURE;
	virtual HINSTANCE GetResInst() PURE;
	virtual const char * GetWorkingDir() PURE;
	virtual BOOL ShowUserIO(BOOL bShow =TRUE) PURE;
	virtual BOOL EnableSetTopWindow(BOOL bEnable =TRUE) PURE;
	virtual HRESULT GetChatCore(IHTMLChat ** ppChatCore) PURE;
	virtual HRESULT GetCommLink(void ** ppLink) PURE;
	virtual BOOL	EnableSound(BOOL bEnable =TRUE) PURE;
	virtual HRESULT	Sound(const char * sndFile, BOOL bAsync =FALSE) PURE;
	virtual HRESULT BackHall() PURE;
	virtual HRESULT GetWorkingDir(char * szDir) PURE;
	virtual HRESULT ShowHelp() PURE;
	virtual HRESULT setAD(const char * url) PURE;

	virtual void setIcon(HICON hIcon) PURE;
	virtual void setWindowText(const char * pText) PURE;
	virtual void getWindowText(char * pText, int nSize) PURE;

	virtual HWND getFrameWnd() PURE;

	virtual HRESULT getClientSite(IClientSite ** ppSite) PURE;
};

interface __declspec(novtable) IClientFrameEx : public IClientFrame
{
	virtual void startGame(char * gameDll) PURE;
	virtual void settingGame() PURE;
	virtual void ShowChat(DWORD dwSender, DWORD dwReciever, COLORREF clr, char * pMsg) PURE;
	virtual void UserIO(DWORD dwUserID, int nState) PURE;
	virtual void setChatColor(COLORREF clr) PURE;
	virtual void showManageMsg(char * pMsg) PURE;
	virtual void ensureWindowVisible() PURE;
	virtual DWORD getGameID() PURE;
};

// {167B9E73-B360-48fe-A7C7-5CB9715F73FF}
DEFINE_GUID(IID_IGameCreator, 
0x167b9e73, 0xb360, 0x48fe, 0xa7, 0xc7, 0x5c, 0xb9, 0x71, 0x5f, 0x73, 0xff);
MIDL_INTERFACE("{167B9E73-B360-48fe-A7C7-5CB9715F73FF}")
IGameCreator {
	virtual void SetClientFrame(IClientFrame * pFrame) PURE;
	virtual DWORD GetGameID() PURE;
	virtual DWORD GetGameVersion() PURE;
	virtual HWND CreateGameWnd(HWND hwndParent) PURE;
	STDMETHOD(GetGameExtend)(int &cx, int &cy) PURE;
	STDMETHOD(QueryRender)(IGameRender ** ppRender) PURE;
	STDMETHOD(QueryOriginalGameView)(IGameView ** ppGameView) PURE;
	virtual void GetGameDescription(char * pGameName, char * pDesc) PURE;
	virtual void OnFrameConnected() PURE;
	virtual void Release() PURE;
};

// {1B061823-9E08-4113-8E87-4A1283B26732}
DEFINE_GUID(IID_IMultiMediaGame, 
0x1b061823, 0x9e08, 0x4113, 0x8e, 0x87, 0x4a, 0x12, 0x83, 0xb2, 0x67, 0x32);
MIDL_INTERFACE("{1B061823-9E08-4113-8E87-4A1283B26732}")
IMultiMediaGame {
	virtual BOOL OnIdle() PURE;
};

// {0EE6FBC7-8AAA-4a43-8DBE-CD2A0CE57B72}
DEFINE_GUID(IID_IMULTISERVICESUPPORT, 
0xee6fbc7, 0x8aaa, 0x4a43, 0x8d, 0xbe, 0xcd, 0x2a, 0xc, 0xe5, 0x7b, 0x72);
MIDL_INTERFACE("{0EE6FBC7-8AAA-4a43-8DBE-CD2A0CE57B72}")
IMultiServiceSupport {
	virtual BOOL QueryService(REFIID iidService, void ** ppServiceInterface) PURE;
};

typedef BOOL (* prototypeGetGameCreator)(IGameCreator ** ppCreator);
typedef BOOL (* prototypeGetServiceSupport)(IMultiServiceSupport ** ppSSupport);

/*
	gameView		the real game impl class
	gameVersion		the version be displayed in about dialog, use MAKEWORD(minorVersion, majorVersion)
	gameExtendX		the game wnd size x
	gameExtendY		the game wnd size y
*/
template <class gameView, DWORD gameID, DWORD gameVersion, int gameExtendX, int gameExtendY>
class CGameCreatorSupportSite : public IGameCreator  
{
public:
	virtual void SetClientFrame(IClientFrame * pFrame) {
		ASSERT(pFrame && !m_pFrame);

		m_pFrame =pFrame;

		g_pFrame =m_pFrame;
	}
	virtual DWORD GetGameID() {
		return gameID;
	}
	DWORD GetGameVersion() {
		return gameVersion;
	}
	virtual HWND CreateGameWnd(HWND hwndParent) {
		if (m_pGameView && ::IsWindow(m_pGameView->m_hWnd)) return m_pGameView->m_hWnd;
		
		ASSERT(m_pFrame);

		m_hInst =m_pFrame->GetResInst();

		AfxSetResourceHandle(m_hInst);
		
		m_pGameView =new gameView;

		m_pFrame->GetCommLink((void **)&(m_pGameView->m_pLink));

		DWORD dwStyle = AFX_WS_DEFAULT_VIEW;

		// Create with the right size (wrong position)
		CRect rect(0, 0, gameExtendX, gameExtendY);

		m_pGameView->OnFrameConnected(m_pFrame);
		CWnd * pWnd =(CWnd *)m_pGameView;
		pWnd->Create(NULL, NULL, dwStyle,
			rect, CWnd::FromHandle(hwndParent), 0, NULL);


		return m_pGameView->m_hWnd;
	}
	STDMETHOD(GetGameExtend)(int &cx, int &cy) {
		cx =gameExtendX;
		cy =gameExtendY;

		return S_OK;
	}
	STDMETHOD(QueryRender)(IGameRender ** ppRender) {
		ASSERT(ppRender);
		return m_pGameView->QueryRender(ppRender);
	}
	STDMETHOD(QueryOriginalGameView)(IGameView ** ppGameView) {
		ASSERT(ppGameView);
		*ppGameView =NULL;

		return E_NOTIMPL;
	}
	void GetGameDescription(char * pGameName, char * pDesc) {
		ASSERT(pGameName && pDesc);

		ASSERT(m_pGameView);
		m_pGameView->GetGameDescription(pGameName, pDesc);
	}
	void OnFrameConnected() {
		//	this function must called after SetClientFrame & CreateGameWnd
		ASSERT(m_pFrame);
		IHTMLChat * pChatCore;
		m_pFrame->GetChatCore(&pChatCore);

		//	the g_pChat can only init once
		g_TraceHelper.attach(pChatCore);
	}
	void Release() {
/*#ifdef _DEBUG
		if (m_pGameView) {
			delete m_pGameView;
			m_pGameView =NULL;
		}
#else
		try {
			if (m_pGameView) {
				delete m_pGameView;
				m_pGameView =NULL;
			}
		} catch(...) {
			m_pGameView =NULL;
		}
#endif*/
	}
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppUnknwn) {
#ifndef EXPOSE_ADDITIONAL_GAME_INTERFACE
		return E_NOTIMPL;
#else
		return m_pGameView->QueryInterface(riid, ppUnknwn);
#endif
	}
public:
	CGameCreatorSupportSite() {
		m_hInst =NULL;

		m_pGameView =NULL;

		m_pFrame =NULL;
	};
	virtual ~CGameCreatorSupportSite() {
		Release();
	};

	HINSTANCE	m_hInst;

	gameView  * m_pGameView;

	IClientFrame * m_pFrame;
};
template <class gameView, DWORD gameID, DWORD gameVersion, int gameExtendX, int gameExtendY>
class CGameCreatorSupportSiteAutoSize : public CGameCreatorSupportSite<gameView,gameID,gameVersion,gameExtendX,gameExtendY>  
{
public:
	virtual HWND CreateGameWnd(HWND hwndParent) {
/*		if (m_pGameView && ::IsWindow(m_pGameView->m_hWnd)) return m_pGameView->m_hWnd;
		
		ASSERT(m_pFrame);

		m_hInst =m_pFrame->GetResInst();

		AfxSetResourceHandle(m_hInst);
		
		m_pGameView =new gameView;

		m_pFrame->GetCommLink((void **)&(m_pGameView->m_pLink));

		DWORD dwStyle = AFX_WS_DEFAULT_VIEW;

		// Create with the right size (wrong position)
		CRect rect(0, 0, gameExtendX, gameExtendY);

		m_pGameView->OnFrameConnected(m_pFrame);
		CWnd * pWnd =(CWnd *)m_pGameView;
		pWnd->Create(NULL, NULL, dwStyle,
			rect, CWnd::FromHandle(hwndParent), 0, NULL);

		//	send initupdate to game
		pWnd->SendMessage(WM_INITIALUPDATE);
*/
		if(NULL == CGameCreatorSupportSite<gameView,gameID,gameVersion,gameExtendX,gameExtendY>::CreateGameWnd(hwndParent))
			return NULL;
		if(!m_pGameView) return NULL;
		if(::IsWindow(m_pGameView->m_hWnd))
		{
			int cx,cy;
			m_pGameView->GetWindowsSize(cx,cy);
			CWnd * pWnd =(CWnd *)m_pGameView;
			pWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,cx,cy,SWP_NOZORDER);
		}
		return m_pGameView->m_hWnd;
	}
	STDMETHOD(GetGameExtend)(int &cx, int &cy) {
		m_pGameView->GetWindowsSize(cx,cy);
		return S_OK;
	}
};

template <class gameView, DWORD gameID, DWORD gameVersion, int gameExtendX, int gameExtendY>
class CGameCreatorSupportGV : public IGameCreator  
{
public:
	virtual void SetClientFrame(IClientFrame * pFrame) {
		ASSERT(pFrame && !m_pFrame);

		m_pFrame =pFrame;

		g_pFrame =m_pFrame;
	}
	virtual DWORD GetGameID() {
		return gameID;
	}
	DWORD GetGameVersion() {
		return gameVersion;
	}
	virtual HWND CreateGameWnd(HWND hwndParent) {
		if (m_pGameView && ::IsWindow(m_pGameView->m_hWnd)) return m_pGameView->m_hWnd;

		ASSERT(m_pFrame);

		m_hInst =m_pFrame->GetResInst();

		AfxSetResourceHandle(m_hInst);
		
		m_pGameView =new gameView;

		m_pFrame->GetCommLink((void **)&(m_pGameView->m_pLink));

		DWORD dwStyle = AFX_WS_DEFAULT_VIEW;

		// Create with the right size (wrong position)
		CRect rect(0, 0, gameExtendX, gameExtendY);

		m_pGameView->OnFrameConnected(m_pFrame);
		CWnd * pWnd =(CWnd *)m_pGameView;
		pWnd->Create(NULL, NULL, dwStyle,
			rect, CWnd::FromHandle(hwndParent), 0, NULL);

		//	send initupdate to game
		pWnd->SendMessage(WM_INITIALUPDATE);

		return m_pGameView->m_hWnd;
	}
	STDMETHOD(GetGameExtend)(int &cx, int &cy) {
		cx =gameExtendX;
		cy =gameExtendY;

		return S_OK;
	}
	STDMETHOD(QueryRender)(IGameRender ** ppRender) {
		ASSERT(ppRender);
		*ppRender =NULL;
		return E_NOTIMPL;
	}
	STDMETHOD(QueryOriginalGameView)(IGameView ** ppGameView) {
		ASSERT(ppGameView);
		*ppGameView =static_cast<IGameView *>(m_pGameView);
		return S_OK;
	}
	void GetGameDescription(char * pGameName, char * pDesc) {
		ASSERT(pGameName && pDesc);

		ASSERT(m_pGameView);
		m_pGameView->GetGameDescription(pGameName, pDesc);
	}
	void OnFrameConnected() {
		//	this function must called after SetClientFrame & CreateGameWnd
		ASSERT(m_pFrame);
		IHTMLChat * pChatCore;
		m_pFrame->GetChatCore(&pChatCore);

		//	the g_pChat can only init once
		g_TraceHelper.attach(pChatCore);
	}
	void Release() {
/*#ifdef _DEBUG
		if (m_pGameView) {
			delete m_pGameView;
			m_pGameView =NULL;
		}
#else
		try {
			if (m_pGameView) {
				delete m_pGameView;
				m_pGameView =NULL;
			}
		} catch(...) {
			m_pGameView =NULL;
		}
#endif*/
	}
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppUnknwn) {
#ifndef EXPOSE_ADDITIONAL_GAME_INTERFACE
		return E_NOTIMPL;
#else
		return m_pGameView->QueryInterface(riid, ppUnknwn);
#endif
	}

public:
	CGameCreatorSupportGV() {
		m_hInst =NULL;

		m_pGameView =NULL;

		m_pFrame =NULL;
	};
	virtual ~CGameCreatorSupportGV() {
		Release();
	};

	HINSTANCE	m_hInst;

	gameView  * m_pGameView;

	IClientFrame * m_pFrame;
};

template <class gameView, DWORD gameID, DWORD gameVersion, int gameExtendX, int gameExtendY>
class CGameCreatorSupportAutoGV : public CGameCreatorSupportGV<gameView, gameID, gameVersion, gameExtendX,  gameExtendY>
{
public:
	virtual ~CGameCreatorSupportAutoGV(){};
public:
	virtual HWND CreateGameWnd(HWND hwndParent) {
		if (m_pGameView && ::IsWindow(m_pGameView->m_hWnd)) return m_pGameView->m_hWnd;

		ASSERT(m_pFrame);

		m_hInst =m_pFrame->GetResInst();

		AfxSetResourceHandle(m_hInst);
		
		m_pGameView =new gameView;

		m_pFrame->GetCommLink((void **)&(m_pGameView->m_pLink));

		DWORD dwStyle = AFX_WS_DEFAULT_VIEW;

		// Create with the right size (wrong position)
		int cx,cy;
		m_pGameView->GetWindowsSize(cx,cy);
		CRect rect(0, 0, cx, cy);

		m_pGameView->OnFrameConnected(m_pFrame);
		CWnd * pWnd =(CWnd *)m_pGameView;
		pWnd->Create(NULL, NULL, dwStyle,
			rect, CWnd::FromHandle(hwndParent), 0, NULL);

		//	send initupdate to game
		pWnd->SendMessage(WM_INITIALUPDATE);

		return m_pGameView->m_hWnd;
	};
	STDMETHOD(GetGameExtend)(int &cx, int &cy) {
		m_pGameView->GetWindowsSize(cx,cy);
		return S_OK;
	};
};
#define EXPOSE_GAMECREATOR_SUPPORTSITE(__gameView, __id, __version, __sizeX, __sizeY)  \
IClientFrame * g_pFrame =NULL;\
CHTMLChat g_TraceHelper;\
CHTMLChat * g_pChat =&g_TraceHelper;\
CGameCreatorSupportSite<__gameView, __id, __version, __sizeX, __sizeY> gameCreator; \
extern "C" BOOL __declspec(dllexport) GetGameCreator(IGameCreator ** ppCreator) \
{\
	ASSERT(ppCreator);\
	*ppCreator =static_cast<IGameCreator *>(&gameCreator);\
	return TRUE;\
}

#define EXPOSE_GAMECREATE_SUPPORTORGGV(__gameView, __id, __version, __sizeX, __sizeY)  \
IClientFrame * g_pFrame =NULL;\
CHTMLChat g_TraceHelper;\
CHTMLChat * g_pChat =&g_TraceHelper;\
CGameCreatorSupportGV<__gameView, __id, __version, __sizeX, __sizeY> gameCreator; \
extern "C" BOOL __declspec(dllexport) GetGameCreator(IGameCreator ** ppCreator) \
{\
	ASSERT(ppCreator);\
	*ppCreator =static_cast<IGameCreator *>(&gameCreator);\
	return TRUE;\
}

#define EXPOSE_GAMECREATE_SUPPORTORGAUTOGV(__gameView, __id, __version, __sizeX, __sizeY)  \
IClientFrame * g_pFrame =NULL;\
CHTMLChat g_TraceHelper;\
CHTMLChat * g_pChat =&g_TraceHelper;\
CGameCreatorSupportAutoGV<__gameView, __id, __version, __sizeX, __sizeY> gameCreator; \
extern "C" BOOL __declspec(dllexport) GetGameCreator(IGameCreator ** ppCreator) \
{\
	ASSERT(ppCreator);\
	*ppCreator =static_cast<IGameCreator *>(&gameCreator);\
	return TRUE;\
}

#define EXPOSE_GAMECREATOR_SUPPORTSITEAUTOSIZE(__gameView, __id, __version, __sizeX, __sizeY)  \
IClientFrame * g_pFrame =NULL;\
CHTMLChat g_TraceHelper;\
CHTMLChat * g_pChat =&g_TraceHelper;\
CGameCreatorSupportSiteAutoSize<__gameView, __id, __version, __sizeX, __sizeY> gameCreator; \
extern "C" BOOL __declspec(dllexport) GetGameCreator(IGameCreator ** ppCreator) \
{\
	ASSERT(ppCreator);\
	*ppCreator =static_cast<IGameCreator *>(&gameCreator);\
	return TRUE;\
}

/*
这是一个全局的方法，该方法在OnIdle循环中被调用。
返回值：TRUE	继续循环
		FALSE	停止循环
在这个函数中可以使用Sleep之类的降帧代码
*/
extern BOOL idleProcess();

#define EXPOSE_MULTIMEDIA_INTERFACE \
class MM_SERVICE : public IMultiMediaGame,  public IMultiServiceSupport\
{\
public:\
	virtual BOOL QueryService(REFIID iidService, void ** ppServiceInterface) {\
		ASSERT(ppServiceInterface);\
		if (iidService==IID_IMultiMediaGame) {\
			*ppServiceInterface =static_cast<IMultiMediaGame *>(this);\
			return TRUE;\
		}\
		*ppServiceInterface =NULL;\
		return FALSE;\
	}\
\
public:\
	virtual BOOL OnIdle() {\
		return ::idleProcess();\
	}\
} mm_servicer;\
\
extern "C" BOOL __declspec(dllexport) GetServiceSupport(IMultiServiceSupport ** ppSSupport) \
{\
	ASSERT(ppSSupport);\
	*ppSSupport =static_cast<IMultiServiceSupport *>(&mm_servicer);\
	return TRUE;\
}

#endif	// only in _CLIENT

#endif	//__GAMECREATE_H__

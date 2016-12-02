// ServerSite.h: interface for the CServerSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSITE_H__79FB37FD_1DC2_4786_A4B0_56B189C6B37B__INCLUDED_)
#define AFX_SERVERSITE_H__79FB37FD_1DC2_4786_A4B0_56B189C6B37B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>

#include "Statistic.h"
#include "site_i.h"

interface AFX_NOVTABLE ILogiWrapper
{
	virtual void free() PURE;
};

template <class T>
class LogiWrapper : public ILogiWrapper
{
public:
	LogiWrapper() {
		m_pInternelImplPtr =new T;
	}

	void setPtr(T * pImplPtr) {
		ASSERT(pImplPtr && !m_pInternelImplPtr);

		m_pInternelImplPtr =pImplPtr;
	};
	void free() {
		ASSERT(m_pInternelImplPtr);

		//	for release version, we will give destroy more reliability
#ifndef _DEBUG
		try {
			if (m_pInternelImplPtr) delete m_pInternelImplPtr;
		} catch(...) 
		{
			Trace("--destroy gameLogical failed!");
		};
#else
		if (m_pInternelImplPtr) delete m_pInternelImplPtr;
#endif

		delete this;
	}

public:
	T * m_pInternelImplPtr;
};

HRESULT Initialize(/* in */CServerBase * pServer, /* in */ITrace * pChatter, /* in */IGameLogical * pGameLogical, /* out */CTableServer ** ppTableServer);
HRESULT Initialize2(/* in */CServerBase * pServer, /* in */ITrace * pChatter, /* in */IGameLogical * pGameLogical, /*in */ILogiWrapper * pWrapper, int nTableID, /* out */CTableServer ** ppTableServer);

#define EXPOSE_STD_CREATETABLESERVER(_logiImpl, _gameid) \
extern "C" __declspec(dllexport) BOOL CreateTableServer(DWORD dwServerVersion,		\
	int nGameID,int nTableID,CServerBase *pServerBase,								\
	ITrace *pChatter, /*out*/CTableServer  **ppTableServer,							\
	/*out*/DWORD& dwTableServerVersion)												\
{																					\
	g_pChatter =pChatter;															\
																					\
	IGameLogical * pLogical = NULL;													\
	LogiWrapper<_logiImpl> *pWrapper =new LogiWrapper<_logiImpl>;					\
	pWrapper->m_pInternelImplPtr->QueryInterface(IID_IGameLogical, (void **)&pLogical);						\
																					\
	if (FAILED(Initialize2(pServerBase, pChatter, pLogical, static_cast<ILogiWrapper *>(pWrapper), nTableID, ppTableServer)))	{ \
		pWrapper->free();	\
		return FALSE;		\
	}\
\
	dwTableServerVersion =VERSION_ITABLESERVER_GAME_SERVER_QUERY;					\
\
	/*	the Trace has ready*/														\
	if(nGameID != _gameid)													\
	{																				\
		Trace("GameID = %d, %d wanted",nGameID,_gameid);						\
		return FALSE;																\
	}																				\
\
	if(nTableID == 0)																\
		Trace(#_logiImpl" init OK, if you see the line");							\
\
	pLogical->SetTableID(nTableID);	/* LeonLv add, 22Aug.2012*/					\
	return TRUE;																	\
};

#define EXPOSE_NPC_CREATETABLESERVER(_logiImpl, _gameid)		\
extern "C" __declspec(dllexport) BOOL CreateTableServer(DWORD dwServerVersion,		\
	int nGameID,int nTableID,CServerBase *pServerBase,								\
	ITrace *pChatter, /*out*/CTableServer  **ppTableServer,							\
	/*out*/DWORD& dwTableServerVersion)												\
{																					\
	g_pChatter =pChatter;															\
\
	/*	the Trace has ready*/														\
	if(nGameID != _gameid)															\
	{																				\
		Trace("GameID = %d, %d wanted",nGameID,_gameid);							\
		return FALSE;																\
	}																				\
\
	IGameLogical * pLogical = NULL;													\
	LogiWrapper<_logiImpl> *pWrapper =new LogiWrapper<_logiImpl>;					\
	pWrapper->m_pInternelImplPtr->QueryInterface(IID_IGameLogical, (void **)&pLogical);	\
\
	if (!CMultiMediaLoop::instance().createSite(nTableID, pServerBase, pLogical, static_cast<ILogiWrapper *>(pWrapper))) { \
		pWrapper->free();															\
		return FALSE;																\
	}																				\
\
	if (!ITableServerProxy::create(nTableID, ppTableServer)) {						\
		pWrapper->free();															\
		return FALSE;																\
	}																				\
\
	dwTableServerVersion =VERSION_ITABLESERVER_GAME_SERVER_QUERY;					\
	if(nTableID == 0)																\
		Trace(#_logiImpl" init OK, if you see the line");							\
\
	return TRUE;																	\
}

#ifdef _SERVERSITE_EXPORT						// only defined in ServerSiteLib project

#include "TimerList.h"	// Added by ClassView
#include "PlayerList.h"

//#include "ui.h"
#include "IGameStateEvent.h"


class CServerSite : 
	public IServerSite,
	public IGameStateEvent,
	public CTableServer,
	public ITimerSupport,
	public IMatchTableExtend,
	public ITimerNotify2
{
public:			//IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return (++m_dwRef);
	};
    virtual ULONG STDMETHODCALLTYPE Release( void) {
		return (--m_dwRef);
	};

private:
	ULONG m_dwRef;

public:				// CTableServer
	virtual BOOL OnGameMessage(DWORD dwUserID,int nChair,LPCSTR buf, int nLen); 
	virtual BOOL OnPostData(DWORD dwUserID,int nChair,DWORD dwDestUserID,LPCSTR buf, int nLen); 
	virtual BOOL OnGameUserEnter(TABLE_USER_INFO *pUser);
	virtual BOOL OnGameUserExit(DWORD dwUserID);
	virtual BOOL OnGameUserReady(DWORD dwUserID);
	virtual BOOL OnGameBegin();
	virtual BOOL OnGameUserOffLine(DWORD dwUserID);
	virtual BOOL OnGameUserReplay(DWORD dwUserID);
	virtual BOOL OnGameSetGameEnd();
	virtual BOOL OnTimePass();
	virtual BOOL OnTableEmpty();
	virtual BOOL OnDispatchInterface(DWORD dwID, DWORD dwParam) {
		if (dwID==IDD_MATCHEXTEND) {
			//	retrive the extend interface pointer
			ASSERT(dwParam);
			MATCHEXTENDPARAM * pParam =(MATCHEXTENDPARAM *)dwParam;
			//	the match extend interface can only be init once
			ASSERT(!m_pMatch);
			m_pMatch =pParam->pServerExtend;
			ASSERT(m_pMatch);

			if (FAILED(m_pGame->QueryInterface(&m_pMatchLogi))) {
				m_isMatch =false;
				pParam->pTableExtend =NULL;
				return FALSE;
			}

			m_isMatch =m_pMatch->IsMatchExtendEnabled();
			pParam->pTableExtend =static_cast<IMatchTableExtend *>(this);

			if (m_isMatch) InitMatch();

			return TRUE;
		}
/*		else if(dwID == IDD_COINEXTEND)
		{
			ASSERT(dwParam);
			m_pCoinServer = (ICoinServerExtend*)dwParam;
			return TRUE;
		}
*/
		// * /LeonLv add Start, 04May.2012
		else if(dwID == IDD_IGAMEQUERY)	// * /托管代打查询接口
		{
			if(m_pGame->QueryInterface(IID_IGameQuery, (void **)dwParam) == S_OK)
				return TRUE;
		}
		else if (dwID == IDD_IPRIZEFUND)	// * /彩池接口
		{
			ASSERT(dwParam);
			if(m_pGame){
				m_pGame->SetPrizeInterface((IPrizeFund *)dwParam);
			}
			return TRUE;
		}
		// * /LeonLv add End, 04May.2012
		else if (dwID == IDD_HAMSTERBALLSPEC)
		{
			ASSERT(dwParam);
			HAMSTERBALLSPEC * pSpec =(HAMSTERBALLSPEC *)dwParam;
			m_phamster_extend_server =pSpec->pHamServer;
		}
		return FALSE;
	};
	virtual void Delete();
	int  __cdecl OnGetClientScoreType() {
		return 1;			 // siguo_class
	}

public:		//IMatchTableExtend
	virtual BOOL OnCheckStart();

public:				// IServerSite
	STDMETHOD(OnGameStart)();									// 基本逻辑，游戏启动
	STDMETHOD(OnGameEnd)();										// 基本逻辑，游戏结束
	STDMETHOD(OnSceneChanged)(int nIndex=-1);					// 基本逻辑，游戏状态改变

	STDMETHOD(CreateTimer)(ITimerObject ** ppTimer);							// 创建定时器

	STDMETHOD(GetServerBase)(CServerBase ** ppServer);				// 直接获取CServerBase指针，注意，可能使ServerSite失去同步
	STDMETHOD(ForceKickOff)(int nIdx);								// 强行踢人，在游戏进行中是断线，游戏结束后是踢人
	STDMETHOD(GetOff)(DWORD dwUserID);								// 强行踢人，在游戏进行中是断线，游戏结束后是踢人
	STDMETHOD(WriteScore)(int nIdx, char * lpScoreBuf);						// 写分数，游戏可以在任何时候写分数

	STDMETHOD(GetPlayerList)(IUserList ** ppList);					// 获取用户列表
	STDMETHOD(GetLogicalServer)(IGameLogical ** ppLogical);				// 获得游戏逻辑服务器

	STDMETHOD(WriteLog)(char * szReason, char * szDescription);		// 写事件纪录
	STDMETHOD(Dismiss)(char * szReason, BOOL bDismissTable=FALSE);	// 解散

	STDMETHOD_(DWORD, GetGameOption)(DWORD * pOption=NULL);			// 获得游戏配置

	STDMETHOD(GetTableMaster)(IGamePlayer ** ppPlayer) {
		if (ppPlayer==NULL) return E_INVALIDARG;

		*ppPlayer =m_pMaster;
		if (m_pMaster) m_pMaster->AddRef();

		return S_OK;
	};

	//	比赛用扩展
	STDMETHOD(GetServerMatchExtend)(IMatchServerExtend ** ppExtend) {
		ASSERT(ppExtend);
		if (ppExtend) *ppExtend =m_pMatch;
		return S_OK;
	};
	STDMETHOD_(DWORD, GetMatchOption)(DWORD * pOption =NULL) {
		DWORD opt =0;
		if (m_pMatch) {
			opt =m_pMatch->GetMatchOption();
		}
		if (pOption) *pOption =opt;

		return opt;
	};
	STDMETHOD(GetLastSetScore)(long * pScores) {
		ASSERT(pScores && m_scorebuf);
		memcpy(pScores, m_scorebuf, m_number.nMax*20);
		return S_OK;
	};
	STDMETHOD(GetMatchContext)(IMatchServerExtend::MATCHCONTEXT ** ppContext) {
		ASSERT(ppContext);
		if (!ppContext) return E_INVALIDARG;

		m_pMatch->GetMatchContext((IMatchServerExtend::MATCHCONTEXT*)m_matchContext);
		*ppContext =(IMatchServerExtend::MATCHCONTEXT*)m_matchContext;

		return S_OK;
	}
	STDMETHOD_(BOOL, IsMatchEnabled)() {
		return (m_pMatch && m_pMatch->IsMatchExtendEnabled());
	}

	//	时钟
	STDMETHOD(SetTimer)(DWORD nID, int nInterval, int nEllapse=-1) {
		for (int i=0; i<sizeof(m_timerlist)/sizeof(ITimerObject *); i++) {
			if (m_timerlist[i]==NULL) {
				//	find the slot
				if (FAILED(CreateTimer(&(m_timerlist[i])))) return E_FAIL;
				ASSERT(m_timerlist[i]);
				if (!m_timerlist[i]) return E_FAIL;
				((TimerObject *)m_timerlist[i])->SetID(nID);
				((TimerObject *)m_timerlist[i])->attach(static_cast<ITimerNotify2 *>(this));
				m_timerlist[i]->Start(nEllapse, nInterval);

				return S_OK;
			}
		}

		return E_OUTOFMEMORY;
	}
	STDMETHOD(KillTimer)(DWORD nID) {
		for (int i=0; i<sizeof(m_timerlist)/sizeof(ITimerObject *); i++) {
			if (m_timerlist[i]!=NULL && ((TimerObject *)m_timerlist[i])->GetID()==nID) {
				//	find the slot
				try {
					m_timerlist[i]->Stop();
					m_timerlist[i]->Release();
					m_timerlist[i] =NULL;

					return S_OK;
				}
				catch(...) {
					Trace_L3("delete timer failed");
					return E_FAIL;
				}
			}
		}

		return E_INVALIDARG;
	}

public:					// IGameStateEvent
	STDMETHOD(OnLookOnEnableChanged)(IGamePlayer * pPlayer, BOOL bEnable);
	STDMETHOD(OnStateChanged)(IGamePlayer * pPlayer, int nState);

public:		//ITimerSupport
	STDMETHOD(startTimer)(int interval) {
		m_pServer->StartTimer(interval);

		return S_OK;
	};
	STDMETHOD(stopTimer)() {
		m_pServer->StopTimer();

		return S_OK;
	};

public:		// ITImerNotify2
	STDMETHOD(OnTimePass)(int nID, int nRemains, int nInterval) {
		if (m_pLogicalTimer) m_pLogicalTimer->OnTimer(nID, nRemains, nInterval);
		return S_OK;
	}
	STDMETHOD(OnTimeOut)(int nID) {
		if (m_pLogicalTimer) m_pLogicalTimer->OnTimerOut(nID);
		return S_OK;
	}

private:
	void RemoveCache();
	void UpdatePlayerScore(IGamePlayer * pPlayer, long * score);
	HRESULT ConfirmStart(DWORD dwUserID);
	HRESULT DumpUserInfo(char * lpszInfo);
	HRESULT InitPeopleLookOn(IGamePlayer * pPeople);
	void SendStorage2Client();
	void SyncPersonalData(int nIndex, UINT uflag, BOOL bfirst=FALSE);
	void PrepareStart();
	void SyncData(UINT uflag, BOOL bfirst=FALSE);

#pragma warning(disable :4200)
	struct OriginMsg {
		DWORD dwUserID;
		int nChair;
		int nLen;
		char buf[];
	};

	std::list<OriginMsg*> m_cache;
	typedef std::list<OriginMsg *>::iterator OML_ITOR;

#pragma warning(default:4200)

	BOOL m_bEnNeedTrans;						//是否发送EnableLookon
	
	TimerList m_TimerList;						// 时钟列表

	GAME_CMD_DATA m_CmdBuf;
	IGameLogical::SCENE sc, scObv, scManager;		//缓冲

	CServerBase * m_pServer;				//服务器
	UINT ServerCaps;							//服务器能力

	IGameLogical * m_pGame;					//游戏逻辑接口
	IGameLogical2 * m_pGameEx;				//扩展的游戏逻辑
	IPlayerIOEvent * m_pIOEvent;			//用户事件接口
	IGameDurative * m_pDurative;			//游戏可继续
	PlayerList m_playerlist;				//用户列表
	IGamePlayer * m_pMaster;				//第一个进来的用户（桌主）
	IGameConstEx * m_pConstEx;				//扩展的常量接口

	DWORD		m_dwOption;

	IGameConst::PLAYERNUMBER m_number;		//游戏人数
	IGameConstEx::ESTIMATED_TIME m_EstimatedTime;	//估计时间
//	DWORD m_EstimatedTime;					
	BOOL  m_CanQuickStart;					//快速启动支持
	BOOL  m_NeedSaveFile;					//存盘支持
	BOOL  m_bDirectStart;					//直接启动
	DWORD m_nStartTime;						//计时

	char * m_scorebuf;						//分数缓冲

	struct  STORAGE {
		int	  cbSize;					// storage大小
		char * lpData;					// storage数据
		//CArray<DWORD, DWORD> wanted;	// 需要数据的玩家列表
		std::vector<DWORD> wanted;	// 需要数据的玩家列表
		typedef std::vector<DWORD>::iterator DWORDVECTOR_ITOR;
	} m_save;								//存盘数据

	int m_ServerState;						//主服务器状态
	int m_LogicalState;						//游戏逻辑状态

public:
	HRESULT SetServerBase(CServerBase * pServer);
	HRESULT Connect(IGameLogical * pGameLogical);

	HRESULT setLogiWrapper(ILogiWrapper * pLogiWrapper) {
		ASSERT(pLogiWrapper && !m_pLogiWrapper);

		m_pLogiWrapper =pLogiWrapper;

		return S_OK;
	};

	CServerSite();
	virtual ~CServerSite();

private:		// 游戏头，用于释放游戏逻辑
	ILogiWrapper * m_pLogiWrapper;

private:		//比赛用
	HRESULT InitMatch();
	void MatchProcess();
	IMatchServerExtend * m_pMatch;			//服务器的比赛扩展
	IMatchLogical	* m_pMatchLogi;
	BOOL	m_isMatch;
	IMatchServerExtend::MATCHCONTEXT * m_matchContext;	//比赛的中间数据区
	char	m_matchContextBuf[4096];					//		数据空间，不要直接访问这个变量。用m_matchContext!
	void	* m_matchScore;
	BOOL	m_isStopPeriod;
private:	//金币服务器使用
	void WriteCoins();
	BOOL m_bNeedWriteCoins;
	ICoinServerExtend * m_pCoinServer;

	void StopGame(UINT nFlags);

private:		// 带Timer的GameLogical
	IGameLogicalWithTimer * m_pLogicalTimer;

	ITimerObject * m_timerlist[20];

public:		// 带NPC方法的ServerSite用
	BOOL	IsTableEmpty() {
		return m_bTableEmpty;
	}

#define IP_NEVERCALL	0x80001000
#define IP_TRUE			0x00000001
#define IP_FALSE		0x00000000

	BOOL	idleProcess() {
		if (m_pNPCLogical) 
			return m_pNPCLogical->idleProcess();

		return IP_NEVERCALL;
	}

private:		// 带NPC方法的ServerSite用
	BOOL		m_bTableEmpty;

	IGameLogicalWithNPC * m_pNPCLogical;

private:		// hamster server使用
	IHamsterBallServer * m_phamster_extend_server;
};

#else	// _SERVERSITE_EXPORT

#ifdef _DEBUG
#pragma comment(lib, "ServerSiteD.lib")
#else
#pragma comment(lib, "ServerSite.lib")
#endif	//_DEBUG

#endif	// _SERVERSITE_EXPORT

#endif // !defined(AFX_SERVERSITE_H__79FB37FD_1DC2_4786_A4B0_56B189C6B37B__INCLUDED_)

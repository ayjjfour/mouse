/*	ServerSite.cpp */

#include "stdafx.h"
#include "ServerSite.h"
#include "SiteStruct.h"
#include "SiteMsg.h"

//	player state
#define SERVER_WAITING_START	1
#define SERVER_RECIEVED_START	2

//	server state
#define MAINSERVER_GAME_RUNNING		1
#define MAINSERVER_GAME_STOPPED		0

#define LOGICAL_GAME_WAITING		1
#define LOGICAL_GAME_STARTED		2
#define LOGICAL_GAME_STOPPED		3

const char StateDesc [3][50] =  {"LOGICAL_GAME_WAITING", "LOGICAL_GAME_STARTED", "LOGICAL_GAME_STOPPED"};

HRESULT Initialize(/* in */CServerBase * pServer, /* in */ITrace * pChatter, /* in */IGameLogical * pGameLogical, /* out */CTableServer ** ppTableServer)
{
	g_pChatter = pChatter;

	CServerSite * pSite = new CServerSite;

	if (FAILED(pSite->SetServerBase(pServer))) goto _exit;

	if (FAILED(pSite->Connect(pGameLogical))) goto _exit;

	return pSite->QueryInterface(IID_CTableServer, (void **)ppTableServer);

_exit:
	if (pSite) delete pSite;

	if (pGameLogical) pGameLogical->Release();

	return E_FAIL;
}

HRESULT Initialize2(/* in */CServerBase * pServer, /* in */ITrace * pChatter, /* in */IGameLogical * pGameLogical, /*in */ILogiWrapper * pWrapper, int nTableID, /* out */CTableServer ** ppTableServer)
{
	g_pChatter = pChatter;
	pGameLogical->SetTableID(nTableID);

	CServerSite * pSite = new CServerSite;

	if (FAILED(pSite->SetServerBase(pServer))) goto _exit;

	if (FAILED(pSite->Connect(pGameLogical))) goto _exit;

	pSite->setLogiWrapper(pWrapper);

	pSite->QueryInterface(IID_CTableServer, (void **)ppTableServer);

	return (HRESULT)pSite;

_exit:
	if (pSite) delete pSite;

	if (pGameLogical) pGameLogical->Release();

	return E_FAIL;
}

//Interfaces
///////////////////////////////////////////////////////////////

////////////////////IGenetic///////////////////////////////////

HRESULT STDMETHODCALLTYPE CServerSite::QueryInterface( 
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	ASSERT(ppvObject);

	*ppvObject =NULL;

	if (riid==IID_IServerSite || riid==IID_IUnknown) *ppvObject =static_cast<IServerSite *>(this);
	else if (riid==IID_CTableServer) *ppvObject =static_cast<CTableServer *>(this);
	else return E_NOINTERFACE;

	AddRef();

	return S_OK;
}

//////////////////// CTableServer/////////////////////////////

BOOL CServerSite::OnGameMessage(DWORD dwUserID,int nChair,LPCSTR buf, int nLen)
{
	if(nLen < 1)
	{
		Trace_L3("--CServerSite::OnGameMessage,  nLen = %d",nLen);
		return FALSE;
	}
	if(!m_playerlist.IsValidChair(nChair))
	{
		Trace_L3("--CServerSite::OnGameMessage,  nChair = %d",nChair);
		return FALSE;
	}

	STANDARDMSG * msg=(STANDARDMSG *)buf;
	switch (msg->msgID) {
	//��Щ��Ϣֻ��Site�йأ������͸�Logical
	case CLIENTSITE_CONFIRM_START:
		if (SUCCEEDED(ConfirmStart(dwUserID))) return TRUE;
		else return FALSE;
	case CLIENTSITE_REQUEST_VERIFY_VERSION:
		//	testing only
/*		this->CreateTimer(&m_pTimer);
		m_pTimer->attach(static_cast<ITimerNotify *>(this));
		m_pTimer->Start(second(10), second(10));*/
		{
		VERIFY_VERSION * pVer = (VERIFY_VERSION *)buf;

		IGamePlayer * player;
		if (FAILED(m_playerlist.FindPeople(dwUserID, &player))) {
			Trace_L3("----CServerSite::OnGameMessage, CLIENTSITE_REQUEST_VERIFY_VERSION, can't perform verify because specified user %d is not found", dwUserID);
			return FALSE;
		}
	
		((GamePlayer *)player)->m_ver.curVersion=pVer->curVersion;			//����ͻ��˰汾��
		((GamePlayer *)player)->m_ver.minVersion=pVer->minVersion;
		((GamePlayer *)player)->m_ver.maxVersion=pVer->maxVersion;

		IGameConst::VERSION_STRUCT vs;
		m_pGame->GetVersion(&vs);

		if (vs.curVersion==pVer->curVersion) break;
		
		char name[255];
		player->get_UserName(name);
		player->Release();
		Trace_L2("----warning, %s�Ŀͻ��˰汾��ͬ, server ver=%d, client ver=%d", name, vs.curVersion, pVer->curVersion);
		if (vs.minVersion<=pVer->curVersion && vs.maxVersion>=pVer->curVersion) break;
		Trace_L2("--error, %s�Ŀͻ��˰汾�쳣, allowed min ver=%d, max ver=%d", name, vs.minVersion, vs.maxVersion);
		char buf[500] ="�ͻ���������������������֮ǰ�޷�ʹ�á���������ĵ�ַ���м���http://202.96.162.68/MyGames/Download/Setup-TestGame.exe�����س���";
/*		GAME_CMD_DATA cmd;
		cmd.InitData(-1, SERVERSITE_REQUEST_NEWVERSION);
		cmd.AddData(buf, sizeof(buf)); 
		m_pServer->SendGameCmdTo(dwUserID, cmd.GetBuf(), cmd.GetDataLen());*/
		m_pServer->ForceUserOffline(dwUserID);
		}

		break;
	case CLIENTSITE_ENABLE_LOOKON:
		{
			IGamePlayer * pPlayer;
			if (FAILED(m_playerlist.FindPlayer(dwUserID, &pPlayer))) {
//				Oh--, it is not an error, a lookon people can send this msg also
				Trace_L1("--warning: CServerSite::OnGameMessage, CLIENTSITE_ENABLE_LOOKON, specified one (id=%d) is not a player", dwUserID);
				return FALSE;
			}

			ENABLE_LOOK_ON * enMsg=(ENABLE_LOOK_ON *)buf;

			BOOL en=enMsg->bEnable;

			m_bEnNeedTrans = FALSE;

			//	����Ƿ��������˵�
			if (enMsg->nNum<0) pPlayer->put_EnableLookOn(en);
			// 	����Ƿ���ĳ���˵���OnPostData�д���
			
			m_bEnNeedTrans = TRUE;
			
			pPlayer->Release();

			return TRUE;
		}
		break;
	case CLIENTSITE_REQUEST_STORAGE:
		m_save.wanted.push_back(dwUserID);
		SendStorage2Client();
		break;
	case TIMERPROBE_TIMERERR:
		GetOff(dwUserID);
		break;
	default:
		if ((unsigned char )buf[0]<128) {
			if (m_LogicalState==LOGICAL_GAME_WAITING) {
				//�����û�����
				OriginMsg * pMsg = (OriginMsg *)malloc(sizeof(OriginMsg) + nLen);
				pMsg->dwUserID= dwUserID;
				pMsg->nChair= nChair;
				pMsg->nLen = nLen;
				memcpy(pMsg->buf, buf, nLen);
				m_cache.push_back(pMsg);
			}
			else {
				if (FAILED(m_playerlist.FindPeople(dwUserID))) {
					Trace_L1("--CServerSite::OnGameMessage, specified user %d not found", dwUserID);
					GetOff(dwUserID);
					return FALSE;
				}
				m_pGame->OnGameOperation(nChair, dwUserID, buf[0], (void*)&buf[1], nLen-1);
			}
		}
	}

	return TRUE;
}

BOOL CServerSite::OnPostData(DWORD dwUserID,int nChair,DWORD dwDestUserID,LPCSTR buf, int nLen)
{
	// ����xx�Թ۵Ĵ����ڴ�
	//��ȡ�Թ����б�
	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.GetUser(nChair, &pPlayer))) {
		Trace_L2("--CServerSite::OnPostData, can't find specified player at %d", nChair);
		return FALSE;
	}

	STANDARDMSG * msg=(STANDARDMSG *)buf;

	if (msg->msgID==CLIENTSITE_ENABLE_LOOKON) {
		ENABLE_LOOK_ON * enMsg=(ENABLE_LOOK_ON *)buf;

		IUserList *ul;
		pPlayer->GetLookOnPeopleList(&ul);

		IGamePlayer * pLookOn = NULL;
		if (SUCCEEDED(ul->FindUser(dwDestUserID, &pLookOn))) {
			if (pLookOn->get_chair()==nChair) {
				m_bEnNeedTrans = FALSE;
				pLookOn->put_EnableLookOn(enMsg->bEnable);
				m_bEnNeedTrans = TRUE;
			}
			else {
				Trace_L3("--CServerSite::OnPostData, Ŀ��ID�뷢��ID����ͬһ��������");
			}
			pLookOn->Release();
		}
		else {
			Trace_L2("--CServerSite::OnPostData, CLIENTSITE_ENABLE_LOOKON, can't find the specified user, id=%d", dwDestUserID);
		}

		ul->Release();
	}

	pPlayer->Release();

	return TRUE;
}

BOOL CServerSite::OnGameUserEnter(TABLE_USER_INFO *pUser)
{
	IGamePlayer *pGamePlayer;
	if (FAILED(m_playerlist.AddPeople(*pUser, &pGamePlayer))) {
		Trace_L3("--CServerSite::OnGameUserEnter, can't add player");
		return FALSE;
	}

	if (m_phamster_extend_server) {
		((GamePlayer *)pGamePlayer)->set_hamster_extend_buf(m_phamster_extend_server->getExtendBuf(pUser->dwUserID));
	}

	m_bTableEmpty =FALSE;
	
	//	notify client if this is a match server
	if (m_isMatch) {
		m_CmdBuf.InitData(-1, SERVERSITE_MATCH_PROGRESSING);
		m_pServer->SendGameCmdTo(pGamePlayer->get_ID(), m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
	}

	GamePlayer * pp=(GamePlayer *)pGamePlayer;
	pp->attach(static_cast<IGameStateEvent *>(this));

	if (m_LogicalState!=LOGICAL_GAME_STOPPED && pUser->IsLookOnPeople()) {
		InitPeopleLookOn(pGamePlayer);
	}

	if (!m_pMaster && pp->IsPlayer()) {
		m_pMaster =pp;
		m_pMaster->AddRef();
	}

	//�ǽ����Ϸ,����ҹ�����һ�ֵ�
	if(pGamePlayer->IsPlayer() && m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled())
	{
		long lDuijuCoins,lHandCoins;
	
		if(m_pCoinServer->GetDuijuCoins(lDuijuCoins) 
			&& m_pCoinServer->GetCoins(pUser->lDBID,lHandCoins))
		{
			//	notify the GameLogical
			if(lHandCoins < lDuijuCoins)	//���̫��
			{
				char msg[1024];
				wsprintf(msg,"ϵͳ��Ϣ: ����Ϸ������Ҫ%dö��ң���Ľ��(%d)����һ�ֵ�",lDuijuCoins,lHandCoins);
				m_pCoinServer->SystemMessage(pUser->dwUserID,msg);
				GetOff(pUser->dwUserID);				
				pGamePlayer->Release();
				return FALSE;
			}
			else
			{
				char msg[1024];
				wsprintf(msg,"ϵͳ��Ϣ: ��ӭ��������Ϸ<BR>����Ϸÿ��%dö��ң���Ŀǰ�Ľ����Ϊ%d",lDuijuCoins,lHandCoins);
				m_pCoinServer->SystemMessage(pUser->dwUserID,msg);
				if (m_pIOEvent) m_pIOEvent->OnUserEnter(pUser->nChair, pGamePlayer);
				pGamePlayer->Release();
				return TRUE;				
			}
		}
		//ASSERT(0);
		pGamePlayer->Release();
		return FALSE;
	}
	//	notify the GameLogical
	if (m_pIOEvent) m_pIOEvent->OnUserEnter(pUser->nChair, pGamePlayer);

	pGamePlayer->Release();
	return TRUE;
}

BOOL CServerSite::OnGameUserExit(DWORD dwUserID)
{
	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.FindPeople(dwUserID, &pPlayer))) {
		Trace_L3("--CServerSite::OnGameUserExit, can't remove player");
		return FALSE;
	}

	m_playerlist.PrepareRemovePeople(dwUserID);

	//	ֻ�ڷ���������ʱ���
	if (m_ServerState == MAINSERVER_GAME_RUNNING) {
		//	��������
		if (pPlayer->IsPlayer()) {
			// if can, try to create a droid here, but not implement yet

			BOOL bDefaultCheck=TRUE;
			BOOL bNeedStop=FALSE;

			int nCheckNumber;
			if (m_LogicalState==LOGICAL_GAME_STARTED) {
				//	��Ϸ����ʱ����ȴ��û���Ӧ����ʱ
				nCheckNumber=m_number.nMaintenance;

				if (m_pDurative) {
					// ������Ϸ�Լ����
					bDefaultCheck=FALSE;
					HRESULT hr=m_pDurative->CheckDurative();
					if (E_NOTIMPL==hr) bDefaultCheck=TRUE;
					else if (FAILED(hr)) bNeedStop=TRUE;
				}
			}
			else {
				nCheckNumber=m_number.nStart;
				if (m_pDurative) {
					// ������Ϸ�Լ����
					bDefaultCheck=FALSE;
					HRESULT hr=m_pDurative->CheckStart();
					if (E_NOTIMPL==hr) bDefaultCheck=TRUE;
					else if (FAILED(hr)) bNeedStop=TRUE;
				}
			}

			// ֻ����Ϸ����ʱ�������
			if (bDefaultCheck) {
				int num=m_playerlist.GetValidUserNumber();
				if (num<nCheckNumber) bNeedStop=TRUE;
			}

			if (bNeedStop) {
				if (m_isMatch) {
					ASSERT(m_pMatchLogi);
					if (E_NOTIMPL==m_pMatchLogi->meMatchStop(STOP_NOENOUGHPLAYER)) 
						StopGame(STOP_NOENOUGHPLAYER);
						//m_pGame->Stop(STOP_NOENOUGHPLAYER);
				}
				else 
					StopGame(STOP_NOENOUGHPLAYER);
					//m_pGame->Stop(STOP_NOENOUGHPLAYER);

				RemoveCache();
				//if (m_LogicalState!=LOGICAL_GAME_STOPPED) 
					MatchProcess();
				m_LogicalState = LOGICAL_GAME_STOPPED;
				if (m_ServerState!=MAINSERVER_GAME_STOPPED) {
					m_ServerState= MAINSERVER_GAME_STOPPED;
					m_pServer->SetGameEnd();
					m_playerlist.ChangePlayerState(sSit);
				}
			}
		}
	}

	//	֪ͨGameLogical
	if (m_pIOEvent) m_pIOEvent->OnUserExit(pPlayer->get_chair(), pPlayer);

	m_playerlist.RemovePeople(dwUserID);

	//	�����������֪ͨ�����߿յ���Ϣ����ô�Լ�������
	if (!(ServerCaps&CAPS_CANNOTIFYEMPTY) && m_playerlist.GetUserNumber()==0) {
		OnTableEmpty();
	}

	pPlayer->Release();
	return TRUE;
}

BOOL CServerSite::OnGameUserReady(DWORD dwUserID)
{
	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.FindPeople(dwUserID, &pPlayer))) {
		Trace_L3("--CServerSite::OnGameUserReady, can't remove player");
		return FALSE;
	}

	// * /LeonLv add for match, 05Mar.2013
	if (m_pIOEvent) m_pIOEvent->OnUserReady(pPlayer->get_chair(), pPlayer);
	// * /add for match End

	pPlayer->put_State(sReady);

	pPlayer->Release();
	
	return TRUE;
}

BOOL CServerSite::OnGameBegin()
{
	m_ServerState=MAINSERVER_GAME_RUNNING;

	ZeroMemory(m_scorebuf, 20*m_number.nMax);
	m_bNeedWriteCoins = TRUE;
	//	check client version first to avoid old program entering
	BOOL bVersionCorrect = TRUE;

	GamePlayer * player;
	for (int i=0; i<m_playerlist.GetUserNumber(); i++) {
		if (FAILED(m_playerlist.GetUser(i, (IGamePlayer **)&player))) {
			Trace_L3("--CServerSite::OnGameBegin, get player %d failed", i);
			continue;
		}
		
		//	if the player's version is not correct
		if (player->IsValidUser() && player->m_ver.curVersion==-1) {
			Trace_L2("--%s used an old version program, kick this player off", player->szUserName);
			GetOff(player->get_ID());
			bVersionCorrect = FALSE;
		}

		player->Release();
	}

	if (!bVersionCorrect) {
		// client version is different, dismiss the game to avoid any more problem
		m_pServer->DismissGame();
		return FALSE;
	}

	//	֪ͨ������������Ϸ��ʼ��ֹͣ��ʱ���
	if (m_isMatch) {
		ASSERT(m_pMatch);
		m_pMatch->OnSetStart();
	}

	//��ʱ�����Ǵ���Ϸ�״ν�������֮�������ֹ֮ͣ���ʱ��
	if (m_LogicalState==LOGICAL_GAME_STOPPED) m_nStartTime = ::GetTickCount();

	if (FAILED(m_pGame->ConstructScene())) {
		Trace_L3("--CServerSite::OnGameBegin, call CGameLogical::ConstructScene() failed");
		m_pServer->DismissGame();
		return FALSE;
	}

	SyncData(SCENE_WHOLE, TRUE);

	//	we should waiting client ack here
	PrepareStart();

	m_playerlist.ChangePlayerState(sPlaying);

	return TRUE;
}

BOOL CServerSite::OnGameUserOffLine(DWORD dwUserID)
{
	IGamePlayer * pGamePlayer;
	if (FAILED(m_playerlist.FindPlayer(dwUserID, &pGamePlayer))) {
		Trace_L2("--CServerSite::OnGameUserOffLine, can't find player");
		return FALSE;
	}

	if (pGamePlayer->IsPlayer()) {
		if (m_LogicalState==LOGICAL_GAME_STOPPED) {
			//	��׼������������Ϸ
			BOOL bNeedStop=TRUE;
			if (m_pDurative && SUCCEEDED(m_pDurative->CheckReplay())) {
				//	��ϷҪ��ȴ�
				bNeedStop=FALSE;
			}

			if (bNeedStop) {
				if (m_isMatch) {
					ASSERT(m_pMatchLogi);
					if (E_NOTIMPL==m_pMatchLogi->meMatchStop(STOP_NOENOUGHPLAYER)) 
						StopGame(STOP_NOENOUGHPLAYER);
						//m_pGame->Stop(STOP_NOENOUGHPLAYER);
				}
				else
					StopGame(STOP_NOENOUGHPLAYER);
					//m_pGame->Stop(STOP_NOENOUGHPLAYER);
				RemoveCache();
				if (m_LogicalState!=LOGICAL_GAME_STOPPED) MatchProcess();
				if (m_ServerState!=MAINSERVER_GAME_STOPPED) {
					m_ServerState=MAINSERVER_GAME_STOPPED;
					m_pServer->SetGameEnd();
				}
			}
		}
		else if (m_LogicalState==LOGICAL_GAME_WAITING) {
			ConfirmStart(dwUserID);
		}
	}

	if (m_pIOEvent) {
		m_pIOEvent->OnUserOffline(pGamePlayer->get_chair(), pGamePlayer);
		//	logical may stop the game
		if (m_ServerState==MAINSERVER_GAME_STOPPED) return TRUE;
	}

	pGamePlayer->put_State(sOffLine);

	pGamePlayer->Release();
	return TRUE;
}

BOOL CServerSite::OnGameUserReplay(DWORD dwUserID)
{
	/* ���´���Ӧ����ͮ��������⣬ԭʼ�����������£�
		��Э�̹����У�������һ����̬����m_cache�������ʱ�䷢��������Ϣ������Э�̹������յ��ͻ��˷���������Ϣ��ȫ���������������Ҹ�����Ϣ�ĵ���˳����л��档
	һ����Ϸ��ʼ�������еĻ�����Ϣ���������һ�Σ���������֤���߼���ִ�С�
	���������ڿͻ��ˣ����������ж���������������û�δ����CLIENT_CONFIRM_STARTʱ������õ���������Ӧ��
	������û����߻��û��˳���Ȼ�����µ�½ʱ���������棬���ܵ������⣺���磺�û��տ�ʼ�����һ���ƣ�Ȼ�������ߣ��ؽ�ʱ���ָ��ֳ��������ʱ��������û������CLIENT_CONFIRM_START��Ϣ���ڻ�����Ϣʱ���ֳ���û�����û�����ƶ������ı䣬��ʱ���ͻ��˻������ٴ��һ���ƣ�����������ٻ��棬�ͻᵼ���߼�����
	���ָ��ʳ��ּ��ʺ�С�������ܳ��֡�
	*/
	if (m_LogicalState==LOGICAL_GAME_WAITING) {
		//	�ȴ���ʼ״̬�����¼���֪ͨlogical
		OriginMsg * pMsg = (OriginMsg *)malloc(sizeof(OriginMsg));
		pMsg->dwUserID= dwUserID;
		pMsg->nChair= -1;
		pMsg->nLen = -1;
		m_cache.push_back(pMsg);

		return TRUE;
	}
	/*
	��ConfirmStart�У�׷���˲��ִ����Դ������׷�ӵ�msg.��msg��ʶ���־��nLen=-1
	*/

	IGamePlayer * pGamePlayer;
	if (FAILED(m_playerlist.FindPlayer(dwUserID, &pGamePlayer))) {
		Trace_L2("--CServerSite::OnGameUserReplay, can't find player");
		return FALSE;
	}

	//	notify client if this is a match server
	if (m_isMatch) {
		m_CmdBuf.InitData(-1, SERVERSITE_MATCH_PROGRESSING);
		m_pServer->SendGameCmdTo(pGamePlayer->get_ID(), m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
	}

	if (m_pIOEvent) {
		m_pIOEvent->OnUserReplay(pGamePlayer->get_chair(), pGamePlayer);
		//	logical may stop the game
		if (m_ServerState==MAINSERVER_GAME_STOPPED) return TRUE;
	}

	pGamePlayer->put_State(sPlaying);

	//	����Ϸ����ʱ���ָ�
	//	get the current scene
	int nIndex;
	pGamePlayer->get_chair(&nIndex);
	if (FAILED(m_pGame->GetPersonalScene(nIndex, SCENE_WHOLE|SCENE_MYSELF, &sc))) {
		Trace_L3("----CServerSite::SyncPersonalData, can't get scene from %d, game dismissed", nIndex);
		m_pServer->DismissGame();
		return FALSE;
	}

	// transfer it to the player
	m_CmdBuf.InitData(-1, SERVERSITE_PERSONAL_SCENE);
	m_CmdBuf.AddData(&sc, sc.cbSize);
	m_pServer->SendGameCmdTo(pGamePlayer->get_ID(), m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());	

	pGamePlayer->Release();
	return TRUE;
}

BOOL CServerSite::OnGameSetGameEnd()
{
	if (m_ServerState == MAINSERVER_GAME_RUNNING) {
		m_ServerState = MAINSERVER_GAME_STOPPED;

		if (m_isMatch) {
			ASSERT(m_pMatchLogi);
			if (E_NOTIMPL==m_pMatchLogi->meMatchStop(STOP_SERVERERROR)) 
				StopGame(STOP_SERVERERROR);
				//m_pGame->Stop(STOP_SERVERERROR);
		}
		else 
			StopGame(STOP_SERVERERROR);
			//m_pGame->Stop(STOP_SERVERERROR);
		RemoveCache();
		if (m_LogicalState!=LOGICAL_GAME_STOPPED) MatchProcess();
	}

	m_LogicalState = LOGICAL_GAME_STOPPED;

	return TRUE;
}

BOOL CServerSite::OnTimePass()
{
	// * /m_TimerList.OnTimer();	// * /LeonLv: why remark it, none know, reference MaoYe Code, 21July.2010
	// * /LeonLv Add Start as reference
	INewTimer *pTimer = NULL;
	m_pGame->QueryInterface(IID_INewTimer, (void **)&pTimer);
	if(pTimer)
	{
		pTimer->OnTimePass();
	}
	// * /Add End

	return TRUE;
}

BOOL CServerSite::OnTableEmpty()
{
	m_bTableEmpty =TRUE;

	m_pGame->Clear();
	m_playerlist.Clear();
	m_playerlist.Init(m_number.nMax);
	m_TimerList.Clear();

	m_ServerState=MAINSERVER_GAME_STOPPED;
	m_LogicalState=LOGICAL_GAME_STOPPED;

	m_bEnNeedTrans = TRUE;

	if (m_pMaster) {
		m_pMaster->Release();
		m_pMaster =NULL;
	}

	RemoveCache();

	return TRUE;
}

void CServerSite::Delete()
{
	if (m_pLogiWrapper) m_pLogiWrapper->free();

	delete this;
}

BOOL CServerSite::OnCheckStart()
{
	ASSERT(m_pMatch && m_pMatchLogi);

	if (m_LogicalState!=LOGICAL_GAME_STARTED)	//����Ѿ�������һ�֣������ټ���� modified by leizw at 3003/06/06
	{
		m_pMatchLogi->meMatchStop(STOP_SOMEONENOTSTART);
		MatchProcess();
	}

	return TRUE;
}

////////////////////IServerSite/////////////////////////////

HRESULT STDMETHODCALLTYPE CServerSite::OnGameStart()
{
	if (m_LogicalState!=LOGICAL_GAME_STOPPED) {
/*		modified by hhh 2005.5.26
���´��뱻�����ˣ����ڲ���Ҫ�̳�IGameConstExҲ����ֱ��ʹ��OnGameStart����ʼ��Ϸ
		//	���֧��ֱ������
//		if (m_bDirectStart) goto _start_game;*/
		goto _start_game;

		Trace_L2("--CServerSite::OnGameStart, the game is already running");
		return E_FAIL;
	}

	if (m_pDurative) {
		HRESULT hr=m_pDurative->CheckStart();
		if (SUCCEEDED(hr)) goto _start_game;
		else if (E_NOTIMPL!=hr) goto _start_fail;
	}
	if (m_playerlist.GetValidUserNumber()<m_number.nStart) {
		goto _start_fail;
	}

_start_game:
	if (m_ServerState==MAINSERVER_GAME_STOPPED) 
	{
		m_pServer->SetGameStart();
	}
	else 
	{
		OnGameBegin();
	}
	return S_OK;

_start_fail:
	Trace_L1("--CServerSite::OnGameStart, �������㣬�޷���ʼ, ����%d", m_playerlist.GetValidUserNumber());
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CServerSite::OnGameEnd()
{
	if (m_LogicalState==LOGICAL_GAME_STOPPED) {
		Trace_L1("--CServerSite::OnGameEnd, the game is already stopped");
		return E_FAIL;
	}

	m_LogicalState=LOGICAL_GAME_STOPPED;

	DWORD timeElapse=GetTickCount()-m_nStartTime;

	BOOL bWriteScore=TRUE;

	if (!m_isMatch) {
		char buf[2048];
		DumpUserInfo(buf);
		//	�����Ϸ����ʱ�����ڹ���ʱ���1/5�����Ʒ�
		if (timeElapse<m_EstimatedTime.MinTime) {
			Trace_L1("--%s��Ϸ����ʱ��%d������������ʱ��%d�����Ʒ�", buf, timeElapse/1000, m_EstimatedTime.MinTime/1000);
			bWriteScore=FALSE;
		}
		else if (timeElapse<m_EstimatedTime.AbnormalTime) {
			Trace_L1("--%s��Ϸ����ʱ��%d���ڿ�������ʱ��%d������������", buf, timeElapse/1000, m_EstimatedTime.AbnormalTime/1000);
		}
	}

	//д����
	ASSERT(m_scorebuf);

	int ws =0;
	
	//	added by hhh 20030605
	long _getScoreBuf[5];		//����GetScore
	for (int i=0; i<m_playerlist.m_nSides; i++) {
//		if (SUCCEEDED(m_pGame->GetScore(i, &(m_scorebuf[i*20])))) {
		if (SUCCEEDED(m_pGame->GetScore(i, _getScoreBuf))) {
			long score[5];
			memcpy(score, _getScoreBuf, 5*sizeof(long));
			
			long * pscoreBuf =(long *)&(m_scorebuf[i*20]);
			for (int j=0; j<5; j++) pscoreBuf[j] +=_getScoreBuf[j];

			ws+=pscoreBuf[0];

			//	ֻ�����ֿ��ܲ���д��
			if (score[0]>0 && !bWriteScore) continue;
/*
#ifdef _TESTGAME
			//���԰�,���۷�
			if (score[0]<0) score[0]=0;
#endif
*/
			IGamePlayer * pPlayer;
			if (SUCCEEDED(m_playerlist.GetUser(i, &pPlayer))) {
				TABLE_USER_INFO ui;
				pPlayer->GetPlayer(&ui);
				if (m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled() && m_isStopPeriod)
					m_pCoinServer->ForceWriteScore(ui.dwUserID, ui.lDBID, (char*)score);
				m_pServer->SetGameScore(ui.dwUserID, ui.lDBID, (char *)score);
				UpdatePlayerScore(pPlayer, score);
				pPlayer->Release();
			}
			else {
				Trace_L2("--ServerSite::OnGameEnd, can not get player %d when SetGameScore", i);
			}
		}
	}

	if(m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled())
		WriteCoins();

#ifdef _BALANCE_SCORE
	if (ws!=0) {
		char buf[255];
		strcpy(buf, "--ServerSite::OnGameEnd, write score is not balance");
		for (i=0; i<m_playerlist.m_nSides; i++) {
			wsprintf(buf+strlen(buf), " %d", (*(long *)(&m_scorebuf[i*20])));
		}
		Trace_L2(buf);
	}
#endif	//_BALANCE_SCORE

	//	transfer the end sign to the client
	// * /LeonLv move Start for Game end and lift bug, 31July.2010
	// * /m_CmdBuf.InitData(-1, SERVERSITE_GAME_OVER);
	// * /m_CmdBuf.AddData((void *)m_scorebuf, 20*m_number.nMax);
	// * /m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
	// * /LeonLv mark end

	//	����
	if (m_NeedSaveFile) {
		IGameStorage * pStorage;
		if (SUCCEEDED(m_pGame->QueryInterface(IID_IGameStorage, (void **)&pStorage))) {
			int size=0;
			pStorage->GetSaveStorage(NULL, &size);
			if (size<=0) return E_FAIL;
			//	����ռ䲻��
			if (size>m_save.cbSize) {
				m_save.lpData=(char *)realloc(m_save.lpData, size);
				m_save.cbSize=size;
			}
			pStorage->GetSaveStorage(m_save.lpData, &size);
			SendStorage2Client();

			pStorage->Release();
		}
	}
	MatchProcess();
	//	����ܷ���ٿ�ʼ
	BOOL bQuickStart=m_CanQuickStart;

	if (bQuickStart) {
		BOOL bDefaultCheck=TRUE;

		if (m_pDurative) {
			bDefaultCheck=FALSE;
			HRESULT hr;
			hr=m_pDurative->CheckStart();
			if (E_NOTIMPL==hr) bDefaultCheck=TRUE;
			else if (FAILED(hr)) bQuickStart=FALSE;
		}

		if (bDefaultCheck && m_number.nStart>m_playerlist.GetValidUserNumber()) bQuickStart=FALSE;
	}


//����

	//���ÿ�����Ƿ���Լ�����һ�ֵ���Ϸ

	if(m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled())
	{
		BOOL bGameOver = FALSE;
		IGamePlayer *pGamePlayer = NULL;
		for(int i = 0; i < m_playerlist.m_nSides; i++)
		{
			m_playerlist.GetUser(i,&pGamePlayer);
			//�ǽ����Ϸ,����ҹ�����һ�ֵ�
			if(pGamePlayer->IsPlayer())
			{
				long lDuijuCoins,lHandCoins;
				TABLE_USER_INFO *pUser = static_cast<TABLE_USER_INFO*>((GamePlayer*)pGamePlayer);
				if(m_pCoinServer->GetDuijuCoins(lDuijuCoins) 
					&& m_pCoinServer->GetCoins(pUser->lDBID,lHandCoins))
				{
					//	notify the GameLogical
					if(lHandCoins < lDuijuCoins)	//���̫��
					{
						char msg[1024];
						wsprintf(msg,"ϵͳ��Ϣ: ����Ϸ������Ҫ%dö��ң���Ľ��(%d)����һ�ֵ�",lDuijuCoins,lHandCoins);
						m_pCoinServer->SystemMessage(pUser->dwUserID,msg);
						//GetOff(pUser->dwUserID);
						//m_pServer->SetGameEnd();
						ForceKickOff(pUser->nChair);
						bGameOver = TRUE;
					}
				}
			}
		}
		if(bGameOver)
		{
			//Dismiss("��Ҳ��㣬������Ϸ");
			//m_playerlist.ChangePlayerState(sSit);
			bQuickStart = FALSE;
		}
	}


	//	��֧�ֿ��ٿ�ʼ�����㿪ʼ����
	if (!bQuickStart) {
		if (m_ServerState!=MAINSERVER_GAME_STOPPED) {
			m_ServerState = MAINSERVER_GAME_STOPPED;
			m_pServer->SetGameEnd();
			m_playerlist.ChangePlayerState(sSit);
		}
	}

	// * /LeonLv move Here for Game end and lift bug, 31July.2010
	m_CmdBuf.InitData(-1, SERVERSITE_GAME_OVER);
	m_CmdBuf.AddData((void *)m_scorebuf, 20*m_number.nMax);
	m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
	// * /LeonLv move end

	//	����
	if (m_NeedSaveFile) {
		IGameStorage * pStorage;
		if (SUCCEEDED(m_pGame->QueryInterface(IID_IGameStorage, (void **)&pStorage))) {
			int size=0;
			pStorage->GetSaveStorage(NULL, &size);
			if (size<=0) return E_FAIL;
			//	����ռ䲻��
			if (size>m_save.cbSize) {
				m_save.lpData=(char *)realloc(m_save.lpData, size);
				m_save.cbSize=size;
			}
			pStorage->GetSaveStorage(m_save.lpData, &size);
			SendStorage2Client();

			pStorage->Release();
		}
	}



	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::OnSceneChanged(int nIndex)
{
	//	when nIndex>=0, it is means update the specified person only
	if (nIndex>=0) {
		//	check arg first
		if (nIndex>=m_playerlist.GetUserNumber()) {
			Trace_L2("--�޷�����%d����ҵ��ֳ�����Ϊ�����ų�����Χ", nIndex);
			return E_INVALIDARG;
		}

		IGamePlayer * pPlayer;
		if (FAILED(m_playerlist.GetUser(nIndex, &pPlayer))) {
			Trace_L2("--ServerSite::OnSceneChanged, can not get player %d", nIndex);
			return E_FAIL;
		}

		//	if this player is not valid user, return now
		if (!pPlayer->IsValidUser()) {
			Trace_L1("--�޷�����%d����ҵ��ֳ�����Ϊ���û���Ч", nIndex);
			pPlayer->Release();
			return E_FAIL;
		}

		SyncPersonalData(nIndex, SCENE_UPDATE);
		pPlayer->Release();
	}
	//	or update all
	else SyncData(SCENE_UPDATE);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::CreateTimer(ITimerObject ** ppTimer)
{
	if (ppTimer==NULL) return E_INVALIDARG;

	TimerObject * pTimer= new TimerObject;

	pTimer->SetMainLink(&m_TimerList);

	return pTimer->QueryInterface(IID_ITimerObject, (void **)ppTimer);
}

HRESULT STDMETHODCALLTYPE CServerSite::GetServerBase(CServerBase ** ppServer)
{
	if (ppServer==NULL) return E_INVALIDARG;
	
	*ppServer=m_pServer;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::ForceKickOff(int nIdx)
{
	HRESULT hr=E_FAIL;

	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.GetUser(nIdx, &pPlayer))) {
		Trace_L1("--CServerSite::ForceKickOff, %d ���û�������", nIdx);
		return E_FAIL;
	}

	if (pPlayer->IsValidUser()) {
		BOOL br;
		if (m_LogicalState==LOGICAL_GAME_STOPPED) {
			if (m_ServerState==MAINSERVER_GAME_RUNNING) {
				m_ServerState=MAINSERVER_GAME_STOPPED;
				m_pServer->SetGameEnd();
				m_playerlist.ChangePlayerState(sSit);
			}
			br=m_pServer->KickUserOnError(pPlayer->get_ID());
		}
		else {
			br=m_pServer->ForceUserOffline(pPlayer->get_ID());
		}

		if (br) hr=S_OK;
	}
	else {
		Trace_L1("--CServerSite::ForceKickOff, %d ������Ч�û�", nIdx);
	}

	pPlayer->Release();

	return hr;
}

HRESULT STDMETHODCALLTYPE CServerSite::GetOff(DWORD dwUserID)
{
	HRESULT hr=E_FAIL;

	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.FindPeople(dwUserID, &pPlayer))) {
		Trace_L1("--CServerSite::GetOff, ָ�������(id=%d)������", dwUserID);
		return E_FAIL;
	}

	if (pPlayer->IsPlayer()) {
		//	�����
		BOOL br;
		if (m_LogicalState==LOGICAL_GAME_STOPPED) {
			if (m_ServerState==MAINSERVER_GAME_RUNNING) {
				m_ServerState=MAINSERVER_GAME_STOPPED;
				m_pServer->SetGameEnd();
				m_playerlist.ChangePlayerState(sSit);
			}
			br=m_pServer->KickUserOnError(pPlayer->get_ID());
		}
		else {
			br=m_pServer->ForceUserOffline(pPlayer->get_ID());
		}

		if (br) hr=S_OK;
	}
	else {
		//	���Թ���
		if (m_pServer->KickUserOnError(dwUserID)) hr=S_OK;
	}

	pPlayer->Release();

	return hr;

}

HRESULT STDMETHODCALLTYPE CServerSite::WriteScore(int nIdx, char * lpScoreBuf)
{
	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.GetUser(nIdx, &pPlayer))) {
		Trace_L1("--CServerSite::WriteScore, %d ���û�������", nIdx);
		return E_FAIL;
	}

	TABLE_USER_INFO ui;
	pPlayer->GetPlayer(&ui);
	
	//	modified by hhh, 20030605
	//	Ϊ��ǿ�Ƽ������Ϸ�÷��Ƿ�ƽ�⣬�����ǲ��Ǳ�����д��m_scorebuf
	long * pScores =(long *)m_scorebuf;
	long *pps =(long *)lpScoreBuf;
	for (int i=0; i<5; i++) pScores[nIdx*5+i]+=pps[i];

	//���ڱ������������ᱻд��������
	if (!m_isMatch) {
		if (m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled() && m_isStopPeriod)
			m_pCoinServer->ForceWriteScore(ui.dwUserID, ui.lDBID, lpScoreBuf);
		m_pServer->SetGameScore(ui.dwUserID, ui.lDBID, lpScoreBuf);

		UpdatePlayerScore(pPlayer, (long *)lpScoreBuf);
	}
/*	else {
		long * pScores =(long *)m_scorebuf;
		long *pps =(long *)lpScoreBuf;
		for (int i=0; i<5; i++) pScores[nIdx*5+i]+=pps[i];
	}
*/

	pPlayer->Release();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::GetPlayerList(IUserList ** ppList)
{
	return m_playerlist.QueryInterface(IID_IUserList, (void **)ppList);
}

HRESULT STDMETHODCALLTYPE CServerSite::GetLogicalServer(IGameLogical ** ppLogical)
{
#ifdef _DEBUG
	if (ppLogical==NULL) return E_INVALIDARG;
#endif
	
	if (m_pGame) {
		*ppLogical =m_pGame;
		m_pGame->AddRef();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CServerSite::WriteLog(char * szReason, char * szDescription)
{
	char buf[2048];

	ZeroMemory(buf, sizeof(buf));

	lstrcpyn(buf, szReason, 2048);
	lstrcpyn(buf+strlen(buf), " : ", 2048-strlen(buf));
	lstrcpyn(buf+strlen(buf), szDescription, 2048-strlen(buf));

//	m_pServer->ReportEvent(buf);
//	Trace(buf);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::Dismiss(char * szReason, BOOL bDismissTable)
{
	m_CmdBuf.InitData(-1, SERVERSITE_GAME_DISMISS);

	if (szReason!=NULL) {
		m_CmdBuf.AddData(szReason, strlen(szReason)+1);
		m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());	//ïҶ 2009-7-7 �ӣ��մ�����ʾ
	}

// * /	m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());	// �� /�մ�������

	if (bDismissTable) m_pServer->DismissGame();
	else if (m_ServerState!=MAINSERVER_GAME_STOPPED) {
		m_ServerState=MAINSERVER_GAME_STOPPED;
		m_pServer->SetGameEnd();
		m_playerlist.ChangePlayerState(sSit);
	}

	m_LogicalState=LOGICAL_GAME_STOPPED;
	m_ServerState=MAINSERVER_GAME_STOPPED;

	if(m_pCoinServer && m_pCoinServer->IsCoinExtendEnabled())
		WriteCoins();

	return S_OK;
}

DWORD STDMETHODCALLTYPE CServerSite::GetGameOption(DWORD * pOption)
{
	if (pOption!=NULL) * pOption = m_dwOption;

	return m_dwOption;
}

//////////////////////////IGameStateEvent/////////////////////////////

HRESULT STDMETHODCALLTYPE CServerSite::OnLookOnEnableChanged(IGamePlayer * pPlayer, BOOL bEnable)
{
	if (!pPlayer->IsValidUser()) return E_FAIL;

	if (pPlayer->IsPlayer()) {
		IUserList * ul;
		pPlayer->GetLookOnPeopleList(&ul);

		IGamePlayer * pPeople;

		ul->BeginEnum();
		while(SUCCEEDED(ul->EnumNext(&pPeople))) {
			//	enable this people to lookon
			InitPeopleLookOn(pPeople);
			pPeople->Release();
		}

		ul->Release();
	}
	else {
		// enable this people's lookon right
		InitPeopleLookOn(pPlayer);
	}

	if (!m_bEnNeedTrans) return S_OK;

	//	notify the players, the people's look-on right is changed
	m_CmdBuf.InitData(-1, SERVERSITE_ENABLE_LOOKON);
	DWORD id=pPlayer->get_ID();
	m_CmdBuf.AddData(&id, sizeof(DWORD));
	m_CmdBuf.AddData(&bEnable, sizeof(BOOL));
	m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CServerSite::OnStateChanged(IGamePlayer * pPlayer, int nState)
{
	return E_NOTIMPL;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServerSite::CServerSite() : m_dwRef(0)
{
	sc.cbSize=0;
	scObv.cbSize=0;
	scManager.cbSize=0;

	m_pServer=NULL;
	m_pMatch =NULL;
	m_pGame=NULL;
	m_pGameEx=NULL;
	m_pIOEvent=NULL;
	m_pDurative=NULL;

	m_EstimatedTime.NormalTime = 0;
	m_EstimatedTime.MinTime = 0;
	m_EstimatedTime.AbnormalTime =0;
	memset(m_timerlist, 0, sizeof(m_timerlist));

	m_nStartTime=0;

	m_ServerState=MAINSERVER_GAME_STOPPED;
	m_LogicalState=LOGICAL_GAME_STOPPED;

	m_NeedSaveFile=FALSE;
	m_CanQuickStart=FALSE;

	m_save.cbSize=0;
	m_save.lpData=NULL;
	m_save.wanted.clear();
	m_save.wanted.reserve(100);

	m_bEnNeedTrans = TRUE;

	m_pMaster =NULL;

	m_TimerList.SetTimerSupport(static_cast<ITimerSupport *>(this));

	m_pLogiWrapper =NULL;

	m_scorebuf =NULL;

	ZeroMemory(m_matchContextBuf, sizeof(m_matchContextBuf));
	
	m_matchContext =(IMatchServerExtend::MATCHCONTEXT *)&m_matchContextBuf;
	m_matchContext->NumOfSets =0;
	m_matchContext->sizeofContext =0;
	m_matchContext->pContextBuf =m_matchContextBuf+sizeof(IMatchServerExtend::MATCHCONTEXT);

//	ZeroMemory(m_matchScore, sizeof(m_matchScore));

	m_pMatch =NULL;
	m_pMatchLogi =NULL;

	m_isMatch =FALSE;

	m_pCoinServer = NULL;

	m_isStopPeriod = FALSE;

	m_pLogicalTimer =NULL;

	m_bTableEmpty =TRUE;

	m_pNPCLogical =NULL;

	m_phamster_extend_server =NULL;

//	m_pStatistic = CStatistic::Instance();
}

CServerSite::~CServerSite()
{
	//	all of these has been removed because LogiWrapper take this.
	//	when guan's server call my Delete method, LogiWrapper will destory GameLogical first,
	//	and then, delete iteself
//	if (m_pGame) m_pGame->Release();
//	if (m_pGameEx) m_pGameEx->Release();
//	if (m_pIOEvent) m_pIOEvent->Release();
//	if (m_pDurative) m_pDurative->Release();

	if (m_scorebuf) delete m_scorebuf;

	if (m_save.lpData) delete m_save.lpData;

	if (m_pMaster) m_pMaster->Release();

	for (int i=0; i<sizeof(m_timerlist)/sizeof(ITimerObject *); i++) {
		if (m_timerlist[i]!=NULL) {
			//	find the slot
			try {
				m_timerlist[i]->Stop();
				m_timerlist[i]->Release();
				m_timerlist[i] =NULL;
			}
			catch(...) {
				Trace_L3("delete timer failed");
			}
		}
	}
}

//DEL HRESULT CServerSite::SiteInitialize(CServerBase *pServer, IGameLogical *pGameLogical, CTableServer **ppTableServer)
//DEL {
//DEL }

HRESULT CServerSite::SetServerBase(CServerBase *pServer)
{
	if (pServer==NULL) return E_INVALIDARG;

	m_pServer=pServer;

	m_pServer->GetCaps(&ServerCaps);

//	m_TimerList.SetServerBase(pServer);

	m_dwOption = pServer->GetGameOption();

	return S_OK;
}

HRESULT CServerSite::Connect(IGameLogical *pGameLogical)
{
	if (pGameLogical==NULL) return E_INVALIDARG;

	m_bDirectStart = FALSE;

	m_pGame=pGameLogical;
	m_pGame->AddRef();

	// make the server logical work
	m_pGame->SetServerSite(static_cast<IServerSite *>(this));

	//	try to find IGameLogical2, support this interface to add managebility
	m_pGame->QueryInterface(IID_IGameLogical2, (void **)&m_pGameEx);

	//	try to find IPlayerIOEvent
	m_pGame->QueryInterface(IID_IPlayerIOEvent, (void **)&m_pIOEvent);

	// try to find IGameDurative
	m_pGame->QueryInterface(IID_IGameDurative, (void **)&m_pDurative);

	//	try to find IGameLogicalWithTimer
	m_pGame->QueryInterface(IID_IGameLogicalWithTimer, (void **)&m_pLogicalTimer);

	//	and find IGamelogicalWithNPC
	m_pGame->QueryInterface(&m_pNPCLogical);

	//	fill in game const
	m_pGame->GetPlayerNum(&m_number);

	if (SUCCEEDED(m_pGame->CanSaveGame())) m_NeedSaveFile=TRUE;
	else m_NeedSaveFile=FALSE;
	
	if (SUCCEEDED(m_pGame->CanQuickStart())) m_CanQuickStart=TRUE;
	else m_CanQuickStart=FALSE;

	//	try to find IGameConstEx
	BOOL bUseOlderVersion=TRUE;
	if (SUCCEEDED(m_pGame->QueryInterface(IID_IGameConstEx, (void **)&m_pConstEx)))
	{
		bUseOlderVersion = FALSE;
		if (E_NOTIMPL==m_pConstEx->GetEstimateTimeEx(&m_EstimatedTime)) bUseOlderVersion = TRUE;
		if (SUCCEEDED(m_pConstEx->CanDirectStart())) m_bDirectStart=TRUE;
	}

	if (bUseOlderVersion) {
		m_pGame->GetEstimateTime(&(m_EstimatedTime.NormalTime));
		m_EstimatedTime.MinTime = m_EstimatedTime.NormalTime/5;
		m_EstimatedTime.AbnormalTime = m_EstimatedTime.NormalTime/2;
	}

	m_scorebuf =(char *)malloc(m_number.nMax*20);
	ZeroMemory(m_scorebuf, 20*m_number.nMax);

	ASSERT(m_scorebuf);
	m_matchScore =malloc(m_number.nMax*20 + sizeof(IMatchServerExtend::MATCHSCORE));
	ASSERT(m_matchScore);

	m_playerlist.Init(m_number.nMax);
	for (int i=0; i<m_number.nMax; i++) 
		m_playerlist.m_pArray[i]->attach(static_cast<IGameStateEvent *>(this));

	return S_OK;
}

void CServerSite::SyncData(UINT uflag, BOOL bfirst)
{
	for (int i=0; i<m_playerlist.GetUserNumber(); i++) {
		IGamePlayer * pPlayer;
		if (FAILED(m_playerlist.GetUser(i, &pPlayer))) {
			Trace_L3("--ServerSite::SyncData, get player %d failed", i);
			continue;
		}
		IUserList * pLookOn;
		pPlayer->GetLookOnPeopleList(&pLookOn);
		if (pPlayer->IsValidUser() || pLookOn->GetUserNumber()>0) SyncPersonalData(i, uflag, bfirst);
		pLookOn->Release();
		pPlayer->Release();
	}
}

void CServerSite::SyncPersonalData(int nIndex, UINT uflag, BOOL bfirst)
{
	IGamePlayer * pPlayer;
	if (FAILED(m_playerlist.GetUser(nIndex, &pPlayer))) {
		Trace_L3("--ServerSite::SyncPersonalData, specified player %d is not found", nIndex);
		return;
	}

//	if (!pPlayer->IsValidUser()) return;

	//	prepare id
	DWORD id=pPlayer->get_ID();

	int chair=pPlayer->get_chair();

	//	ͬ��Player
	if (FAILED(m_pGame->GetPersonalScene(nIndex, uflag|SCENE_MYSELF, &sc))) {
		Trace_L3("----CServerSite::SyncPersonalData, can't get scene from %d, game dismissed", nIndex);
		m_pServer->DismissGame();
		return;
	}

	char msgID = SERVERSITE_PERSONAL_SCENE;
	if (bfirst) msgID = SERVERSITE_FIRST_SCENE;

	// do transfer only if the player is valid
	if (pPlayer->IsValidUser()) {
		if ( pPlayer->get_State()!=sOffLine ) {
			m_CmdBuf.InitData(-1, msgID);
			m_CmdBuf.AddData(&sc, sc.cbSize);
			m_pServer->SendGameCmdTo(id, m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
		}
		else if (bfirst) {
			ConfirmStart(pPlayer->get_ID());
		}
	}

	IUserList * ul;
	pPlayer->GetLookOnPeopleList(&ul);
	pPlayer->Release();

	IGamePlayer * pLookOn;
	//	����ͬ��LookOn
	scObv.cbSize=0;

	ul->BeginEnum();
	while (SUCCEEDED(ul->EnumNext(&pLookOn))) {
		id=pLookOn->get_ID();
		if (!pLookOn->get_EnableLookOn()) {
			//�Բ������Թ۵���
			if (scObv.cbSize==0) {
				if (FAILED(m_pGame->GetPersonalScene(nIndex, uflag, &scObv))) {
					Trace_L3("----CServerSite::SyncPersonalData, can't get scene from %d, game dismissed", nIndex);
					m_pServer->DismissGame();
					return;
				}
				m_CmdBuf.InitData(-1, msgID);
			}
			m_CmdBuf.AddData(&scObv, scObv.cbSize);
			m_pServer->SendGameCmdTo(id, m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
		}
		else {
			//�Ա������Թ۵���
			m_CmdBuf.InitData(-1, msgID);
			m_CmdBuf.AddData(&sc, sc.cbSize);
			m_pServer->SendGameCmdTo(id, m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
		}

		pLookOn->Release();
	}

	ul->Release();
}

void CServerSite::SendStorage2Client()
{
	//	no data, no file
	if (m_save.cbSize<=0) return;

	if (m_save.wanted.size()<=0) return;

	int nPackages=(m_save.cbSize/1024)+1;
	GAME_CMD_DATA *buf=(GAME_CMD_DATA *)malloc(sizeof(GAME_CMD_DATA)*(nPackages+1));

	buf[0].InitData(-1, SERVERSITE_SAVE_STORAGE);

	int nPackageSize=m_save.cbSize>1024? 1024: m_save.cbSize;
	buf[0].AddData(m_save.lpData, nPackageSize);

	for (int i=1; i<nPackages; i++) {
		buf[i].InitData(-1, SERVERSITE_SAVE_STORAGE_CONTINUE);

		nPackageSize=m_save.cbSize-i*1024;
		if (nPackageSize>1024) nPackageSize=1024;
		buf[i].AddData(&(m_save.lpData[i*1024]), nPackageSize);
	}

	buf[nPackages].InitData(-1, SERVERSITE_SAVE_STORAGE_END);
	buf[nPackages].AddData(&m_save.cbSize, sizeof(int));

	STORAGE::DWORDVECTOR_ITOR head =m_save.wanted.begin();
	STORAGE::DWORDVECTOR_ITOR tail =m_save.wanted.end();
	for (STORAGE::DWORDVECTOR_ITOR itor=head; itor!=tail; itor++) {
		for (int j=0; j<nPackages+1; j++)
			m_pServer->SendGameCmdTo(*itor, buf[j].GetBuf(), buf[j].GetDataLen());
	}

	m_save.wanted.clear();

	delete buf;
}

void CServerSite::PrepareStart()
{
	m_LogicalState=LOGICAL_GAME_WAITING;

	for (int i=0; i<m_playerlist.GetUserNumber(); i++) {
		GamePlayer * player=m_playerlist.m_pArray[i];
 		if (player->IsValidUser()) {
			player->m_ServerData=SERVER_WAITING_START;
		}
	}

	ConfirmStart(0x0ffffffff);
}

HRESULT CServerSite::InitPeopleLookOn(IGamePlayer *pPeople)
{
	TABLE_USER_INFO ui;
	pPeople->GetPlayer(&ui);

	//	try to fill the observer scene
	UINT flag=SCENE_WHOLE;
	if (pPeople->get_EnableLookOn()) flag|=SCENE_MYSELF;

	if (FAILED(m_pGame->GetPersonalScene(ui.nChair, flag, &scObv))) {
		Trace_L2("--CServerSite::OnGameUserEnter, can't get the game scene, this look on people %s will be kicked off", ui.szUserName);
		//	sorry, but this is the only hope
		m_pServer->ForceUserOffline(ui.dwUserID);
		return E_FAIL;
	}
	//	set data
	m_CmdBuf.InitData(-1, SERVERSITE_PERSONAL_SCENE);
	m_CmdBuf.AddData(&scObv, scObv.cbSize);
	m_pServer->SendGameCmdTo(ui.dwUserID, m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());

	return S_OK;
}

HRESULT CServerSite::DumpUserInfo(char *lpszInfo)
{
	if (lpszInfo==NULL) return E_INVALIDARG;

	lpszInfo[0]='\0';

	IGamePlayer * player;
	m_playerlist.BeginEnum();
	while(SUCCEEDED(m_playerlist.EnumNext(&player))) {
		char name[255];
		player->get_UserName(name);
		wsprintf(lpszInfo+strlen(lpszInfo), "%s, id=%d\r\n", name, player->get_ID());
	}

	return S_OK;
}

HRESULT CServerSite::ConfirmStart(DWORD dwUserID)
{
	//	���ڵȴ�״̬��Ч
	if (m_LogicalState==LOGICAL_GAME_WAITING)
	{	
		// if dwUserID is not valid, it means testing condition
		if (dwUserID!=0x0ffffffff) {
			//�ͻ�����ȷ�Ͽ�ʼ
			int nIdx;
			if (FAILED(nIdx=m_playerlist.FindPlayer(dwUserID))) {
				Trace_L2("--CServerSite::OnGameMessage, CLIENTSITE_CONFIRM_START, can't find the specified player, userID=%d", dwUserID);
				return E_FAIL;
			}
			m_playerlist.m_pArray[nIdx]->m_ServerData=SERVER_RECIEVED_START;
		}

		//�Ƿ������˶�ͬ�⿪ʼ
		for (int i=0; i<m_playerlist.GetUserNumber(); i++) {
			GamePlayer * player=m_playerlist.m_pArray[i];
			//	if this people is offline, it will never send confirm signal
			if (player->IsOffline()) continue;

			//	only if the player is valid, check the confirm signal
 			if (player->IsValidUser()) {
				//if (player->m_ServerData!=SERVER_RECIEVED_START) return E_FAIL;
			}
		}

		m_pGame->Start();
		m_LogicalState=LOGICAL_GAME_STARTED;

		//	if store has data, pass it to logical here
		OML_ITOR head =m_cache.begin();
		OML_ITOR tail =m_cache.end();
		for (OML_ITOR itor =head; itor!=tail; itor++) {
			OriginMsg * pMsg =*itor;
			if (pMsg) {
				if (pMsg->nLen>0) m_pGame->OnGameOperation(pMsg->nChair, pMsg->dwUserID, pMsg->buf[0], (void *)&pMsg->buf[1], pMsg->nLen-1);
				else {
					//	spec code
					if (pMsg->nLen==-1) {
						//	replay signal
						OnGameUserReplay(pMsg->dwUserID);
					}
				}
				delete pMsg;
			}
		}
		m_cache.clear();
/*		while (!m_cache.IsEmpty()) {
			OriginMsg * pMsg = m_cache.RemoveHead();
			if (pMsg) {
				if (pMsg->nLen>0) m_pGame->OnGameOperation(pMsg->nChair, pMsg->dwUserID, pMsg->buf[0], (void *)&pMsg->buf[1], pMsg->nLen-1);
				else {
					//	spec code
					if (pMsg->nLen==-1) {
						//	replay signal
						OnGameUserReplay(pMsg->dwUserID);
					}
				}
				delete pMsg;
			}
		}*/
	}
	else {
		Trace_L1("--CServerSite::OnGameMessage, CLIENTSITE_CONFIRM_START, ������״̬�쳣%s", StateDesc[m_LogicalState-1]);
		return E_FAIL;
	}

	return S_OK;
}

void CServerSite::UpdatePlayerScore(IGamePlayer *pPlayer, long *score)
{
	if (m_isMatch) return;

	long * ps  =(long *)((GamePlayer *)pPlayer)->lScoreBuf;

	for (int i=0; i<5; i++) ps[i]+=score[i];
}

void CServerSite::RemoveCache()
{
	OML_ITOR head =m_cache.begin();
	OML_ITOR tail =m_cache.end();
	for (OML_ITOR itor =head; itor!=tail; itor++) {
		OriginMsg * pMsg =*itor;
		if (pMsg) {
			delete pMsg;
		}
	}
	m_cache.clear();
/*	while (!m_cache.IsEmpty()) {
		OriginMsg * pMsg = m_cache.RemoveHead();
		if (pMsg) delete pMsg;
	}*/
}

void CServerSite::MatchProcess()
{
	if (m_isMatch) {
		ASSERT(m_pMatch);

		m_pMatch->GetMatchContext((IMatchServerExtend::MATCHCONTEXT *)m_matchContext);
		IMatchServerExtend::MATCHCONTEXT *pContext =(IMatchServerExtend::MATCHCONTEXT *)m_matchContext;

		if (m_pMatchLogi->meIsSetOver()) {
			m_pMatchLogi->meGetMatchContext(pContext);
			m_pMatch->OnSetOver(pContext->pContextBuf, pContext->sizeofContext);
		}

		m_pMatch->GetMatchContext((IMatchServerExtend::MATCHCONTEXT *)m_matchContext);
		if (m_pMatchLogi->meIsMatchOver(pContext)) {
			IMatchServerExtend::MATCHSCORE * pms =(IMatchServerExtend::MATCHSCORE *)m_matchScore;
			pms->nPlayers =m_number.nMax;
			m_pMatchLogi->meCalcMatchScore(pContext, pms->personal_score);
			m_pMatch->OnMatchOver(pms);

			m_CmdBuf.InitData(-1, SERVERSITE_MATCH_END);
			m_pServer->SendGameCmd(m_CmdBuf.GetBuf(), m_CmdBuf.GetDataLen());
		}

		ZeroMemory(m_scorebuf, 20*m_number.nMax);
	}
}

HRESULT CServerSite::InitMatch()
{
	ASSERT(m_pMatch);

	for (int i=0; i<m_number.nMax; i++) {
		long dbID;
		m_pMatch->GetSeat(i, dbID);

		TABLE_USER_INFO ui;
		ZeroMemory(&ui, sizeof(ui));

		ui.lDBID =dbID;
		ui.nChair =i;

		m_playerlist.AddPeople(ui);
	}

	return S_OK;
}

void CServerSite::WriteCoins()
{
	long lDuijuCoins = 0;
	if(m_pCoinServer && m_bNeedWriteCoins && m_pCoinServer->IsCoinExtendEnabled() && m_pCoinServer->GetDuijuCoins(lDuijuCoins))
	{		
		if(lDuijuCoins == 0) return;	//<0����������Ű�
		int nWinScore =0;								// ���ֵ�����������������ʹ��
		int nLoseCoins =0;								// �ܹ��ɷֵĽ�������ɸ����߸���
		long nPlayerLose[20];					// ÿ����ҵĽ�ҵ���
		ZeroMemory(nPlayerLose, sizeof(nPlayerLose));
		long scorebuf[20];
		memset(scorebuf,0,sizeof(scorebuf));

		for (int i=0; i<m_playerlist.m_nSides; i++) 
		{
			//ͳ�Ƹ����Ľ����
			scorebuf[i] = m_scorebuf[i*20];
			if (scorebuf[i] < 0) 
			{
				nLoseCoins += lDuijuCoins;
				nPlayerLose[i] -= lDuijuCoins;
			}
			else 
			{
				if (scorebuf[i] > 0) nWinScore += scorebuf[i];
			}
		}
		//	����ÿ�˲���Ǯ�����������ȫ���ܳ��ֵġ����磬����У���ƽ��ʱ�������˶���1��
		if (nLoseCoins==0) return;

		//	����
		long resultScore[20];
		memset(resultScore,0,sizeof(resultScore));
		for (int i = 0; i < m_playerlist.m_nSides; i++) 
		{
			if(scorebuf[i] < 0)	//ÿһ����ֵ��ˣ��������������ָ�������
			{
				IGamePlayer *pPlayer;
				m_playerlist.GetUser(i,&pPlayer);
				TABLE_USER_INFO *pTableUserInfo = static_cast<TABLE_USER_INFO*>((GamePlayer*)pPlayer);
				long lHandCoins;
				m_pCoinServer->GetCoins(pTableUserInfo->lDBID,lHandCoins);
				long needpay = 0;
				if(lHandCoins - lDuijuCoins < 0)	//Ǯ����
				{
					needpay = lHandCoins;
				}
				else
				{
					needpay = lDuijuCoins;
				}
				long realpay = 0;
				for(int j=0; j < m_playerlist.m_nSides; j++)
				{
					if(scorebuf[j] > 0)
					{
						resultScore[j] += needpay * scorebuf[j] / nWinScore;
						realpay += needpay * scorebuf[j] / nWinScore;
					}
				}
				resultScore[i] -= realpay;			
			}
		}
		//�ܵĵ�ʧ��Ӧ��ƽ��
#ifdef _DEBUG
		long lBalanceScore = 0;
		for(int i = 0; i < m_playerlist.m_nSides; i++)
		{
			lBalanceScore += resultScore[i];
		}
		ASSERT(lBalanceScore == 0);
#endif
		char szMsg[2048];
		memset(szMsg,0,sizeof(szMsg));
		wsprintf(szMsg,"���ͨ����<br>");
		for(int i = 0; i < m_playerlist.m_nSides; i++)
		{
			char szPlayerInfo[1024];
			memset(szPlayerInfo,0,sizeof(szPlayerInfo));
			int nPlayerInfoLen = 0;
			IGamePlayer *pPlayer = NULL;
			m_playerlist.GetUser(i,&pPlayer);
			TABLE_USER_INFO *pTableUserInfo = static_cast<TABLE_USER_INFO*>((GamePlayer*)pPlayer);
			char szName[256];
			memset(szName,0,sizeof(szName));
			pPlayer->get_UserName(szName);
			long lHandCoins = 0;
			m_pCoinServer->GetCoins(pTableUserInfo->lDBID,lHandCoins);
			if(resultScore[i] > 0)
				wsprintf(szPlayerInfo,"%s Ӯ %d���   ʣ��%d���<br>",szName,resultScore[i],lHandCoins + resultScore[i]);
			else
				wsprintf(szPlayerInfo,"%s �� %d���   ʣ��%d���<br>",szName,resultScore[i],lHandCoins + resultScore[i]);
			strcat(szMsg,szPlayerInfo);
		}

		//ͨ������
		for(int i = 0; i < m_playerlist.m_nSides; i++)
		{
			IGamePlayer *pPlayer = NULL;
			m_playerlist.GetUser(i,&pPlayer);
			//������д�����ݿ�
			TABLE_USER_INFO *pTableUserInfo = static_cast<TABLE_USER_INFO*>((GamePlayer*)pPlayer);

			m_pCoinServer->WriteCoins(pTableUserInfo->lDBID,resultScore[i]);

			//֪ͨ�ͻ��˵÷���Ϣ 
			m_pCoinServer->SystemMessage(pPlayer->get_ID(),szMsg);
		}
	}
	//ȷ��ÿ��ֻдһ�η���
	m_bNeedWriteCoins = FALSE;
}

void CServerSite::StopGame(UINT nFlags)
{
	m_pGame->Stop(nFlags);
	if(nFlags == STOP_NOENOUGHPLAYER) m_isStopPeriod = TRUE;
}


// LogicalHelper.cpp: implementation of the LogicalHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogicalHelper.h"
#include "GameHelperMsg.h"
#include "traceout.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogicalHelper::LogicalHelper()
{
	m_pSite=NULL;
	m_ready =NULL;
}

LogicalHelper::~LogicalHelper()
{
	if (m_pSite) m_pSite->Release();

	if (m_ready) delete m_ready;
}

HRESULT LogicalHelper::OnGameOperation(int nIndex, DWORD dwUserID, int opID, void * oData)
{
	if (m_ready) {
		if (opID==LOGICALHELPER_READY) {
			m_ready[nIndex]=TRUE;

			CServerBase * pServer;
			m_pSite->GetServerBase(&pServer);

			GAME_CMD_DATA cmd;
			cmd.InitData(-1, LOGICALHELPER_READY_CONFIRMED);
			cmd.AddData(&dwUserID, sizeof(DWORD));
			pServer->SendGameCmd(cmd.GetBuf(), cmd.GetDataLen());

			tryStartGame();

			return S_OK;
		}
	}

	return E_NOTIMPL;
}

HRESULT LogicalHelper::SetServerSite(IServerSite *pSite)
{
	if (pSite) {
		if (FAILED(pSite->QueryInterface(IID_IServerSite, (void **)&m_pSite))) return E_FAIL;

		IGameLogical * pGame;
		m_pSite->GetLogicalServer(&pGame);

		IGameConst *pConst;
		pGame->QueryInterface(IID_IGameConst, (void **)&pConst);
		pGame->Release();

		IGameConst::PLAYERNUMBER number;
		pConst->GetPlayerNum(&number);
		pConst->Release();

		m_nPlayers=number.nMax;
		m_nRequiredPlayer =number.nStart;

		if (NULL==(m_ready=(BOOL *)malloc(sizeof(BOOL)*m_nPlayers))) return E_OUTOFMEMORY;
		ZeroMemory(m_ready, sizeof(BOOL)*m_nPlayers);

		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT LogicalHelper::Reset()
{
	if (m_ready) ZeroMemory(m_ready, sizeof(BOOL)*m_nPlayers);

	return S_OK;
}

HRESULT LogicalHelper::OnUserExit(int nIdx, IGamePlayer * pGamePlayer)
{
	if (nIdx<0 || nIdx>=m_nPlayers) return E_INVALIDARG;

	if (pGamePlayer->IsPlayer()) {
		m_ready[nIdx]=FALSE;
		tryStartGame();
		return S_OK;
	}

	return E_NOTIMPL;
}

HRESULT LogicalHelper::tryStartGame()
{
	IUserList * pList;
	if (FAILED(m_pSite->GetPlayerList(&pList))) return E_FAIL;

	int	 n =0;
	IGamePlayer * pPlayer =NULL;
	for (int i=0; i<m_nPlayers; i++) {
		if (FAILED(pList->GetUser(i, &pPlayer))) {
			Trace_L2("--LogicalHelper::tryStartGame, get player %d failed", i);
			continue;
		}
		if (pPlayer->IsValidUser()) { 
			if (!m_ready[i]) {
				pPlayer->Release();
				return S_OK;
			}
			n++;
		}

		pPlayer->Release();
	}

	if (n<m_nRequiredPlayer) return S_OK;

	for (int i = 0; i<m_nPlayers; i++) m_ready[i]=FALSE;
	return m_pSite->OnGameStart();
}

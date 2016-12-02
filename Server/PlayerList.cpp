// PlayerList.cpp: implementation of the PlayerList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayerList.h"
#include "TraceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PlayerList::PlayerList()
{
	m_nSides=0;

	m_bInited =FALSE;
//	m_sides=NULL;
}

PlayerList::~PlayerList()
{
	Clear();
}

HRESULT PlayerList::Init(int nMaxPlayers)
{
	if (nMaxPlayers>0) {
		m_nSides=nMaxPlayers;
		m_nNum=nMaxPlayers;
		return SetBoundary(nMaxPlayers);
	}

	return E_INVALIDARG;
}

HRESULT PlayerList::AddPeople(TABLE_USER_INFO & PlayerInfo, IGamePlayer ** ppGamePlayer)
{
	if (!IsValidUser(PlayerInfo)) {
		Trace_L3("----PlayerList::AddPeople, invalid user, chair=%d, id=%d, for call stack, see below", PlayerInfo.nChair, PlayerInfo.dwUserID);
		return E_INVALIDARG;
	}

	ensureClearSamePlayer(PlayerInfo);

	//	make a shortcut to the player
	GamePlayer * pgp=m_pArray[PlayerInfo.nChair];

	if (PlayerInfo.IsLookOnPeople()) {
		// for look on people

		//	make another shortcut to lookon people first
		UserList * ul=pgp->m_lookons;
		ul->m_nNum++;
		//	检查UserList大小
		if ((UINT)ul->m_nNum > ul->m_nMax) {
			//	should adjust it
			ul->Append(20);
		}
		ul->m_pArray[ul->m_nNum-1]->SetPlayer(PlayerInfo);

		ul->m_pArray[ul->m_nNum-1]->QueryInterface(IID_IGamePlayer, (void **)ppGamePlayer);

		//	设置默认旁观状态
		BOOL en;
		pgp->get_EnableLookOn(&en);
		ul->m_pArray[ul->m_nNum-1]->put_EnableLookOn(en);

		ul->m_pArray[ul->m_nNum-1]->m_ver.curVersion=-1;
		ul->m_pArray[ul->m_nNum-1]->m_ver.maxVersion=-1;
		ul->m_pArray[ul->m_nNum-1]->m_ver.minVersion=-1;
	}
	else {
		//	for player
		pgp->SetPlayer(PlayerInfo);
		pgp->QueryInterface(IID_IGamePlayer, (void **)ppGamePlayer);

		pgp->m_ver.curVersion=-1;
		pgp->m_ver.maxVersion=-1;
		pgp->m_ver.minVersion=-1;
	}

	m_bInited =TRUE;

	return S_OK;
}

HRESULT PlayerList::RemovePeople(UINT dwUserID)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L3("----PlayerList::RemovePeople, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	if (!m_bInited) return E_FAIL;

	//	search the player list first
	for (int i=0; i<m_nSides; i++) {
		// if the quiter is a player
		if (m_pArray[i]->dwUserID==dwUserID) {
			// remove itself and all of its lookons
			m_pArray[i]->put_State(sGetOut);
			return S_OK;
		}
		else {
			//	search the lookon people list
			UserList * ul=m_pArray[i]->m_lookons;
			IGamePlayer * pPlayer;
			for (int j=0; j<ul->GetUserNumber(); j++) {
				if (FAILED(ul->GetUser(j, &pPlayer))) {
					Trace_L2("--PlayerList::RemovePeople, get user %d failed", j);
					continue;
				}
				if (pPlayer->get_ID()==dwUserID) {
					ul->Remove(j);
					return S_OK;
				}
			}
		}
	}

	Trace_L1("----PlayerList::RemovePeople, specified user id=%d is not found", dwUserID);

	return E_FAIL;
}

HRESULT PlayerList::FindPeople(UINT dwUserID, IGamePlayer ** ppPlayer)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L2("----PlayerList::FindPeople, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	// first, let's see whether it is a player
	HRESULT hr=FindUser(dwUserID, ppPlayer);
	if (SUCCEEDED(hr)) return hr;

	// next, search the lookon list
	IUserList * ul;
	for (int i=0; i<GetUserNumber(); i++) {
		m_pArray[i]->GetLookOnPeopleList(&ul);
		if (SUCCEEDED(ul->FindUser(dwUserID, ppPlayer))) {
			ul->Release();
			return i;
		}
		ul->Release();
	}

	Trace_L1("----PlayerList::FindPeople, can't find people dwUserID=%d, for call stack, see below", dwUserID);

	return E_FAIL;
}

HRESULT PlayerList::FindPlayer(UINT dwUserID, IGamePlayer ** ppPlayer)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L2("----PlayerList::FindPlayer, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	HRESULT hr=FindUser(dwUserID, ppPlayer);
	if (SUCCEEDED(hr)) return hr;
	
	Trace_L1("----PlayerList::FindPlayer, can't find player dwUserID=%d, for call stack, see below", dwUserID);
	
	return E_FAIL;
}

void PlayerList::Clear()
{
	for (unsigned int i=0; i<m_nMax; i++) {
		m_pArray[i]->m_lookons->Clear();
		m_pArray[i]->Clear(TRUE);
	}

	m_nNum=0;
	m_nSides=0;

	m_bInited =FALSE;
}

HRESULT PlayerList::PrepareRemovePeople(DWORD dwUserID)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L2("----PlayerList::PrepareRemovePeople, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	//	search the player list first
	for (int i=0; i<m_nSides; i++) {
		// if the quiter is a player
		if (m_pArray[i]->dwUserID==dwUserID) {
			// remove itself and all of its lookons
			m_pArray[i]->put_State(sFree);
			return S_OK;
		}
		else {
			//	search the lookon people list
			UserList * ul=m_pArray[i]->m_lookons;
			IGamePlayer * pPlayer;
			for (int j=0; j<ul->GetUserNumber(); j++) {
				ul->GetUser(j, &pPlayer);
				if (pPlayer->get_ID()==dwUserID) {
					((GamePlayer *)pPlayer)->nState =sFree;
					pPlayer->Release();
					return S_OK;
				}
				pPlayer->Release();
			}
		}
	}

	Trace_L2("----PlayerList::PrepareRemovePeople, specified user id=%d is not found", dwUserID);

	return E_FAIL;
}

HRESULT PlayerList::ChangePlayerState(int nState)
{
	for (int i=0; i<m_nNum; i++) {
		if (m_pArray[i]->IsValidUser()) {
			//	对于断线的玩家，不能把他们的状态置为sPlaying
			if (nState==sPlaying && m_pArray[i]->get_State()==sOffLine) continue;
			m_pArray[i]->put_State(nState);
		}
	}

	return S_OK;
}

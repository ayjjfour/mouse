// GamePlayer.cpp: implementation of the CGamePlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GamePlayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////
//	Interfaces
/////////////////////////////////////////////////////////////////////////

///////////////////////IGamePlayer///////////////////////////////////////

STDMETHODIMP GamePlayer::GetLookOnPeopleList(IUserList **ppUserList)
{
	return m_lookons->QueryInterface(IID_IUserList, (void **)ppUserList);
}

STDMETHODIMP GamePlayer::put_EnableLookOn(BOOL bEnable)
{
	BOOL bOldValue=m_bEnableLookOn;

	m_bEnableLookOn=bEnable;

	for (int i=0; i<m_lookons->GetUserNumber(); i++) {
		m_lookons->m_pArray[i]->m_bEnableLookOn=bEnable;
	}

	if (bOldValue!=m_bEnableLookOn && m_pStateNotify) m_pStateNotify->OnLookOnEnableChanged(this, bEnable);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GamePlayer::GamePlayer()
{
	m_dwRef=0;

	m_lookons = new UserList;

	m_pStateNotify = NULL;

	Clear(TRUE);
}

GamePlayer::~GamePlayer()
{
	if (m_lookons) delete m_lookons;

	if (m_pStateNotify) m_pStateNotify->Release();
}

void GamePlayer::Clear(BOOL bClearAll)
{
	if (bClearAll) {
		ZeroMemory(static_cast<TABLE_USER_INFO *>(this), sizeof(TABLE_USER_INFO));
		m_bIsLookOn = FALSE;
		m_phamster_extend_buf =NULL;
	}

	dwUserID=INVALID_USER_ID;

	m_bEnableLookOn=FALSE;
	
	m_status=0;
	m_userdata=0;
	m_pdata=0;

//	m_lookons->Clear();

	m_ServerData=0;
}

HRESULT GamePlayer::SetPlayer(TABLE_USER_INFO & PlayerInfo)
{
	*(static_cast<TABLE_USER_INFO *>(this))=PlayerInfo;

//	m_bEnableLookOn=FALSE;
	m_status=0;

	if (nState==sLookOn) {
		m_bIsLookOn =TRUE;
	}
	else {
		m_bIsLookOn =FALSE;
	}

	put_EnableLookOn(FALSE);

	return S_OK;
}

HRESULT GamePlayer::attach(IGameStateEvent *pEvent)
{
	m_pStateNotify=pEvent;

	m_pStateNotify->AddRef();

	return S_OK;
}

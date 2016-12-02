// GamePlayer.h: interface for the CGamePlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEPLAYER_H__F7BD89CA_282E_43F5_8D37_21D9256A21EB__INCLUDED_)
#define AFX_GAMEPLAYER_H__F7BD89CA_282E_43F5_8D37_21D9256A21EB__INCLUDED_

#include "IGameStateEvent.h"	// Added by ClassView
#include "UserList.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UserList;

class GamePlayer : public IGamePlayer, public TABLE_USER_INFO
{
public:			//IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (ppvObject==NULL) return E_INVALIDARG;

		*ppvObject =NULL;
		if (riid==IID_IGamePlayer || riid==IID_IUnknown) *ppvObject =(void *)this;
		else return E_NOINTERFACE;

		return AddRef();
	};
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return (++m_dwRef);
	};
    virtual ULONG STDMETHODCALLTYPE Release( void) {
		return (--m_dwRef);
	};

private:
	ULONG m_dwRef;

public:				// IGamePlayer
	STDMETHOD(GetPlayer)(void * pPlayerInfo) {
		if (pPlayerInfo==NULL) return E_INVALIDARG;

		memcpy(pPlayerInfo, static_cast<TABLE_USER_INFO *>(this), sizeof(TABLE_USER_INFO));

		return S_OK;
	};
	
	STDMETHOD(put_UserStatus)(int nStatus) {
		m_status=nStatus;

		return S_OK;
	};
	STDMETHOD_(int, get_UserStatus)(int * pStatus=NULL){
		if (pStatus!=NULL) 	*pStatus=m_status;

		return m_status;
	};
	STDMETHOD(put_UserData)(int nValue) {
		m_userdata=nValue;
		return S_OK;
	};
	STDMETHOD_(int, get_UserData)(int * pValue=NULL) {
		if (pValue!=NULL) *pValue=m_userdata;

		return m_userdata;
	};
	STDMETHOD(put_UserDataBlock)(void * lpdata) {
		m_pdata=lpdata;
		return S_OK;
	};
	STDMETHOD(get_UserDataBlock)(void ** ppdata) {
		if (ppdata==NULL) return E_INVALIDARG;
		*ppdata=m_pdata;
		return S_OK;
	};

	STDMETHOD(GetLookOnPeopleList)(IUserList **ppUserList);

	STDMETHOD_(BOOL, IsValidUser)() {
		return (IsValid() && nState>sFree);
	};
	STDMETHOD_(BOOL, IsPlayer)(){
		return (!m_bIsLookOn);
	};

	STDMETHOD(put_State)(int sState) {
		nState=sState;

		if (nState==sLookOn) m_bIsLookOn = TRUE;
		else m_bIsLookOn = FALSE;

		if (m_pStateNotify) m_pStateNotify->OnStateChanged(this, sState);

		return S_OK;
	};
	STDMETHOD_(int, get_State)(int * pState=NULL) {
		if (pState!=NULL) *pState=nState;

		return nState;
	};

	STDMETHOD(put_EnableLookOn)(BOOL bEnable);
	STDMETHOD_(BOOL, get_EnableLookOn)(BOOL * pEnable) {
		if (pEnable!=NULL)	*pEnable=m_bEnableLookOn;

		return m_bEnableLookOn;
	};
	
	STDMETHOD_(DWORD, get_ID)(DWORD * pID=NULL) {
		if (pID!=NULL)	*pID=dwUserID;

		return dwUserID;
	};
	STDMETHOD_(long, get_DBID)(long * pID=NULL) 
	{
		if (pID!=NULL)	*pID=lDBID;

		return lDBID;
	}
	STDMETHOD_(int, get_chair)(int * chair=NULL) {
		if (chair!=NULL) *chair=nChair;

		return nChair;
	};
	STDMETHOD(get_UserName)(char * name) {
		if (name==NULL) return E_INVALIDARG;

		lstrcpyn(name, szUserName, MAX_NAME_LEN);

		return S_OK;
	};
	STDMETHOD_(DWORD, get_FaceID)(DWORD * pFaceID=NULL) {
		if (pFaceID!=NULL) *pFaceID=nFaceID;

		return nFaceID;
	};
	// 获取用户权利
	STDMETHOD_(DWORD, get_UserRight)(DWORD * pRight=NULL) {
		if (pRight!=NULL) *pRight=dwRightLevel;

		return dwRightLevel;
	};
	// 获取用户所属社团名
	STDMETHOD(get_GroupName)(char * lpName) {
		if (lpName==NULL) return E_INVALIDARG;

		lpName[0]='\0';

		return S_OK;
	};
	// 获取用户成绩缓冲区
	STDMETHOD(get_ScoreBuf)(long * lpScoreBuf) {
		if (lpScoreBuf==NULL) return E_INVALIDARG;

		memcpy(lpScoreBuf, lScoreBuf, sizeof(lScoreBuf));

		return S_OK;
	};

	STDMETHOD_(BOOL, IsRunAway)() {
		return (nState==sFree);
	};

	STDMETHOD_(char*, get_hamster_extend_buf)() {
		return (m_phamster_extend_buf);
	};
	STDMETHOD(set_hamster_extend_buf)(char * pBuf) {
		m_phamster_extend_buf =pBuf;
		return S_OK;
	}

	// * /LeonLv add:原普通写分接口导致分数缓存不能随写分更新，添加此接口用于写分时单独更新分数缓存；普通写分方式不能使用此接口，22Sep.2012
	STDMETHOD(update_ScoreBuf)(long * lpAltScore){
		long * ps  =(long *)lScoreBuf;

		for (int i=0; i<5; ++i)
		{
			ps[i] += lpAltScore[i];
		}
		return S_OK;
	}

	STDMETHOD_(LONGLONG, get_Exp)(LONGLONG *pExp = NULL)
	{
		if (pExp != NULL) *pExp = UserExp.llExp;

		return UserExp.llExp;
	}

	STDMETHOD(put_Exp)(LONGLONG nValue)
	{
		UserExp.llExp = nValue;

		return S_OK;
	}

public:
	HRESULT attach(IGameStateEvent * pEvent);
	HRESULT SetPlayer(TABLE_USER_INFO & PlayerInfo);
	
	void Clear(BOOL bClearAll=FALSE);				//清扫函数

	GamePlayer();
	virtual ~GamePlayer();

private:
	IGameStateEvent * m_pStateNotify;

	BOOL m_bEnableLookOn;
	BOOL m_bIsLookOn;

	int m_status;
	int m_userdata;
	void * m_pdata;

	char * m_phamster_extend_buf;

protected:					// for PlayerList & CServerSite
	UserList * m_lookons;
	int m_ServerData;
	IGameConst::VERSION_STRUCT m_ver;		//客户端版本号

friend class PlayerList;
friend class CServerSite;
};


#endif // !defined(AFX_GAMEPLAYER_H__F7BD89CA_282E_43F5_8D37_21D9256A21EB__INCLUDED_)

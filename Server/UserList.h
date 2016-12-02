// UserList.h: interface for the UserList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERLIST_H__8E6BE1EB_7C49_4EFD_A60F_3A91C19FC19A__INCLUDED_)
#define AFX_USERLIST_H__8E6BE1EB_7C49_4EFD_A60F_3A91C19FC19A__INCLUDED_

#include "GamePlayer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "site_i.h"

class GamePlayer;

class UserList  : public IUserList
{
public:			//IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef( void);
    virtual ULONG STDMETHODCALLTYPE Release( void);

private:
	DWORD m_dwRef;

public:		// IEnumUser
	STDMETHOD(Next)(ULONG celt, IGamePlayer **ppElements, ULONG *pceltFetched);
	STDMETHOD(Reset)(void) {
		m_cur =0;

		return S_OK;
	};
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Clone)(IEnumUser **ppEnum) {
		return E_NOTIMPL;
	};
private:
	int m_cur;

public:			//IUserList
	STDMETHOD(BeginEnum)();									// 枚举函数，开始枚举
	STDMETHOD(EnumNext)(IGamePlayer ** ppPlayer);			// 枚举函数，枚举下一个

	STDMETHOD(GetUser)(int nIdx, IGamePlayer ** ppPlayer);	// 索引函数，索引nIdx处的User
	STDMETHOD_(int, GetUserNumber)();						// 索引函数，索引范围
	
	STDMETHOD(FindUser)(DWORD dwUserID, IGamePlayer ** ppPlayer);	// 辅助函数，搜索指定的User
	STDMETHOD_(int, GetValidUserNumber)();							// 辅助函数，获取当前有效User数量
	STDMETHOD(ensureClearPlayer)(DWORD dwUserID, long lDBID);
private:
	int m_nCursor;						// 用于枚举，目前枚举到第几个

protected:
	UINT m_nMax;							// 列表上限
	int m_nNum;								// 当前数量
	GamePlayer ** m_pArray;					// User列表

public:
	HRESULT SetBoundary(UINT nMax);
	void Clear();
	HRESULT Append(UINT nIncrease);
	HRESULT Remove(int nIdx);

	UserList();
	virtual ~UserList();

friend class PlayerList;
friend class CServerSite;
friend class GamePlayer;
};

#endif // !defined(AFX_USERLIST_H__8E6BE1EB_7C49_4EFD_A60F_3A91C19FC19A__INCLUDED_)

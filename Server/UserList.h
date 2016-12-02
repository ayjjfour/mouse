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
	STDMETHOD(BeginEnum)();									// ö�ٺ�������ʼö��
	STDMETHOD(EnumNext)(IGamePlayer ** ppPlayer);			// ö�ٺ�����ö����һ��

	STDMETHOD(GetUser)(int nIdx, IGamePlayer ** ppPlayer);	// ��������������nIdx����User
	STDMETHOD_(int, GetUserNumber)();						// ����������������Χ
	
	STDMETHOD(FindUser)(DWORD dwUserID, IGamePlayer ** ppPlayer);	// ��������������ָ����User
	STDMETHOD_(int, GetValidUserNumber)();							// ������������ȡ��ǰ��ЧUser����
	STDMETHOD(ensureClearPlayer)(DWORD dwUserID, long lDBID);
private:
	int m_nCursor;						// ����ö�٣�Ŀǰö�ٵ��ڼ���

protected:
	UINT m_nMax;							// �б�����
	int m_nNum;								// ��ǰ����
	GamePlayer ** m_pArray;					// User�б�

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

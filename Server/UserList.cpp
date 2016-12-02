// UserList.cpp: implementation of the UserList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserList.h"
#include "TraceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//	Interfaces
////////////////////IUnknown///////////////////////////////////

HRESULT STDMETHODCALLTYPE UserList::QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (ppvObject==NULL) return E_INVALIDARG;

	*ppvObject=NULL;

	if (riid==IID_IUserList || riid==IID_IEnumUser || riid==IID_IUnknown) *ppvObject=(void *)this;
	else return E_NOINTERFACE;

	AddRef();

	return S_OK;
}

ULONG	STDMETHODCALLTYPE UserList::AddRef()
{
	return (++m_dwRef);
}

ULONG	STDMETHODCALLTYPE UserList::Release()
{
	return (--m_dwRef);
}

////////////////////////IEnumUser///////////////////////////////////

STDMETHODIMP UserList::Next(ULONG celt, IGamePlayer **ppElements, ULONG *pceltFetched)
{
	if (ppElements==NULL) return E_INVALIDARG;

	int nEnd =m_cur + celt;
	if (nEnd>=m_nNum) nEnd =m_nNum;

	for (int i=m_cur; i<nEnd; i++) {
		m_pArray[i]->QueryInterface(IID_IGamePlayer, (void **)&ppElements[i-m_cur]);
	}

	unsigned int fetched =nEnd -m_cur;
	if (pceltFetched) *pceltFetched =fetched;

	m_cur =nEnd;

	if (fetched<celt) return S_FALSE;

	return S_OK;
}

STDMETHODIMP UserList::Skip(ULONG celt)
{
	m_cur +=celt;

	if (m_cur >= m_nNum) {
		m_cur =m_nNum-1;
		return S_FALSE;
	}
	else if (m_cur<0) {
		m_cur =0;
		return S_FALSE;
	}

	return S_OK;
}

///////////////////////////////IUserList//////////////////////////////

HRESULT STDMETHODCALLTYPE UserList::BeginEnum()
{
	m_nCursor=0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE UserList::EnumNext(IGamePlayer ** ppPlayer)
{
	if (m_nCursor<m_nNum) {
		m_pArray[m_nCursor]->QueryInterface(IID_IGamePlayer, (void **)ppPlayer);
		m_nCursor++;

		return S_OK;
	}

	return E_FAIL;
}


HRESULT STDMETHODCALLTYPE UserList::GetUser(int nIdx, IGamePlayer ** ppPlayer)
{
	if (ppPlayer==NULL) return E_INVALIDARG;
	*ppPlayer =NULL;

	if (nIdx<0 || nIdx>=m_nNum) {
		Trace_L2("----UserList::GetUser, %d 索引号超出范围", nIdx);
		return E_FAIL;
	}

	m_pArray[nIdx]->QueryInterface(IID_IGamePlayer, (void **)ppPlayer);

	return S_OK;
}

int STDMETHODCALLTYPE UserList::GetUserNumber()
{
	return m_nNum;
}


HRESULT STDMETHODCALLTYPE UserList::FindUser(DWORD dwUserID, IGamePlayer ** ppPlayer)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L2("----UserList::FindUser, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	for (int i=0; i<m_nNum; i++) {
		if (m_pArray[i]->IsValidUser() && m_pArray[i]->dwUserID==dwUserID) {
			m_pArray[i]->QueryInterface(IID_IGamePlayer, (void **)ppPlayer);
			return i;
		}
	}

//	this warning is so much, don't trace it any more
//	Trace("----UserList::FindUser, can't find user dwUserID=%d, for call stack, see below", dwUserID);
	
	return E_FAIL;

}

int STDMETHODCALLTYPE UserList::GetValidUserNumber()
{
	int num=0;

	for (int i=0; i<m_nNum; i++) {
		if (m_pArray[i]->IsValidUser()) num++;
	}

	return num;

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserList::UserList() : m_dwRef(0)
{
	m_nCursor=0;
	m_nNum=0;
	m_nMax=0;
	m_pArray=NULL;
	m_cur =0;
}

UserList::~UserList()
{
	Clear();
}

HRESULT UserList::Append(UINT nIncrease)
{
	UINT nOrg=m_nMax;

	m_nMax+=nIncrease;

	m_pArray=(GamePlayer **)realloc(m_pArray, m_nMax*sizeof(GamePlayer *));

	if (m_pArray==NULL) {
		Clear();
		return E_OUTOFMEMORY;
	}

	for (UINT i=nOrg; i<m_nMax; i++) {
		m_pArray[i] = new GamePlayer;
	}

	return S_OK;
}

HRESULT UserList::Remove(int nIdx)
{
	if (nIdx<0 || nIdx>=m_nNum) return E_FAIL;

	GamePlayer * temp=m_pArray[nIdx];
	//	remove this user's data
	temp->Clear();

	//and then, adjust array
	m_nNum--;
	memmove(&m_pArray[nIdx], &m_pArray[nIdx+1], (m_nNum-nIdx)*sizeof(GamePlayer *));

	//	注意，我们从不放弃内存
	m_pArray[m_nNum]=temp;
	
	return S_OK;
}

void UserList::Clear()
{
	if (m_pArray) {
		for (UINT i=0; i<m_nMax; i++) delete m_pArray[i];

		delete m_pArray;
	}

	m_nCursor=0;
	m_nNum=0;
	m_nMax=0;
	m_pArray=NULL;
}

HRESULT UserList::SetBoundary(UINT nMax)
{
	if (nMax<=m_nMax) return E_FAIL;
	return Append(nMax-m_nMax);
}

//	added by hhh 20030605
HRESULT STDMETHODCALLTYPE UserList::ensureClearPlayer(DWORD dwUserID, long lDBID)
{
	if (!IsValidUserID(dwUserID)) {
		Trace_L2("----PlayerList::ensureClearSamePlayer, invalid arg dwUserID=%d, for call stack, see below", dwUserID);
		return E_FAIL;
	}

	for (int i=0; i<m_nNum; i++) {
		if (m_pArray[i]->dwUserID==dwUserID || m_pArray[i]->lDBID==lDBID) {
			m_pArray[i]->dwUserID=INVALID_USER_ID;
		}
	}

	return S_OK;
}

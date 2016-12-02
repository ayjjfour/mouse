// EventHostObj.h: interface for the EventHostObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTHOSTOBJ_H__7C5F5FF3_3E99_470C_8256_57F11FE20A38__INCLUDED_)
#define AFX_EVENTHOSTOBJ_H__7C5F5FF3_3E99_470C_8256_57F11FE20A38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "site_i.h"

class EventHostObj : public IEventHost
{
public:				// IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) {
		ASSERT(ppvObject);

		*ppvObject=NULL;

		if (riid==IID_IEventHost || riid==IID_IUnknown) *ppvObject=(void *)(this);
		else return E_NOINTERFACE;

		AddRef();

		return S_OK;
	};
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return (++m_dwRef);
	};
    virtual ULONG STDMETHODCALLTYPE Release( void) {
		return (--m_dwRef);
	};
private:
	ULONG m_dwRef;

public:			// IEventHost
	STDMETHOD(attach)(void * pNotify, int nlevel =9);
	STDMETHOD(detach)(void * pNotify);
	STDMETHOD(enableNotify)(void * pNotify, BOOL bEnabled =TRUE);

protected:
	HRESULT __detach(HANDLE hEvent);
	HRESULT __enable(HANDLE hEvent, BOOL bEnable=TRUE);

public:
	void * EnumNextNode();
	void * BeginEnumNode();
	LPUNKNOWN BeginEnum();
	LPUNKNOWN EnumNext();
	bool doNext(int status);

public:
	EventHostObj();
	virtual ~EventHostObj();

private:
	// private type
	typedef struct tagNODE{
		tagNODE * pPrev;
		tagNODE * pNext;
		int nlevel;
		void * peh;
		BOOL bEnable;
	} NODE;

protected:
	NODE * pStandalongHandle;
	NODE * evtMap;

private:
//	NODE ** ppExternalNode;
	NODE * cur;
};

#endif // !defined(AFX_EVENTHOSTOBJ_H__7C5F5FF3_3E99_470C_8256_57F11FE20A38__INCLUDED_)

// TimerObject.h: interface for the TimerObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEROBJECT_H__A3170A38_DE61_41E1_8BFE_E5207A88B860__INCLUDED_)
#define AFX_TIMEROBJECT_H__A3170A38_DE61_41E1_8BFE_E5207A88B860__INCLUDED_

#include "TimerList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eventHostObj.h"
#include "site_i.h"

class TimerList;

class TimerObject : public ITimerObject
{
public:			//IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) {
		if (ppvObject==NULL) return E_INVALIDARG;

		*ppvObject =NULL;
		if (riid==IID_ITimerObject || riid==IID_IUnknown)
			*ppvObject=static_cast<ITimerObject *>(this);
		else return E_NOINTERFACE;

		AddRef();

		return S_OK;
	};
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return (++m_dwRef);
	};
    virtual ULONG STDMETHODCALLTYPE Release( void) {
		--m_dwRef;

		if (m_dwRef==0) {
			delete this;
			return 0;
		}
		return m_dwRef;
	};

private:
	ULONG m_dwRef;

public:					// ITimerObject
	STDMETHOD(attach)(ITimerNotify * pNotify, int nLevel=9) {
		return m_eh.attach((void *)pNotify, nLevel);
	};
	STDMETHOD(attach)(ITimerNotify2 * pNotify2, int nLevel =9) {
		return m_eh2.attach((void *)pNotify2, nLevel);
	}
	STDMETHOD(detach)(HANDLE hEvent) {
		return m_eh.detach(hEvent);
	};

	STDMETHOD(Start)(int nElapse, int nInterval);
	STDMETHOD(Stop)();

public:
	STDMETHOD(OnTimer)();

public:
	HRESULT SetMainLink(TimerList * pTimerList);
	TimerObject();
	virtual ~TimerObject();

	void SetID(DWORD nID) {
		m_nTimerID =nID;
	}

	DWORD GetID() {
		return m_nTimerID;
	}

private:
	TimerList * m_pMainList;

private:
	DWORD m_nTimerID;
	DWORD m_nID;
	int m_Elapse;
	int m_Interval;
	DWORD m_lastTime;
	DWORD m_startTime;
	int m_state;

	EventHostObj m_eh, m_eh2;
};

#endif // !defined(AFX_TIMEROBJECT_H__A3170A38_DE61_41E1_8BFE_E5207A88B860__INCLUDED_)

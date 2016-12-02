// TimerObject.cpp: implementation of the TimerObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimerObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//	Interfaces
//////////////////////////////////////////////////////////////////////


////////////////////// ITimerObject///////////////////////////////////
/*
HRESULT STDMETHODCALLTYPE TimerObject::attach(ITimerNotify * pNotify, int nLevel)
{
	return IEventHostImpl::attach((void *)pNotify, nLevel);
}

HRESULT STDMETHODCALLTYPE TimerObject::detach(HANDLE hEvent)
{
	return IEventHostImpl::detach(hEvent);
}
*/
HRESULT STDMETHODCALLTYPE TimerObject::Start(int nElapse, int nInterval)
{
	if (m_pMainList==NULL) return E_FAIL;

	if (m_state) return E_FAIL;			// the timer is already started

	HRESULT hr=m_pMainList->Start(this, nInterval);
	if (FAILED(hr)) return hr;

	m_nID	=hr;
	m_Elapse	=nElapse;
	m_Interval	=nInterval;

	m_startTime = ::GetTickCount();

	m_state=1;
	
	ITimerNotify * ptn;
	//	Generate the first notify
	ptn=(ITimerNotify *)m_eh.BeginEnum();
	while (ptn && m_eh.doNext(ptn->OnTimePass(m_Elapse, m_Interval)))
		ptn=(ITimerNotify *)m_eh.EnumNext();

	m_lastTime	=m_startTime;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE TimerObject::Stop()
{
	if (m_pMainList==NULL) return E_FAIL;

	if (!m_state) return E_FAIL;		// the timer is not started

	m_state=0;

	return m_pMainList->Stop(m_nID);
}

HRESULT STDMETHODCALLTYPE TimerObject::OnTimer()
{
	if (!m_state) return E_FAIL;		// timer is not started

	DWORD curTime=::GetTickCount();

	if ((curTime-m_lastTime)>(DWORD)m_Interval) {
		int n=(curTime-m_startTime);
		
		ITimerNotify * ptn;
		//	notify the timer is pass
		ptn=(ITimerNotify *)m_eh.BeginEnum();
		while (ptn && m_eh.doNext(ptn->OnTimePass(m_Elapse-n, m_Interval)))
			ptn=(ITimerNotify *)m_eh.EnumNext();

		//	notify timer is out
		if (m_Elapse>0 && n>m_Elapse) {
			m_pMainList->Stop(m_nID);

			ptn=(ITimerNotify *)m_eh.BeginEnum();
			while (ptn && m_eh.doNext(ptn->OnTimeOut()))
				ptn=(ITimerNotify *)m_eh.EnumNext();
		}

		ITimerNotify2 * ptn2;
		//	notify the timer is pass
		ptn2=(ITimerNotify2 *)m_eh2.BeginEnum();
		while (ptn2 && m_eh2.doNext(ptn2->OnTimePass(m_nTimerID, m_Elapse-n, m_Interval)))
			ptn2=(ITimerNotify2 *)m_eh2.EnumNext();

		//	notify timer is out
		if (m_Elapse>0 && n>m_Elapse) {
			m_pMainList->Stop(m_nID);

			ptn2=(ITimerNotify2 *)m_eh2.BeginEnum();
			while (ptn2 && m_eh2.doNext(ptn2->OnTimeOut(m_nTimerID)))
				ptn2=(ITimerNotify2 *)m_eh2.EnumNext();
		}
		
		m_lastTime=curTime;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TimerObject::TimerObject()
{
	m_dwRef =0;
	m_state	=0;
	m_lastTime	=0;
	m_startTime	=0;
	m_Elapse	=0;
	m_Interval	=0;
	m_nID	=0;

	m_nTimerID =0;
}

TimerObject::~TimerObject()
{
	Stop();
}

HRESULT TimerObject::SetMainLink(TimerList *pTimerList)
{
	m_pMainList=pTimerList;

	return S_OK;
}

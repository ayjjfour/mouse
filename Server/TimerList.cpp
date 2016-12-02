// TimerList.cpp: implementation of the TimerList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimerList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TimerList::TimerList()
{
//	m_nNum=0;
	m_factor=0;
	m_pSupport=NULL;
//	for (int i=0; i<20; i++) m_pTimer[i]=NULL;
	ZeroMemory(m_pTimer, sizeof(m_pTimer));
}

TimerList::~TimerList()
{
	Clear();

	SAFE_RELEASE(m_pSupport);
}

HRESULT TimerList::Start(TimerObject * pTimer, int nInterval)
{
	ASSERT(m_pSupport);

	if (pTimer==NULL) return E_INVALIDARG;

	int i = 0;

	for (i=0; i<20; i++) {
		if (m_pTimer[i]==NULL) break;
	}
	if (i<20) {
		m_pTimer[i]=pTimer;
		pTimer->AddRef();
	}
	else return E_OUTOFMEMORY;

	if (m_factor==0) m_factor=nInterval;
	else {
		//求最大公因数
		if (m_factor>nInterval) {
			int mid=nInterval;
			nInterval=m_factor;
			m_factor=mid;
		}
		int remainder=nInterval%m_factor;

		while (remainder!=0) {
			int mid=remainder;
			remainder=m_factor%remainder;
			m_factor=mid;
		}
	}

	m_pSupport->startTimer(m_factor);

	return i;
}

HRESULT TimerList::Stop(int nTimerID)
{
	if (!m_pSupport) return E_FAIL;

	if (nTimerID>=20 || nTimerID<0) return E_INVALIDARG;

	if (m_pTimer[nTimerID]) {
		m_pTimer[nTimerID]->Release();
		m_pTimer[nTimerID]= NULL;

		int i = 0;

		for (i=0; i<20; i++)
			if (m_pTimer[i]!=NULL) break;

		if (i>=20) {
			m_factor=0;
			m_pSupport->stopTimer();
		}
	}

	return S_OK;
}

void TimerList::OnTimer()
{
	for (int i=0; i<20; i++) 
		if (m_pTimer[i]) m_pTimer[i]->OnTimer();
}

void TimerList::Clear()
{
	for (int i=0; i<20; i++) {
		if (m_pTimer[i]) m_pTimer[i]->Release();
		m_pTimer[i] = NULL;
	}
//	m_nNum=0;
	m_pSupport->stopTimer();
}

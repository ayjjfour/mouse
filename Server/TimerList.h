// TimerList.h: interface for the TimerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERLIST_H__3590D559_7510_4FCC_ACC6_2C9E7B56F8E5__INCLUDED_)
#define AFX_TIMERLIST_H__3590D559_7510_4FCC_ACC6_2C9E7B56F8E5__INCLUDED_

#include "site_i.h"
#include "ui.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimerObject.h"

class TimerObject;

class TimerList  
{
public:
	void Clear();
	void OnTimer();
	HRESULT SetTimerSupport(ITimerSupport * pSupport) {
		ASSERT(pSupport);

		SAFE_RELEASE(m_pSupport);

		m_pSupport =pSupport;
		m_pSupport->AddRef();

		return S_OK;
	};
	HRESULT Stop(int nTimerID);
	HRESULT Start(TimerObject * pTimer, int nInterval);
	TimerList();
	virtual ~TimerList();

private:
	ITimerSupport * m_pSupport;
//	int m_nNum;
	int m_factor;
	TimerObject * m_pTimer[20];
};

#endif // !defined(AFX_TIMERLIST_H__3590D559_7510_4FCC_ACC6_2C9E7B56F8E5__INCLUDED_)

// IGameStateEvent.h: interface for the IGameStateEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IGAMESTATEEVENT_H__F5734FCE_EF29_4540_B70C_61CBDB13087E__INCLUDED_)
#define AFX_IGAMESTATEEVENT_H__F5734FCE_EF29_4540_B70C_61CBDB13087E__INCLUDED_

#include "site_i.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	IGameStateEvent
// {F27F17E0-5A14-4400-AC88-78BDE88F0EDE}
DEFINE_GUID(IID_IGameStateEvent, 
0xf27f17e0, 0x5a14, 0x4400, 0xac, 0x88, 0x78, 0xbd, 0xe8, 0x8f, 0xe, 0xde);
MIDL_INTERFACE("{F27F17E0-5A14-4400-AC88-78BDE88F0EDE}")
IGameStateEvent : public IUnknown  
{
public:		//IGameStateEvent
	STDMETHOD(OnLookOnEnableChanged)(IGamePlayer * pPlayer, BOOL bEnable) PURE;
	STDMETHOD(OnStateChanged)(IGamePlayer * pPlayer, int nState) PURE;
};

#endif // !defined(AFX_IGAMESTATEEVENT_H__F5734FCE_EF29_4540_B70C_61CBDB13087E__INCLUDED_)

#ifndef __SUPPORT_INTERFACES_H__
#define __SUPPORT_INTERFACES_H__

#include "IGenetic.h"

//	include the common COM header file
#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID __declspec(selectany) name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

/*
 *	辅助interface定义
 *		created by hhh, 2001.12.5
 */

#define second(m)		(m*1000)
#define minute(m)		(m*60000)

//	IITimerNotify
// {8A16353D-8A07-42aa-9408-50C101980BFA}
DEFINE_GUID(IID_ITimerNotify, 
0x8a16353d, 0x8a07, 0x42aa, 0x94, 0x8, 0x50, 0xc1, 0x1, 0x98, 0xb, 0xfa);
MIDL_INTERFACE("{8A16353D-8A07-42aa-9408-50C101980BFA}")
ITimerNotify : public IUnknown
{
public:		//ITimerNotify
	STDMETHOD(OnTimePass)(int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)() PURE;
};

//	ITimerNotify2
// {5103B569-282D-487d-85C7-7D1FA09E3CC1}
DEFINE_GUID(IID_ITimerNotify2, 
0x5103b569, 0x282d, 0x487d, 0x85, 0xc7, 0x7d, 0x1f, 0xa0, 0x9e, 0x3c, 0xc1);
MIDL_INTERFACE("{5103B569-282D-487d-85C7-7D1FA09E3CC1}")
ITimerNotify2 : public IUnknown
{
public:		//ITimerNotify2
	STDMETHOD(OnTimePass)(int nID, int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)(int nID) PURE;
};

//	ITimerNotifyEx
// {F98FEED5-A9E9-4106-87F7-1C69638FBDBE}
DEFINE_GUID(IID_ITimerNotifyEx, 
0xf98feed5, 0xa9e9, 0x4106, 0x87, 0xf7, 0x1c, 0x69, 0x63, 0x8f, 0xbd, 0xbe);
MIDL_INTERFACE("{F98FEED5-A9E9-4106-87F7-1C69638FBDBE}")
ITimerNotifyEx : public IUnknown
{
public:		//ITimerNotifyEx
	STDMETHOD(OnTimePass)(IUnknown * pUnkwn/*this is the timerObj's IUnknown*/, int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)(IUnknown * pUnkwn/*this is the timerObj's IUnknown*/) PURE;
};

//	ITimerObject
// {E2CF84DF-5DB8-4259-9E7C-FA6D90CA0605}
DEFINE_GUID(IID_ITimerObject, 
0xe2cf84df, 0x5db8, 0x4259, 0x9e, 0x7c, 0xfa, 0x6d, 0x90, 0xca, 0x6, 0x5);
MIDL_INTERFACE("{E2CF84DF-5DB8-4259-9E7C-FA6D90CA0605}")
ITimerObject : public IUnknown
{
public:		//ITimerObject
	STDMETHOD(attach)(ITimerNotify * pNotify, int nlevel=9) PURE;
	STDMETHOD(detach)(HANDLE hEvent) PURE;

	STDMETHOD(Start)(int nElapse, int nInterval) PURE;
	STDMETHOD(Stop)() PURE;
};

/*
 *	this interface is a lite version of IConnectionXXX family.
 *	这个接口不认识所连通的事件，更完整的处理应该用IConnectionXXX
 */
//	IEventHost
// {2999C5C8-F8A2-48b1-9D17-FF1A9E2E76AD}
DEFINE_GUID(IID_IEventHost, 
0x2999c5c8, 0xf8a2, 0x48b1, 0x9d, 0x17, 0xff, 0x1a, 0x9e, 0x2e, 0x76, 0xad);
MIDL_INTERFACE("{2999C5C8-F8A2-48b1-9D17-FF1A9E2E76AD}")
IEventHost : public IUnknown
{
public:			// IEventHost
	STDMETHOD(attach)(void * pNotify, int nlevel =9) PURE;
	STDMETHOD(detach)(void * pNotify) PURE;
	STDMETHOD(enableNotify)(void * pNotify, BOOL bEnabled =TRUE) PURE;
};


#endif	//__SUPPORT_INTERFACES_H__

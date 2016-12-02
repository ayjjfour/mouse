#ifndef __UI_H__
#define __UI_H__

#include "errcode.h"
#include "event_st.h"
#include "utility.h"
#include "site_i.h"

interface IViewContainer;
interface IActiveWindow2;
interface IActiveObject2;

// Button values for mouse events
#ifndef MK_LBUTTON 
#define MK_LBUTTON          0x0001
#endif
#ifndef MK_RBUTTON
#define MK_RBUTTON          0x0002
#endif
#ifndef MK_SHIFT
#define MK_SHIFT            0x0004
#endif
#ifndef MK_CONTROL
#define MK_CONTROL          0x0008
#endif
#ifndef MK_MBUTTON
#define MK_MBUTTON          0x0010
#endif

#define LEFT_BUTTON		MK_LBUTTON
#define RIGHT_BUTTON	MK_RBUTTON
#define DBLCLICK		0x0100
#define BTN_DOWN		0x0200

//	ICWnd
// {2271A619-F9AA-4cb0-916A-E1645ABEC24F}
DEFINE_GUID(IID_ICWnd, 
0x2271a619, 0xf9aa, 0x4cb0, 0x91, 0x6a, 0xe1, 0x64, 0x5a, 0xbe, 0xc2, 0x4f);
//	this interface is reference to the vtable of CWnd


//	IActiveWindow2
// {134C3C4A-F18E-4c39-B076-50D00344B42B}
DEFINE_GUID(IID_IActiveWindow2, 
0x134c3c4a, 0xf18e, 0x4c39, 0xb0, 0x76, 0x50, 0xd0, 0x3, 0x44, 0xb4, 0x2b);
MIDL_INTERFACE("{134C3C4A-F18E-4c39-B076-50D00344B42B}")
IActiveWindow2 : public IUnknown
{
public:			// IActiveWindow2
	STDMETHOD(GetWindow)(/*out*/HWND &hWnd) PURE;

	STDMETHOD(GetCapture)(IActiveObject2 * pObj) PURE;

	STDMETHOD(SetCapture)(/* [in] */ IActiveObject2 * pObj, /* [in] */ BOOL fCapture =TRUE) PURE;

	STDMETHOD(GetFocus)(IActiveObject2 * pObj) PURE;

	STDMETHOD(SetFocus)( IActiveObject2 * pObj,
		/* [in] */ BOOL fFocus) PURE;

	STDMETHOD(GetDC)(/* [out] */ HDC & hdc) PURE;

	STDMETHOD(ReleaseDC)(/* [in] */ HDC hDC) PURE;

	STDMETHOD(InvalidateRect)(
		/* [in] */ LPCRECT pRect,
		/* [in] */ BOOL fErase =TRUE) PURE;

	STDMETHOD(InvalidateRgn)(
		/* [in] */ HRGN hRGN,
		/* [in] */ BOOL fErase =TRUE) PURE;

	STDMETHOD(DirectlyDraw)(RECT &rc) PURE;

	STDMETHOD(CreateTimer)(ITimerObject ** ppTimer) PURE;							// 创建定时器
};
//	this interface define a container function

//	IElementDocument2
// {4808A3D0-668A-4e24-A260-B8AEC6DD9EEC}
DEFINE_GUID(IID_IElementDocument2, 
0x4808a3d0, 0x668a, 0x4e24, 0xa2, 0x60, 0xb8, 0xae, 0xc6, 0xdd, 0x9e, 0xec);
MIDL_INTERFACE("{4808A3D0-668A-4e24-A260-B8AEC6DD9EEC}")
IElementDocument2 : public IUnknown
{
public:			// IElementDocument2
	STDMETHOD(AddElement)(IActiveObject2 * pObj, char * szID=NULL, int nlevel=9, RECT * pRC=NULL) PURE;
	STDMETHOD(GetElement)(char * szID, IActiveObject2 ** ppObj) PURE;
	STDMETHOD(GetElement)(HANDLE hEle, IActiveObject2 ** ppObj) PURE;
	STDMETHOD(EnableElement)(char * szID, BOOL bEnable=TRUE) PURE;
	STDMETHOD(EnableElement)(HANDLE hEle, BOOL bEnable=TRUE) PURE;
	STDMETHOD(isEnabled)(HANDLE hEle) PURE;
	STDMETHOD(isEnabled)(char * szID) PURE;
	STDMETHOD(DelElement)(char *szID) PURE;
	STDMETHOD(DelElement)(HANDLE hEle) PURE;

	STDMETHOD(BeginEnumEle)() PURE;
	STDMETHOD(EnumNextEle)(IActiveObject2 ** pObj) PURE;
};


//	IActiveObject2
// {7C630DBE-AAF2-442b-AF37-FD393C4647E1}
DEFINE_GUID(IID_IActiveObject2, 
0x7c630dbe, 0xaaf2, 0x442b, 0xaf, 0x37, 0xfd, 0x39, 0x3c, 0x46, 0x47, 0xe1);
MIDL_INTERFACE("{7C630DBE-AAF2-442b-AF37-FD393C4647E1}")
IActiveObject2 : public IUnknown
{
public:
	enum {
		UI_MOUSE =1,
		UI_KEYBRD =2,
		UI_OTHER =4,
	};

public:			// IActiveObject2
	STDMETHOD(GetOperative)(UINT &ability) PURE;
	STDMETHOD(SetContainerWnd)(IActiveWindow2 * pActWnd) PURE;
	STDMETHOD(SetPlace)(/*in*/RECT &rc, /*out, adjusted*/RECT * pOut=NULL) PURE;
	STDMETHOD(GetPlace)(RECT &rc) PURE;
	STDMETHOD(OnActivate)(BOOL bActivate=TRUE) PURE;
	STDMETHOD(Draw)(HDC hdc, RECT &ur) PURE;
	STDMETHOD(OnWindowMessage)(
	  UINT msg,           //Message Identifier as provided by Windows
	  WPARAM wParam,      //Message parameter as provided by Windows
	  LPARAM lParam,      //Message parameter as provided by Windows
	  LRESULT* plResult   //Pointer to message result code
	) PURE;
};

//	IAnimation
// {0333D000-0DA1-4a2b-A6C4-33C2F246EC24}
DEFINE_GUID(IID_IAnimation, 
0x333d000, 0xda1, 0x4a2b, 0xa6, 0xc4, 0x33, 0xc2, 0xf2, 0x46, 0xec, 0x24);
MIDL_INTERFACE("{0333D000-0DA1-4a2b-A6C4-33C2F246EC24}")
IAnimation : public IUnknown
{
public:		// IAnimation
	STDMETHOD(UpdateFrame)(unsigned int elapse) PURE;
	STDMETHOD(SetCoord)(CRect &coPlace, CRect * pOut=NULL) PURE;
	STDMETHOD(GetCoord)(CRect &coPlace) PURE;
};

//	IAniMgr
// {BC847118-AB94-4530-9A20-64EA76C24ABE}
DEFINE_GUID(IID_IAniMgr, 
0xbc847118, 0xab94, 0x4530, 0x9a, 0x20, 0x64, 0xea, 0x76, 0xc2, 0x4a, 0xbe);
MIDL_INTERFACE("{BC847118-AB94-4530-9A20-64EA76C24ABE}")
IAniMgr : public IUnknown
{
public:			//IAniMgr
	STDMETHOD(setViewportOrg)(int x, int y) PURE;
};

/*
 *	the following interface should be implemented by programmer 
 *	that will support drag & drop with ActiveWindow existed
 */
//	IDragObject
// {A2BEFB56-2E3D-4144-9534-24B0A8DDAB93}
DEFINE_GUID(IID_IDragObject, 
0xa2befb56, 0x2e3d, 0x4144, 0x95, 0x34, 0x24, 0xb0, 0xa8, 0xdd, 0xab, 0x93);
MIDL_INTERFACE("{A2BEFB56-2E3D-4144-9534-24B0A8DDAB93}")
IDragObject : public IActiveObject2
{
public:			//IDragObject
	STDMETHOD(GiveFeedBack)(DWORD dwEffect) PURE;
	STDMETHOD(QueryContinueDrag)(
	  BOOL fEscapePressed,  //Status of escape key since previous call
	  DWORD grfKeyState     //Current state of keyboard modifier keys
	) PURE;
};

//	IDragable
// {355D5C0F-9C5C-4ea4-9137-B5C157EE10AD}
DEFINE_GUID(IID_IDragable, 
0x355d5c0f, 0x9c5c, 0x4ea4, 0x91, 0x37, 0xb5, 0xc1, 0x57, 0xee, 0x10, 0xad);
MIDL_INTERFACE("{355D5C0F-9C5C-4ea4-9137-B5C157EE10AD}")
IDragable : public IUnknown
{
public:		// IDragable
	STDMETHOD(BeginDrag)(IDragObject ** ppObj, CPoint point) PURE;
};

//	IDropable
// {4C960DE9-A8AE-4f68-8862-C0D2D8B204C1}
DEFINE_GUID(IID_IDropable, 
0x4c960de9, 0xa8ae, 0x4f68, 0x88, 0x62, 0xc0, 0xd2, 0xd8, 0xb2, 0x4, 0xc1);
MIDL_INTERFACE("{4C960DE9-A8AE-4f68-8862-C0D2D8B204C1}")
IDropable : public IUnknown
{
	STDMETHOD(DragEnter)(
	  IDragObject * pObject,
						 //Pointer to the interface of the source data 
						 // object
	  DWORD grfKeyState, //Current state of keyboard modifier keys
	  POINT pt,         //Current cursor coordinates
	  DWORD * pdwEffect  //Pointer to the effect of the drag-and-drop 
						 // operation
	) PURE;

	STDMETHOD(DragLeave)(void) PURE;

	STDMETHOD(DragOver)(
	  DWORD grfKeyState, //Current state of keyboard modifier keys
	  POINT pt,         //Current cursor coordinates
	  DWORD * pdwEffect  //Pointer to the effect of the drag-and-drop 
						 // operation
	) PURE;

	STDMETHOD(Drop)(
	  IDragObject * pObject,
						 //Pointer to the interface for the source data
	  DWORD grfKeyState, //Current state of keyboard modifier keys
	  POINT pt,         //Current cursor coordinates
	  DWORD * pdwEffect  //Pointer to the effect of the drag-and-drop 
						 // operation
	) PURE;
};

//	IDataExchange
// {A06FC2F5-645F-4d9d-A50D-288F4B1098DA}
DEFINE_GUID(IID_IDataExchange, 
0xa06fc2f5, 0x645f, 0x4d9d, 0xa5, 0xd, 0x28, 0x8f, 0x4b, 0x10, 0x98, 0xda);
MIDL_INTERFACE("{A06FC2F5-645F-4d9d-A50D-288F4B1098DA}")
IDataExchange : public IUnknown
{
	STDMETHOD(GetData)(void ** ppData) PURE;
	STDMETHOD(Accept)() PURE;
	STDMETHOD(Reject)() PURE;
};

//	IDataEvent
// {6AA79211-2ED4-4e41-810C-1E8FBE26F39E}
DEFINE_GUID(IID_IDataEvent, 
0x6aa79211, 0x2ed4, 0x4e41, 0x81, 0xc, 0x1e, 0x8f, 0xbe, 0x26, 0xf3, 0x9e);
MIDL_INTERFACE("{6AA79211-2ED4-4e41-810C-1E8FBE26F39E}")
IDataEvent : public IUnknown
{
	STDMETHOD(OnDataAccepted)(void * pObj) PURE;
	STDMETHOD(OnDataRejected)(void * pObj) PURE;
};
//	this event is used to notify dragable source

//	IToolTip
// {BD39DCFB-AEE8-4f81-9F9D-D5D8CD2F5099}
DEFINE_GUID(IID_IToolTip, 
0xbd39dcfb, 0xaee8, 0x4f81, 0x9f, 0x9d, 0xd5, 0xd8, 0xcd, 0x2f, 0x50, 0x99);
MIDL_INTERFACE("{BD39DCFB-AEE8-4f81-9F9D-D5D8CD2F5099}")
IToolTip : public IUnknown
{
public:				// IToolTip
	STDMETHOD(GetTip)(char * lpszStr) PURE;
};

//	IViewCtrl
// {A99928CF-4B8F-4b5f-AE4D-12FED3D3B7ED}
DEFINE_GUID(IID_IViewCtrl, 
0xa99928cf, 0x4b8f, 0x4b5f, 0xae, 0x4d, 0x12, 0xfe, 0xd3, 0xd3, 0xb7, 0xed);
MIDL_INTERFACE("{A99928CF-4B8F-4b5f-AE4D-12FED3D3B7ED}")
IViewCtrl : public IUnknown
{
	STDMETHOD(setContainer)(IViewContainer * pContainer) PURE;

	STDMETHOD(OnActive)() PURE;				// called by frame when this view will entering active, 
											// return failed will cause this active canceled
	STDMETHOD(OnDeactive)() PURE;			// notify the view it will be deactivated

	STDMETHOD(getWnd)(HWND &hWnd) PURE;		// get the window's handle, failed if the window is not existed
	STDMETHOD(create)(HWND hParentWnd) PURE;	// create the view
};

//	IViewCtrlEx
// {CAF21658-CB34-47e0-90E6-8C7CD45D45BC}
DEFINE_GUID(IID_IViewCtrlEx, 
0xcaf21658, 0xcb34, 0x47e0, 0x90, 0xe6, 0x8c, 0x7c, 0xd4, 0x5d, 0x45, 0xbc);
MIDL_INTERFACE("{CAF21658-CB34-47e0-90E6-8C7CD45D45BC}")
IViewCtrlEx : public IViewCtrl
{
public:			//IViewCtrlEx
	STDMETHOD(OnClose)() PURE;
};

//	IEnumView
// {447D9354-1C1E-427d-B246-361CF5F436C0}
DEFINE_GUID(IID_IEnumView, 
0x447d9354, 0x1c1e, 0x427d, 0xb2, 0x46, 0x36, 0x1c, 0xf5, 0xf4, 0x36, 0xc0);
MIDL_INTERFACE("{447D9354-1C1E-427d-B246-361CF5F436C0}")
IEnumView : public IUnknown
{
	STDMETHOD(Next)(ULONG celt, IViewCtrl **ppElements, ULONG *pceltFetched) PURE;
	STDMETHOD(Reset)(void) PURE;
	STDMETHOD(Skip)(ULONG celt) PURE;
	STDMETHOD(Clone)(IEnumView **ppEnum) PURE;
};

//	IViewContainer
// {B371CBC5-FE14-4e71-800A-6252070AD05D}
DEFINE_GUID(IID_IViewContainer, 
0xb371cbc5, 0xfe14, 0x4e71, 0x80, 0xa, 0x62, 0x52, 0x7, 0xa, 0xd0, 0x5d);
MIDL_INTERFACE("{B371CBC5-FE14-4e71-800A-6252070AD05D}")
IViewContainer : public IUnknown
{
	STDMETHOD(add)(IViewCtrl * pView, char * lpszName =NULL) PURE;
	STDMETHOD(remove)(IViewCtrl * pView) PURE;
	
	STDMETHOD(active)(IViewCtrl * pView) PURE;
	STDMETHOD(active)(char * pName) PURE;
	STDMETHOD(deactive)(IViewCtrl * pView) PURE;

	STDMETHOD(Quit)(int sCode) PURE;

	STDMETHOD(GetWindow)(HWND &hWNd) PURE; 
};


//	IButtonEvent2
// {96AC5148-5A03-419c-98C4-D67861A86B75}
DEFINE_GUID(IID_IButtonEvent2, 
0x96ac5148, 0x5a03, 0x419c, 0x98, 0xc4, 0xd6, 0x78, 0x61, 0xa8, 0x6b, 0x75);
MIDL_INTERFACE("{96AC5148-5A03-419c-98C4-D67861A86B75}")
IButtonEvent2 : public IUnknown
{
public:			// IButtonEvent2
	STDMETHOD(OnPress)(int nID) PURE;
	STDMETHOD(OnMouseOut)(int nID) PURE;
	STDMETHOD(OnMouseIn)(int nID) PURE;
	STDMETHOD(OnMouseOver)(int nID) PURE;
	STDMETHOD(OnClick)(int nID, unsigned int uflag) PURE;
};

//	IScrollEvent
// {BB8A40A3-A6F5-4761-9F0C-FB3DE63F8F4A}
DEFINE_GUID(IID_IScrollEvent, 
0xbb8a40a3, 0xa6f5, 0x4761, 0x9f, 0xc, 0xfb, 0x3d, 0xe6, 0x3f, 0x8f, 0x4a);
MIDL_INTERFACE("{BB8A40A3-A6F5-4761-9F0C-FB3DE63F8F4A}")
IScrollEvent : public IUnknown
{
public:			// IScrollEvent
	STDMETHOD(OnLineUp)(int orient) PURE;
	STDMETHOD(OnLineDn)(int orient) PURE;
	STDMETHOD(OnPageUp)(int orient) PURE;
	STDMETHOD(OnPageDn)(int orient) PURE;
	STDMETHOD(OnScroll)(UINT nPos, int orient) PURE;
};

//	IAnimateButtonEvent
// {0C8A9958-9B6B-4fe9-BD57-4F04A3A9B58B}
DEFINE_GUID(IID_IAnimateButtonEvent, 
0xc8a9958, 0x9b6b, 0x4fe9, 0xbd, 0x57, 0x4f, 0x4, 0xa3, 0xa9, 0xb5, 0x8b);
MIDL_INTERFACE("{0C8A9958-9B6B-4fe9-BD57-4F04A3A9B58B}")
IAnimateButtonEvent : public IButtonEvent2
{
public:			//IAnimateButtonEvent
	STDMETHOD(onAnimationReverse)(void * pObj) PURE;
};

//	ITimerSupport
// {093700C1-5F57-4364-A894-5F93BF9702DF}
DEFINE_GUID(IID_ITimerSupport, 
0x93700c1, 0x5f57, 0x4364, 0xa8, 0x94, 0x5f, 0x93, 0xbf, 0x97, 0x2, 0xdf);
MIDL_INTERFACE("{093700C1-5F57-4364-A894-5F93BF9702DF}")
ITimerSupport : public IUnknown
{
public:		//ITimerSupport
	STDMETHOD(startTimer)(int interval) PURE;
	STDMETHOD(stopTimer)() PURE;
};

#endif //__UI_H__

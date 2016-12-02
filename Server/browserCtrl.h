#ifndef __BROWSERCTRL_H__
#define __BROWSERCTRL_H__

interface __declspec(novtable) IMinBrowser {
	virtual HWND getWnd() PURE;
	STDMETHOD(Navigate)(char * szURL) PURE;
	STDMETHOD_(DWORD, Delete)() PURE;
	STDMETHOD(SetBgColor)(COLORREF clr) PURE;
	STDMETHOD(SetBackground)(char * szURL) PURE;
};

interface __declspec(novtable) IHTMLChat : public IMinBrowser {
	STDMETHOD(AutoScroll)(BOOL bAuto =TRUE) PURE;
	STDMETHOD(Clear)() PURE;
	STDMETHOD(addString)(const char * lpStr) PURE;
};


#endif	//__BROWSERCTRL_H__

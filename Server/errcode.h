#ifndef _GLOBALERRCODE
#define _GLOBALERRCODE

#include <afxwin.h>

//standard status define
const int S_FAIL=E_FAIL;
//const int S_OK=0;		already defined at Winerror.h
//error level
const DWORD S_GE=0xC0000000;			//一般性错误
const DWORD S_FE=0xA0000000;			//致命错误，当前操作无法完成
const DWORD S_CE=0x90000000;			//临界错误，程序即将崩溃

//error mask
#define S_M_GE	0x40000000
#define S_M_FE	0x20000000
#define S_M_CE	0x10000000

//error macro
#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#endif

#ifndef FAILED
#define FAILED(Status) ((HRESULT)(Status)<0)
#endif

#ifndef IsGE
#define IsGE(Status) ((DWORD)(Status&0xF0000000)==S_GE)
#define IsFE(Status) ((DWORD)(Status&0xF0000000)==S_FE)
#define IsCE(Status) ((DWORD)(Status&0xF0000000)==S_CE)
#endif

#endif	//_GLOBALERRCODE

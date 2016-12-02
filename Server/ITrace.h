/******************************************************************
** 文件名:	ITrace.h
** 版  权:	(c) 2002.3-2002.4 深圳网域开发部
** 创建人:	Peak Gao
** 日  期:	2002.3
** 版  本:	0.2		(0.1版是刘和国先前的那个,这里保持与0.1版兼容)
** 描  述:	ITrace接口定义
//123
********************** 注意事项 ***********************

1、每个独立的程序个体(如exe,dll)都应该定义全局变量CITraceHelper g_XXXXXTraceHelper;
   以便于在程序内部各处可以使用,请确保你的变量g_XXXXXTraceHelper与其他人的不同,以防
   止在用静态连接时出现重复定义的错误,再定义#define g_XXXXXTraceHelper g_TraceHelper
   就可以使用Trace自定义的宏了,这些宏主要包括信息输出宏(Trace,DebugMsg,...)和用于诊断
   、校验方面的宏(Assert,Verify,Test,AssertR,VerifyR)
   
2、怎样使用ITrace接口?用户可以照抄下面的内容,稍作修改即可完全使用:
   注:请将下面的部分放在你的所有其他的#include "ITrace.h"语句前面,否则宏定义失效
//**************************************************************************************
// 支持Trace的环境设置
#ifdef _DEBUG
	#ifndef _TRACE_LEVEL_3					// 此级别能显示所有的调试信息,如果
	#define _TRACE_LEVEL_3					// 给他人用,可设为_TRACE_LEVEL_1
	#endif
#else
	#ifndef _TRACE_LEVEL_0
	#define _TRACE_LEVEL_0
	#endif
#endif
#define MY_TRACE_COLOR		RGB(146,0,123)	// 定义你自己喜欢的Trace错误的颜色
#include "ITrace.h"							// 改成你的文件相应路径
//#ifdef _DEBUG
//#pragma comment(lib,"TraceD.lib")			// 如果用lib方式，取消此行注释
//#else
//#pragma comment(lib,"Trace.lib")			// 如果用lib方式，取消此行注释
//#endif
extern CITraceHelper		g_XXXXXTraceHelper;
#define g_TraceHelper		g_XXXXXTraceHelper		
// 下面一行放到你的全局变量定义处,为防止lib连接时的名字冲突,"XXXXX"最好改成你的dll或exe的名字
//CITraceHelper				g_XXXXXTraceHelper;			
//**************************************************************************************

3、如果你调用的是ITrace::ThreadSafeShowTrace2()的话(本接口的所有宏定义均采用这个接
   口方法)，记得在你的主应用程序窗口过程(MainWndProc)中加入下面这条消息处理，它会取
   出消息并显示，当然你的模块没有建立窗口的话就没必要了

	case WM_THREAD_TRACE: // 接收到线程安全消息
		g_XXXXXTraceHelper.ThreadSafeGetTraceMsg(); 
		// 或用宏(如果定义了的话)GetTraceMsg();
		break;

4、你的模块必须在WinMain()中嵌入下面几行代码,以便构成消息循环:

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	否则Trace窗口一闪即过,当然,如果没设置OS_WINDOWD的话，那就另当别论

5、一定要读懂CITraceHelper::FirstCreate(...)中的参数意义，有些参数必须配合才能有效,
   FirstCreate(...)一般只在主执行文件中创建一次；在DLL中用Create(ITrace*)就可以了,
   其中的ITrace*可通过g_XXXXXTraceHelper.GetTraceObject()获得;对于用lib方式连接的用
   户,则相应将FirstCreate(...)换成libFirstCreate(...)即可。

**************************** 修改记录 ******************************
** 修改人: 
** 日  期:
** 描  述: 
********************************************************************/
#ifndef __ITRACE_H__
#define __ITRACE_H__

#include <richedit.h>
#include <stdio.h>

// Trace当前版本号
#define VERSION_QUERY_TRACE			6

// 一些常量定义
#ifdef _DEBUG
#define MAX_RICHEDIT_MESSAGE_LEN	(60 * 1024)		// RichEdit中最大容纳长度
#else
#define MAX_RICHEDIT_MESSAGE_LEN	(32 * 1024)
#endif
#define MAX_TRACE_MSG_LEN			2048			// 每次Trace的最大长度
#define MAX_THREAD_MESSAGE			400				// 最大线程消息容纳数
#define WM_THREAD_TRACE_EX			(WM_USER+0x127)	// 线程安全Trace消息

// 输出类型(Output Style)定义
#define OS_OUTPUTDEBUGSTRING		0x00000001L		// OutputDebugString输出
#define OS_FILE						0x00000002L		// 输出到文件
#define OS_RICHEDIT					0x00000004L		// 输出到RichEdit
#define OS_WINDOW					0x00000008L		// 输出到窗口

// 系统Trace的颜色
#define TC_DEFAULT					RGB(0,0,255)	// 默认颜色
#define TC_WARNING					RGB(106,79,154)	// 警告颜色
#define TC_ERROR					RGB(255,0,0)	// 错误颜色
#define TC_SYSTEMMESSAGE			RGB(155,65,14)	// 系统消息颜色

// 系统Trace的字体大小
#define TH_DEFAULT					9				// 默认9号字
#define TH_SYSTEMMESSAGE			12				// 系统消息12号字

// 系统Trace字体样式
#define TS_DEFAULT					0				// 默认样式
#define TS_SYSTEMMESSAGE			TS_BOLD			// 系统消息为粗体
// *********** RichEdit *************
#define TS_BOLD						0x00000001
#define TS_ITALIC					0x00000002
#define TS_UNDERLINE				0x00000004
#define TS_STRIKEOUT				0x00000008
#define TS_PROTECTED				0x00000010
#define TS_LINK						0x00000020		/* Exchange hyperlink extension */
#define TS_SIZE						0x80000000
#define TS_COLOR					0x40000000
#define TS_FACE						0x20000000
#define TS_OFFSET					0x10000000
#define TS_CHARSET					0x08000000


// 接口定义
struct ITrace
{
	//********************************  0.1版  ***********************************
	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ShowTrace(LPCSTR szMsg, COLORREF color = 0,
		int nTextHeight = 9,DWORD dwStyle = 0) = NULL;

	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ThreadSafeShowTrace(LPCSTR szMsg, COLORREF color = 0,
		int nTextHeight = 9,DWORD dwStyle = 0) = NULL;
	//********************************  0.1版  ***********************************




	//********************************  0.2版  ***********************************
	// 直接调试信息显示
	// 说明: 不带换行回车符("\r\n"), dwStyle可选值: TS_BOLD,TS_ITALIC ... 见上面的宏定义
	virtual BOOL ShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn =	FALSE, BOOL bShowTime = FALSE) = 0;
	// 线程安全调试信息显示(通过消息机制实现)
	// 说明同上
	virtual BOOL ThreadSafeShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn = FALSE, BOOL bShowTime = FALSE) = 0;
		// 线程安全获取消息队列中的消息并显示出来
	virtual BOOL ThreadSafeGetTraceMsg() = 0;
	// 获取内部创建的窗口句柄
	virtual HWND GetTraceWindow() = 0;
	// 释放(包括自带窗口的销毁)
	virtual BOOL Release() = 0;
	//********************************  0.2版  ***********************************

};

#endif // __ITRACE_H__

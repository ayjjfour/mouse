/******************************************************************
** �ļ���:	ITrace.h
** ��  Ȩ:	(c) 2002.3-2002.4 �������򿪷���
** ������:	Peak Gao
** ��  ��:	2002.3
** ��  ��:	0.2		(0.1�������͹���ǰ���Ǹ�,���ﱣ����0.1�����)
** ��  ��:	ITrace�ӿڶ���
//123
********************** ע������ ***********************

1��ÿ�������ĳ������(��exe,dll)��Ӧ�ö���ȫ�ֱ���CITraceHelper g_XXXXXTraceHelper;
   �Ա����ڳ����ڲ���������ʹ��,��ȷ����ı���g_XXXXXTraceHelper�������˵Ĳ�ͬ,�Է�
   ֹ���þ�̬����ʱ�����ظ�����Ĵ���,�ٶ���#define g_XXXXXTraceHelper g_TraceHelper
   �Ϳ���ʹ��Trace�Զ���ĺ���,��Щ����Ҫ������Ϣ�����(Trace,DebugMsg,...)���������
   ��У�鷽��ĺ�(Assert,Verify,Test,AssertR,VerifyR)
   
2������ʹ��ITrace�ӿ�?�û������ճ����������,�����޸ļ�����ȫʹ��:
   ע:�뽫����Ĳ��ַ����������������#include "ITrace.h"���ǰ��,����궨��ʧЧ
//**************************************************************************************
// ֧��Trace�Ļ�������
#ifdef _DEBUG
	#ifndef _TRACE_LEVEL_3					// �˼�������ʾ���еĵ�����Ϣ,���
	#define _TRACE_LEVEL_3					// ��������,����Ϊ_TRACE_LEVEL_1
	#endif
#else
	#ifndef _TRACE_LEVEL_0
	#define _TRACE_LEVEL_0
	#endif
#endif
#define MY_TRACE_COLOR		RGB(146,0,123)	// �������Լ�ϲ����Trace�������ɫ
#include "ITrace.h"							// �ĳ�����ļ���Ӧ·��
//#ifdef _DEBUG
//#pragma comment(lib,"TraceD.lib")			// �����lib��ʽ��ȡ������ע��
//#else
//#pragma comment(lib,"Trace.lib")			// �����lib��ʽ��ȡ������ע��
//#endif
extern CITraceHelper		g_XXXXXTraceHelper;
#define g_TraceHelper		g_XXXXXTraceHelper		
// ����һ�зŵ����ȫ�ֱ������崦,Ϊ��ֹlib����ʱ�����ֳ�ͻ,"XXXXX"��øĳ����dll��exe������
//CITraceHelper				g_XXXXXTraceHelper;			
//**************************************************************************************

3���������õ���ITrace::ThreadSafeShowTrace2()�Ļ�(���ӿڵ����к궨������������
   �ڷ���)���ǵ��������Ӧ�ó��򴰿ڹ���(MainWndProc)�м�������������Ϣ��������ȡ
   ����Ϣ����ʾ����Ȼ���ģ��û�н������ڵĻ���û��Ҫ��

	case WM_THREAD_TRACE: // ���յ��̰߳�ȫ��Ϣ
		g_XXXXXTraceHelper.ThreadSafeGetTraceMsg(); 
		// ���ú�(��������˵Ļ�)GetTraceMsg();
		break;

4�����ģ�������WinMain()��Ƕ�����漸�д���,�Ա㹹����Ϣѭ��:

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	����Trace����һ������,��Ȼ,���û����OS_WINDOWD�Ļ����Ǿ�������

5��һ��Ҫ����CITraceHelper::FirstCreate(...)�еĲ������壬��Щ����������ϲ�����Ч,
   FirstCreate(...)һ��ֻ����ִ���ļ��д���һ�Σ���DLL����Create(ITrace*)�Ϳ�����,
   ���е�ITrace*��ͨ��g_XXXXXTraceHelper.GetTraceObject()���;������lib��ʽ���ӵ���
   ��,����Ӧ��FirstCreate(...)����libFirstCreate(...)���ɡ�

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��:
** ��  ��: 
********************************************************************/
#ifndef __ITRACE_H__
#define __ITRACE_H__

#include <richedit.h>
#include <stdio.h>

// Trace��ǰ�汾��
#define VERSION_QUERY_TRACE			6

// һЩ��������
#ifdef _DEBUG
#define MAX_RICHEDIT_MESSAGE_LEN	(60 * 1024)		// RichEdit��������ɳ���
#else
#define MAX_RICHEDIT_MESSAGE_LEN	(32 * 1024)
#endif
#define MAX_TRACE_MSG_LEN			2048			// ÿ��Trace����󳤶�
#define MAX_THREAD_MESSAGE			400				// ����߳���Ϣ������
#define WM_THREAD_TRACE_EX			(WM_USER+0x127)	// �̰߳�ȫTrace��Ϣ

// �������(Output Style)����
#define OS_OUTPUTDEBUGSTRING		0x00000001L		// OutputDebugString���
#define OS_FILE						0x00000002L		// ������ļ�
#define OS_RICHEDIT					0x00000004L		// �����RichEdit
#define OS_WINDOW					0x00000008L		// ���������

// ϵͳTrace����ɫ
#define TC_DEFAULT					RGB(0,0,255)	// Ĭ����ɫ
#define TC_WARNING					RGB(106,79,154)	// ������ɫ
#define TC_ERROR					RGB(255,0,0)	// ������ɫ
#define TC_SYSTEMMESSAGE			RGB(155,65,14)	// ϵͳ��Ϣ��ɫ

// ϵͳTrace�������С
#define TH_DEFAULT					9				// Ĭ��9����
#define TH_SYSTEMMESSAGE			12				// ϵͳ��Ϣ12����

// ϵͳTrace������ʽ
#define TS_DEFAULT					0				// Ĭ����ʽ
#define TS_SYSTEMMESSAGE			TS_BOLD			// ϵͳ��ϢΪ����
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


// �ӿڶ���
struct ITrace
{
	//********************************  0.1��  ***********************************
	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ShowTrace(LPCSTR szMsg, COLORREF color = 0,
		int nTextHeight = 9,DWORD dwStyle = 0) = NULL;

	// show a string without '\r\n'
	// dwStyle support : CFM_BOLD,CFM_ITALIC,CFM_STRIKEOUT,CFM_UNDERLINE,CFM_LINK,CFM_SHADOW
	virtual BOOL ThreadSafeShowTrace(LPCSTR szMsg, COLORREF color = 0,
		int nTextHeight = 9,DWORD dwStyle = 0) = NULL;
	//********************************  0.1��  ***********************************




	//********************************  0.2��  ***********************************
	// ֱ�ӵ�����Ϣ��ʾ
	// ˵��: �������лس���("\r\n"), dwStyle��ѡֵ: TS_BOLD,TS_ITALIC ... ������ĺ궨��
	virtual BOOL ShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn =	FALSE, BOOL bShowTime = FALSE) = 0;
	// �̰߳�ȫ������Ϣ��ʾ(ͨ����Ϣ����ʵ��)
	// ˵��ͬ��
	virtual BOOL ThreadSafeShowTrace2(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn = FALSE, BOOL bShowTime = FALSE) = 0;
		// �̰߳�ȫ��ȡ��Ϣ�����е���Ϣ����ʾ����
	virtual BOOL ThreadSafeGetTraceMsg() = 0;
	// ��ȡ�ڲ������Ĵ��ھ��
	virtual HWND GetTraceWindow() = 0;
	// �ͷ�(�����Դ����ڵ�����)
	virtual BOOL Release() = 0;
	//********************************  0.2��  ***********************************

};

#endif // __ITRACE_H__

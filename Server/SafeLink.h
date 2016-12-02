// SafeLink.h: interface for the CSafeLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFELINK_H__F9FEEDC0_2A49_11D4_A043_0080AD315721__INCLUDED_)
#define AFX_SAFELINK_H__F9FEEDC0_2A49_11D4_A043_0080AD315721__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "game.h"
class CSafeLink  
{
public:
	BOOL ConnectServer(LPCSTR szServerName,HWND hMainWnd);
	/*
	 * 功能：连接服务器
	 * 输入： 
	 *		lpServerName : 服务器名，大厅通过程序命令行传入游戏，可通过AfxGetApp()->m_lpCmdLine得到
	 *		hMainWnd	 : 游戏主窗口句柄，一般通过m_hWnd得到
	 * 输出：
	 *		成功返回TRUE，否则返回FALSE
	 */

	int SendCmd(BYTE cCmdID, LPCSTR buf, int nLen);
	/*
	 * 功能：发送命令
	 * 输入： 
	 *		cCmdID : 命令码
	 *		buf 和 nLen :此命令所带的数据和长度，没有数据时可为NULL和0，nLen最大为MAX_SEND_LEN - 1
	 * 输出：
	 *		成功返回RET_OK，否则为出错
	 */

	int RecvCmd(BYTE& cCmdID, char *buf, int& nLen);
	/*
	 * 功能：接收命令
	 * 输入： 
	 *		 : 无
	 * 输出：
	 *		成功返回RET_OK，cCmdID、buf 和 nLen 为此命令所带的数据和长度，nLen最大为MAX_RECV_LEN - 1
	 *		没有数据包时返回RET_EMPTY，返回其它值为出错
	 */

	void CloseConnect();
	/*
	 * 功能：关闭连接
	 * 说明：当连接断开或退出程序时，应关闭连接并退出程序
	 */

	BOOL IsConnectOK();
	/*
	 * 功能：测试连接是否连通
	 */

	CSafeLink();
	virtual ~CSafeLink();

private:
	CGameLink	m_link;
	WORD		m_wSendKey;
	WORD		m_wRecvKey;
	WORD		SeedMap(WORD wSeed);
};

#ifdef _CLIENT
#pragma comment(lib, "cmnfunc.lib")
#endif

#endif // !defined(AFX_SAFELINK_H__F9FEEDC0_2A49_11D4_A043_0080AD315721__INCLUDED_)

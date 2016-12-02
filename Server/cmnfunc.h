// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GAMEDEV_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GAMEDEV_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifndef _GAME_DEV_H
#define _GAME_DEV_H

#include "common.h"

/*
 * 基本开发支持
 *		1、IPC通讯
 *		2、chat驱动
 *		3、CImage图形驱动
 *		3、公用函数
 */
#ifdef GAMEDEV_EXPORTS
#define GAMEDEV_API __declspec(dllexport)
#else
#define GAMEDEV_API __declspec(dllimport)
#endif


//********************** 以下为发布类和函数 *********************************
/*
 * 通讯类调用约定
 *		1、GameDev.Dll中的功能函数，需在工程中加入GameDev.lib
 *		2、发送和接收的buf中的数据最多为MAX_SEND_LEN - 1 和 MAX_RECV_LEN - 1(因为含命令码共4K)
 *		3、当连接断开时，应立即退出程序
 */
class GAMEDEV_API CGameLink  
{
public:
	BOOL ConnectServer(LPCSTR szServerName,HWND hMainWnd);
	/*
	 * 功能：连接服务器
	 * 输入： 
	 *		lpServerName : 服务器名，大厅通过程序命令行传入游戏，可通过AfxGetApp()->m_lpCmdLine得到
	 *		hMainWnd		: 游戏主窗口句柄，一般通过m_hWnd得到
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
	 *		没有数据包时返回RET_EMPTY，否则为出错
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


	CGameLink();
	virtual ~CGameLink();

private:
	HANDLE		m_hLink;
	static	int m_nReferenceCount;
};



//***************** 注册表操作函数，根是HKEY_CURRENT_USER\Software\szDomain **********************
GAMEDEV_API BOOL	RegReadKey(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD& dwType,LPSTR lpResult,DWORD& dwDataLen);
/*
 * 功能：读注册表信息
 * 输入：
 *		szMainKey	：一般为游戏名或Share，不能为空
 *		szSubKey	：一般为游戏名下的子键名或Share键下的子键名，没有子键时，此项为空
 *		szItem		：选项名，当此项为空时表示仅测试szMainKey或szSubKey是否存在
 *		dwDataLen	：缓冲区最大长度，最大不能超过MAX_PATH
 * 输出：
 *		操作成功返回TRUE，否则返回FALSE
 *		lpResult	：返回所取的值
 *		dwDataLen	：返回数据长度
 */

GAMEDEV_API BOOL RegReadKeyInt(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD& dwData);
// 功能：从注册表中读一个DWORD
// 调用：参见RegReadKey()
// 返回：dwData为返回的数据

GAMEDEV_API BOOL RegReadKeyStr(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPSTR lpResult,DWORD& dwDataLen);
// 功能：从注册表中读一个String
// 调用：参见RegReadKey()
// 输入：dwDataLen为lpResult的缓冲区大小，其中dwDataLen最大不能超过MAX_PATH
// 输出：lpResult和dwDataLen为返回的字符串

GAMEDEV_API BOOL RegReadKeyBin(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPSTR lpResult,DWORD& dwDataLen);
// 功能：从注册表中读Binary数据
// 调用：参见RegReadKey()



GAMEDEV_API BOOL	RegWriteKey(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD dwType,LPCSTR lpData,DWORD dwDataLen);
/*
 * 功能：写注册表，对不同数据类型可选用RegWriteStr(),RegWriteInt(),RegWriteBin()
 * 输入：
 *		szMainKey	：一般为游戏名或Share，不能为空
 *		szSubKey	：一般为游戏名或Share键下的子键名
 *		szItem		：选项名
 *		szType		：可选择REG_BINARY、REG_DWORD、REG_SZ
 *		dwDataLen	：数据长度
 * 输出：
 *		操作成功返回TRUE，否则返回FALSE
 * 实现：
 *		写一个注册项，若此项不存在就新建一个
 */

GAMEDEV_API BOOL RegWriteKeyStr(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPCSTR lpData);
// 功能：向注册表中写一个字符串
// 调用：参见RegWriteKey()

GAMEDEV_API BOOL RegWriteKeyInt(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,int nData);
// 功能：向注册表中写一个DWORD
// 调用：参见RegWriteKey()

GAMEDEV_API BOOL RegWriteKeyBin(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,BYTE *pData,DWORD dwDataLen);
// 功能：向注册表中写Binary数据
// 调用：参见RegWriteKey()


GAMEDEV_API void GetLevelName(int nGameID,LPCSTR ScoreBuf,LPSTR szLevelName, long* plLevel);
/*
 * 功能：不同游戏通过分数获取级别的名称
 * 输入：
 *		nGameID		：游戏的种类如，GAMEID_XXXX
 *		lScore		：分数
 *		
 * 输出：
 *		操作成功返回TRUE，否则返回FALSE
 */
/************************** 以下为系统专用函数和变量 ***************************************/
typedef struct IPC_LINK_CONFIG
{
	WORD	wMagic;							// 有效标志为0x613
	DWORD	dwPlazaCount;					// 大厅已经运行的标志
	BOOL	bMultiChannel;					// 默认为多通道
	char	szCommonChannelName[MAX_PATH];
}IPC_LINK_CONFIG;
extern GAMEDEV_API IPC_LINK_CONFIG g_ipcConfig;


#endif // _GAME_DEV_H

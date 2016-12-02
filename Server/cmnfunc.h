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
 * ��������֧��
 *		1��IPCͨѶ
 *		2��chat����
 *		3��CImageͼ������
 *		3�����ú���
 */
#ifdef GAMEDEV_EXPORTS
#define GAMEDEV_API __declspec(dllexport)
#else
#define GAMEDEV_API __declspec(dllimport)
#endif


//********************** ����Ϊ������ͺ��� *********************************
/*
 * ͨѶ�����Լ��
 *		1��GameDev.Dll�еĹ��ܺ��������ڹ����м���GameDev.lib
 *		2�����ͺͽ��յ�buf�е��������ΪMAX_SEND_LEN - 1 �� MAX_RECV_LEN - 1(��Ϊ�������빲4K)
 *		3�������ӶϿ�ʱ��Ӧ�����˳�����
 */
class GAMEDEV_API CGameLink  
{
public:
	BOOL ConnectServer(LPCSTR szServerName,HWND hMainWnd);
	/*
	 * ���ܣ����ӷ�����
	 * ���룺 
	 *		lpServerName : ��������������ͨ�����������д�����Ϸ����ͨ��AfxGetApp()->m_lpCmdLine�õ�
	 *		hMainWnd		: ��Ϸ�����ھ����һ��ͨ��m_hWnd�õ�
	 * �����
	 *		�ɹ�����TRUE�����򷵻�FALSE
	 */

	int SendCmd(BYTE cCmdID, LPCSTR buf, int nLen);
	/*
	 * ���ܣ���������
	 * ���룺 
	 *		cCmdID : ������
	 *		buf �� nLen :���������������ݺͳ��ȣ�û������ʱ��ΪNULL��0��nLen���ΪMAX_SEND_LEN - 1
	 * �����
	 *		�ɹ�����RET_OK������Ϊ����
	 */

	int RecvCmd(BYTE& cCmdID, char *buf, int& nLen);
	/*
	 * ���ܣ���������
	 * ���룺 
	 *		 : ��
	 * �����
	 *		�ɹ�����RET_OK��cCmdID��buf �� nLen Ϊ���������������ݺͳ��ȣ�nLen���ΪMAX_RECV_LEN - 1
	 *		û�����ݰ�ʱ����RET_EMPTY������Ϊ����
	 */

	void CloseConnect();
	/*
	 * ���ܣ��ر�����
	 * ˵���������ӶϿ����˳�����ʱ��Ӧ�ر����Ӳ��˳�����
	 */

	BOOL IsConnectOK();
	/*
	 * ���ܣ����������Ƿ���ͨ
	 */


	CGameLink();
	virtual ~CGameLink();

private:
	HANDLE		m_hLink;
	static	int m_nReferenceCount;
};



//***************** ע����������������HKEY_CURRENT_USER\Software\szDomain **********************
GAMEDEV_API BOOL	RegReadKey(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD& dwType,LPSTR lpResult,DWORD& dwDataLen);
/*
 * ���ܣ���ע�����Ϣ
 * ���룺
 *		szMainKey	��һ��Ϊ��Ϸ����Share������Ϊ��
 *		szSubKey	��һ��Ϊ��Ϸ���µ��Ӽ�����Share���µ��Ӽ�����û���Ӽ�ʱ������Ϊ��
 *		szItem		��ѡ������������Ϊ��ʱ��ʾ������szMainKey��szSubKey�Ƿ����
 *		dwDataLen	����������󳤶ȣ�����ܳ���MAX_PATH
 * �����
 *		�����ɹ�����TRUE�����򷵻�FALSE
 *		lpResult	��������ȡ��ֵ
 *		dwDataLen	���������ݳ���
 */

GAMEDEV_API BOOL RegReadKeyInt(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD& dwData);
// ���ܣ���ע����ж�һ��DWORD
// ���ã��μ�RegReadKey()
// ���أ�dwDataΪ���ص�����

GAMEDEV_API BOOL RegReadKeyStr(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPSTR lpResult,DWORD& dwDataLen);
// ���ܣ���ע����ж�һ��String
// ���ã��μ�RegReadKey()
// ���룺dwDataLenΪlpResult�Ļ�������С������dwDataLen����ܳ���MAX_PATH
// �����lpResult��dwDataLenΪ���ص��ַ���

GAMEDEV_API BOOL RegReadKeyBin(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPSTR lpResult,DWORD& dwDataLen);
// ���ܣ���ע����ж�Binary����
// ���ã��μ�RegReadKey()



GAMEDEV_API BOOL	RegWriteKey(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,DWORD dwType,LPCSTR lpData,DWORD dwDataLen);
/*
 * ���ܣ�дע����Բ�ͬ�������Ϳ�ѡ��RegWriteStr(),RegWriteInt(),RegWriteBin()
 * ���룺
 *		szMainKey	��һ��Ϊ��Ϸ����Share������Ϊ��
 *		szSubKey	��һ��Ϊ��Ϸ����Share���µ��Ӽ���
 *		szItem		��ѡ����
 *		szType		����ѡ��REG_BINARY��REG_DWORD��REG_SZ
 *		dwDataLen	�����ݳ���
 * �����
 *		�����ɹ�����TRUE�����򷵻�FALSE
 * ʵ�֣�
 *		дһ��ע�����������ھ��½�һ��
 */

GAMEDEV_API BOOL RegWriteKeyStr(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,LPCSTR lpData);
// ���ܣ���ע�����дһ���ַ���
// ���ã��μ�RegWriteKey()

GAMEDEV_API BOOL RegWriteKeyInt(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,int nData);
// ���ܣ���ע�����дһ��DWORD
// ���ã��μ�RegWriteKey()

GAMEDEV_API BOOL RegWriteKeyBin(LPCSTR szMainKey,LPCSTR szSubKey,LPCSTR szItem,BYTE *pData,DWORD dwDataLen);
// ���ܣ���ע�����дBinary����
// ���ã��μ�RegWriteKey()


GAMEDEV_API void GetLevelName(int nGameID,LPCSTR ScoreBuf,LPSTR szLevelName, long* plLevel);
/*
 * ���ܣ���ͬ��Ϸͨ��������ȡ���������
 * ���룺
 *		nGameID		����Ϸ�������磬GAMEID_XXXX
 *		lScore		������
 *		
 * �����
 *		�����ɹ�����TRUE�����򷵻�FALSE
 */
/************************** ����Ϊϵͳר�ú����ͱ��� ***************************************/
typedef struct IPC_LINK_CONFIG
{
	WORD	wMagic;							// ��Ч��־Ϊ0x613
	DWORD	dwPlazaCount;					// �����Ѿ����еı�־
	BOOL	bMultiChannel;					// Ĭ��Ϊ��ͨ��
	char	szCommonChannelName[MAX_PATH];
}IPC_LINK_CONFIG;
extern GAMEDEV_API IPC_LINK_CONFIG g_ipcConfig;


#endif // _GAME_DEV_H

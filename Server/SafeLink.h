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
	 * ���ܣ����ӷ�����
	 * ���룺 
	 *		lpServerName : ��������������ͨ�����������д�����Ϸ����ͨ��AfxGetApp()->m_lpCmdLine�õ�
	 *		hMainWnd	 : ��Ϸ�����ھ����һ��ͨ��m_hWnd�õ�
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
	 *		û�����ݰ�ʱ����RET_EMPTY����������ֵΪ����
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

// MapIPC.h: interface for the CMapIPC class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GAME_H
#define _GAME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "common.h"
#include "gamedev.h"


//******************** ��SERVER֪ͨgame״̬���� *****************************
#define	CS_GAME_READY			11	// �����Լ�׼����ϣ����յ�SC_GAME_BEGINʱ�ſɿ�ʼ��Ϸ
// ���ݣ���


#define CS_GAME					18	// ��Server������Ϸ���ݣ�SERVER��������ת���������������
// ���ݸ�ʽ : BYTE data[nVarLen],vVarLenΪ���ⳤ��(С��MAX_RECV_LEN)

#define CS_POST_DATA			19	// ����ָ���û������ݣ���CS_GAME�Ὣ���ݷ��������������
// ���ݸ�ʽ��POST_DATA���ܳ��Ȳ��ܳ���4095������Ϸ����̫��ʱ�����Կ��Ǿܾ����˰�
/*
typedef struct tagPOST_DATA
{
	WORD wUserCount;				// �û��б�ĳ��ȣ����500��
	WORD wUserID[wUserCount];		// ���մ����ݵ����ID�б�
	char data[1];					// ����
}POST_DATA;
*/

#endif // _GAME_H

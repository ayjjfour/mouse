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


//******************** 向SERVER通知game状态命令 *****************************
#define	CS_GAME_READY			11	// 宣布自己准备完毕，当收到SC_GAME_BEGIN时才可开始游戏
// 数据：无


#define CS_GAME					18	// 向Server发送游戏数据，SERVER将此数据转发给本桌所有玩家
// 数据格式 : BYTE data[nVarLen],vVarLen为任意长度(小于MAX_RECV_LEN)

#define CS_POST_DATA			19	// 向本桌指定用户发数据，而CS_GAME会将数据发给所有其它玩家
// 数据格式：POST_DATA，总长度不能超过4095，当游戏数据太长时，可以考虑拒绝发此包
/*
typedef struct tagPOST_DATA
{
	WORD wUserCount;				// 用户列表的长度，最多500人
	WORD wUserID[wUserCount];		// 接收此数据的玩家ID列表
	char data[1];					// 数据
}POST_DATA;
*/

#endif // _GAME_H

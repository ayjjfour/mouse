#ifndef COMMON_H
#define COMMON_H

// 数据范围
#define MAX_MSG_LEN			256
#define MAX_NAME_LEN		32
#define MAX_PASSWORD_LEN	16
#define MAX_USER_ID			500			// 每个房间最多可容纳的用户数，用户ID也不应超出此范围
#define MAX_CHAIR			6			// 最大椅子数
#define MAX_SCORE_BUF_LEN	20			// 用户成绩的最大长度
#define MAX_RESERVE_LEN		32			// 何留字节最大长度

#define MAGIC_NUM			((WORD)0x613)

// IPC name
#define GAME_CHANNEL_NAME		"shPMC"

//#define MAX_FREE_ACTIVITY	1

#define MAX_SEND_LEN		4096		// SendCmd() 最大一次发送Buf字节数
#define MAX_RECV_LEN		4096		// RecvCmd() 最大一次接收Buf字节数

#define RET_OK				0	// do the right thing
#define RET_ERROR			1	// common error occur
#define RET_EMPTY			2	// nothing 
#define RET_LINK_ERROR		3	// 通讯错误
#define RET_NO_MEMORY		4	// out of memory
#define RET_CANCEL			5	// cancel current operation
#define RET_TIME_OUT		6	// out if time error
#define RET_BUSY			7	// recv is retentering
#define RET_REFUSE			8	// server refuse to send data or function is refused
#define RET_WAIT			9
#define RET_INVALID_PARAM	10	// one of parameters is error

// 用户用于game的返回码从此值开始，与以上返回码不能冲突
#define RET_USER_DEFINE		100
#define RET_USER_DEFINE1	(RET_USER_DEFINE + 1)

#define INVALID_USER_ID			0xFFFF	// 无效用户索引ID

enum FRIEND_TYPE
{
	friendNone = 0,friendFriend = 1,friendEnemy = 2
};


#endif	// COMMON_H

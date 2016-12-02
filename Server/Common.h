#ifndef COMMON_H
#define COMMON_H

// ���ݷ�Χ
#define MAX_MSG_LEN			256
#define MAX_NAME_LEN		32
#define MAX_PASSWORD_LEN	16
#define MAX_USER_ID			500			// ÿ�������������ɵ��û������û�IDҲ��Ӧ�����˷�Χ
#define MAX_CHAIR			6			// ���������
#define MAX_SCORE_BUF_LEN	20			// �û��ɼ�����󳤶�
#define MAX_RESERVE_LEN		32			// �����ֽ���󳤶�

#define MAGIC_NUM			((WORD)0x613)

// IPC name
#define GAME_CHANNEL_NAME		"shPMC"

//#define MAX_FREE_ACTIVITY	1

#define MAX_SEND_LEN		4096		// SendCmd() ���һ�η���Buf�ֽ���
#define MAX_RECV_LEN		4096		// RecvCmd() ���һ�ν���Buf�ֽ���

#define RET_OK				0	// do the right thing
#define RET_ERROR			1	// common error occur
#define RET_EMPTY			2	// nothing 
#define RET_LINK_ERROR		3	// ͨѶ����
#define RET_NO_MEMORY		4	// out of memory
#define RET_CANCEL			5	// cancel current operation
#define RET_TIME_OUT		6	// out if time error
#define RET_BUSY			7	// recv is retentering
#define RET_REFUSE			8	// server refuse to send data or function is refused
#define RET_WAIT			9
#define RET_INVALID_PARAM	10	// one of parameters is error

// �û�����game�ķ�����Ӵ�ֵ��ʼ�������Ϸ����벻�ܳ�ͻ
#define RET_USER_DEFINE		100
#define RET_USER_DEFINE1	(RET_USER_DEFINE + 1)

#define INVALID_USER_ID			0xFFFF	// ��Ч�û�����ID

enum FRIEND_TYPE
{
	friendNone = 0,friendFriend = 1,friendEnemy = 2
};


#endif	// COMMON_H

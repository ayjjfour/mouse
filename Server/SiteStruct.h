#ifndef _SITE_STRUCT_H_
#define _SITE_STRUCT_H_

#pragma pack (push)
#pragma warning (push)

#pragma pack(1)						// 采用压缩储存
#pragma warning (disable: 4200)		// 可以使用0长度数组

struct STANDARDMSG{
	unsigned char msgID;				// 消息ID
};

struct ENABLE_LOOK_ON: public STANDARDMSG {
	int GetSize() {
		return sizeof(ENABLE_LOOK_ON)+(nNum<=0? 0 : nNum*sizeof(DWORD));
	};

	unsigned char bEnable;				// 是否允许旁观
	int nNum;							// 用户ID列表长度，负数表示所有用户
	DWORD UserIDs[];					// 用户ID列表
};

struct VERIFY_VERSION: public STANDARDMSG {
	int curVersion;
	int minVersion;
	int maxVersion;
};

struct NOTIFY_MSG {
	enum {
		SNM_NORMAL	=1,			// 一般消息，显示在聊天框中
		SNM_POPUP	=2,			// 弹处消息，用对话框显示
		SNM_MUSTSHOW =0x8000	// 强制显示消息
	};
	UINT	msgType;
	UINT	length;
	char	msg[];

	int GetSize() {
		return sizeof(NOTIFY_MSG)+length+1;
	}
};

#pragma warning (pop)
#pragma pack(pop)

#endif	//_SITE_STRUCT_H_

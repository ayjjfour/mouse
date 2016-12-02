#ifndef _SITE_STRUCT_H_
#define _SITE_STRUCT_H_

#pragma pack (push)
#pragma warning (push)

#pragma pack(1)						// ����ѹ������
#pragma warning (disable: 4200)		// ����ʹ��0��������

struct STANDARDMSG{
	unsigned char msgID;				// ��ϢID
};

struct ENABLE_LOOK_ON: public STANDARDMSG {
	int GetSize() {
		return sizeof(ENABLE_LOOK_ON)+(nNum<=0? 0 : nNum*sizeof(DWORD));
	};

	unsigned char bEnable;				// �Ƿ������Թ�
	int nNum;							// �û�ID�б��ȣ�������ʾ�����û�
	DWORD UserIDs[];					// �û�ID�б�
};

struct VERIFY_VERSION: public STANDARDMSG {
	int curVersion;
	int minVersion;
	int maxVersion;
};

struct NOTIFY_MSG {
	enum {
		SNM_NORMAL	=1,			// һ����Ϣ����ʾ���������
		SNM_POPUP	=2,			// ������Ϣ���öԻ�����ʾ
		SNM_MUSTSHOW =0x8000	// ǿ����ʾ��Ϣ
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

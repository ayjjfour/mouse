#ifndef _EVENT_STATUS_HEADER_FILE_DEFINED
#define _EVENT_STATUS_HEADER_FILE_DEFINED

const ULONG S_PROCESSED=0x01000000;					//�¼�����ȷ�Ĵ���
const ULONG S_CANCELPROCESS=0x02000000;				//�¼������������跢�͸���һ������
const ULONG S_CAPTURE=0x04000000;						//�¼�������, ��ȡ����ȥ�������¼�
const ULONG S_RELEASECAPTURE=0x08000000;				//�¼�������, ���ٽ�ȡ����¼�

#endif //!define _EVENT_STATUS_HEADER_FILE_DEFINED


#ifndef		__STATISTIC_H__
#define		__STATISTIC_H__

#include"afxmt.h"
#include <time.h>
#include <map>
#include <queue>

#include "GameConfig.h "

using namespace std;

#define		CREATE_KEY(u,t)		((__int64)((((__int64)(u))<<32) | ((__int64)(t))))

typedef enum enumEvent_s{
	EVENT_NONE		= -1,
	EVENT_INIT		= 0,
	EVENT_CHANGE,
	EVENT_END,
	EVENT_ALL
}enumEvent_t;

// ��ʼ��Ϣ
typedef struct Init_s {
	int				RoomID;								// �����
	int				TableID;							// ��Ϸ����
	int				DBID;								// ���ͨ��֤ID
	char			UserName[MAX_NAME_LEN];				// ����ǳ�
	long long		InitMoney;							// ��ҳ�ʼ���
	int				Level;								// ��ҳ�ʼ�ȼ�
	__time64_t		StartTime;							// ���뷿����ʼʱ��
} Init_t;

// �����ۼ���Ϣ
typedef struct Accumulation_s {
	long long		MoneyGain;							// �ۼӻ�ɱ������
	long long		MoneyCost;							// �ۼӻ�ɱ��֧��
	unsigned int	MissingCount;						// �ۼ������жϴ���
	unsigned int	MissingValid;						// �ۼ�������Ч����
	long long		MissingGain;						// �ۼ�����ϵͳ����
	unsigned int	SkillCount[SkillTypeCount];			// �ۼӼ��ܴ�������
	unsigned int	KillMonster[MonsterTypeCount];		// �ۼӻ�ɱ��������(���ܹ����Ƿ��Ǯ)
	unsigned int	KillValid[MonsterTypeCount];		// �ۼ���Ч��ɱ����(�����Ǯ��)
	long long		KillCost[MonsterTypeCount];			// �ۼӻ�ɱ����֧��
	long long		KillGain[MonsterTypeCount];			// �ۼӻ�ɱ��������
	int				MonsterRate;						// ��ֻ���������
	long long		MonsterMostGain;					// ��ֻ�����������
	int				MosterID;							// ����������ID
	long long		MostGain;							// ���������ʼ��ʱ����ڳ�ʼ��
	__time64_t		T;									// �������ʱ���
	int				Level;								// ��Ϸ�ȼ�
} Accumulation_t;

// ����������Ϣ
typedef struct Delta_s {
	long long		MoneyGain;							// ʱ����������
	long long		MoneyCost;							// ʱ������֧��
	unsigned int	MissingCount;						// �����жϴ���
	unsigned int	MissingValid;						// ������Ч����
	long long		MissingGain;						// ����ϵͳ����
	unsigned int	SkillCount[SkillTypeCount];			// ���ܴ�������
	unsigned int	KillMonster[MonsterTypeCount];		// ��ɱ��������(���ܹ����Ƿ��Ǯ)
	unsigned int	KillValid[MonsterTypeCount];		// ��Ч��ɱ����(�����Ǯ��)
	long long		KillCost[MonsterTypeCount];			// ��ɱ����֧��
	long long		KillGain[MonsterTypeCount];			// ��ɱ��������
} Delta_t;

// ͳ����Ϣ�ṹ
typedef struct Statistic_s {
	__time64_t		Lasttime;							// �ϴμ�¼ʱ��
	Init_t			Init;
	Accumulation_t	Accumulation;
	Delta_t			Delta;
}Statistic_t;

//////////////////////////////////////////////////////////////////////////
// ֪ͨЭ��ṹ
typedef struct Change_s {
	int				RoomID;								// �����
	int				TableID;							// ��Ϸ����
	long			DBID;								// ���ͨ��֤ID
	long long		MoneyGain;							// ���λ�ɱ������
	long long		MoneyCost;							// ���λ�ɱ��֧��
	unsigned int	MissingCount;						// �����жϴ���
	unsigned int	MissingValid;						// ������Ч����
	long long		MissingGain;						// ����ϵͳ����
	unsigned int	SkillCount[SkillTypeCount];			// ���ܴ�������
	unsigned int	KillMonster[MonsterTypeCount];		// ��ɱ��������(���ܹ����Ƿ��Ǯ)
	unsigned int	KillValid[MonsterTypeCount];		// ��Ч��ɱ����(�����Ǯ��)
	long long		KillCost[MonsterTypeCount];			// ��ɱ����֧��
	long long		KillGain[MonsterTypeCount];			// ��ɱ��������
	int				MonsterRate;						// ��ֻ���������
	long long		MonsterMostGain;					// ��ֻ�����������
	int				MosterID;							// ����������ID
	__time64_t		T;									// ��ֻ������������ʱ���
	int				Level;								// ��Ϸ�ȼ�
} Change_t;

typedef struct End_s {
	int				RoomID;								// �����
	int				TableID;							// ��Ϸ����
	int				DBID;								// ���ͨ��֤ID
} End_t;

typedef struct File_s {
	int				RoomID;								// �����
	FILE*			fd_interval;						// ÿ�����ӽ׶���־			
	FILE*			fd_final;							// ÿ����Ϸ�׶���־
}File_t;

// ��ʵ������
class CStatistic
{
private:
	CStatistic() {}

	static CStatistic*				m_pStatistic;
	static CMutex					m_queueLock;

	CString m_szRoomName;

private:
	queue<enumEvent_t>				m_queueEvent;
	queue<Statistic_t>				m_queueStatistc;
	map<int, File_t>				m_mapFile;

	int								m_running;

	HANDLE							m_event;
	HANDLE							m_event_exit;
	__time64_t						m_currtime;
	int								m_interval;
	char							m_logPath[MAX_PATH];

	CWinThread						*m_pThread;
public:
	static CStatistic* Instance();
	static void Release();

	void SetRoomName(CString szName)
	{
		m_szRoomName = szName;
	}

private:
	void Initialize();
	void Free();

public:
	void PlayerSitdown(Statistic_t& Statistic);
	void IncomingChange(Statistic_t& Statistic);
	void PlayerLeave(Statistic_t& Statistic);

public:
	static DWORD ServerThread(LPVOID lpVoid);
	void Run();

private:
	void _DaySwitch(void);				// ����
//	BOOL _CheckTimeOut(void);			// ����Ƿ�ʱ
	enumEvent_t	_GetEvent(void);		// ���¼������л�ȡ�¼�
	void _GetInit(void);
	void _GetChange(void);
	void _GetEnd(void);
	void _Wait(int ms);

	bool FolderExists(CString sPath);
	bool SuperMkDir(CString sPath);
	void EnsureDir(CString sPath);

	FILE* _GetIntervalFile(int RoomID);
	FILE* _GetFinalFile(int RoomID);

	void _Write_Log(Statistic_t*);
	void _Write_Final(Statistic_t*);
	void _SaveAll(void);
	void _CloseAll(void);
};

#endif		// __STATISTIC_H__
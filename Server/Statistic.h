
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

// 初始信息
typedef struct Init_s {
	int				RoomID;								// 房间号
	int				TableID;							// 游戏桌号
	int				DBID;								// 玩家通行证ID
	char			UserName[MAX_NAME_LEN];				// 玩家昵称
	long long		InitMoney;							// 玩家初始金币
	int				Level;								// 玩家初始等级
	__time64_t		StartTime;							// 进入房间起始时间
} Init_t;

// 保存累加信息
typedef struct Accumulation_s {
	long long		MoneyGain;							// 累加击杀总收益
	long long		MoneyCost;							// 累加击杀总支出
	unsigned int	MissingCount;						// 累加免伤判断次数
	unsigned int	MissingValid;						// 累加免伤生效次数
	long long		MissingGain;						// 累加免伤系统收益
	unsigned int	SkillCount[SkillTypeCount];			// 累加技能触发次数
	unsigned int	KillMonster[MonsterTypeCount];		// 累加击杀怪物数量(不管怪物是否掉钱)
	unsigned int	KillValid[MonsterTypeCount];		// 累加有效击杀数量(怪物掉钱了)
	long long		KillCost[MonsterTypeCount];			// 累加击杀怪物支出
	long long		KillGain[MonsterTypeCount];			// 累加击杀怪物收入
	int				MonsterRate;						// 单只怪物最大倍率
	long long		MonsterMostGain;					// 单只怪物最大收益
	int				MosterID;							// 倍率最大怪物ID
	long long		MostGain;							// 最大收益额（开始的时候等于初始金额）
	__time64_t		T;									// 最大收益时间点
	int				Level;								// 游戏等级
} Accumulation_t;

// 保存增量信息
typedef struct Delta_s {
	long long		MoneyGain;							// 时段内总收益
	long long		MoneyCost;							// 时段内总支出
	unsigned int	MissingCount;						// 免伤判断次数
	unsigned int	MissingValid;						// 免伤生效次数
	long long		MissingGain;						// 免伤系统收益
	unsigned int	SkillCount[SkillTypeCount];			// 技能触发次数
	unsigned int	KillMonster[MonsterTypeCount];		// 击杀怪物数量(不管怪物是否掉钱)
	unsigned int	KillValid[MonsterTypeCount];		// 有效击杀数量(怪物掉钱了)
	long long		KillCost[MonsterTypeCount];			// 击杀怪物支出
	long long		KillGain[MonsterTypeCount];			// 击杀怪物收入
} Delta_t;

// 统计信息结构
typedef struct Statistic_s {
	__time64_t		Lasttime;							// 上次记录时间
	Init_t			Init;
	Accumulation_t	Accumulation;
	Delta_t			Delta;
}Statistic_t;

//////////////////////////////////////////////////////////////////////////
// 通知协议结构
typedef struct Change_s {
	int				RoomID;								// 房间号
	int				TableID;							// 游戏桌号
	long			DBID;								// 玩家通行证ID
	long long		MoneyGain;							// 本次击杀总收益
	long long		MoneyCost;							// 本次击杀总支出
	unsigned int	MissingCount;						// 免伤判断次数
	unsigned int	MissingValid;						// 免伤生效次数
	long long		MissingGain;						// 免伤系统收益
	unsigned int	SkillCount[SkillTypeCount];			// 技能触发次数
	unsigned int	KillMonster[MonsterTypeCount];		// 击杀怪物数量(不管怪物是否掉钱)
	unsigned int	KillValid[MonsterTypeCount];		// 有效击杀数量(怪物掉钱了)
	long long		KillCost[MonsterTypeCount];			// 击杀怪物支出
	long long		KillGain[MonsterTypeCount];			// 击杀怪物收入
	int				MonsterRate;						// 单只怪物最大倍率
	long long		MonsterMostGain;					// 单只怪物最大收益
	int				MosterID;							// 倍率最大怪物ID
	__time64_t		T;									// 单只怪物最大收益的时间点
	int				Level;								// 游戏等级
} Change_t;

typedef struct End_s {
	int				RoomID;								// 房间号
	int				TableID;							// 游戏桌号
	int				DBID;								// 玩家通行证ID
} End_t;

typedef struct File_s {
	int				RoomID;								// 房间号
	FILE*			fd_interval;						// 每两分钟阶段日志			
	FILE*			fd_final;							// 每次游戏阶段日志
}File_t;

// 单实例对象
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
	void _DaySwitch(void);				// 日切
//	BOOL _CheckTimeOut(void);			// 检查是否超时
	enumEvent_t	_GetEvent(void);		// 从事件队列中获取事件
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
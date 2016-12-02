#pragma once
#include "GameConfig.h"
#include <list>
#include <vector>

struct HitInfo
{
	LONGLONG lBet;
	LONGLONG lReturn;
	LONGLONG lCount;
	LONGLONG lProb;
	LONGLONG lMiss;

	HitInfo()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

class StatisticMgr
{
public:
	static StatisticMgr& getInstance();

private:
	StatisticMgr(void);
	~StatisticMgr(void);

	void StartThread();

	void StopThread();

	void EnsurePath(CString strPath);

public:
	void Initialize();

	void DumpConfig(bool bStartup);

	void SetRoomName(CString szName)
	{
		m_szRoomName = szName;
	}

	void SetRoomID(int RoomID)
	{
		m_iRoomID = RoomID;
	}

public:
	void NormalHit(MonsterType monster, long lBet, long lReturn, long lCount, long lMiss, long lProb);
	void KingHit(MonsterType monster, long lReturn, long lCount, long lMiss, long lProb);

private:
	CRITICAL_SECTION m_Lock;

	HitInfo m_NormalHit[MonsterTypeCount];
	HitInfo m_KingHit[MonsterTypeCount];
	CString m_MonsterNames[MonsterTypeCount];

private:
	HANDLE m_hThread;
	bool m_bStop;

	DWORD m_dwStartTime;

	static void _WriteLogProc(void* apParam);

	CString m_szRoomName;
	int m_iRoomID;
	CString m_szTarget;
	DWORD m_dwFreq;

	COleDateTime m_dtLastWrite;
};


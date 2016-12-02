#include "stdafx.h"
#include "Statistic.h"

CMutex CStatistic::m_queueLock;
CStatistic* CStatistic::m_pStatistic;

CStatistic* CStatistic::Instance()
{
	m_queueLock.Lock();
	if (m_pStatistic == NULL)
	{
		m_pStatistic = new CStatistic();
		if (m_pStatistic != NULL)
			m_pStatistic->Initialize();
	}
	m_queueLock.Unlock();

	return m_pStatistic;
}

void CStatistic::Initialize()
{
	m_event			= ::CreateEvent(NULL, FALSE, 0, "日志记录事件");
	m_event_exit	= ::CreateEvent(NULL, FALSE, 0, "退出事件");
	m_currtime		= ::_time64(NULL);
	m_interval		= 120;		// 时间间隔是120秒

	//
	char szFilePath[MAX_PATH];
	char szPath[MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	::PathRemoveFileSpec(szFilePath);
	strncat(szFilePath, "\\GameServerConfig.ini", strlen( "\\GameServerConfig.ini"));

	GetPrivateProfileString("PATH", TEXT("DBpath"), TEXT("C:\\"), szPath, sizeof(szPath), szFilePath);

	_sntprintf(m_logPath, _countof(m_logPath), TEXT("%s%s"), szPath, ".\\打地鼠");

	m_running = TRUE;
	m_pThread = AfxBeginThread((AFX_THREADPROC)ServerThread, this);
}

void CStatistic::Release()
{
	m_queueLock.Lock();
	if (m_pStatistic != NULL)
	{
		m_pStatistic->Free();
		delete m_pStatistic;
		m_pStatistic = NULL;
	}
	m_queueLock.Unlock();
}

void CStatistic::Free()
{
	m_running = FALSE;
	::WaitForSingleObject(m_event_exit, INFINITE);
}

void CStatistic::PlayerSitdown(Statistic_t& Statistic)
{
/*	m_queueLock.Lock();

	m_queueStatistc.push(Statistic);
	m_queueEvent.push(EVENT_INIT);

	m_queueLock.Unlock();

	SetEvent(m_event);

	return;
*/
}

void CStatistic::IncomingChange(Statistic_t& Statistic)
{
	m_queueLock.Lock();

	m_queueStatistc.push(Statistic);
	m_queueEvent.push(EVENT_CHANGE);

	m_queueLock.Unlock();

	SetEvent(m_event);

	return;
}

void CStatistic::PlayerLeave(Statistic_t& Statistic)
{
	m_queueLock.Lock();

	m_queueStatistc.push(Statistic);
	m_queueEvent.push(EVENT_END);

	m_queueLock.Unlock();

	SetEvent(m_event);

	return;
}

void CStatistic::_DaySwitch()
{
	char		path[MAX_PATH];
	char		file[MAX_PATH];
	CTime		t1 = CTime::GetCurrentTime();
	CTime		t2 = CTime(m_currtime);
	map<int, File_t>::iterator		it;

	if (t1.GetDay() != t2.GetDay())
	{
		// 关闭所有文件，然后重命名并打开
		for(it = m_mapFile.begin(); it != m_mapFile.end(); it++)
		{
			File_t&	f = (*it).second;

			if (f.fd_interval){fclose(f.fd_interval); f.fd_interval = NULL;}
			if (f.fd_final) {fclose(f.fd_final); f.fd_final = NULL;}

			_snprintf(path, sizeof(path), "%s\\%s", m_logPath, m_szRoomName);
			_snprintf(file, sizeof(file), "%s\\时段日志_%04d_%02d_%02d.log", path, t1.GetYear(), t1.GetMonth(), t1.GetDay());
			EnsureDir(file);
			f.fd_interval = fopen(file, "a");
			_snprintf(file, sizeof(file), "%s\\总体日志_%04d_%02d_%02d.log", path, t1.GetYear(), t1.GetMonth(), t1.GetDay());
			EnsureDir(file);
			f.fd_final = fopen(file, "a");
		}
	}

	m_currtime = t1.GetTime();
}

enumEvent_t	CStatistic::_GetEvent()
{
	enumEvent_t	Event;
	m_queueLock.Lock();

	if (m_queueEvent.size() > 0)
	{
		Event = m_queueEvent.front();
		m_queueEvent.pop();
	}
	else
		Event = EVENT_NONE;

	m_queueLock.Unlock();

	return Event;
}

void CStatistic::_GetInit()
{
#if 0
	Init_t			Init;
	Statistic_s		Statistic;
	__int64			key;

	m_queueLock.Lock();
	Init = m_queueInit.front();
	m_queueInit.pop();
	m_queueLock.Unlock();

	memset(&Statistic, 0, sizeof(Statistic_t));
	key = CREATE_KEY(Init.DBID,Init.TableID);
	memcpy(&Statistic.Init, &Init, sizeof(Init_t));
	Statistic.Accumulation.MostGain = Init.InitMoney;
	Statistic.Accumulation.Level	= Init.Level;
	Statistic.Accumulation.T		= m_currtime;
	Statistic.Lasttime				= m_currtime;

	m_mapStatistic.insert(make_pair(key, Statistic));
#endif
}

void CStatistic::_GetChange()
{
	Statistic_t		Statistic;

	m_queueLock.Lock();
	Statistic = m_queueStatistc.front();
	m_queueStatistc.pop();
	m_queueLock.Unlock();

	_Write_Log(&Statistic);
}

void CStatistic::_GetEnd()
{
	Statistic_t		Statistic;

	m_queueLock.Lock();
	Statistic = m_queueStatistc.front();
	m_queueStatistc.pop();
	m_queueLock.Unlock();
	
	_Write_Final(&Statistic);
}

#if 0
BOOL CStatistic::_CheckTimeOut()
{
	Statistic_t*		pStatistic;
	map<__int64, Statistic_t>::iterator		it;

	for (it = m_mapStatistic.begin(); it != m_mapStatistic.end(); it++ )
	{
		pStatistic = &(*it).second;
		if (m_currtime - pStatistic->Lasttime >= m_interval)
			_Write_Log(pStatistic);
	}

	return TRUE;
}
#endif

void CStatistic::_Wait(int ms)
{
	::WaitForSingleObject(m_event, ms);
}

//是否存在该文件
bool CStatistic::FolderExists(CString sPath)
{
	DWORD attr;
	attr = GetFileAttributes(sPath);
	return (attr != (DWORD)(-1) ) &&
		( attr & FILE_ATTRIBUTE_DIRECTORY);
}

// 递归创建目录， 如果目录已经存在或者创建成功返回TRUE
bool CStatistic::SuperMkDir(CString sPath)
{
	int len=sPath.GetLength();
	if ( len <2 ) return false;

	if('\\'==sPath[len-1])
	{
		sPath=sPath.Left(len-1);
		len=sPath.GetLength();
	}
	if ( len <=0 ) return false;

	if (len <=3)
	{
		if (FolderExists(sPath))return true;
		else return false;
	}

	if (FolderExists(sPath))return true;    

	CString Parent;
	Parent=sPath.Left(sPath.ReverseFind('\\') );

	if(Parent.GetLength()<=0)return false;

	bool Ret=SuperMkDir(Parent);

	if(Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor=NULL;
		sa.bInheritHandle=0;
		Ret=(CreateDirectory(sPath,&sa)==TRUE);
		return Ret;
	}
	else
		return false;
}

void CStatistic::EnsureDir(CString sPath)
{
	TCHAR szDir[MAX_PATH] = {0};
	CString::CopyChars(szDir, sPath, sPath.GetLength());

	PathRemoveFileSpec(szDir);

	SuperMkDir(szDir);
}

FILE* CStatistic::_GetIntervalFile(int RoomID)
{
	char		path[MAX_PATH] = {0};
	char		file[MAX_PATH] = {0};
	FILE*		fp;
	CTime		t = CTime(m_currtime);
	map<int, File_t>::iterator		it;

	it = m_mapFile.find(RoomID);
	if ( it != m_mapFile.end() )
	{
		File_t&	f = (*it).second;

		if (NULL == f.fd_interval)
		{
			_snprintf(path, sizeof(path), "%s\\%s", m_logPath, m_szRoomName);
			_snprintf(file, sizeof(file), "%s\\时段日志_%04d_%02d_%02d.log", path, t.GetYear(), t.GetMonth(), t.GetDay());
			EnsureDir(file);
			f.fd_interval = fopen(file, "a");
		}

		fp = f.fd_interval;
	}
	else
	{
		File_t		f = {RoomID, NULL, NULL};
		_snprintf(path, sizeof(path), "%s\\%s", m_logPath, m_szRoomName);
		_snprintf(file, sizeof(file), "%s\\时段日志_%04d_%02d_%02d.log", path, t.GetYear(), t.GetMonth(), t.GetDay());
		EnsureDir(file);
		f.fd_interval = fopen(file, "a");
		m_mapFile.insert(make_pair(RoomID,f));
		fp = f.fd_interval;
	}

	return fp;
}

FILE* CStatistic::_GetFinalFile(int RoomID)
{
	char		path[MAX_PATH];
	char		file[MAX_PATH];
	FILE*		fp;
	CTime		t = CTime(m_currtime);
	map<int, File_t>::iterator		it;

	it = m_mapFile.find(RoomID);
	if ( it != m_mapFile.end() )
	{
		File_t&	f = (*it).second;

		if (NULL == f.fd_final)
		{
			_snprintf(path, sizeof(path), "%s\\%s", m_logPath, m_szRoomName);
			_snprintf(file, sizeof(file), "%s\\总体日志_%04d_%02d_%02d.log", path, t.GetYear(), t.GetMonth(), t.GetDay());
			EnsureDir(file);
			f.fd_final = fopen(file, "a");
		}

		fp = f.fd_final;
	}
	else
	{
		File_t		f = {RoomID, NULL, NULL};
		_snprintf(path, sizeof(path), "%s\\%s", m_logPath, m_szRoomName);
		_snprintf(file, sizeof(file), "%s\\总体日志_%04d_%02d_%02d.log", path, t.GetYear(), t.GetMonth(), t.GetDay());
		EnsureDir(file);
		f.fd_final = fopen(file, "a");
		m_mapFile.insert(make_pair(RoomID,f));
		fp = f.fd_final;
	}

	return fp;
}

void CStatistic::_Write_Log(Statistic_t* pStatistic)
{
	char				buf[4096];
	int					len = 0;
	size_t				size;
	FILE				*fp;
	Accumulation_t		*pac = &pStatistic->Accumulation;
	Delta_t				*pdl = &pStatistic->Delta;

	fp = _GetIntervalFile(pStatistic->Init.RoomID);
	if (NULL == fp)
		return;	

	CTime	start	= CTime(pStatistic->Lasttime);
	CTime	end		= CTime(_time64(NULL));

	// 下面开始生成日志文本
	len += _snprintf(buf + len, sizeof(buf) - len, "用户ID=%u，昵称=%.32s，起始时间=%04d-%02d-%02d %02d:%02d:%02d，桌号=%d\n",
						pStatistic->Init.DBID, pStatistic->Init.UserName,
						start.GetYear(), start.GetMonth(), start.GetDay(), start.GetHour(), start.GetMinute(), start.GetSecond(),
						pStatistic->Init.TableID );
	len += _snprintf(buf + len, sizeof(buf) - len, "时段结算分=%lld，免伤判断次数=%d，免伤生效次数=%d，免伤系统收益=%lld，技能触发次数=%d,%d,%d,%d\n", 
						pdl->MoneyGain - pdl->MoneyCost, pdl->MissingCount, pdl->MissingValid, pdl->MissingGain, 
						pdl->SkillCount[Skill_LianSuoShanDian], pdl->SkillCount[Skill_HengSaoQianJun],
						pdl->SkillCount[Skill_GaoShanLiuShui], pdl->SkillCount[Skill_LiuXingHuoYu]);
	len += _snprintf(buf + len, sizeof(buf) - len, "击杀怪物数=%d,%d,%d,%d,%d,%d,%d,%d,%d,，有效击杀数=%d,%d,%d,%d,%d,%d,%d,%d,%d,，"
						"击杀怪物玩家支出=%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,，击杀怪物玩家收益=%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,，结束时间=%04d-%02d-%02d %02d:%02d:%02d\n",
						pdl->KillMonster[mtMoleBoSi], pdl->KillMonster[mtMoleDaiMeng], pdl->KillMonster[mtMoleFeiXing], pdl->KillMonster[mtMoleHuShi], pdl->KillMonster[mtMoleKuangGong],
						pdl->KillMonster[mtMoleKing], pdl->KillMonster[mtSpider], pdl->KillMonster[mtFly], pdl->KillMonster[mtFlyGoblin],
						pdl->KillValid[mtMoleBoSi], pdl->KillValid[mtMoleDaiMeng], pdl->KillValid[mtMoleFeiXing], pdl->KillValid[mtMoleHuShi], pdl->KillValid[mtMoleKuangGong], 
						pdl->KillValid[mtMoleKing], pdl->KillValid[mtSpider], pdl->KillValid[mtFly], pdl->KillValid[mtFlyGoblin],
						pdl->KillCost[mtMoleBoSi], pdl->KillCost[mtMoleDaiMeng], pdl->KillCost[mtMoleFeiXing], pdl->KillCost[mtMoleHuShi], pdl->KillCost[mtMoleKuangGong],
						pdl->KillCost[mtMoleKing], pdl->KillCost[mtSpider], pdl->KillCost[mtFly], pdl->KillCost[mtFlyGoblin],
						pdl->KillGain[mtMoleBoSi], pdl->KillGain[mtMoleDaiMeng], pdl->KillGain[mtMoleFeiXing], pdl->KillGain[mtMoleHuShi], pdl->KillGain[mtMoleKuangGong], 
						pdl->KillGain[mtMoleKing], pdl->KillGain[mtSpider], pdl->KillGain[mtFly], pdl->KillGain[mtFlyGoblin],
						end.GetYear(), end.GetMonth(), end.GetDay(), end.GetHour(), end.GetMinute(), end.GetSecond());

//	memset(pdl, 0, sizeof(Delta_t));

	// 开始写文件
	size = fwrite(buf, 1, len, fp);
	fflush(fp);

	return;
}

void CStatistic::_Write_Final(Statistic_t* pStatistic)
{
	char				buf[4096];
	int					len = 0;
	size_t				size;
	FILE				*fp;
	Init_t				*pit	= &pStatistic->Init;
	Accumulation_t		*pac	= &pStatistic->Accumulation;
	map<int, File_t>::iterator		it;

	fp = _GetFinalFile(pStatistic->Init.RoomID);
	if (NULL == fp)
		return;	

	CTime	start	= CTime(pit->StartTime);
	CTime	hight	= CTime(pac->T);
	CTime	end		= CTime(m_currtime);

	len += _snprintf(buf + len, sizeof(buf) - len, "%04d-%02d-%02d %02d:%02d:%02d，玩家ID=%u，昵称=%.32s，桌号=%d\n",
		start.GetYear(), start.GetMonth(), start.GetDay(), start.GetHour(), start.GetMinute(), start.GetSecond(),
		pStatistic->Init.DBID, pStatistic->Init.UserName,
		pStatistic->Init.TableID );
	len += _snprintf(buf + len, sizeof(buf) - len, "进入时携带金币=%lld，时段结算分=%lld，免伤判断次数=%d，免伤生效次数=%d，免伤系统收益=%lld，技能触发次数=%d,%d,%d,%d，游戏等级=%d\n", 
		pit->InitMoney, pac->MoneyGain - pac->MoneyCost, pac->MissingCount, pac->MissingValid, pac->MissingGain,
		pac->SkillCount[Skill_LianSuoShanDian], pac->SkillCount[Skill_HengSaoQianJun],
		pac->SkillCount[Skill_GaoShanLiuShui], pac->SkillCount[Skill_LiuXingHuoYu], pac->Level);
	len += _snprintf(buf + len, sizeof(buf) - len, "武器消费=%lld，玩家击杀怪物收益=%lld，单只怪物最大倍率=%d，单只怪物最大收益=%lld，怪物ID=%d，时段内最高分=%lld，"
		"最高分时间点=%04d-%02d-%02d %02d:%02d:%02d，离开时剩余分=%lld，离开房间时间=%04d-%02d-%02d %02d:%02d:%02d\n",
		pac->MoneyCost, pac->MoneyGain, pac->MonsterRate, pac->MonsterMostGain, pac->MosterID, pac->MostGain, 
		hight.GetYear(), hight.GetMonth(), hight.GetDay(), hight.GetHour(), hight.GetMinute(), hight.GetSecond(),
		pit->InitMoney + pac->MoneyGain - pac->MoneyCost,
		end.GetYear(), end.GetMonth(), end.GetDay(), end.GetHour(), end.GetMinute(), end.GetSecond());

	// 开始写文件
	size = fwrite(buf, 1, len, fp);
	fflush(fp);

	return;
}

void CStatistic::_SaveAll()
{
#if 0
	Statistic_t*		pStatistic;
	map<__int64, Statistic_t>::iterator		it;

	for (it = m_mapStatistic.begin(); it != m_mapStatistic.end(); it++ )
	{
		pStatistic = &(*it).second;
		_Write_Log(pStatistic);
		_Write_Final(pStatistic);
	}
#endif
}

void CStatistic::_CloseAll()
{
	map<int, File_t>::iterator				it;

	for (it = m_mapFile.begin(); it != m_mapFile.end(); it++)
	{
		File_t	&f = (*it).second;

		if ( f.fd_interval != NULL )
		{
			fclose(f.fd_interval);
			f.fd_interval = NULL;
		}

		if (f.fd_final != NULL)
		{
			fclose(f.fd_final);
			f.fd_final = NULL;
		}
	}
}

void CStatistic::Run()
{
	enumEvent_t		ev;

	while( m_running )
	{
		_DaySwitch();	// 判断并作日切

		ev = _GetEvent();
		switch( ev )
		{
		case EVENT_INIT:	// 玩家进入桌子开始游戏
			_GetInit(); 		break;
		case EVENT_CHANGE:	// 玩家游戏过程中，数据变化
			_GetChange();		break;
		case EVENT_END:		// 玩家离开，写结束日志
			_GetEnd();			break;
		default:
			break;
		}

		if ( ev == EVENT_NONE )
			_Wait(1000);	// 等待1秒
	}

	_SaveAll();
	_CloseAll();
	::SetEvent(m_event_exit);
}

DWORD CStatistic::ServerThread(LPVOID lpVoid)
{
	CStatistic*	pStatistic = (CStatistic*)lpVoid;
	if (NULL == pStatistic)
		return 0;

	pStatistic->Run();

	return 0;
}
#include "StdAfx.h"
#include "GameLogi.h"
#include "CharSetHelper.h"
#include "MonsterManager.h"

//IPlayerIOEvent
STDMETHODIMP CGameServer::OnUserEnter(int nIdx, IGamePlayer * pGamePlayer)
{
	int nState = 0;
	if (FALSE == pGamePlayer->IsPlayer())
	{
		return E_NOTIMPL;
	}

	switch (pGamePlayer->get_DBID())
	{
	case 54134725:	//石舰
	case 53737291:	//杨灵
	case 34203756:	//陈芬
	case 52697881:	//李志琴
		m_bEnableInfoLog = true;
		m_iLogID = pGamePlayer->get_DBID();
		CMonsterManager::Instance().Attach(m_nTableID, this);
		break;
	default:
		m_bEnableInfoLog = false;
		m_iLogID = 0;
		CMonsterManager::Instance().Detach(m_nTableID);
		break;
	}

	char AnsiName[128] = { 0 };
	char Utf8Name[128] = { 0 };
	pGamePlayer->get_UserName(Utf8Name);

	if (CString(Utf8Name).Find("ran_") == 0)	//内网测试冉家瑞系列账号
	{
		m_bEnableInfoLog = true;
		m_iLogID = pGamePlayer->get_DBID();
		CMonsterManager::Instance().Attach(m_nTableID, this);
	}

	if (m_bEnableInfoLog)
	{
		m_bEnableInfoLog = GameConfigMgr::getInstance().Current().GetGamePropertyInfo(m_nRoomID)->bEnableLog;
	}
	
	WriteInfoLog("玩家进入游戏");

	CharSetHelper::UTF8ToANSI(Utf8Name, AnsiName, sizeof(AnsiName));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), "OnUserEnter: DBID=%d, UserName=%s, RoomID=%d, TableID=%d, Idx=%d", pGamePlayer->get_DBID(), AnsiName, m_nRoomID, m_nTableID, nIdx);

	//m_pTableChannel->TableDispatch(ITableChannel::IID_Get_TableID, (void*)&m_nTableID, 0);

	//初始化玩家进入房间携带分数
	m_UserStatisticsData[nIdx].Clear();

	// 同步所有玩家数据， nIdx>=0时，会局部同步，<0时同步所有数据
	m_pSite->OnSceneChanged(nIdx);     //去除m_started判断直接调用场景切换   2016/1/22

	//游戏未开始 就开始启动计时器 开始游戏
	if (!m_bIsPlaying)
	{
		//开始游戏conscons
		//暂时注释 StartGame();
	}

	//int iTempPercent = AuthPermissions(nIdx);

	//memset(&m_arFishingData[nIdx], 0, sizeof(tagFishingData));
	//if (false == getFishingData(nIdx, pGamePlayer->get_DBID(), &m_arFishingData[nIdx]))
	//{
	//    _sntprintf(m_szErrorInfo, CountArray(m_szErrorInfo), TEXT("Error:%.64s Fail to GetFishingData."), m_userName[nIdx]);
	//    m_LogFile.WriteErrorLogFile(m_szErrorInfo);
	//    return E_FAIL;
	//}

	// 读交互数据
	//DeserializeUserInteractData(nIdx);

	//玩家进入坐下即根据玩家身上的金币数计算可以最大购买多少子弹 /100*100保证子弹个数是炮火的倍数
	m_dwUserID[nIdx] = pGamePlayer->get_ID();							//DBID置为0
	m_UserGameScore[nIdx].llMoneyNum = GetPlayerScore(nIdx, pGamePlayer);    // m_llUserTotalScore
	m_UserGameScore[nIdx].llTicketNum = 0;//GetPlayerTicket(nIdx, pGamePlayer);
	m_UserPreGameScore[nIdx] = m_UserGameScore[nIdx];
	m_dbUserWelFarePoint[nIdx] = 0;// 福利点
	m_dbUserPreWelFarePoint[nIdx] = 0;// 福利点
	m_lDrawnCount[nIdx] = 0;							//平局次数
	m_enUserState[nIdx] = enPlayerEnter;		//标记进入状态
	m_nTableUserCount++;

	// 玩家经验等级
	GetFishingCommonData(nIdx, m_dwUserID[nIdx], &m_FishingData[nIdx]);
	GetCustomData(nIdx, m_dwUserID[nIdx], &m_UserInteractData[nIdx]);
	m_UserPreInteractData[nIdx] = m_UserInteractData[nIdx];

	m_UserStatisticsData[nIdx].llUserCarryScore = m_UserGameScore[nIdx];
	m_UserStatisticsData[nIdx].dwLastHitTime = GetTickCount();  
	m_UserStatisticsData[nIdx].nUserEnterRoomTick = GetTickCount();     //记录玩家进入房间tick

	//设置用户Exp
	pGamePlayer->put_Exp(m_FishingData[nIdx].llGradeScore);

	m_llGradeScore[nIdx] = GetPlayerExp(nIdx, pGamePlayer, &m_nUserLevel[nIdx]);
	m_llPreGradeScore[nIdx] = m_llGradeScore[nIdx];

	// 判断是否需要自动激活技能
	//CheckCanAutoActiveSkill(pGamePlayer);

	// 进入房间时，房间是否需要返之前购买的技能金币
	//CheckCanReturnUserCoin(pGamePlayer);

	//玩家进入即上分
	MoblieDealUserAddScore(nIdx);	

	//////////////////////////////////////////////////////////////////////////
	// 开始统计玩家信息 add by 袁俊杰 2016-10-13
	Statistic_Start(pGamePlayer, nIdx);
	
	//////////////////////////////////////////////////////////////////////////

	//启动玩家相应写分定时器
	switch (nIdx)
	{
	case 0:
		SetGameTimer(TIME_INTERVAL_PLAYER_ONE, 2 * 60 * 1000, INFINITE, 0L);				//
		break;
	case 1:
		SetGameTimer(TIME_INTERVAL_PLAYER_TWO, 2 * 60 * 1000, INFINITE, 0L);				//
		break;
	case 2:
		SetGameTimer(TIME_INTERVAL_PLAYER_THREE, 2 * 60 * 1000, INFINITE, 0L);				//
		break;
	case 3:
		SetGameTimer(TIME_INTERVAL_PLAYER_FOUR, 2 * 60 * 1000, INFINITE, 0L);				//
		break;
	default:
		break;
	}

	buildGameNo();

	if (!IsSameDay(m_LastTime, time(0)))
	{
		UpdateRecordLogPath();

		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "losspool", 0, true, m_LastTime);
	}

	std::string lName;

	if (GenerateRecordLogName(lName))
	{
		//这些代码经过长时间测试可以稳定运行
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "losspool", 0, true, LOGID_Losspool(m_nRoomID));
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "trace", 0, true, LOGID_Trace(m_nRoomID));
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "Statictis", 0, LOGID_Statictis(m_nRoomID));

		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "scorebugtrace", 0, true, LOGID_ScoreBugTrace(m_nRoomID));

		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath,"EnetrNoGun",0,true,100);

	}

	//m_pServer->StartTimer(STANDARD_TIME_ELAPSE);

	//m_LogFile.WriteDataLogFile("玩家进入");
	return S_OK;
}

STDMETHODIMP CGameServer::OnUserExit(int nIdx, IGamePlayer * pGamePlayer)
{

	if (NULL == pGamePlayer || FALSE == pGamePlayer->IsPlayer())
		return E_NOTIMPL;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), "OnUserExit: DBID=%d, RoomID=%d, TableID=%d, Idx=%d, state=%d", pGamePlayer->get_DBID(), m_nRoomID, m_nTableID, nIdx, m_enUserState[nIdx]);

	//退出时计算玩家的分数,只有没写分时才写，                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
	if (enPlayerBuy == m_enUserState[nIdx])
	{
		CalcScore(pGamePlayer, nIdx);
		m_enUserState[nIdx] = enPlayerSettled;
	}

	memset(&(m_UserInteractData[nIdx]), 0, sizeof(UserInteractData));
	memset(&(m_UserPreInteractData[nIdx]), 0, sizeof(UserInteractData));

	m_dwUserID[nIdx] = 0;									//DBID置为0
	m_lDrawnCount[nIdx] = 0;								//平局次数
	m_enUserState[nIdx] = enPlayerNull;				        //标记无玩家状态
	m_nTableUserCount--;
	memset(m_userName, 0, sizeof(m_userName));
	m_UserStatisticsData[nIdx].Clear();

	m_dbUserWelFarePoint[nIdx] = 0;            //同上
	m_dbUserPreWelFarePoint[nIdx] = 0;            //同上

	//获得本桌内的玩家个数
	int nUserCount = m_pList->GetValidUserNumber();
	//桌子内没有玩家了就关闭所有的TIMER
	if (0 == nUserCount)
	{
		//杀掉所有计时器
		//	KillAllTimer();       2015.5.6
		m_bIsPlaying = false;
		//游戏结束 将本桌子的奖池写入配置奖池中
		//	RecordDeskPoolToIni(m_nGameRoomID);

	}

	Statistic_Change(pGamePlayer, nIdx);
	Statistic_End(pGamePlayer, nIdx);

	switch (nIdx)
	{
	case 0:
		KillGameTimer(TIME_INTERVAL_PLAYER_ONE);
		break;
	case 1:
		KillGameTimer(TIME_INTERVAL_PLAYER_TWO);
		break;
	case 2:
		KillGameTimer(TIME_INTERVAL_PLAYER_THREE);
		break;
	case 3:
		KillGameTimer(TIME_INTERVAL_PLAYER_FOUR);
		break;
	default:
		break;
	}

	WriteInfoLog("玩家离开游戏");

	m_bEnableInfoLog = false;
	m_iLogID = 0;
	CMonsterManager::Instance().Detach(m_nTableID);

	return S_OK;
}

STDMETHODIMP CGameServer::OnUserOffline(int nIdx, IGamePlayer * pGamePlayer)
{
	TRACEDEBUG("Player=%d OnUserOffline", nIdx);
	//OnUserExit(nIdx, pGamePlayer);	// * /LeonLv add:离线做退出处理，25Feb.2011
	return E_NOTIMPL;
}

STDMETHODIMP CGameServer::OnUserReplay(int nIdx, IGamePlayer * pGamePlayer)
{
	TRACEDEBUG("Player=%d OnUserReplay", nIdx);

	// 顶号登录时，重新刷新当前金币数量
	SendUserMoneyCountTo(nIdx, pGamePlayer, true);

	return S_OK;	// * /OnUserEnter(nIdx, pGamePlayer);
}

STDMETHODIMP CGameServer::OnUserReady(int nIdx, IGamePlayer * pGamePlayer)
{
	return S_OK;
}
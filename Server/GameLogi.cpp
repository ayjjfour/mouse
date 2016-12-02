// SampleServer.cpp: implementation of the CGameServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Protocol/GameMessageTags.h"
#include "Protocol/GameMessage.pb.h"

#include <math.h>
#include "serverSite.h"
#include "CharSetHelper.h"
#include "Gameserver.h"
#include "GameLogi.h"
#include "MathAide.h"
#include <time.h>
#include "tinyxml2.h"

#include "Monster.h"
#include "MonsterManager.h"

using namespace tinyxml2;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


EXPOSE_STD_CREATETABLESERVER(CGameServer, GAME_ID)

template<bool b> struct Complier_Error;
template<> struct Complier_Error<true> { };

CGameServer::CGameServer()
{
	m_pList =NULL;
	m_pServer =NULL;
	m_pList =NULL;
	
	m_pPrize = NULL;	// * /彩池接口

	m_nTableUserCount = 0;

	m_bEnableInfoLog = false;

	Clear();

	int x = sizeof(tagFishingData);
	wild_skill_status = false;
}

CGameServer::~CGameServer()
{
	SAFE_RELEASE(m_pList);
	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pStatistic);
	// TODO:释放其他资源

}

//IUnknown
HRESULT STDMETHODCALLTYPE CGameServer::QueryInterface( 
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
		if (ppvObject==NULL) return E_INVALIDARG;

		if (riid==IID_IGameLogical || riid==IID_IUnknown) *ppvObject=static_cast<IGameLogical *>(this);
		else if (riid==IID_IGameConst) *ppvObject=static_cast<IGameConst *>(this);
		else if (riid==IID_IPlayerIOEvent) *ppvObject =static_cast<IPlayerIOEvent *>(this);					// * /
		//////////////////////////////////////////////////////////////////////////
		else if (riid==IID_INewTimer)
		{
			*ppvObject=static_cast<INewTimer*>(this);
		}
		// * /托管查询接口实例化
		else if(riid == IID_IGameQuery)
		{ 
			*ppvObject = static_cast<IGameQuery*>(this);
		}
		// * /End
		else {
			*ppvObject=NULL;
			return E_NOINTERFACE;
		}

		AddRef();

		return S_OK;
}

// IGameConst
STDMETHODIMP CGameServer::GetEstimateTime(DWORD * pMillesecond)
{
	// TODO:填写游戏运行的估计时间。如果少于这个时间的1/5，那么系统不会计分。直接返回E_NOTIMPL可以关闭这个检查

	return S_OK;
}

STDMETHODIMP CGameServer::GetPlayerNum(IGameConst::PLAYERNUMBER * pNumber)
{
	ASSERT(pNumber);

	// TODO:填写玩家的数量
	pNumber->nMax =1;
	pNumber->nStart =1;
	pNumber->nMaintenance =1;

	return S_OK;
}

STDMETHODIMP CGameServer::GetGameIdentifier(char * lpszIdentifier)
{
	lstrcpyn(lpszIdentifier, "Game", 255);
	return S_OK;
}

STDMETHODIMP CGameServer::CanQuickStart()
{
	return S_OK;
}

STDMETHODIMP CGameServer::CanSaveGame()
{
	return E_NOTIMPL;
}

STDMETHODIMP CGameServer::GetVersion(IGameConst::VERSION_STRUCT * pVS)
{
	ASSERT(pVS);

	// TODO: 填写内部版本号
	// 这个号码用于避免玩家使用老的客户端来连入新的服务器
	pVS->curVersion = MAKEWORD(0,1);
	pVS->maxVersion = MAKEWORD(0,1);
    pVS->minVersion = MAKEWORD(0,1);
	
	return S_OK;
}

// * /实例化彩池接口
STDMETHODIMP CGameServer::SetPrizeInterface(IPrizeFund * pPrize)
{
	return S_OK;
}

void CGameServer::SetAdditionalData(DWORD dwUserID, void* apData, int size)
{

}

// * /IGameQuery接口：托管代打查询接口
//返回值非S_OK，表示不支持该查询
HRESULT CGameServer::QueryGameInfo(DWORD dwMark, LPVOID lpData)
{
	if(lpData == NULL)
		return E_FAIL;

	EnableLeaveQuery *stELQ = NULL;

	StatisticBuffer  *sbBuf = NULL;

	IGamePlayer * lpPlayer = NULL;

	SRoomTotal	* m_stRoomTotal = NULL;

	int lproomsize = 0;

	//LONGLONG temp[6];


	stUserMoney	temllSysIncome;
	stUserMoney	temllSysExpense;
	LONGLONG	temllSumAvoidHitGetScore		=  0 ;
	LONGLONG	temllBuySkillCost				=  0 ;
	LONGLONG	temllBuySkillReturn				=  0 ;

	//dwMark = 25;   //test
	
	switch(dwMark)
	{
		case IID_IsEnableLeave:
			stELQ = (EnableLeaveQuery *)lpData;

			m_pList->GetUser(stELQ->m_ChairID, &lpPlayer);

			//TRACEDEBUG(LOGID_ScoreBugTrace(m_nRoomID), "QueryGameInfo:EnableLeave DBID=%d", lpPlayer->get_DBID());

			//在这里处理玩家下分包括断线消息
			MoblieDealUserRemoveSocer(stELQ,lpPlayer);

			stELQ->m_EnableLeave = 1;

			return S_OK;
		case IGameQuery::IID_GameEndStatus:				//请求更新场景
		{
			return S_OK;
		}	
		case IID_GetGameSupportNo:	// * /捕鱼类游戏需要共享数据块时支持
			*((int*)lpData) = emSupptFishData;
			return S_OK;
		default:
			break;
	}
	
	return E_FAIL;
}

//即将下次触发的计时器    //TODO
int CGameServer::GetComingTimerTick(DWORD &dwComingID)
{
	int nComingTick = STANDARD_TIME_ELAPSE;
	int nMinTick = MAX_TICK_COUNT;                   //最大计时器毫秒数  1000000000
	int i = 0, nNextElapse = 0;
	DWORD dwCurTick = m_dwCurTick; //GetTickCount();
	//LOGI("当前tick %d",m_dwCurTick);

	dwComingID = 0;
	for (i = 1; i<= TIMER_COUNT; ++i)         //计时器个数 = 12
	{
		if (TRUE == m_arTimerManage[i].bEnable && 0 != m_arTimerManage[i].dwRepeat)
		{
			nNextElapse = (m_arTimerManage[i].dwSetTick + m_arTimerManage[i].dwElapse) - dwCurTick;       //下次时间间隔
			if (nNextElapse < nMinTick)
			{
				nMinTick = nNextElapse;             //找到下次时间间隔中的最小值
				dwComingID = i;	                    //保存最小值对应的i
			}	
		}
	}
	if (nMinTick < 200)
	{
		nMinTick = 200;                              //最小毫秒数不能低于200ms
	}
	if (nComingTick > nMinTick)						//
	{
		nComingTick = nMinTick;
	}
	return nComingTick;
}

// * /查询应该激活的计时器
DWORD CGameServer::CheckActiveTimer()
{
	DWORD dwNoTimerID = 0;
	int nCurElapse = 0;
	DWORD dwCurTick = m_dwCurTick;	
	DWORD dwComingID = 0;

	if (GetComingTimerTick(dwComingID) > STANDARD_TIME_ELAPSE)         
		return dwNoTimerID;

	if (0 == dwComingID)
		return dwNoTimerID;

	nCurElapse = dwCurTick - m_arTimerManage[dwComingID].dwSetTick;				

	if (nCurElapse >= (int)m_arTimerManage[dwComingID].dwElapse)
	{
		m_arTimerManage[dwComingID].dwSetTick = dwCurTick;
		if (INFINITE != m_arTimerManage[dwComingID].dwRepeat)
		{
			m_arTimerManage[dwComingID].dwRepeat -= 1;                             
			if (m_arTimerManage[dwComingID].dwRepeat <= 0)
			{
				m_arTimerManage[dwComingID].bEnable = FALSE;
				m_arTimerManage[dwComingID].dwRepeat = 0;
			}
		}

		return dwComingID;
	}

	return dwNoTimerID;
}

//模仿源码计时器接口
//设置定时器
bool CGameServer::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	if (dwTimerID< 0 || dwTimerID > TIMER_COUNT || dwElapse < 0 || dwRepeat <= 0)
	{
		return false;
	}
	m_arTimerManage[dwTimerID].bEnable = TRUE;            //          计时器有效
	m_arTimerManage[dwTimerID].dwElapse = dwElapse;					// 
	m_arTimerManage[dwTimerID].dwRepeat = dwRepeat;					//
	m_arTimerManage[dwTimerID].wBindParam = wBindParam;
	m_arTimerManage[dwTimerID].dwSetTick = m_dwCurTick;	//

	return true;
}

//重新设置计时器时延
bool CGameServer::ReSetGameTimer(DWORD dwTimerID, DWORD dwElapse)
{
	if (dwTimerID< 0 || dwTimerID > TIMER_COUNT || dwElapse < 0)
	{
		return false;
	}
	m_arTimerManage[dwTimerID].bEnable = TRUE;     //
	m_arTimerManage[dwTimerID].dwElapse = dwElapse;   //
	return true;
}

//删除定时器
bool CGameServer::KillGameTimer(DWORD dwTimerID)
{
	int i = 0;
	bool bStop = true;
	if (dwTimerID< 0 || dwTimerID > TIMER_COUNT)
	{
		return false;
	}
	m_arTimerManage[dwTimerID].bEnable = FALSE;                  //
	m_arTimerManage[dwTimerID].dwRepeat = 0;				

	for (i = 1; i<= TIMER_COUNT; ++i)
	{
		if (TRUE == m_arTimerManage[i].bEnable)
		{
			bStop = false;
			break;
		}
	}
	return true;
}

void CGameServer::OnTimePass()          //定时器事件
{
	DWORD dwTimerID = CheckActiveTimer();
	DWORD wBindParam = 0;
	DWORD dwNextTick = STANDARD_TIME_ELAPSE;
	int nMinTick = STANDARD_TIME_ELAPSE;
	int i = 0;

	// 	_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("m_started %d,dwtimerID:%d"), m_started,dwTimerID);
	// 	m_LogFile.WriteErrorLogFile( m_szErrorInfo );

	//	LOGI("调用定时器事件");

	//合格的计时器才有效
	if (0 < dwTimerID && dwTimerID <= TIMER_COUNT)
	{
		wBindParam = m_arTimerManage[dwTimerID].wBindParam;
	}


	m_dwCurTick = GetTickCount();
	// 		1		//添加鱼
	// 		2		//产生鱼轨迹
	// 		3		//清除鱼轨迹
	// 		4		//切换场景
	// 		5		//生成规则鱼群
	// 		6		//加载配置文件
	// 		7		//检查奖池
	// 		8		//预写入分数
	try
	{
		//事件处理
		// 						char lBuffer[512];
		// 				 		sprintf(lBuffer, "-------------------------------%s\n", lMessage.DebugString().c_str());
		// 				 		OutputDebugString(lBuffer);

		//  		char lBuffer[512];
		//  		sprintf(lBuffer, "------------触发事件编号%d  tickcount %u---------\n", dwTimerID, GetTickCount());
		//  		OutputDebugString(lBuffer);

		//  		if (dwTimerID == 2 || dwTimerID ==5)     
		//  		{
		//  			dwTimerID = 1;
		//  		}

		for (int i = 0 ; i<PLAY_COUNT; i++)
		{
			// 玩家刚进入时设为 enPlayerEnter， 上分（转换为游戏内积分）完毕时设为 enPlayerBuy（即正常游戏状态）
			// 正常游戏状态下，一段时间不开枪会强制踢下线
			if ((m_dwCurTick > (m_UserStatisticsData[i].dwLastHitTime + 200*1000)) && (m_enUserState[i] == enPlayerBuy))
			{
				IGamePlayer *lpPlayer = NULL;  

				m_pList->GetUser(i,&lpPlayer);

				TRACEWARNING("=====KillOffTest : TimeOut in 200s, Player=%d, curMoney=%d, preMoney=%d", lpPlayer->get_DBID(), m_UserGameScore[i], m_UserPreGameScore[i]);
				m_pSite->GetOff(lpPlayer->get_ID());

			}
		}     

		//LOGI("m_dwCurTick=%ld, dwTimerID=%d, TableID=%d", m_dwCurTick, dwTimerID, (*((TableServer.exe!BaseObject*)(&(*((TableServer.exe!BaseContainer<Chair, GamePlayer>*)(&(*((TableServer.exe!PveTable*)((this)->m_pServer))))))))).m_ID);

		switch (dwTimerID)
		{
		case TIME_ADD_MONSTER:			//产生鱼
			{
				CreateMonster();
				break;
			}
		case TIME_ADD_GROUP_MONSTER:	//创建鱼轨迹
			{
				//创建鱼轨迹
				CreateGroupMonsters();                 
				break;
			}
		case TIME_CLEAR_MONSTER:	//清除鱼
			{
				OnTimerDestroyMonsters();
				break;
			}
		case TIME_CHANGE_SCENE:	//切换场景
			{
				//发送切换场景消息
				OnTimerChangeScene();
				break;
			}
		//case TIME_LOAD_FILE:	//加载配置文件  TODO: 待实现部分更新，或者修改了哪部分更新哪部分
		//	{
		//		bool isModified = GameConfigMgr::getInstance().Current().ReloadConfig();
		//		if (isModified)
		//		{
		//			TRACEDEBUG("房间[%d]的配置文件修改了！ 重新加载 WhacAMole.xml", m_nRoomID);

		//			GameConfigMgr::getInstance().Current().LoadConfig();

		//			StatisticMgr::getInstance().DumpConfig(m_config, m_nTableID);
		//		}	
		//		break;
		//	}
		case TIME_CHECK_POOL:	//5秒检查奖池
			{
				OnTimerCheckPool();
				break;
			}
		case TIME_WILD_SKILL_WILL_END://
			{
				wild_skill_will_end();
				break;
			}
		case TIME_WILD_SKILL_WAIT_STEP:
			{
				wild_skill_wait_step();
				break;
			}
		case TIME_WILD_SKILL_DELEY:
			{
				wild_skill_timer_deley();
				break;
			}
		case TIME_WILD_SKILL_END:
			{
				wild_skill_state_end();
				break;
			}
		case TIME_WILD_END_CREATE_MONSTER:
			{
				CreateMonster();
				CreateGroupMonsters();
				KillGameTimer(TIME_WILD_END_CREATE_MONSTER);
				break;
			}
		case TIME_INTERVAL_PLAYER_ONE:
			if (enPlayerNull != m_enUserState[0])
			{
				CalcPreScore(0);
				Statistic_Change(NULL, 0);
			}
			break;
		case TIME_INTERVAL_PLAYER_TWO:
			if (enPlayerNull != m_enUserState[1])
			{
				CalcPreScore(1);
				Statistic_Change(NULL, 1);
			}
			break;
		case TIME_INTERVAL_PLAYER_THREE:
			if (enPlayerNull != m_enUserState[2])
			{
				CalcPreScore(2);
				Statistic_Change(NULL, 2);
			}
			break;
		case TIME_INTERVAL_PLAYER_FOUR:
			if (enPlayerNull != m_enUserState[3])
			{
				CalcPreScore(3);
				Statistic_Change(NULL, 3);
			}
			break;
		default:
			break;
		}
		// 是否需要立即再触发计时器，0xFeb.2011
		nMinTick = GetComingTimerTick(dwNextTick);	//精度200ms
		if (nMinTick < STANDARD_TIME_ELAPSE)
		{
			m_pServer->StartTimer(nMinTick);
		}
		else
		{
			m_pServer->StartTimer(STANDARD_TIME_ELAPSE);
		}
	}
	catch (...)
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("Error: OnTimePass(%d) catch!"), dwTimerID);
		m_LogFile.WriteErrorLogFile( m_szErrorInfo);

		WriteInfoLog(_T("Error: OnTimePass(%d) catch!"));
	}
}

void	CGameServer::RecordStartTime()
{
	CTime Cttm;
	CString sDay;
	Cttm=CTime::GetCurrentTime();
	sDay  = Cttm.Format(("%Y%m%d"));
	//暂时注释购买代码的框架	G_iRecPreDayTime  = StrToInt(sDay);
}

void CGameServer::UpdateRecordLogPath()
{
	//LOGI("Old RecordLogPath: %s", m_RecordLogPath);

	m_LastTime = 0;
	memset(m_RecordLogPath,0,sizeof(m_RecordLogPath));
	//tm t;
	//localtime_s(&t,&m_LastTime);
	//_snprintf(m_RecordLogPath,sizeof(m_RecordLogPath)-1, "D:\\GameServerLog\\2pMJGameRecordLog\\");
	//GetRecordLogPath("GameServerLog\\WhacAMoleGameRecordLog\\",m_RecordLogPath,sizeof(m_RecordLogPath));

	_snprintf(m_RecordLogPath,sizeof(m_RecordLogPath)-1, AfxGetApplicationDir() + "\\GameServerLog\\WhacAMoleGameRecordLog\\");

	//LOGI("New RecordLogPath: %s", m_RecordLogPath);
}

bool CGameServer::GenerateRecordLogName(std::string& aName)
{
	// 	IGamePlayer* lpGamePlayer1 = NULL;
	// 	IGamePlayer* lpGamePlayer2 = NULL;
	// 	m_pList->GetUser(0, &lpGamePlayer1);
	// 	m_pList->GetUser(1, &lpGamePlayer2);
	// 
	// 	if(lpGamePlayer1 != NULL && lpGamePlayer2 != NULL) {
	// 		char lBuffer[256] = {0};
	// 
	// 		_snprintf(lBuffer, sizeof(lBuffer) - 1, "%d_%d_%s"
	// 			, lpGamePlayer1->get_DBID()
	// 			, lpGamePlayer2->get_DBID()
	// 			, m_GameNo);
	// 
	// 		aName = lBuffer;
	// 
	// 		return true;
	// 	}
	// 
	// 	return false;
	char lBuffer[256] = {0};

	_snprintf(lBuffer, sizeof(lBuffer) - 1, "%s", m_GameNo);

	aName = lBuffer;

	return true;
}

/**
* 停止所有计时器	
*/
void CGameServer::KillAllTimer()
{
	KillGameTimer(TIME_ADD_MONSTER);													//添加鱼
	KillGameTimer(TIME_ADD_GROUP_MONSTER);												//产生鱼轨迹
	KillGameTimer(TIME_CLEAR_MONSTER);											   //清除鱼轨迹
	KillGameTimer(TIME_CHANGE_SCENE);                                        //切换场景
	KillGameTimer(TIME_CHECK_POOL);											//加载配置文件	
	//KillGameTimer(TIME_LOAD_FILE);											//检查奖池                    

	m_pServer->StopTimer();
	return ;
}

/**
* 数据初始化
*/
void CGameServer::InitData()
{
	m_bIsPlaying = false;

	m_nSceneIndex = 0;
	m_llSumIntervalIncome = 0;
	m_llSumIntervalExpense.Clear();
	m_llSumIncome = 0;              
	m_llSumExpense.Clear();				
	m_llSumAvoidHitSystemGetScore = 0;

	m_nStatictisAvoidCounts = 0;		//免伤判断次数
	m_nStatictisAvoidValidCounts = 0;		//免伤有效次数   
	memset(m_nStatictisMonsterKillCounts, 0, sizeof(m_nStatictisMonsterKillCounts));
	memset(m_nStatictisMonsterIncomingCounts, 0, sizeof(m_nStatictisMonsterIncomingCounts));

	memset(m_UserStatisticsData, 0, sizeof(m_UserStatisticsData));
	memset(m_nMonsterPlayerCost, 0, sizeof(m_nMonsterPlayerCost));
	memset(m_nMonsterPlayerIncome, 0, sizeof(m_nMonsterPlayerIncome));
	memset(m_nBroadcastTime, 0, sizeof(m_nBroadcastTime));

}

void CGameServer::CreateMonster()
{
    if (m_nTableUserCount <= 0)
        return;

	if (!m_UserStatisticsData[0].bClientReady)
	{
		WriteInfoLog("CGameServer::CreateGroupMonsters 刷组合怪，但是玩家客户端并未准备好！");
		//return;
	}

    //LOGI("nRoomID=%d, nTableID=%d, nTableUserNum=%d", m_nRoomID, m_nTableID, m_nTableUserCount);

    /*
    CMonster* pMonster = CMonsterManager::Instance().CreateMonster(m_nTableID);
    if (pMonster == nullptr)
        return;

    //LOGI("CreateMonster : ID=%d, Type=%d, RoomID=%d, TableID=%d, nTableUserNum=%d", pMonster->id(), pMonster->monsterType(), m_nRoomID, m_nTableID, m_nTableUserCount);

    SY::MonsterTrace syTrace;
    //设置变量

    syTrace.set_monsterid(pMonster->id());
    syTrace.set_typeid_(pMonster->monsterType());
    syTrace.set_curhp(pMonster->curHP());
    syTrace.set_maxhp(pMonster->totalHP());
    syTrace.set_speed(pMonster->moveSpeed());

    // 地鼠类需要设置 RespawnIndex， 非地鼠需要设置 3个Point，不设置时为空
    if (CMonsterManager::IsMole(pMonster->monsterType()))
    {
        syTrace.set_respawnindex(pMonster->holeidx());
    }
    else    
    {
        CFly* pFly = (CFly*)pMonster;
        SY::Point* pt = syTrace.add_respawnposition();
        pt->set_x(pFly->pt_respawn().x);
        pt->set_y(pFly->pt_respawn().y);

        pt = syTrace.add_movedestination();
        pt->set_x(pFly->pt_move().x);
        pt->set_y(pFly->pt_move().y);

        pt = syTrace.add_escapsedestination();
        pt->set_x(pFly->pt_escape().x);
        pt->set_y(pFly->pt_escape().y);
    }

    syTrace.set_delaymovetime(pMonster->waittoescapetime());
    syTrace.set_delayescapetime(pMonster->escapetime());
    
    char buf[200] = {0};  
    if (syTrace.SerializeToArray(buf,sizeof(buf)))
    {
        sendGameCmdProtBufAll(GameMessages::CreateMonsterTrace, buf, syTrace.ByteSize());
    }
    */


    list<CMonster*> lstMonster;
	float create_monster_speed = 1.0;
	const RoomConfig * room_config = GameConfigMgr::getInstance().Current().get_room_config_int_info(m_nRoomID);
	if(NULL != room_config)
	{
		create_monster_speed = room_config->create_monster_speed_rate;
		if(wild_skill_status)
			create_monster_speed *= get_wild_config_val(WILD_SKILL_CREATE_MONSTER_SPEED); 
	}
	bool wild_monster_hp = false;
#ifdef WILD_MONSTER_HP
	if(wild_skill_status)
		wild_monster_hp = true;
#endif
    CMonsterManager::Instance().CreateMonster(m_nTableID, lstMonster,wild_monster_hp,create_monster_speed);
    if (lstMonster.empty())
        return;

    SY::MonsterTraces syGroupTrace;
    syGroupTrace.set_isgroup(true);

    //设置变量
    for (auto it = lstMonster.begin(); it != lstMonster.end(); ++it)
    {
        CMonster* pMonster = *it;

        SY::MonsterTrace* syTrace = syGroupTrace.add_traces();
        syTrace->set_monsterid(pMonster->id());
        syTrace->set_typeid_(pMonster->monsterType());
        syTrace->set_curhp(pMonster->curHP());
        syTrace->set_maxhp(pMonster->totalHP());
//        syTrace->set_speed(pMonster->moveSpeed());
		syTrace->set_speed(pMonster->moveSpeed());
		if(wild_skill_status)//把狂暴怪加入到map里去
			wild_skill_monster_map[pMonster->id()] = 0;
		syTrace->set_speedrate((wild_skill_status)?get_wild_config_val(WILD_SKILL_SPEED):-1);

        // 地鼠类需要设置 RespawnIndex， 非地鼠需要设置 3个Point，不设置时为空
        if (CMonsterManager::IsMole(pMonster->monsterType()))
        {
            syTrace->set_respawnindex(pMonster->holeidx());
        }
        else
        {
            CFly* pFly = (CFly*)pMonster;
            SY::Point* pt = syTrace->add_respawnposition();
            pt->set_x(pFly->pt_respawn().x);
            pt->set_y(pFly->pt_respawn().y);

            pt = syTrace->add_movedestination();
            pt->set_x(pFly->pt_move().x);
            pt->set_y(pFly->pt_move().y);

            pt = syTrace->add_escapsedestination();
            pt->set_x(pFly->pt_escape().x);
            pt->set_y(pFly->pt_escape().y);
        }

        syTrace->set_delaymovetime(pMonster->waittoescapetime());
        syTrace->set_delayescapetime(pMonster->escapetime());
    }

    char buf[1024] = { 0 };
    if (syGroupTrace.SerializeToArray(buf, sizeof(buf)))
    {
        sendGameCmdProtBufAll(GameMessages::CreateMonsterTraces, buf, syGroupTrace.ByteSize());
    }

}

void CGameServer::CreateGroupMonsters()
{
    if (m_nTableUserCount <= 0)
        return;

	if (!m_UserStatisticsData[0].bClientReady)
	{
		WriteInfoLog("CGameServer::CreateGroupMonsters 刷组合怪，但是玩家客户端并未准备好！");
		//return;
	}

    list<CMonster*> lstMonster;
	float create_monster_speed = 1.0;
	const RoomConfig * room_config = GameConfigMgr::getInstance().Current().get_room_config_int_info(m_nRoomID);
	if(NULL != room_config)
	{
		create_monster_speed = room_config->create_monster_speed_rate;
		if(wild_skill_status)
			create_monster_speed *= get_wild_config_val(WILD_SKILL_CREATE_MONSTER_SPEED); 
	}
	bool wild_monster_hp = false;
#ifdef WILD_MONSTER_HP
	if(wild_skill_status)
		wild_monster_hp = true;
#endif
    CMonsterManager::Instance().CreateGroupMonster(m_nTableID, 10, lstMonster,wild_monster_hp,create_monster_speed);

    //LOGI("CreateGroupMonster : CreateNum=%d, RoomID=%d, TableID=%d, nTableUserNum=%d", lstMonster.size(), m_nRoomID, m_nTableID, m_nTableUserCount);
 
    if (lstMonster.empty())
        return;    

    SY::MonsterTraces syGroupTrace;
    syGroupTrace.set_isgroup(true);
    
    //设置变量
    for (auto it = lstMonster.begin(); it != lstMonster.end(); ++it)
    {
        CMonster* pMonster = *it;

        SY::MonsterTrace* syTrace = syGroupTrace.add_traces();
        syTrace->set_monsterid(pMonster->id());
        syTrace->set_typeid_(pMonster->monsterType());
        syTrace->set_curhp(pMonster->curHP());
        syTrace->set_maxhp(pMonster->totalHP());
        syTrace->set_speed(pMonster->moveSpeed());
		if(wild_skill_status)//把狂暴怪加入到map里去
			wild_skill_monster_map[pMonster->id()] = 0;
		syTrace->set_speedrate((wild_skill_status)?get_wild_config_val(WILD_SKILL_SPEED):-1);
        // 地鼠类需要设置 RespawnIndex， 非地鼠需要设置 3个Point，不设置时为空
        if (CMonsterManager::IsMole(pMonster->monsterType()))
        {
            syTrace->set_respawnindex(pMonster->holeidx());
        }
        else
        {
            CFly* pFly = (CFly*)pMonster;
            SY::Point* pt = syTrace->add_respawnposition();
            pt->set_x(pFly->pt_respawn().x);
            pt->set_y(pFly->pt_respawn().y);

            pt = syTrace->add_movedestination();
            pt->set_x(pFly->pt_move().x);
            pt->set_y(pFly->pt_move().y);

            pt = syTrace->add_escapsedestination();
            pt->set_x(pFly->pt_escape().x);
            pt->set_y(pFly->pt_escape().y);
        }

        syTrace->set_delaymovetime(pMonster->waittoescapetime());
        syTrace->set_delayescapetime(pMonster->escapetime());
    }

    char buf[1024] = { 0 };
    if (syGroupTrace.SerializeToArray(buf, sizeof(buf)))
    {
        sendGameCmdProtBufAll(GameMessages::CreateMonsterTraces, buf, syGroupTrace.ByteSize());
    }
}

void CGameServer::OnTimerDestroyMonsters()
{
// 	CMonsterPool* pool = CMonsterManager::Instance().GetMonsterPool(m_nTableID);
// 	if (!pool)
// 		return;
// 	vector<DWORD> lstMonster;
// 	pool->CheckMonsterToDestroy(lstMonster);
// 
// 	if (lstMonster.empty())
// 		return;
// 	for(auto it = lstMonster.begin();it != lstMonster.end();++it)
// 	{
// 		std::map<DWORD,int>::iterator itmap = wild_skill_monster_map.find(*it);
// 		if(itmap != wild_skill_monster_map.end())
// 			wild_skill_monster_map.erase(itmap);
// 	}
	WriteInfoLog(_T("开始清理无效的耗子"));

	CMonsterManager::Instance().CheckMonsterToDestroy(m_nTableID);

	//for (int i = 0; i < MAX_FISH_TRACE_COUNT; i++)
	//{
	//    if (m_stFishTrace[i][0].bExist == false)
	//    {
	//        continue;
	//    }
	//    //获取当前时间
	//    DWORD dwNowTime = GetTickCount();

	//    //当前时间 >= 鱼儿生成的时间+鱼儿存活的时间  就要及时清除掉该鱼
	//    if (dwNowTime >= (m_stFishTrace[i][0].nBuildTime + FISH_ALIVE_TIME))
	//    {
	//        for (int j = 0; j < 4; j++)
	//        {
	//            m_llLossPoolCount[j] += m_stFishTrace[i][0].nHitCountID[j];  //其它玩家击中子弹按个数进入亏损池
	//                                                                         // 								_sntprintf(m_szDatalogInfo , _countof(m_szDatalogInfo) ,
	//                                                                         // 									TEXT("1142 :%I64d"), 
	//                                                                         // 									m_llLossPoolCount[j]);
	//                                                                         // 								GLOGFI(1,m_szDatalogInfo);
	//            m_stFishTrace[i][0].nHitCountID[j] = 0;     //清除非捕获玩家击中数
	//            m_stFishTrace[i][0].bCountedFull[j] = false;
	//            m_stFishTrace[i][0].bMustdieZero = false;
	//        }
	//        m_stFishTrace[i][0].bExist = false;
	//        //	OUTLOOK("因存活时间到此鱼置为无效:%d",m_stFishTrace[i][0].nFishID);
	//    }
	//}
}

//发送切换场景消息	
void	CGameServer::OnTimerChangeScene()
{
	m_nSceneIndex++;	//场景ID
	if(m_nSceneIndex>=3)
	{
		m_nSceneIndex=0;
	}
	//变量定义

	SY::ChangeScene syChangeScene;

	//设置变量

	syChangeScene.set_sceneindex(m_nSceneIndex);


	//发送场景切换消息
	char buf[200] = {0};
	if (syChangeScene.SerializeToArray(buf, sizeof(buf)))      //s11
	{
		sendGameCmdProtBufAll(GameMessages::ChangeScene, buf, syChangeScene.ByteSize());
	}

	//杀死添加鱼计时器(防止刚切换场景就发送新的鱼群)
	//KillGameTimer(TIME_ADD_MONSTER);   //TODO: 切换场景后过一段时间再开始创建鱼
	//KillGameTimer(TIME_ADD_GROUP_MONSTER);	

	//规则鱼群
	//SetGameTimer(TIME_REGULAR_FISH,10*1000,INFINITE, 0L);

}


//2016.10.20 hl 狂暴状态计时器接口和其他相关接口 start

//狂暴状态结束
void CGameServer::wild_skill_state_end()
{
	//通知客户端结束了  进入下一轮狂暴计时
	KillGameTimer(TIME_WILD_SKILL_END);
	make_tip_info(WILD_SKILL_STATUS_END,player_index);
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		SetGameTimer(TIME_WILD_SKILL_DELEY,5*1000,INFINITE,0L);//延时5秒
//		SetGameTimer(TIME_WILD_SKILL_WAIT_STEP,DEFAULT_WILD_SKILL_ELSPSE,INFINITE,0L);
//		wild_skill_wait_step();
	}
	change_monster_time();
	wild_skill_blank = 0;
	wild_skill_status = false; //狂暴结束，费率恢复正常
	SetGameTimer(TIME_WILD_END_CREATE_MONSTER,1000,INFINITE,0L);//避免在狂暴后怪没有了，定时器重开，导致没怪，强制走一波刷怪流程
}
//狂暴状态即将结束
void CGameServer::wild_skill_will_end()
{
	//通知客户端还剩下最后3秒了
	KillGameTimer(TIME_WILD_SKILL_WILL_END);
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		SetGameTimer(TIME_WILD_SKILL_END,DEFAULT_WILD_SKILL_PRETIP_TIME*1000,INFINITE,0L);
		make_tip_info(WILD_SKILL_STATUS_WILL_END,player_index);
		return ;
	}
	{
		change_monster_time();
		wild_skill_blank = 0;
		wild_skill_status = false;
		make_tip_info(WILD_SKILL_STATUS_END,player_index);
	}
}
//狂暴状态倒计时阶段
void CGameServer::wild_skill_wait_step()
{
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
//		static int wild_skill_blank = 0;
		if(0 == wild_skill_blank--)
		{
			wild_skill_blank = get_wild_config_val(WILD_SKILL_BLANK_TIME); //触发间隔为30秒 配置时间为60*n
		}
//		if(!((wild_skill_blank)%2) && wild_skill_blank)
		if(2 == wild_skill_blank)
		{
			//在这里做整点报时 通知客户端  2分钟整点报告
			make_tip_info(WILD_SKILL_STATUS_BLANK,player_index,wild_skill_blank);
			return ;
		}
		else if(1 == wild_skill_blank && 1 == get_wild_config_val(WILD_SKILL_BLANK_TIME))
		{
			//如果设置只有1分钟就进入狂暴，则在此通知
			make_tip_info(WILD_SKILL_STATUS_BLANK,player_index,wild_skill_blank);
		}

		else if(0 == wild_skill_blank)
		{	//通知客户端进入狂暴状态
			KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);
			SetGameTimer(TIME_WILD_SKILL_WILL_END,(get_wild_config_val(WILD_SKILL_CONTINUE_TIME)-DEFAULT_WILD_SKILL_PRETIP_TIME)*1000,INFINITE,0L);
			make_tip_info(WILD_SKILL_STATUS_START,player_index);
			wild_skill_monster_map.clear();//清除上次狂暴怪
			wild_skill_status = true;
			change_monster_time(get_wild_config_val(WILD_SKILL_CREATE_MONSTER_SPEED));
			SetGameTimer(TIME_WILD_END_CREATE_MONSTER,1000,INFINITE,0L);
			return ;
		}
	}
	else
		KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);
}

float CGameServer::get_wild_config_val(GET_CONFIG_VAL type)
{
	const RoomConfig * room_config_wild_skill = GameConfigMgr::getInstance().Current().get_room_config_int_info(m_nRoomID);
	if(NULL == room_config_wild_skill)
		return 0;
	switch(type)
	{
		case WILD_SKILL_GET_RATE:
			return room_config_wild_skill->wild_skill_get_rate;
		case WILD_SKILL_PAY_RETE:
			return room_config_wild_skill->wild_skill_pay_rate;
		case WILD_SKILL_BLANK_TIME:
			return room_config_wild_skill->wild_skill_blank_time;
		case WILD_SKILL_CONTINUE_TIME:
			return room_config_wild_skill->wild_skill_continue_time;
		case WILD_SKILL_OPEN:
			return room_config_wild_skill->wild_skill;
		case WILD_SKILL_SPEED:
			return room_config_wild_skill->wild_skill_monster_speed;
		case WILD_SKILL_CREATE_MONSTER_SPEED:
			return room_config_wild_skill->wild_skill_create_monster_speed;
		default:
			return 0;
	}
}
void CGameServer::make_tip_info(const int wild_skill_status,const int player_index_id,const int remain_time/* = 0 */)
{
	SY::WildSkillStatus wild_skill_status_pb;
	char tip_pb_buf[1024] = {0};
	wild_skill_status_pb.set_wild_skill_status(wild_skill_status);
	wild_skill_status_pb.set_remain_time(remain_time);
	wild_skill_status_pb.set_wild_skill_cost_rate(get_wild_config_val(WILD_SKILL_PAY_RETE));
	if(WILD_SKILL_STATUS_START == wild_skill_status)
	{
		auto all_monster_id = CMonsterManager::Instance().get_all_monster_id(m_nTableID);
		for(auto it = all_monster_id.begin(); it != all_monster_id.end(); ++it)
		{
			SY::AllMonsterId * id_list = wild_skill_status_pb.add_wild_monster_id();
			id_list->set_id(*it);
		}
	}
	IGamePlayer * pPlayer = NULL;
	if(FAILED(m_pList->GetUser(player_index_id,&pPlayer)))
		return ;
	if (wild_skill_status_pb.SerializeToArray(tip_pb_buf, sizeof(tip_pb_buf)))     
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(),GameMessages::WildSkillStatus, tip_pb_buf, wild_skill_status_pb.ByteSize());
	}
}

void CGameServer::send_weapon_cost_ex(const int modify_cost,const int player_index_id)
{
	SY::WildSkillWeaponCostModify weapon_cost_modify;
	char pb_buf[100] = {0};
	weapon_cost_modify.set_wild_skill_weapon_cost_modify(modify_cost);
	IGamePlayer * pPlayer = NULL;
	if(FAILED(m_pList->GetUser(player_index_id,&pPlayer)))
		return ;
	if (weapon_cost_modify.SerializeToArray(pb_buf, sizeof(pb_buf)))     
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(),GameMessages::WildSkillWeaponCostModify,pb_buf, weapon_cost_modify.ByteSize());
	}
}
//2016.10.20 hl 狂暴状态计时器接口和其他相关接口 end
void CGameServer::OnTimerCheckPool()
{
	//if (m_nTableID == 0)
	//{
	//    std::string txt = CMonsterManager::Instance().GetPoolState(m_nTableID);

	//    char txtUTF8[MAX_MSG_LEN] = {0};
	//    CharSetHelper::ANSIToUTF8(txt.c_str(), txtUTF8, MAX_MSG_LEN);

	//    char txtANSI[MAX_MSG_LEN] = { 0 };
	//    CharSetHelper::UTF8ToANSI(txtUTF8, txtANSI, MAX_MSG_LEN);
	//    LOGI("PoolState: %s", txtANSI);
	//}

	//检查是否新的一天
	if (!IsSameDay(m_LastTime, time(0)))
	{
		buildGameNo();
		UpdateRecordLogPath();
		//日志记录文件初始化
		m_LogFile.WriteErrorLogFile("Table Initialize.", LOG_OPEN);
		m_LogFile.WriteDataLogFile("SetServerSite...", LOG_OPEN);

		m_LastTime = time(0);

		/*ILog4zManager::CreateInstance()->SpecifiedLogIdCloseLog(LOGID_Losspool(m_nRoomID));
		ILog4zManager::CreateInstance()->SpecifiedLogIdCloseLog(LOGID_Trace(m_nRoomID));
		ILog4zManager::CreateInstance()->SpecifiedLogIdCloseLog(LOGID_Statictis(m_nRoomID));

		ILog4zManager::CreateInstance()->SpecifiedLogIdCloseLog(LOGID_ScoreBugTrace(m_nRoomID));


		ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "losspool", 0, true, LOGID_Losspool(m_nRoomID));
		ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "trace", 0, true, LOGID_Trace(m_nRoomID));
		ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "Statictis", 0, true, LOGID_Statictis(m_nRoomID));

		ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "scorebugtrace", 0, true, LOGID_ScoreBugTrace(m_nRoomID));*/
	}
}

bool CGameServer::OnBuySkill(IGamePlayer *pPlayer, void *pData, int nSize)
{
	if (pData == NULL)
		return false;

	SY::BuySkillReq lMessage;   
	if (false == lMessage.ParseFromArray(pData, nSize))
	{
		return false;
	}

	int ChairID = pPlayer->get_chair(); // lMessage.chairid();
	SkillID skillID = (SkillID)lMessage.skillid();

	TRACEDEBUG("OnBuySkill: ChairID=%d, SkillID=%d", ChairID, skillID);

	// 先取技能状态，如果已经激活那就说明存在逻辑错误，  如客户端未判断或玩家存在作弊可能性
	enSkillBuyStatus buyStatus = m_UserInteractData[ChairID].GetSkillBuyStatus(skillID);
	if (buyStatus == enSBS_Buyed || buyStatus == enSBS_Payed)
	{
		TRACEFALTAL("OnBuySkill, Already BUYED!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
		return false;
	}

	SY::SkillStatus activeStatus = m_UserInteractData[ChairID].GetSkillActiveStatus(skillID);
	if (activeStatus == SY::Actived || activeStatus == SY::Disabled)
	{
		TRACEFALTAL("OnBuySkill, Already ACTIVED!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
		return false;
	}


	// 根据玩家等级判断是提前激活（购买），还是到等级的激活
	int curLevel = m_nUserLevel[ChairID];
	const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
	if (pSkillCostInfo == nullptr)
	{
		TRACEFALTAL("OnBuySkill, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d", m_nRoomID, skillID);
		return false;
	}

	bool isBuy = false; // 购买的情况需要返还
	bool canActive = false;

	int nActiveLevel = pSkillCostInfo->ActiveLevel;
	int nBuyCost = pSkillCostInfo->BuyCost;
	if (curLevel < nActiveLevel)
	{
		// 购买, 先判断钱是否足够
		if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
		{
			TRACEFALTAL("OnBuySkill, Money NOT ENOUGHT!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
			return false;
		}

		// 先扣钱，再激活
		m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
		if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

		m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;


		isBuy = true;
		canActive = true;

		TRACEDEBUG("BuySkill[%d] With Money, UserID=%d, BuyCost=%d, Level=%d", skillID, pPlayer->get_DBID(), nBuyCost, curLevel);
	}
	else
	{
		// 直接激活
		canActive = true;

		TRACEDEBUG("BuySkill[%d] For Free, UserID=%d, BuyCost=%d, Level=%d", skillID, pPlayer->get_DBID(), nBuyCost, curLevel);
	}

	if (canActive)
	{
		// 激活的时候，就设置购买状态，默认激活，1星
		if (isBuy)
		{
			m_UserInteractData[ChairID].SetSkillBuyStatus(skillID, enSBS_Buyed);
			m_UserPreInteractData[ChairID].nSkillBuyStatus = m_UserInteractData[ChairID].nSkillBuyStatus;
		}

		m_UserInteractData[ChairID].SetSkillActiveStatus(skillID, SY::Actived);
		m_UserPreInteractData[ChairID].nSkillActiveStatus = m_UserInteractData[ChairID].nSkillActiveStatus;

		m_UserInteractData[ChairID].SetSkillLevel(skillID, 1);
		m_UserPreInteractData[ChairID].nSkillLevel = m_UserInteractData[ChairID].nSkillLevel;

		/*if (isBuy)
		{
		SetAdditionalGameData(ChairID, enADT_SkillBuyStatus, m_UserInteractData[ChairID].nSkillBuyStatus);
		}

		SetAdditionalGameData(ChairID, enADT_SkillActiveStatus, m_UserInteractData[ChairID].nSkillActiveStatus);
		SetAdditionalGameData(ChairID, enADT_SkillLevel, m_UserInteractData[ChairID].nSkillLevel);*/

		_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
			TEXT("购买技能: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_Buyed), activeStatus=0x%x(SY::Actived), star=0x%x(1)"),
			pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillBuyStatus, m_UserInteractData[ChairID].nSkillActiveStatus, m_UserInteractData[ChairID].nSkillLevel);
		TRACEDEBUG(m_szDatalogInfo);
	}

	//TODO: 等级提升后，会返回此金币
	SendAdditionalGameData(ChairID);

	// 返回ID: 0表示成功， >0表示错误ID
	SY::BuySkillRsp syBuySkillRsp;
	syBuySkillRsp.set_chairid(ChairID);
	syBuySkillRsp.set_skillid(skillID);
	syBuySkillRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuySkillRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuySkillRsp, buf, syBuySkillRsp.ByteSize());
	}

	//发送分数到客户端
	SendUserMoneyCountTo(ChairID, pPlayer, true);

	return true;
}
//购买自动锤等级
bool CGameServer::OnBuyAutoHammer(IGamePlayer * pPlayer,void * pData,int nSize)
{
	if(NULL == pData || NULL == pPlayer || 0 == nSize)
		return false;
	SY::BuyHammerLevelReq lMessage;
	if(false == lMessage.ParseFromArray(pData,nSize))
		return false;


	int ChairID = pPlayer->get_chair(); // lMessage.chairid();
	unsigned char nAutoHammerLv = (unsigned char)lMessage.hammerlevel();

	TRACEDEBUG("OnBuyAutoAttack: ChairID=%d, AutoAttackLevel=%d", ChairID, nAutoHammerLv);

	int auto_hammer_default_lv = GameConfigMgr::getInstance().Current().get_auto_hammer_default_lv();

	unsigned char buyStatus = m_UserInteractData[ChairID].auto_hammer_lv;
	// 默认激活到3级
	if (buyStatus == 0)
		buyStatus = auto_hammer_default_lv;

	if (nAutoHammerLv <= buyStatus)
	{
		TRACEFALTAL("OnBuyAutoAttack, Already BUYED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoHammerLv, buyStatus);
		return false;
	}

	// 不能越级激活
	//if (nAutoAttackLevel > buyStatus + 1)
	if (nAutoHammerLv != buyStatus + 1)
	{
		TRACEFALTAL("OnBuyAutoAttack, USER MAY CHEATED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoHammerLv, buyStatus);
		return false;
	}

	// nAutoAttackLevel == buyStatus + 1
	const AutoHammerConfig* pAutoHammerCostInfo = GameConfigMgr::getInstance().Current().get_auto_hammer_info(buyStatus + 1);
	if (pAutoHammerCostInfo == nullptr)
	{
		TRACEFALTAL("OnBuyAutoAttack, GetAutoAttackInfo Failed, RoomID=%d, nAutoAttackLevel=%d", m_nRoomID, buyStatus + 1);
		return false;
	}

	// 购买, 先判断钱是否足够
	int nBuyCost = pAutoHammerCostInfo->active_cost;
	if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
	{
		TRACEFALTAL("OnBuyAutoAttack, Money NOT ENOUGHT!!!, UserID=%d, nAutoAttackLevel=%d", pPlayer->get_DBID(), nAutoHammerLv);
		return false;
	}

	// 先扣钱，再激活
	m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
	if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

	m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), TEXT("BuyAutoAttack[%d] With Money, UserID=%d, BuyCost=%d, buyStatus=%d"), nAutoAttackLevel, pPlayer->get_DBID(), nBuyCost, buyStatus);

	m_UserInteractData[ChairID].auto_hammer_lv = buyStatus + 1;
	m_UserPreInteractData[ChairID].auto_hammer_lv = m_UserInteractData[ChairID].auto_hammer_lv;

	//SetAdditionalGameData(ChairID, enADT_AutoAttackLevel, m_UserInteractData[ChairID].nAutoAttackLevel);

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("购买自动挂机等级: UserID=%d, nAutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), (int)nAutoAttackLevel, (int)buyStatus));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID);

	// 返回ID: 0表示成功， >0表示错误ID
	SY::BuyHammerLevelRsp syBuyAutoHammerRsp;
	syBuyAutoHammerRsp.set_chairid(ChairID);
	syBuyAutoHammerRsp.set_hammerlevel(nAutoHammerLv);
	syBuyAutoHammerRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuyAutoHammerRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuyHammerLevelRsp, buf, syBuyAutoHammerRsp.ByteSize());
	}

	//发送分数到客户端
	SendUserMoneyCountTo(pPlayer->get_chair(), pPlayer, true);    

	return true;      


}

bool CGameServer::OnBuyAutoAttack(IGamePlayer *pPlayer, void *pData, int nSize)
{
	if (pData == NULL)
		return false;

	SY::BuyAutoAttackReq lMessage;
	if (false == lMessage.ParseFromArray(pData, nSize))
	{
		return false;
	}

	int ChairID = pPlayer->get_chair(); // lMessage.chairid();
	unsigned char nAutoAttackLevel = (unsigned char)lMessage.autoattacklevel();

	TRACEDEBUG("OnBuyAutoAttack: ChairID=%d, AutoAttackLevel=%d", ChairID, nAutoAttackLevel);

	int nAutoAttackDefaultLevel = GameConfigMgr::getInstance().Current().GetAutoAttackDefaultLevel();

	unsigned char buyStatus = m_UserInteractData[ChairID].nAutoAttackLevel;
	// 默认激活到3级
	if (buyStatus == 0)
		buyStatus = nAutoAttackDefaultLevel;

	if (nAutoAttackLevel <= buyStatus)
	{
		TRACEFALTAL("OnBuyAutoAttack, Already BUYED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoAttackLevel, buyStatus);
		return false;
	}

	// 不能越级激活
	//if (nAutoAttackLevel > buyStatus + 1)
	if (nAutoAttackLevel != buyStatus + 1)
	{
		TRACEFALTAL("OnBuyAutoAttack, USER MAY CHEATED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoAttackLevel, buyStatus);
		return false;
	}

	// nAutoAttackLevel == buyStatus + 1
	const AutoAttackConfig* pAutoAttackCostInfo = GameConfigMgr::getInstance().Current().GetAutoAttackInfo(buyStatus + 1);
	if (pAutoAttackCostInfo == nullptr)
	{
		TRACEFALTAL("OnBuyAutoAttack, GetAutoAttackInfo Failed, RoomID=%d, nAutoAttackLevel=%d", m_nRoomID, buyStatus + 1);
		return false;
	}

	// 购买, 先判断钱是否足够
	int nBuyCost = pAutoAttackCostInfo->ActiveCost;
	if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
	{
		TRACEFALTAL("OnBuyAutoAttack, Money NOT ENOUGHT!!!, UserID=%d, nAutoAttackLevel=%d", pPlayer->get_DBID(), nAutoAttackLevel);
		return false;
	}

	// 先扣钱，再激活
	m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
	if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

	m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), TEXT("BuyAutoAttack[%d] With Money, UserID=%d, BuyCost=%d, buyStatus=%d"), nAutoAttackLevel, pPlayer->get_DBID(), nBuyCost, buyStatus);

	m_UserInteractData[ChairID].nAutoAttackLevel = buyStatus + 1;
	m_UserPreInteractData[ChairID].nAutoAttackLevel = m_UserInteractData[ChairID].nAutoAttackLevel;

	//SetAdditionalGameData(ChairID, enADT_AutoAttackLevel, m_UserInteractData[ChairID].nAutoAttackLevel);

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("购买自动挂机等级: UserID=%d, nAutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), (int)nAutoAttackLevel, (int)buyStatus));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID);

	// 返回ID: 0表示成功， >0表示错误ID
	SY::BuyAutoAttackRsp syBuyAutoAttackRsp;
	syBuyAutoAttackRsp.set_chairid(ChairID);
	syBuyAutoAttackRsp.set_autoattacklevel(nAutoAttackLevel);
	syBuyAutoAttackRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuyAutoAttackRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuyAutoAttackRsp, buf, syBuyAutoAttackRsp.ByteSize());
	}

	//发送分数到客户端
	SendUserMoneyCountTo(pPlayer->get_chair(), pPlayer, true);    

	return true;      
}

bool CGameServer::OnChangeSkillStatus(IGamePlayer *pPlayer, void *pData, int nSize)
{
	if (pData == NULL)
		return false;

	SY::ChangeSkillStatus lMessage;
	if (false == lMessage.ParseFromArray(pData, nSize))
	{
		return false;
	}

	int clientChairID = lMessage.chairid();
	int ChairID = pPlayer->get_chair();

	TRACEDEBUG("OnChangeSkillStatus: ChairID=%d, clientChairID=%d", ChairID, clientChairID);


	SY::ChangeSkillStatus syChangeSkillStatusRsp;
	syChangeSkillStatusRsp.set_chairid(ChairID);

	// 只有启用/禁用状态时才发发送，故技能首先必须是启用状态
	int nCount = lMessage.skillinf_size();
	for (int i = 0; i < nCount; ++i)
	{
		const SY::SkillInfo& skillInf = lMessage.skillinf(i);
		SkillID skillID = (SkillID)skillInf.skillid();
		SY::SkillStatus status = skillInf.status();

		SY::SkillStatus srcStatus = m_UserInteractData[ChairID].GetSkillActiveStatus(skillID);
		if (srcStatus == SY::InActive || srcStatus == status)
			continue;


		SY::SkillInfo* pSkillInfo = syChangeSkillStatusRsp.add_skillinf();
		pSkillInfo->set_skillid(skillID);
		pSkillInfo->set_status(status);


		m_UserInteractData[ChairID].SetSkillActiveStatus(skillID, status);
		m_UserPreInteractData[ChairID].nSkillActiveStatus = m_UserInteractData[ChairID].nSkillActiveStatus;

		//SetAdditionalGameData(ChairID, enADT_SkillActiveStatus, m_UserInteractData[ChairID].nSkillActiveStatus);

		_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
			TEXT("切换技能启用状态: UserID=%d, SkillID=%d, activeStatus=0x%x(%d)"),
			pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillActiveStatus, status);
		//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);
	}

	SendAdditionalGameData(ChairID, 112);

	// 返回ID: 0表示成功， >0表示错误ID   客户端在接收后，会刷新界面
	char buf[1024] = { 0 };
	if (syChangeSkillStatusRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::ChangeSkillStatus, buf, syChangeSkillStatusRsp.ByteSize());
	}

	return true;
}

bool CGameServer::OnChangeWeapon(IGamePlayer *pPlayer, void *pData, int nSize)
{
	if (pData == NULL)
		return false;
	SY::ChangeWeaponReq lMessage;

	if (false == lMessage.ParseFromArray(pData, nSize))
	{
		return false;
	}

	// 服务器不保存玩家的当前武器
	int clientChairID = lMessage.chairid();
	int ChairID = pPlayer->get_chair();
	int WeaponID = lMessage.weaponid();

	TRACEDEBUG("OnChangeWeapon: ChairID=%d, SkillID=%d,  clientChairID=%d", ChairID, WeaponID, clientChairID);

	// 此消息不需要处理，客户端自行完成

	//
	//// 失败才返回消息
	//SY::ChangeWeaponRsp syChangeWeaponRsp;
	//syChangeWeaponRsp.set_chairid(ChairID);
	//syChangeWeaponRsp.set_weaponid(WeaponID);
	//syChangeWeaponRsp.set_result(1);
	//char buf[200] = { 0 };
	//if (syChangeWeaponRsp.SerializeToArray(buf, sizeof(buf)))
	//{
	//    sendGameCmdProtBufTo(ChairID, GameMessages::ChangeWeaponRsp, buf, syChangeWeaponRsp.ByteSize());
	//}

	return true;
}

bool CGameServer::OnUpgradeSkill(IGamePlayer *pPlayer, void *pData, int nSize)
{
	if (pData == NULL)
		return false;
	SY::BuySkillReq lMessage;

	if (false == lMessage.ParseFromArray(pData, nSize))
	{
		return false;
	}

	int clientChairID = lMessage.chairid();   
	int ChairID = pPlayer->get_chair();
	SkillID skillID = (SkillID)lMessage.skillid();

	TRACEDEBUG("OnUpgradeSkill: ChairID=%d, SkillID=%d,  clientChairID=%d", ChairID, skillID, clientChairID);

	// 先取技能状态，如果已经激活那就说明存在逻辑错误，  如客户端未判断或玩家存在作弊可能性
	SY::SkillStatus activeStatus = m_UserInteractData[ChairID].GetSkillActiveStatus(skillID);
	if (activeStatus == SY::InActive)
	{
		TRACEFALTAL("OnUpgradeSkill, Skill UNACTIVE!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
		return false;
	}


	// 技能星级
	unsigned char SkillLevel = m_UserInteractData[ChairID].GetSkillLevel(skillID);
	// 如果已经满级，就不能再提升了
	if (SkillLevel >= 5)
	{
		TRACEWARNING("OnUpgradeSkill, SkillLevel[%d] is Invalid", SkillLevel);
		return false;
	}
	else if (SkillLevel == 0)
	{
		// 正常来说，技能在激活时就自动设为1星， 这里特殊修正一下
		TRACEWARNING("OnUpgradeSkill, SkillLevel[%d] is Invalid", SkillLevel);
		SkillLevel = 1;
	}


	// 根据玩家等级判断是提前激活（购买），还是到等级的激活
	int curLevel = m_nUserLevel[ChairID];
	const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
	if (pSkillCostInfo == nullptr)
	{
		TRACEFALTAL("OnUpgradeSkill, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d", m_nRoomID, skillID);
		return false;
	}

	// 从激活等级开始，每2级可升一星
	int nMoreLevelReq = pSkillCostInfo->ActiveLevel + pSkillCostInfo->UpgradeLevel * SkillLevel - curLevel;
	if (nMoreLevelReq > 0)
	{
		TRACEFALTAL("OnUpgradeSkill, Level NOT ENOUGH, RoomID=%d, UserID=%d, SkillID=%d, MoreLevelReq=%d", m_nRoomID, pPlayer->get_DBID(), skillID, nMoreLevelReq);
		return false;
	}


	m_UserInteractData[ChairID].SetSkillLevel(skillID, ++SkillLevel);
	m_UserPreInteractData[ChairID].nSkillLevel = m_UserInteractData[ChairID].nSkillLevel;

	//SetAdditionalGameData(ChairID, enADT_SkillLevel, m_UserInteractData[ChairID].nSkillLevel);

	_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
		TEXT("技能升星: UserID=%d, SkillID=%d, star=0x%x(%d)"),
		pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillLevel, SkillLevel);
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID, 113);

	// 返回ID: 0表示成功， >0表示错误ID
	SY::BuySkillRsp syUpgradeSkillRsp;
	syUpgradeSkillRsp.set_chairid(ChairID);
	syUpgradeSkillRsp.set_skillid(skillID);
	syUpgradeSkillRsp.set_result(0);
	char buf[200] = { 0 };
	if (syUpgradeSkillRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::UpgradeSkillRsp, buf, syUpgradeSkillRsp.ByteSize());
	}

	return true;
}

BOOL CGameServer::_is_CriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit )
{
	// 判断是否暴击
	int	nMonsterCount = PlayerHit.monsteridlist_size();
	if (nMonsterCount <= 1)	// 怪物列表为空或者只有一只怪物（不管是否鼠王）的时候不处发暴击
		return false;

	//判断是不是开了鼠王
	{
		const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, mtMoleKing);
		if (!pMonsterProperty)
		{
			return FALSE;
		}

		if (pMonsterProperty->RespawnProb <= 0)
		{
			TRACEFALTALFILE(_T("打地鼠外挂清单"), _T("发现使用外挂，桌子号: %d, 用户ID: %ld"), m_nTableID, pPlayer->get_DBID(NULL));

			m_pServer->ForceUserOffline(pPlayer->get_ID(NULL));

			return FALSE;
		}
	}

	//判断第一只是不是鼠王
	{
		int nMonsterID = PlayerHit.monsteridlist(0);
		MonsterType monsterType = CMonsterManager::Instance().GetMonsterType(m_nTableID, nMonsterID);

		if (monsterType != mtMoleKing)
		{
			if (monsterType != mtNone && monsterType != MonsterTypeCount)
			{
				TRACEFALTALFILE(_T("打地鼠外挂清单"), _T("发现使用外挂，桌子号: %d, 用户ID: %ld"), m_nTableID, pPlayer->get_DBID(NULL));

				m_pServer->ForceUserOffline(pPlayer->get_ID(NULL));
			}

			return FALSE;
		}
	}

	// 判断鼠王是否免伤
	const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)mtMoleKing);
	if (infMonsterProperty == nullptr)
		return false;

	// 计算免伤（如果被免伤，实际是系统的收益）
	if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
	{
		//未暴击的鼠王要参与后面的计算，所以这里只加一个免伤，其它都不要加，不要加！
		StatisticMgr::getInstance().NormalHit(mtMoleKing, 0, 0, 0, 0, 1);

		return -1;
	}

	// 上面都过了，开始计算暴击率
	double dRate = -1;

#ifdef DEBUG
	const KingConfig* king_config = GameConfigMgr::getInstance().Current().GetKingConfig(m_nRoomID);
	if (nullptr != king_config)
		dRate = (double)king_config->king_critical_rate;

	if (dRate < 0)											// 配置中没有读取到，就根据屏幕上的怪物数量计算概率
		dRate = 10000.0 / (nMonsterCount-1);
#else
	dRate = 10000.0 / (nMonsterCount-1);
#endif
	

	if ((genrand_real2() * 10000) >= dRate)
		return FALSE;

	return TRUE;
}

// 玩家击晕鼠王
void CGameServer::_PlayerHitKing(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit)
{
	// 判断是否暴击
	Change_t& Change	= m_Change[pPlayer->get_chair(NULL)];
	int	nMonsterCount	= PlayerHit.monsteridlist_size();
	WeaponID eWeaponID	= (WeaponID)PlayerHit.weaponid();
	const WeaponCostInfo* infWeaponCost = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, eWeaponID);
	if (infWeaponCost == nullptr)
	{
		TRACEFALTAL("玩家参数有误， 武器ID[%d]未找到。", eWeaponID);
		return;
	}
	
	// 计算暴击总金额
	int nChairID			= pPlayer->get_chair(NULL);
	int	nGainMoneyNum		= 0;						
	int nGainTickNum		= 0;
	int nWeaponKingCost		= infWeaponCost->Cost;	// 单次基本花费
	int nWeaponCostTotal	= 0;
	bool is_wild_skill_monster = false;
	float wild_skill_pay_rate = 1.0;
	float wild_skill_get_rate = 1.0;
	stUserMoney UserIncoming;
	SY::PlayerHitIncoming syPlayerHitIncoming;		// 计算每个怪物的收益
	syPlayerHitIncoming.set_chairid(nChairID);		// 座位号

#ifdef	_DEBUG
	long long GameScore = m_UserGameScore[nChairID].llMoneyNum;
#endif
#ifdef LOG_TRACE
	CString info("");
	info.Format("鼠王暴击id ");
	WriteKillLog(info);
#endif

	for (int i = 0; i < nMonsterCount; ++i)
	{
		int nMonsterID = PlayerHit.monsteridlist(i);
		//查找是否是狂暴怪
		std::map<DWORD,int>::iterator itmap = wild_skill_monster_map.find(nMonsterID);
		if(itmap != wild_skill_monster_map.end())
		{//在此处不进行狂暴怪从map中删除，直接在下次狂暴开始前清除上次狂暴怪 2016.10.26 hl
			is_wild_skill_monster = true;
			//		wild_skill_monster_map.erase(itmap);
		}

		if(wild_skill_status || is_wild_skill_monster)
		{
			wild_skill_pay_rate = get_wild_config_val(WILD_SKILL_PAY_RETE);
			wild_skill_get_rate = get_wild_config_val(WILD_SKILL_GET_RATE);
		}

		if(0 == i)
		{
			// 在这里进行判断，鼠王只扣一次钱，只要不够第一次扣 就返还
			if(m_UserGameScore[nChairID].llMoneyNum < nWeaponKingCost * wild_skill_pay_rate)
			{
				send_weapon_cost_ex(nWeaponKingCost,nChairID);
				_PlayerNotCriticalHit(pPlayer, PlayerHit);
				return;
			}

			// 在此处将花费修正 直接单独形成一条收入记录
			if(0 != (int)(infWeaponCost->Cost * wild_skill_pay_rate - infWeaponCost->Cost))
			{
				SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
				pMonsterIncoming->set_monsterid(nMonsterID);
				pMonsterIncoming->set_killrate(-1);
				pMonsterIncoming->set_usermoneynum(0);
				pMonsterIncoming->set_userticketnum(0);
				pMonsterIncoming->set_weaponcostex((int)(infWeaponCost->Cost * wild_skill_pay_rate - infWeaponCost->Cost));
			}
			UserIncoming.llMoneyNum -= infWeaponCost->Cost * wild_skill_pay_rate;	// 去掉武器消费(敲击鼠王的消费，其它怪物暴击就不计武器消费了)
			Change.KillCost[mtMoleKing] += infWeaponCost->Cost * wild_skill_pay_rate;
			nWeaponKingCost = infWeaponCost->Cost * wild_skill_pay_rate;			// 狂暴状态下击暴鼠王，扣狂暴的武器费用
			nWeaponCostTotal += nWeaponKingCost;

			StatisticMgr::getInstance().NormalHit(mtMoleKing, nWeaponKingCost, 0, 1, 0, 0);
		}

		MonsterType monsterType = CMonsterManager::Instance().GetMonsterType(m_nTableID, nMonsterID);
		if (monsterType == mtNone)
			continue;
		
		if (monsterType == mtFlyGoblin)		// 盗宝小妖暂时不开通
			continue;

		if(monsterType == MonsterTypeCount)
			continue;
		const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
		if (infMonsterProperty == nullptr)
			continue;
		
		// 计算免伤（如果被免伤，实际是系统的收益）
		Change.KillMonster[monsterType]++;
		Change.MissingCount++;
		if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
		{
			StatisticMgr::getInstance().KingHit(monsterType, 0, 1, 0, 1);

			Change.MissingValid++;
			continue;
		}
		
		// 获得击杀倍率
		int nKillRate = CMonsterManager::Random(infMonsterProperty->KillRateMin, infMonsterProperty->KillRateMax + 1);
		CMonster* monster = CMonsterManager::Instance().GetMonster(m_nTableID, nMonsterID);
		if (monster != nullptr)
			monster->Destroy();

		// 计算是否获得收益
		bool havIncoming = CMonsterManager::Prob(1, nKillRate);
		if (!havIncoming)	// 不会获得收益
		{
			StatisticMgr::getInstance().KingHit(monsterType, 0, 1, 1, 0);

			continue;
		}

		// 获取游戏币
		nGainMoneyNum = nKillRate * infWeaponCost->Cost * wild_skill_get_rate;
		StatisticMgr::getInstance().KingHit(monsterType, nGainMoneyNum, 1, 0, 0);
		UserIncoming.llMoneyNum += nGainMoneyNum;

		// 记录每个怪物的收益，之后返回给客户端，播放暴金币动画
		if (nGainMoneyNum > 0)
		{
			SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
			pMonsterIncoming->set_monsterid(nMonsterID);
			pMonsterIncoming->set_killrate(nKillRate);
			pMonsterIncoming->set_usermoneynum(nGainMoneyNum);
			pMonsterIncoming->set_userticketnum(nGainTickNum);
			pMonsterIncoming->set_weaponcostex(0);

#ifdef	_DEBUG
			//TRACEFALTAL("weapon_cost_ex=%d, nKillrate=%d", (int)(infWeaponCost->Cost * wild_skill_pay_rate - infWeaponCost->Cost), -1);
			TRACEWARNING("【GK】总金额【%lld】，击杀前总额【%lld】，击杀怪物【%d】，怪物类型【%d】，击杀倍率【%d】，获得收益【%d】",
				GameScore + nGainMoneyNum, GameScore, nMonsterID, monsterType, nKillRate, nGainMoneyNum);
			GameScore += nGainMoneyNum;
#endif
		}

		{	// 以下部分是统计信息，和游戏逻辑无关
			Change.KillGain[monsterType] += nGainMoneyNum;
			Change.KillValid[monsterType]++;

			if ( nKillRate > Change.MonsterRate )
			{
				Change.MonsterRate = nKillRate;		// 击杀倍率
				Change.MosterID = monsterType;		// 最大倍率的怪物id
			}

			if (nGainMoneyNum > Change.MonsterMostGain)
			{
				Change.MonsterMostGain = nGainMoneyNum;	// 单只怪物最大收益
			}

			//////////////////////////////////////////////////////////////////////////
			// 统计数据：
			m_UserStatisticsData[nChairID].llUserIncome.llMoneyNum += nGainMoneyNum;
			m_UserStatisticsData[nChairID].llIntervalExpense.llMoneyNum += nGainMoneyNum;
			m_nMonsterPlayerIncome[monsterType] += nGainMoneyNum;
			m_nStatictisMonsterIncomingCounts[monsterType]++;
			//////////////////////////////////////////////////////////////////////////
			m_nStatictisMonsterKillCounts[monsterType] += 1;
		}
	}
	// 击打鼠王的总开销和总收益
	Change.MoneyGain += UserIncoming.llMoneyNum;
	Change.MoneyCost += nWeaponKingCost;
	Init_t&				it	= m_Statistic[nChairID].Init;
	Accumulation_t&		ac	= m_Statistic[nChairID].Accumulation;

	if (ac.MostGain < it.InitMoney + ac.MoneyGain + Change.MoneyGain - ac.MoneyCost - Change.MoneyCost)
	{
		ac.MostGain = it.InitMoney + ac.MoneyGain + Change.MoneyGain - ac.MoneyCost - Change.MoneyCost;
		ac.T = _time64(NULL);
	}

	m_UserGameScore[nChairID] += UserIncoming;
	if (m_UserGameScore[nChairID].llMoneyNum < 0)
		m_UserGameScore[nChairID].llMoneyNum = 0;
#ifdef _DEBUG
	char strbuf[1024];
	sprintf(strbuf, "KingCriticalHit total:%lld, gain:%lld\n", m_UserGameScore[nChairID].llMoneyNum, UserIncoming.llMoneyNum);
	TRACE(strbuf);
#endif

	// 广播通知鼠王暴击消息
	const BroadcastInfo* infBroadcast = GameConfigMgr::getInstance().Current().GetBroadcastInfo(m_nRoomID);
	const int times = (UserIncoming.llMoneyNum + nWeaponKingCost)/ nWeaponKingCost;
	if (m_pTableChannel && (infBroadcast->lstBroadKingInfo.Limit > 0) && (times >= infBroadcast->lstBroadKingInfo.Limit))
	{
		char		lBuffer[MAX_MSG_LEN];
		char		lansiBuffer[MAX_MSG_LEN];
		char		AnsiName[128];
		char		Utf8Name[256];

		pPlayer->get_UserName(AnsiName);
		CharSetHelper::ANSIToUTF8(AnsiName, Utf8Name, sizeof(Utf8Name) - 1);

		wsprintf(lBuffer, infBroadcast->lstBroadKingInfo.Text.c_str(), Utf8Name, times, (int)(UserIncoming.llMoneyNum + nWeaponKingCost));
		CharSetHelper::UTF8ToANSI(lBuffer, lansiBuffer, sizeof(lansiBuffer) - 1);

		m_pTableChannel->TableDispatch(ITableChannel::IID_ShowMSGInOneRoom, lansiBuffer, strlen(lansiBuffer) + 1);
	}

	const GameIncomingInfo* pGameInComingInfo = GameConfigMgr::getInstance().Current().GetGameIncomingInfo(m_nRoomID);
	if (pGameInComingInfo)
		AddUserExp(pPlayer, (int)(nWeaponCostTotal * pGameInComingInfo->ExpFactor * 0.01));

	// 获取玩家等级
	GetPlayerExp(nChairID, pPlayer, &Change.Level);

	// 发送经验值
	SendGeneralLevel(nChairID, pPlayer);

	// 游戏收益同步给所有玩家
	if (syPlayerHitIncoming.incominglist_size() > 0)
	{
		char buf[1024] = { 0 };
		if (syPlayerHitIncoming.SerializeToArray(buf, sizeof(buf)))
		{
			sendGameCmdProtBufAll(GameMessages::PlayerHitIncomingSync, buf, syPlayerHitIncoming.ByteSize());
		}
	}

	// 将暴击事件发给客户端
	SendCriticalStrikeTo(pPlayer, UserIncoming, PlayerHit, nWeaponKingCost);
}

void CGameServer::_OnPlayerHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, BOOL isCriticalHit)
{
	int				ChairID = pPlayer->get_chair();  
	Change_t&		Change = m_Change[pPlayer->get_chair(NULL)];
    vector<stDeadMonster>		vecAllDeadMonster[MonsterTypeCount];				// 统计所有打死的怪物MonsterID，并将总数量放在Type为0的区域保存

	stPlayerHit playerHit;     
	playerHit.nWeaponID		= PlayerHit.weaponid();
	playerHit.nMonsterID	= PlayerHit.monsterid();
	playerHit.isRobot		= PlayerHit.isrobot();
	int nKingID				= -1;

	SY::PlayerHitIncoming syPlayerHitIncoming;
	syPlayerHitIncoming.set_chairid(ChairID);     //座位号

	if (PlayerHit.monsteridlist_size() > 0)
		nKingID = PlayerHit.monsteridlist(0);

    // 需要记录下每只怪各自获得的钱数用于客户端显示
    vector<stMonsterIncoming> vecMonsterIncoming[MonsterTypeCount];		// 统计能实际参与收入的计算，并将总数量放在Type为0的区域保存
    vector<stMonsterIncoming> vecMonsterMissing[MonsterTypeCount];      // 统计被免伤的怪物，并将总数量放在Type为0的区域保存

	int temp_weapon_cost = 0;
	bool is_wild_monster = false;

    // 避免玩家作弊，判断玩家是否能使用此武器， 即钱是否足够使用
    const WeaponCostInfo* pWeaponCostInfo = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, (WeaponID)playerHit.nWeaponID);
    if (!pWeaponCostInfo)
    {
        TRACEFALTAL("配置数据错误, UserID=%d, WeaponID=%d", pPlayer->get_ID(), playerHit.nWeaponID);
        return;
    }

    int nSkillTriggerCount = PlayerHit.skilllist_size();
    for (int i = 0; i < nSkillTriggerCount; ++i)
    {
        stSkillHit skillHit;

        const SY::SkillHit& sySkillHit = PlayerHit.skilllist(i);
        skillHit.nSkillID = sySkillHit.triggerskillid();

        int nMonsterCount = sySkillHit.monsterid_size();
        for (int j = 0; j < nMonsterCount; ++j)
        {
            int nMonsterID = sySkillHit.monsterid(j);
			if (isCriticalHit > 0 && nKingID == nMonsterID)	// 被技能击暴的鼠王就不统计了
			{
#ifdef LOG_TRACE
				CString info("");
				info.Format("技能鼠王击杀id{%d}",nMonsterID);
				WriteKillLog(info);
#endif
				continue;
			}
            skillHit.lstMonster.push_back(nMonsterID);

            MonsterType monsterType = CMonsterManager::Instance().PlayerHit(m_nTableID, nMonsterID,wild_skill_status);
            if (monsterType != mtNone && monsterType != MonsterTypeCount)
			{
				is_wild_monster = wild_monster(nMonsterID);
                vecAllDeadMonster[monsterType].push_back(stDeadMonster(nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && nMonsterID == nKingID));
                vecAllDeadMonster[0].push_back(stDeadMonster(nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && nMonsterID == nKingID));
				temp_weapon_cost += pWeaponCostInfo->Cost * (is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1);
#ifdef LOG_TRACE
				CString info("");
				info.Format("技能击杀id{%d}",nMonsterID);
					WriteKillLog(info);
#endif
            }
			else /*if(monsterType == MonsterTypeCount)*///找了个无效的类型作为服务端找不到怪的返回值  无特殊意义  新增一个改动太多 2016.11.9 hl
			{
				int list_size = PlayerHit.skillhitlisthaspay_size();
				if(0 == list_size)
					continue;
				bool has_pay = false;
				for(int num = 0; num < list_size; ++num)
				{
					if(nMonsterID == PlayerHit.skillhitlisthaspay(num))
					{
						has_pay = true;
						break;
					}
				}
				if(!has_pay)
				{
#ifdef LOG_TRACE
					CString info("");
					info.Format("技能服务端未找到怪并且技能击杀列表中未出现,击杀id{%d}",nMonsterID);
					WriteKillLog(info);
#endif
					continue ;
				}
				SY::MonsterIncoming* pMonsterIncomingMiss = syPlayerHitIncoming.add_incominglist();
				pMonsterIncomingMiss->set_monsterid(nMonsterID);
				pMonsterIncomingMiss->set_killrate(0);
				pMonsterIncomingMiss->set_usermoneynum(0);
				pMonsterIncomingMiss->set_userticketnum(0);
				pMonsterIncomingMiss->set_weaponcostex(-pWeaponCostInfo->Cost);
#ifdef LOG_TRACE
				CString info("");
				info.Format("服务端未找到怪(技能),击杀id{%d}",nMonsterID);
				WriteKillLog(info);
#endif
			}
        }

        playerHit.lstSkilTrigger.push_back(skillHit);


        if (0 < skillHit.nSkillID && skillHit.nSkillID <= SKILL_COUNT)
        {
            m_UserStatisticsData[ChairID].m_nStatictisSkillTriggerCounts[skillHit.nSkillID]++;
			Change.SkillCount[skillHit.nSkillID]++;		// 统计技能施放次数
        }
    }

    {
       	if (!(nKingID == playerHit.nMonsterID && isCriticalHit > 0))
		{
			MonsterType monsterType = CMonsterManager::Instance().PlayerHit(m_nTableID, playerHit.nMonsterID,wild_skill_status);	// 服务器打击一次怪物
			if(monsterType != mtNone && monsterType != MonsterTypeCount && PlayerHit.haspayweaponcost())
			{

					is_wild_monster = wild_monster(playerHit.nMonsterID);
					vecAllDeadMonster[monsterType].push_back(stDeadMonster(playerHit.nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && playerHit.nMonsterID == nKingID));
					vecAllDeadMonster[0].push_back(stDeadMonster(playerHit.nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && playerHit.nMonsterID == nKingID));
					temp_weapon_cost += pWeaponCostInfo->Cost * (is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1);
#ifdef LOG_TRACE
					CString info("");
					info.Format("击杀id{%d}",playerHit.nMonsterID/*,m_UserGameScore[ChairID].llMoneyNum*/);
					WriteKillLog(info);
#endif
					
			}
			else if((monsterType == MonsterTypeCount || monsterType == mtNone )&& PlayerHit.haspayweaponcost())//找了个无效的类型作为服务端找不到怪的返回值  无特殊意义  新增一个改动太多 2016.11.9 hl
			{
				SY::MonsterIncoming* pMonsterIncomingMiss = syPlayerHitIncoming.add_incominglist();
				pMonsterIncomingMiss->set_monsterid(playerHit.nMonsterID);
				pMonsterIncomingMiss->set_killrate(0);
				pMonsterIncomingMiss->set_usermoneynum(0);
				pMonsterIncomingMiss->set_userticketnum(0);
				pMonsterIncomingMiss->set_weaponcostex(-pWeaponCostInfo->Cost);
#ifdef LOG_TRACE
				CString info("");
				info.Format("服务端未找到怪,击杀id{%d}",playerHit.nMonsterID);
				WriteKillLog(info);
#endif
			}
		}
    }
  //客户端与服务端狂暴状态不一致，导致客户端钱够扣，但服务端不够扣，此情况下不计算击打，返还客户端已扣武器费用2016.11.1 hl
    if (m_UserGameScore[ChairID].llMoneyNum < temp_weapon_cost && vecAllDeadMonster[0].size())
    {
        //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "触发了技能，玩家分数不足支付此次最大消耗，UserID=%d, UserMoney=%d, WeaponID=%d, TargetNum=%d", pPlayer->get_ID(), m_UserGameScore[ChairID].llMoneyNum, playerHit.nWeaponID, vecAllDeadMonster[0].size());
        //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "=====KillOffTest : OnPlayerHit, Player=%d, curMoney=%d, preMoney=%d", pPlayer->get_DBID(), m_UserGameScore[ChairID].llMoneyNum, m_UserPreGameScore[ChairID].llMoneyNum);
        //m_pSite->GetOff(pPlayer->get_ID());
		send_weapon_cost_ex(pWeaponCostInfo->Cost * vecAllDeadMonster->size(),ChairID);
		TRACEDEBUG("武器扣费不够，客户端消耗金币回退! UserID=%d", pPlayer->get_ID());
        return;
    }
	m_UserStatisticsData[ChairID].m_nStatictisHitCounts++;
    m_UserStatisticsData[ChairID].dwLastHitTime = ::GetTickCount();

    //GLOGFI(LOGID_Trace(m_nRoomID), "玩家【%d】用武器【%d】击中了怪物【%d】，本次共击杀【%d】只", pPlayer->get_DBID(), playerHit.nWeaponID, playerHit.nMonsterID, vecAllDeadMonster[0].size());

    // 为0可能没打死，或者有其它错误（正常来说是前者）
    if (vecAllDeadMonster[0].empty())
    {
		if (syPlayerHitIncoming.incominglist_size() > 0)
		{
			char buf_pre[1024] = { 0 };
			if (syPlayerHitIncoming.SerializeToArray(buf_pre, sizeof(buf_pre)))
			{
				sendGameCmdProtBufAll(GameMessages::PlayerHitIncomingSync, buf_pre, syPlayerHitIncoming.ByteSize());
			}
		}
		 return;
	}

	//玩家分数不够
	if (m_UserGameScore[ChairID].llMoneyNum <= 0 )
	{	
        TRACEDEBUG("玩家分数不够，踢下线, UserID=%d", pPlayer->get_ID());
		m_pSite->GetOff(pPlayer->get_ID());
		return;
	}

    for (int monsterType = mtMonsterTypeFirst; monsterType < MonsterTypeCount; ++monsterType)
    {
        if (vecAllDeadMonster[monsterType].empty())
            continue;

        const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
        if (infMonsterProperty == nullptr)
            continue;

        // 统计计杀记录
        int nKillNum = vecAllDeadMonster[monsterType].size();
        if (nKillNum > 0)
        {
			Change.KillMonster[monsterType] += nKillNum;			// 统计击杀怪物数量
            if (CMonsterManager::IsMole((MonsterType)monsterType))
            {
                if (monsterType == mtMoleKing)
                    m_UserInteractData[ChairID].llMoleKingKillNum += nKillNum;
                //矿工地鼠数据合并到普通地鼠中
                //else if (monsterType == mtMoleKuangGong)
                //    m_UserInteractData[ChairID].llMoleKuangGongKillNum += nKillNum;
                else
                    m_UserInteractData[ChairID].llMoleNormalKillNum += nKillNum;
            }
            else if (monsterType == mtSpider)
                m_UserInteractData[ChairID].llSpiderKillNum += nKillNum;
            else if (monsterType == mtFly)
                m_UserInteractData[ChairID].llFlyKillNum += nKillNum;
            else if (monsterType == mtFlyGoblin)
                m_UserInteractData[ChairID].llFlyGoblinKillNum += nKillNum;  
        }

        for (auto &it = vecAllDeadMonster[monsterType].begin(); it != vecAllDeadMonster[monsterType].end(); ++it)
        {
            // 计算免伤（如果被免伤，实际是系统的收益）
            if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
            {
                // 免伤，玩家不会获得收益，还会扣掉本次需要打击消耗的金币
                vecMonsterMissing[monsterType].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
                vecMonsterMissing[0].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
            }
            else
            {
                // 未被免伤
                vecMonsterIncoming[monsterType].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
                vecMonsterIncoming[0].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
            }
        }
    }

	Change.MissingCount += vecMonsterMissing[0].size() + vecMonsterIncoming[0].size();		// 免伤判断次数
	Change.MissingValid += vecMonsterMissing[0].size();										// 免伤生效次数

	vector<SY::MonsterIncoming> lstReback;

    // 计算玩家收益（也可能会亏钱哟）
    stUserMoney UserIncoming;
    for (int monsterType = mtMonsterTypeFirst; monsterType < MonsterTypeCount; ++monsterType)
    {
        if (vecMonsterIncoming[monsterType].empty())
            continue;

		int iCount = vecMonsterIncoming[monsterType].size();

		stIncoming tmpInc;
        CalcPlayerIncoming(pPlayer, PlayerHit, (MonsterType)monsterType, (WeaponID)playerHit.nWeaponID, vecMonsterIncoming[monsterType], true, tmpInc, Change, lstReback);

		UserIncoming.llMoneyNum += tmpInc.llCoin;
		UserIncoming.llTicketNum += tmpInc.llTicket;

		StatisticMgr::getInstance().NormalHit((MonsterType)monsterType, tmpInc.llWeapon, tmpInc.llCoin + tmpInc.llWeapon, iCount, tmpInc.lMiss, 0);
    }

    // 对于被免伤的怪，扣掉本次需要打击消耗的金币 
    for (int monsterType = mtMonsterTypeFirst; monsterType < MonsterTypeCount; ++monsterType)
    {
        if (vecMonsterMissing[monsterType].empty())
            continue;

		int iCount = vecMonsterMissing[monsterType].size();
		int iProb = iCount;
		
		stIncoming tmpInc;
        CalcPlayerIncoming(pPlayer, PlayerHit, (MonsterType)monsterType, (WeaponID)playerHit.nWeaponID, vecMonsterMissing[monsterType], false, tmpInc, Change, lstReback);

		UserIncoming.llMoneyNum += tmpInc.llCoin;
		UserIncoming.llTicketNum += tmpInc.llTicket;

		//这里需要检查鼠王是不是被免伤了，如果是的话，就要处理
		if (monsterType == mtMoleKing)
		{
			for(auto &it = vecMonsterMissing[monsterType].begin(); it != vecMonsterMissing[monsterType].end(); ++it)
			{
				if (it->bKingProb)
				{
					iProb--;
				}
			}
		}

		StatisticMgr::getInstance().NormalHit((MonsterType)monsterType, tmpInc.llWeapon, tmpInc.llCoin + tmpInc.llWeapon, iCount, 0, iProb);
    }

	//返还
	for (vector<SY::MonsterIncoming>::iterator itFind = lstReback.begin(); itFind != lstReback.end(); itFind++)
	{
		SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
		*pMonsterIncoming = *itFind;
	}

    // 发送本次分数变化给客户端 还是 同步当前分数呢？  目前采用的是后者 
    m_UserGameScore[ChairID] += UserIncoming;
    if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;
	Init_t&				it	= m_Statistic[ChairID].Init;
	Accumulation_t&		ac	= m_Statistic[ChairID].Accumulation;

	if (ac.MostGain < it.InitMoney + ac.MoneyGain + Change.MoneyGain - ac.MoneyCost - Change.MoneyCost)
	{
		ac.MostGain = it.InitMoney + ac.MoneyGain + Change.MoneyGain - ac.MoneyCost - Change.MoneyCost;
		ac.T = _time64(NULL);
	}
	GetPlayerExp(ChairID, pPlayer, &Change.Level);

	//发送经验值
	SendGeneralLevel(ChairID, pPlayer);
    // 将得分发给自己
    SendUserMoneyCountTo(ChairID, pPlayer, true);

    // 游戏收益同步给所有玩家
	{
        for (int monsterType = mtMonsterTypeFirst; monsterType < MonsterTypeCount; ++monsterType)
        {
            for (auto it = vecMonsterIncoming[monsterType].begin(); it != vecMonsterIncoming[monsterType].end(); ++it)
            {
                const stUserMoney& incoming = it->incoming;
                if (incoming.llMoneyNum == 0 && incoming.llTicketNum == 0 && it->weapon_cost_ex == 0)
                    continue;

				bool bHit = false;
				for (vector<SY::MonsterIncoming>::iterator itFind = lstReback.begin(); itFind != lstReback.end(); itFind++)
				{
					if (itFind->monsterid() == it->nMonsterID)
					{
						bHit = true;
						break;
					}
				}

				if (bHit) continue;

                SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
                pMonsterIncoming->set_monsterid(it->nMonsterID);
                pMonsterIncoming->set_killrate(it->nKillRate);
                pMonsterIncoming->set_usermoneynum(incoming.llMoneyNum);
                pMonsterIncoming->set_userticketnum(incoming.llTicketNum);
				pMonsterIncoming->set_weaponcostex(it->weapon_cost_ex);
            }
			for (auto it = vecMonsterMissing[monsterType].begin(); it != vecMonsterMissing[monsterType].end(); ++it)
			{
				if (it->weapon_cost_ex == 0)
					continue;

				bool bHit = false;
				for (vector<SY::MonsterIncoming>::iterator itFind = lstReback.begin(); itFind != lstReback.end(); itFind++)
				{
					if (itFind->monsterid() == it->nMonsterID)
					{
						bHit = true;
						break;
					}
				}

				if (bHit) continue;

				SY::MonsterIncoming* pMonsterIncomingMiss = syPlayerHitIncoming.add_incominglist();
				pMonsterIncomingMiss->set_monsterid(it->nMonsterID);
				pMonsterIncomingMiss->set_killrate(it->nKillRate);
				pMonsterIncomingMiss->set_usermoneynum(0);
				pMonsterIncomingMiss->set_userticketnum(0);
				pMonsterIncomingMiss->set_weaponcostex(it->weapon_cost_ex);

			}
        }

        if (syPlayerHitIncoming.incominglist_size() > 0)
        {
            char buf[1024] = { 0 };
            if (syPlayerHitIncoming.SerializeToArray(buf, sizeof(buf)))
            {
                sendGameCmdProtBufAll(GameMessages::PlayerHitIncomingSync, buf, syPlayerHitIncoming.ByteSize());
            }
        }
    }
}

bool CGameServer::wild_monster(const int monster_id)
{
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		std::map<DWORD,int>::iterator itmap = wild_skill_monster_map.find(monster_id);
		if(itmap != wild_skill_monster_map.end() || wild_skill_status)
			return true;
	}
	return false;
}
#define NO_CRITICAL_HIT	0
#define OK_CRITICAL_HIT	1

// 通知客户端鼠王未暴击，可以继续刷怪
void CGameServer::_PlayerNotCriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit)
{
	int						i;
	int						nMonsterCount = PlayerHit.monsteridlist_size();
	int						nChairID = pPlayer->get_chair();
	SY::KingCriticalHit		syKingCriticalHit;
	char					buf[4096];

	syKingCriticalHit.set_chairid(pPlayer->get_chair(NULL));
	syKingCriticalHit.set_weaponid(PlayerHit.weaponid());
	syKingCriticalHit.set_usermoneygain(0);
	syKingCriticalHit.set_usermoneynum(m_UserGameScore[nChairID].llMoneyNum);
	syKingCriticalHit.set_userticketgain(0);
	syKingCriticalHit.set_userticketnum(m_UserGameScore[nChairID].llTicketNum);
	syKingCriticalHit.set_iscriticalhit(NO_CRITICAL_HIT);

	if (syKingCriticalHit.SerializeToArray(buf,sizeof(buf)))   //s13
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(), GameMessages::KingCriticalHit, buf, syKingCriticalHit.ByteSize());
	}
}

// 通知客户端鼠王被暴击
void CGameServer::SendCriticalStrikeTo(IGamePlayer *pPlayer, const stUserMoney& UserMoney, SY::PlayerHit& PlayerHit, int nWeaponKingCost)
{
	int						i;
	int						anim_mid_last = 500;
	int						anim_end_last = 500;
	int						nMonsterCount = PlayerHit.monsteridlist_size();
	int						nChairID = pPlayer->get_chair();
	SY::KingCriticalHit		syKingCriticalHit;
	char					buf[4096];
	const KingConfig*		king_conf = GameConfigMgr::getInstance().Current().GetKingConfig(m_nRoomID);
	if (nullptr != king_conf)
	{
		anim_mid_last = king_conf->anim_mid_last;
		anim_end_last = king_conf->anim_end_last;
	}

	syKingCriticalHit.set_chairid(pPlayer->get_chair(NULL));
	syKingCriticalHit.set_weaponid(PlayerHit.weaponid());
	syKingCriticalHit.set_usermoneygain(UserMoney.llMoneyNum + nWeaponKingCost); // 应该加回武器消费，这样才是纯怪物收益
	syKingCriticalHit.set_usermoneynum(m_UserGameScore[nChairID].llMoneyNum);
	syKingCriticalHit.set_userticketgain(UserMoney.llTicketNum);
	syKingCriticalHit.set_userticketnum(m_UserGameScore[nChairID].llTicketNum);
	syKingCriticalHit.set_iscriticalhit(OK_CRITICAL_HIT);
	syKingCriticalHit.set_animmidlast(anim_mid_last);
	syKingCriticalHit.set_animendlast(anim_end_last);

	for (i = 0; i < nMonsterCount; i++)
	{
		syKingCriticalHit.add_monsteridlist(PlayerHit.monsteridlist(i));
	}


	if (syKingCriticalHit.SerializeToArray(buf,sizeof(buf)))   //s13
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(), GameMessages::KingCriticalHit, buf, syKingCriticalHit.ByteSize());
	}
}

//玩家发射子弹	
void CGameServer::OnPlayerHit(IGamePlayer *pPlayer,void *pBuffer,int nSize)
{
	if (pBuffer == NULL)
		return;

	int nChairID = pPlayer->get_chair();    
	if (nChairID < 0 || nChairID > PLAY_COUNT)
	{
		m_pSite->GetOff(pPlayer->get_ID());
		return;
	}

	SY::PlayerHit lMessage;
	if (false == lMessage.ParseFromArray(pBuffer, nSize))
	{	
		return;
	}

	stPlayerHit playerHit;     
	playerHit.nWeaponID		= lMessage.weaponid();
	playerHit.nMonsterID	= lMessage.monsterid();
	playerHit.isRobot		= lMessage.isrobot();

	// add by yuanjj 2016-10-18
	// 判断鼠王暴击
	enum HITSTATUS{
		emNormal,
		emKingCriticalHit,
		emNoCriticalHit,
	};

	enum HITSTATUS		hitStatus;
	BOOL isCriticalHit = _is_CriticalHit(pPlayer, lMessage);
	MonsterType monsterType = CMonsterManager::Instance().GetMonsterType(m_nTableID, playerHit.nMonsterID);
// 	if(mtMoleKing == monsterType)
// 	{
// 		std::map<DWORD,int>::iterator it = not_normal_dead_king.find(playerHit.nMonsterID);
// 		if(not_normal_dead_king.end()!= it)
// 		{
// 			isCriticalHit = false;
// 		}
// 	}
	if (isCriticalHit > 0)
		hitStatus = emKingCriticalHit;
	else if (lMessage.monsteridlist_size() > 0)
		hitStatus = emNoCriticalHit;
	else
		hitStatus = emNormal;
	switch (hitStatus)
	{
	case emKingCriticalHit:			// 鼠王击暴
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		_PlayerHitKing(pPlayer, lMessage);
		break;
	case emNoCriticalHit:			// 鼠王未击暴
		_PlayerNotCriticalHit(pPlayer, lMessage);
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		break;
	case emNormal:					// 一般打击
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		break;
	}

	if(m_UserGameScore[nChairID].llMoneyNum < GameConfigMgr::getInstance().Current().GetEnterLimitMin(m_nRoomID) ||
		m_UserGameScore[nChairID].llMoneyNum >= GameConfigMgr::getInstance().Current().GetEnterLimitMax(m_nRoomID))
	{
		m_pSite->GetOff(pPlayer->get_ID());	// 金币超过上限或下限，踢人
	}

	// add end
}

void CGameServer::AddUserExp(IGamePlayer* pPlayer, int nExpAdd)
{
	int nChairID = pPlayer->get_chair();

	bool isLevelUp = GameConfigMgr::getInstance().Current().AddExp(m_nUserLevel[nChairID], m_llGradeScore[nChairID], nExpAdd);
	//更新到玩家
	pPlayer->put_Exp(m_llGradeScore[nChairID]);

	if (isLevelUp)
	{
		//判断是否需要返还
		CheckCanReturnUserCoin(pPlayer);
		//TODO: 玩家游戏内升级处理（判断是否需要自动激活技能）
		CheckCanAutoActiveSkill(pPlayer);
	}
}

void CGameServer::CheckCanReturnUserCoin(IGamePlayer* pPlayer)
{
	int nChairID = pPlayer->get_chair();

	for (int i = Skill_LianSuoShanDian; i < SkillTypeCount; ++i)
	{
		SkillID skillID = (SkillID)i;
		enSkillBuyStatus buyStatus = m_UserInteractData[nChairID].GetSkillBuyStatus(skillID);

		if (buyStatus != enSBS_Buyed)
			continue;

		const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
		if (pSkillCostInfo == nullptr)
		{
			TRACEFALTAL(_T("CheckCanReturnUserCoin, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d"), m_nRoomID, skillID);
			continue;
		}

		if (m_nUserLevel[nChairID] >= pSkillCostInfo->ActiveLevel)
		{
			//直接写入金币
			m_UserGameScore[nChairID].llMoneyNum += pSkillCostInfo->BuyCost;

			// 修改交互数据
			m_UserInteractData[nChairID].SetSkillBuyStatus(skillID, enSBS_Payed);    
			m_UserPreInteractData[nChairID].nSkillBuyStatus = m_UserInteractData[nChairID].nSkillBuyStatus;  

			SendAdditionalGameData(nChairID, enADT_SkillBuyStatus);

			m_UserStatisticsData[nChairID].llBuySkillReturn += pSkillCostInfo->BuyCost;

			_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
				TEXT("邮件返还购买技能金币: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_Payed), returnMoney=%d, activeLevel=%d, UserLevel=%d"),
				pPlayer->get_DBID(), skillID, m_UserInteractData[nChairID].nSkillBuyStatus, pSkillCostInfo->BuyCost, pSkillCostInfo->ActiveLevel, m_nUserLevel[nChairID]);
			m_LogFile.WriteDataLogFile(m_szDatalogInfo);

			SendBasicInfo(nChairID, pPlayer);

			// 通知客户端
			SY::ReturnUserMoney syReturnUserMoney;
			syReturnUserMoney.set_chairid(nChairID);
			syReturnUserMoney.set_returndata(skillID);
			syReturnUserMoney.set_returnreason(SY::RRT_BuySkill);
			syReturnUserMoney.set_returnusermoneynum(pSkillCostInfo->BuyCost);

			char buf[200] = { 0 };   
			if (syReturnUserMoney.SerializeToArray(buf, sizeof(buf)))   //s12
			{
				sendGameCmdProtBufTo(nChairID, GameMessages::ReturnUserMoney, buf, syReturnUserMoney.ByteSize());
			}
		}  
	}
}

void CGameServer::CheckCanAutoActiveSkill(IGamePlayer* pPlayer, bool bFromeInit)
{
	int nChairID = pPlayer->get_chair();

	for (int i = Skill_LianSuoShanDian; i < SkillTypeCount; ++i)
	{
		SkillID skillID = (SkillID)i;
		SY::SkillStatus activeStatus = m_UserInteractData[nChairID].GetSkillActiveStatus(skillID);
		if (activeStatus != SY::InActive)
			continue;

		const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
		if (pSkillCostInfo == nullptr)
		{
			TRACEFALTAL("CheckCanAutoActiveSkill, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d", m_nRoomID, skillID);
			continue;
		}

		if (m_nUserLevel[nChairID] >= pSkillCostInfo->ActiveLevel)
		{
			m_UserInteractData[nChairID].SetSkillActiveStatus(skillID, SY::Actived);
			m_UserPreInteractData[nChairID].nSkillActiveStatus = m_UserInteractData[nChairID].nSkillActiveStatus;

			m_UserInteractData[nChairID].SetSkillLevel(skillID, 1);
			m_UserPreInteractData[nChairID].nSkillLevel = m_UserInteractData[nChairID].nSkillLevel;

			/*SetAdditionalGameData(nChairID, enADT_SkillActiveStatus, m_UserInteractData[nChairID].nSkillActiveStatus);
			SetAdditionalGameData(nChairID, enADT_SkillLevel, m_UserInteractData[nChairID].nSkillLevel);*/

			_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
				TEXT("自动激活技能: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_None), activeStatus=0x%x(SY::Actived), star=0x%x(1)"),
				pPlayer->get_DBID(), skillID, m_UserInteractData[nChairID].nSkillBuyStatus, m_UserInteractData[nChairID].nSkillActiveStatus, m_UserInteractData[nChairID].nSkillLevel);
			//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);     

			SendAdditionalGameData(nChairID);

			if (!bFromeInit)
			{
				// 返回ID: 0表示成功， >0表示错误ID
				SY::BuySkillRsp syBuySkillRsp;
				syBuySkillRsp.set_chairid(nChairID);
				syBuySkillRsp.set_skillid(skillID);
				syBuySkillRsp.set_result(1);
				char buf[200] = { 0 };
				if (syBuySkillRsp.SerializeToArray(buf, sizeof(buf)))
				{
					sendGameCmdProtBufTo(nChairID, GameMessages::BuySkillRsp, buf, syBuySkillRsp.ByteSize());
				}
			}
		}
	}
}

void CGameServer::CalcPlayerIncoming(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, MonsterType monsterType, WeaponID weaponID, vector<stMonsterIncoming>& lstMonsterIncoming, 
										bool isIncoming, stIncoming& UserIncoming, Change_t& Change, vector<SY::MonsterIncoming>& lstMonsterReback)
{
	// 验证客户端传过来的参数
	const WeaponCostInfo* infWeaponCost = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, weaponID);
	if (infWeaponCost == nullptr)
	{
		TRACEFALTAL("玩家参数有误， 武器ID[%d]未找到。", weaponID);
		return;
	}

	int ChairID = pPlayer->get_chair();
	int iKingList = PlayerHit.monsteridlist_size();

	int nCount = lstMonsterIncoming.size();
	int weaponCost = infWeaponCost->Cost; // 单次基本花费  
	float wild_skill_pay_rate = 1.0;
	float wild_skill_get_rate = 1.0;
	for(auto &it = lstMonsterIncoming.begin(); it != lstMonsterIncoming.end(); ++it)
	{
		if (isIncoming && it->nMonsterType == mtMoleKing && iKingList <= 1)
		{
			it->weapon_cost_ex = 0;
			WriteInfoLog(_T("鼠王落单"));
		}
		else
		{
			wild_skill_pay_rate = (it->is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1);

			Change.KillCost[monsterType] += weaponCost * wild_skill_pay_rate;		// 统计此类型怪物击杀支出

			Change.MoneyCost += weaponCost * wild_skill_pay_rate;	               // 统计怪物击杀总支出
			it->weapon_cost_ex =weaponCost*(wild_skill_pay_rate - 1.0);
		}
	}
// 	for(auto &it =lstMonsterIncoming.begin(); it != lstMonsterIncoming.end();++it)  //每个收支列表添加是否需要额外支付击打费用 不管有没收入 都要记入 2016.10.28 hl
// 	{
// 		it->weapon_cost_ex =weaponCost*(wild_skill_pay_rate - 1.0);
// 	}
//	lstMonsterIncoming[0].weapon_cost_ex += (Change.KillCost[monsterType] - weaponCost * nCount); //统计客户端需要额外支付武器消费 2016.10.28 hl
	if (isIncoming)
	{
		// 是否被免伤，   未被免伤的怪
		const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
		if (infMonsterProperty == nullptr)
			return;

		// 计算击杀率，实际就是是否能获得此次奖励    独立计算nCount次
		for (int i = 0; i < nCount; ++i)
		{
			int nKillRate = CMonsterManager::Random(infMonsterProperty->KillRateMin, infMonsterProperty->KillRateMax + 1);
			if (nKillRate == 0) continue;

			bool havIncoming = false;   // 是否能获得收益
			LONGLONG GainTicketNum = 0;
			LONGLONG GainMoneyNum = 0;
			wild_skill_get_rate = lstMonsterIncoming[i].is_wild_monster?get_wild_config_val(WILD_SKILL_GET_RATE):1;
			if (monsterType == mtFlyGoblin)
			{
				havIncoming = CMonsterManager::Prob(nKillRate, 100);

				UserIncoming.lMiss += havIncoming ? 0 : 1;

				if (havIncoming)
				{
					// 盗宝小妖 根据使用武器 随机获取礼票
					const WeaponTicketIncoming* pWeaponTicketIncomingInfo = GameConfigMgr::getInstance().Current().GetWeaponTicketIncoming(m_nRoomID, weaponID);
					if (pWeaponTicketIncomingInfo)
					{
						GainTicketNum = CMonsterManager::Random(pWeaponTicketIncomingInfo->TicketNumMin, pWeaponTicketIncomingInfo->TicketNumMax + 1);
						GainTicketNum += infMonsterProperty->TicketNum * wild_skill_get_rate;//礼票翻倍

						UserIncoming.llTicket += GainTicketNum;
						lstMonsterIncoming[i].incoming.llTicketNum = GainTicketNum;

						//////////////////////////////////////////////////////////////////////////
						// 统计数据：
						m_UserStatisticsData[ChairID].llUserIncome.llTicketNum += GainTicketNum;
						m_UserStatisticsData[ChairID].llIntervalExpense.llTicketNum += GainTicketNum;
						m_nMonsterPlayerIncome[monsterType] += GainTicketNum;
						m_nStatictisMonsterIncomingCounts[monsterType]++;

						//////////////////////////////////////////////////////////////////////////
						Change.KillValid[monsterType]++;		// 怪物有效击杀
						Change.KillGain[monsterType] += GainTicketNum;
					}
					else
					{
						TRACEFALTAL("房间[%d] WeaponTicketIncoming配置错误，未找到相应数据, weaponID=%d", m_nRoomID, weaponID);
					}
				}
			}
			else
			{
				// 是否击杀成功（能否获取奖励）
				havIncoming = CMonsterManager::Prob(1, nKillRate);

				UserIncoming.lMiss += havIncoming ? 0 : 1;

				if (havIncoming)
				{
					// 获取游戏币  
					GainMoneyNum = nKillRate * weaponCost * wild_skill_get_rate; //狂暴期间再次提升倍率
					UserIncoming.llCoin += GainMoneyNum;

					lstMonsterIncoming[i].nKillRate = nKillRate;
					lstMonsterIncoming[i].incoming.llMoneyNum = GainMoneyNum;

					//////////////////////////////////////////////////////////////////////////
					// 统计数据：
					m_UserStatisticsData[ChairID].llUserIncome.llMoneyNum += GainMoneyNum;
					m_UserStatisticsData[ChairID].llIntervalExpense.llMoneyNum += GainMoneyNum;
					m_nMonsterPlayerIncome[monsterType] += GainMoneyNum;
					m_nStatictisMonsterIncomingCounts[monsterType]++;
					//////////////////////////////////////////////////////////////////////////

					//////////////////////////////////////////////////////////////////////////
					// 统计日志 add by 袁俊杰 2016-10-13
					Change.KillValid[monsterType]++;		// 怪物有效击杀
					if ( nKillRate > Change.MonsterRate )
					{
						Change.MonsterRate = nKillRate;		// 击杀倍率
						Change.MosterID = monsterType;		// 最大倍率的怪物id
					}

					if (GainMoneyNum > Change.MonsterMostGain)
					{
						Change.MonsterMostGain = GainMoneyNum;	// 单只怪物最大收益
					}

					Change.KillGain[monsterType] += GainMoneyNum;
					Change.MoneyGain += GainMoneyNum;			// 累加到总收益
					//////////////////////////////////////////////////////////////////////////

					// 广播通报
					const BroadcastInfo* infBroadcast = GameConfigMgr::getInstance().Current().GetBroadcastInfo(m_nRoomID);
					if (infBroadcast == nullptr)
					{
						TRACEFALTAL("房间[%d]未配置对应广播消息", m_nRoomID);
						continue;
					}

					// 判断是否需要播报
					if (nKillRate >= infBroadcast->ServerLimit)
					{
						for (auto it = infBroadcast->lstBroadLevelInfo.rbegin(); it != infBroadcast->lstBroadLevelInfo.rend(); ++it)
						{
							if (nKillRate < it->Limit)
								continue;

							// 播报内容， 格式化为ANSI后， 最终需要转为UTF8输出  ， 或者直接格式化为UTF8
							// [%s]一锤定音，敲晕%d倍“%s”获得%d金币       ===>      [玩家昵称]一锤定音，敲晕xx倍“地鼠或怪物名称”获得xxxxxxx金币

							char AnsiName[128] = { 0 };
							//char Utf8Name[128] = { 0 };
							pPlayer->get_UserName(AnsiName);
							//CharSetHelper::UTF8ToANSI(Utf8Name, AnsiName, sizeof(AnsiName) - 1);   
							//CharSetHelper::ANSIToUTF8(AnsiName, Utf8Name, sizeof(Utf8Name) - 1);

							//char lBuffer[MAX_MSG_LEN] = { 0 };
							//wsprintf(lBuffer, it->Text.c_str(), AnsiName, nKillRate, infMonsterProperty->MonsterName.c_str(), GainMoneyNum);

							//utf->ansi
							//char lansiBuffer[MAX_MSG_LEN] = { 0 };
							//CharSetHelper::UTF8ToANSI(lBuffer, lansiBuffer, sizeof(lansiBuffer) - 1);

							//TRACEDEBUG(lBuffer);
							BroadcastMessage(pPlayer, it->Text.c_str(), AnsiName, weaponID, nKillRate, infMonsterProperty->MonsterName.c_str(), GainMoneyNum, 0);

							break;
						}
					}
				}
			}

			//GLOGFI(LOGID_Statictis(m_nRoomID), "UserID=%d, 是否免伤=否, 是否有收益=%d, 怪物类型ID=%d, 武器ID=%d, 击杀倍数=%d, 收支=+%I64d-%d,+%I64d, 当前金币=%I64d,%I64d",
			//    pPlayer->get_DBID(), havIncoming, monsterType, weaponID, nKillRate, GainMoneyNum, weaponCost, GainTicketNum, m_UserGameScore[ChairID].llMoneyNum, m_UserGameScore[ChairID].llTicketNum);

			////TODO: 测试数据
			//UserIncoming.llTicketNum++;
			//lstMonsterIncoming[i].incoming.llTicketNum += 1;
		}

		m_nStatictisMonsterKillCounts[monsterType] += nCount;
	}
	else
	{
		//GLOGFI(LOGID_Statictis(m_nRoomID), "UserID=%d, 是否免伤=是, 怪物类型ID=%d, 武器ID=%d, 免伤数量=%d, 收支=-%d,+0, 当前金币=%I64d,%I64d",
		//    pPlayer->get_DBID(), monsterType, weaponID, nCount, weaponCost * nCount, m_UserGameScore[ChairID].llMoneyNum, m_UserGameScore[ChairID].llTicketNum);

		m_nStatictisAvoidValidCounts += nCount;
		for(auto it = lstMonsterIncoming.begin(); it != lstMonsterIncoming.end(); ++it)
		{
			wild_skill_get_rate = it->is_wild_monster?get_wild_config_val(WILD_SKILL_GET_RATE):1;
			m_llSumAvoidHitSystemGetScore += weaponCost * wild_skill_pay_rate;
			m_UserStatisticsData[ChairID].llAvoidHitSystemGetScore += weaponCost * wild_skill_pay_rate;
			Change.MissingGain += weaponCost * wild_skill_pay_rate;
		}
		
	}

	// 扣道具消耗
	int nWeaponCostTotal = 0;
	for(auto it = lstMonsterIncoming.begin(); it != lstMonsterIncoming.end(); ++it)
	{
		if (isIncoming && it->nMonsterType == mtMoleKing && iKingList <= 1)
		{
			SY::MonsterIncoming inc;
			inc.set_monsterid(it->nMonsterID);
			inc.set_killrate(0);
			inc.set_usermoneynum(0);
			inc.set_userticketnum(0);
			inc.set_weaponcostex(-weaponCost);

			lstMonsterReback.push_back(inc);
		}
		else
		{
			wild_skill_pay_rate = it->is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1;
			nWeaponCostTotal += (weaponCost * wild_skill_pay_rate);
		}
	}
	
	UserIncoming.llCoin -= nWeaponCostTotal;
	UserIncoming.llWeapon += nWeaponCostTotal;

	//////////////////////////////////////////////////////////////////////////
	// 统计数据：
	m_nStatictisAvoidCounts += nCount;

	m_UserStatisticsData[ChairID].llUserCost.llMoneyNum += nWeaponCostTotal;



	m_UserStatisticsData[ChairID].llIntervalIncome.llMoneyNum += nWeaponCostTotal;
	m_nMonsterPlayerCost[monsterType] += nWeaponCostTotal;
	//////////////////////////////////////////////////////////////////////////

	// 消耗的金币转换为福利点数
	const GameIncomingInfo* pGameInComingInfo = GameConfigMgr::getInstance().Current().GetGameIncomingInfo(m_nRoomID);
	if (pGameInComingInfo)
	{
		m_dbUserWelFarePoint[ChairID] += nWeaponCostTotal * 0.01 * pGameInComingInfo->WelfarePointFactor * 0.01;
	}
	else
	{
		TRACEFALTAL("房间[%d]福利点数配置有误，未找到相应数据", m_nRoomID);
	}

	AddUserExp(pPlayer, (int)(nWeaponCostTotal * pGameInComingInfo->ExpFactor * 0.01));
}

void CGameServer::BroadcastMessage(IGamePlayer* pPlayer, const char* txt, const char* userName, WeaponID weaponID, int nKillRate, const char* monsterName, int rewardMoney, int rewardTicket)
{
	char cBrBuf[200] = { 0 };

	int ChairID = pPlayer->get_chair();

	if (GetTickCount() - m_nBroadcastTime[ChairID] <= Max_Broadcast_TIME)   //两次播报间隔时间不能小于Max_Broadcast_TIME秒
	{
		return;
	}

	char lBuffer[MAX_MSG_LEN] = { 0 };

	//发送对玩家单独播报
	{
		char Utf8Name[128] = { 0 };
		CharSetHelper::ANSIToUTF8("您", Utf8Name, sizeof(Utf8Name) - 1);

		wsprintf(lBuffer, txt, Utf8Name, nKillRate, monsterName, rewardMoney);

		SY::BroadcastS2C syBroadcastS2C;
		syBroadcastS2C.set_text(lBuffer);
		syBroadcastS2C.set_username(userName);
		syBroadcastS2C.set_weaponid(weaponID);
		syBroadcastS2C.set_monstername(monsterName);
		syBroadcastS2C.set_rewardmoneynum(rewardMoney);
		syBroadcastS2C.set_rewardticketnum(rewardTicket);

		if (GetTickCount() - m_nBroadcastTime[ChairID] > Max_Broadcast_TIME)   //两次播报间隔时间不能小于Max_Broadcast_TIME秒
		{
			if (syBroadcastS2C.SerializePartialToArray(cBrBuf, sizeof(cBrBuf)))
			{
				sendGameCmdProtBufTo(ChairID, GameMessages::BroadcastS2C, cBrBuf, syBroadcastS2C.ByteSize());
			}
		}
	}
	

	int nResult = 0;

	try
	{
		// * /m_pServer->QueryInterface(IID_ITableChannel, (void **)&pTableChannel);
		if(m_pTableChannel)
		{
			char lansiBuffer[MAX_MSG_LEN] = { 0 };

			ZeroMemory(lBuffer, sizeof(lBuffer));

			char ansiName[128] = { 0 };
			sprintf(ansiName, "[%s]", userName);

			char utf8Name[128] = {0};
			CharSetHelper::ANSIToUTF8(ansiName, utf8Name, sizeof(utf8Name) - 1);

			wsprintf(lBuffer, txt, utf8Name, nKillRate, monsterName, rewardMoney);
			CharSetHelper::UTF8ToANSI(lBuffer, lansiBuffer, sizeof(lansiBuffer) - 1);

			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_ShowMSGInOneRoom, lansiBuffer, strlen(txt) + 1);
		}
		else
		{
			_sntprintf(m_szErrorInfo, CountArray(m_szErrorInfo), TEXT("Catch Error: Table=%d, 广播接口命令为空."), m_nTableID);
		}
	}
	catch(...)
	{
		TRACEFALTAL("Unknown Error as Broadcast bonus information.");
		_sntprintf(m_szErrorInfo, CountArray(m_szErrorInfo), TEXT("Catch Error: Table=%d, Unknown Error as Broadcast bonus information."), m_nTableID);
	}

	m_nBroadcastTime[ChairID] = GetTickCount();
}

void CGameServer::buildGameNo()
{
	if (m_nGameIndx>2500) //只是为了区分唯一加的一个标识
	{
		m_nGameIndx = 0;
	}
	memset(m_GameNo,0,sizeof(m_GameNo));
	m_nGameIndx++;
	time_t now = time(0);
	tm t;
	localtime_s(&t,&now);
	//sprintf(m_GameNo,"%04d%02d%02d-%04d-%04d",t.tm_year+1900, t.tm_mon+1, t.tm_mday, m_nRoomID, m_nTableID);
	sprintf(m_GameNo,"%04d%02d%02d",t.tm_year+1900, t.tm_mon+1, t.tm_mday);
	// 	_snprintf(m_GameNo,27, "%04d%02d%02d0000000000000000000",t.tm_year+1900, t.tm_mon+1, t.tm_mday);
	// 	_snprintf(m_GameNo+13,5, "-%04d",m_nRoomID);
	// 	_snprintf(m_GameNo+17,5, "-%04d",m_nTableID);



	//	_snprintf(m_GameNo+21,5, "%04d",m_nGameIndx);

	// 	m_MjDBInfo[0].reset();
	// 	m_MjDBInfo[1].reset();
	// 	m_MjDBInfo[0].gameNo = m_GameNo;
	// 	m_MjDBInfo[1].gameNo = m_GameNo;
	// 	char gameTime[26] ={0};
	// 	_snprintf(gameTime,25,"%04d-%02d-%02d  %02d:%02d:%02d",t.tm_year+1900, t.tm_mon+1, t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
	// 	m_MjDBInfo[0].gameTime = gameTime;
	// 	m_MjDBInfo[1].gameTime = gameTime;
}

// * / 房间信息，如房间名、桌子号等赋值，26Apr.2011
void CGameServer::InitRoomInfo()
{
	int nResult = 0;
	//static int i;
	//i++;
	//	OUTLOOK("%d",i);
	try
	{
		m_pServer->QueryInterface(IID_ITableChannel, (void **)&m_pTableChannel);
		if(m_pTableChannel)
		{
			memset(&m_zsServerName, 0, sizeof(m_zsServerName));
			_sntprintf(m_zsServerName.szServerName, _countof(m_zsServerName.szServerName), TEXT("捕鱼房间名"));
			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Get_Room_Name, &m_zsServerName, sizeof(m_zsServerName));    //TODO: 房间名实际未更新
			m_pTableChannel->TableDispatch(ITableChannel::IID_Get_TableID, (void*)&m_nTableID, 0);
		}
		else
		{
			TRACEDEBUG("房间信息接口命令为空");
		}
	}
	catch(...)
	{
		TRACEDEBUG("Unknown Error as Broadcast bonus information.");
	}

}

//设置游戏状态
enGamePhase	CGameServer::SetGameStatus(enGamePhase emGamePase)
{
	return m_enGamePhase = emGamePase;
}

//有效的椅子号
bool CGameServer::CheckValidChairID(int nChairID)
{
	if (0 <= nChairID && nChairID < PLAY_COUNT)
	{
		return true;
	}

	return false;
}

//获得玩家的分数
LONGLONG CGameServer::GetPlayerScore(int nChair, IGamePlayer *pPlayer)
{
	Game_SCORE lScore;

	if (nChair < 0 || PLAY_COUNT <= nChair || NULL == pPlayer)
	{
		return 0;
	}
	pPlayer->get_ScoreBuf((long*)&lScore);

	return lScore.lScore;
}

LONGLONG CGameServer::GetPlayerExp(int nChair, IGamePlayer *pPlayer, int* pLevel)
{
	if (nChair < 0 || PLAY_COUNT <= nChair || NULL == pPlayer)
	{
		return 0;
	}

	LONGLONG curExp = pPlayer->get_Exp();
	if (pLevel)
	{
		*pLevel = GameConfigMgr::getInstance().Current().CalcLevel(curExp);
	}

	return curExp;
}

//计算分数
bool CGameServer::CalcScore(IGamePlayer *pPlayer, int nChairID)
{
	//不为空的玩家是有效玩家
	if ( enPlayerNull == m_enUserState[ nChairID ] || !m_UserStatisticsData[nChairID].bClientReady )
	{
		return false;
	}

	//写分数值
	stUserMoney llWriteScore = m_UserGameScore[nChairID] - m_UserStatisticsData[nChairID].llUserCarryScore;

	//税率
	LONGLONG llTax = llWriteScore.llMoneyNum;//llWriteScore.llMoneyNum;
	//为负数就变为正数
	/*if ( llTax < 0)
	llTax =  -llTax;*/


	// 不会被执行到！！！

	//写分失败返回
	if (false == WriteExitGameScore(pPlayer, nChairID, m_lDrawnCount[nChairID], llWriteScore, -llTax, m_UserStatisticsData[nChairID].llUserCost.llMoneyNum))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s WriteExitGameScore  WriteScore = %I64d,%I64d    TotalScore =  %I64d,%I64d"), 
			m_userName[nChairID], llWriteScore.llMoneyNum, llWriteScore.llTicketNum, m_UserGameScore[nChairID].llMoneyNum, m_UserGameScore[nChairID].llTicketNum);
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		return false;
	}

	//写了分数后就同步
	m_UserPreGameScore[nChairID] = m_UserGameScore[nChairID];
	return true;
}

//使用扩展写分接口，记录税率贡献和常客分，10Nov.2011
bool CGameServer::WriteExitGameScore(IGamePlayer *pPlayer, int nChairID, long lDrawn, const stUserMoney& llWinScore, LONGLONG llTax, LONGLONG llFreqScore)
{
	int nResult = 0;
	static tagExtGameScore sExtGameScore = {0};

	if (m_pTableChannel)
	{
		memset(&sExtGameScore, 0, sizeof(sExtGameScore));
		sExtGameScore.dwUserID = m_dwUserID[nChairID];
		sExtGameScore.stExtScore.nChairID = nChairID;
		sExtGameScore.stExtScore.llWinLose = llWinScore.llMoneyNum;
		sExtGameScore.stExtScore.lDrawn = lDrawn;			// * /局数为平局累计
		sExtGameScore.stExtScore.llTax = llTax;
		sExtGameScore.stExtScore.llFreqPile = llFreqScore;
		nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_EXTEND_GAME_SCORE, &sExtGameScore, sizeof(sExtGameScore));

		TRACEDEBUG("=====WriteExitGameScoreTest : Player=%d, curMoney=%d, diffMoney=%d", pPlayer->get_DBID(), m_UserGameScore[nChairID], llWinScore.llMoneyNum);
	}

	TRACEDEBUG("writeExtGameScore() ITableChannel::IID_EXTEND_GAME_SCORE Result = %d.", nResult);

	if (nResult == S_OK)
	{
		//写入捕鱼等级
		m_FishingData[nChairID].llGradeScore = pPlayer->get_Exp();

		SaveFishingCommonData(nChairID, m_dwUserID[nChairID], &m_FishingData[nChairID]);
		SaveCustomData(nChairID, m_dwUserID[nChairID], &m_UserInteractData[nChairID]);
	}


	//扩展写分成功返回
	_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s 写分成功  进入携带 %I64d,%I64d 金币  输赢金币 = %I64d,%I64d  结算后金币 %I64d,%I64d"),
		m_userName[nChairID],										            //昵称
		m_UserStatisticsData[nChairID].llUserCarryScore.llMoneyNum,
		m_UserStatisticsData[nChairID].llUserCarryScore.llTicketNum,			//进入携带
		llWinScore.llMoneyNum, llWinScore.llTicketNum,													//输赢金币
		m_UserGameScore[nChairID].llMoneyNum, m_UserGameScore[nChairID].llTicketNum			        //结算后金币
		);
	//写数据日志
	m_LogFile.WriteDataLogFile(m_szErrorInfo);


	// 写当局数据到日志
	m_llSumIncome += m_UserStatisticsData[nChairID].llIntervalIncome.llMoneyNum;						//系统收入
	m_llSumExpense += m_UserStatisticsData[nChairID].llIntervalExpense;					            //系统支出
	m_llSumIntervalIncome += m_UserStatisticsData[nChairID].llIntervalIncome.llMoneyNum;
	m_llSumIntervalExpense += m_UserStatisticsData[nChairID].llIntervalExpense;

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: 桌子号=%d, UserDBID=%d, 系统时段收入=%I64d, 系统时段支出=%I64d,%I64d, 系统收入=%I64d, 系统支出=%I64d,%I64d, 免伤率功能系统提取金币数=%I64d"),
	//    m_nTableID, pPlayer->get_DBID(), m_llSumIntervalIncome, m_llSumIntervalExpense.llMoneyNum, m_llSumIntervalExpense.llTicketNum,
	//    m_llSumIncome, m_llSumExpense.llMoneyNum, m_llSumExpense.llTicketNum, m_llSumAvoidHitSystemGetScore);
	//GLOGFI(LOGID_Losspool(m_nRoomID), m_szDatalogInfo);


	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: 桌子号=%d, UserDBID=%d, 玩家本场消费=%I64d, 玩家本场收入=%I64d,%I64d, 玩家购买消耗=%I64d, 系统返还玩家=%I64d, 玩家进入时金币=%I64d, 玩家当前金币=%I64d"),
	//    m_nTableID, pPlayer->get_DBID(), m_UserStatisticsData[nChairID].llUserCost, m_UserStatisticsData[nChairID].llUserIncome.llMoneyNum, m_UserStatisticsData[nChairID].llUserIncome.llTicketNum,
	//    m_UserStatisticsData[nChairID].llBuySkillCost, m_UserStatisticsData[nChairID].llBuySkillReturn,
	//    m_UserStatisticsData[nChairID].llUserCarryScore, m_UserGameScore[nChairID].llMoneyNum);
	//GLOGFI(LOGID_Losspool(m_nRoomID), m_szDatalogInfo);


	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: 技能触发, UserDBID=%d, 玩家攻击次数=%d, 技能触发次数=%d,%d,%d,%d"),
	//    pPlayer->get_DBID(), m_UserStatisticsData[nChairID].m_nStatictisHitCounts,
	//    m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_LianSuoShanDian], m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_HengSaoQianJun],
	//    m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_GaoShanLiuShui], m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_LiuXingHuoYu]);
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	m_UserStatisticsData[nChairID].Clear();

	//暂时没有等级分,设置为0
	if (false == WritePreGameScore(nChairID, llWinScore, m_UserGameScore[nChairID].llMoneyNum, 0))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s 预存取写分失败 输赢分数 =%I64d  总分数 =  %I64d  "),
			m_userName[nChairID],
			llWinScore.llMoneyNum,
			m_UserGameScore[nChairID].llMoneyNum);
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		return false;
	}

	return true;
}

//TODO: 预结算玩家得分
bool CGameServer::CalcPreScore(int nChairID)
{
	//算出用了或者增加了多少分数
	stUserMoney llPreWinScore = m_UserGameScore[nChairID] - m_UserPreGameScore[nChairID] ;

	//玩家总分数
	stUserMoney llPreTotalScore = m_UserGameScore[nChairID];			   //m_UserGameScore全场景时由接口读出

	for (int i = 0; i < PLAY_COUNT; i++) 
	{
		m_llSumIncome += m_UserStatisticsData[i].llIntervalIncome.llMoneyNum;						//系统收入
		m_llSumExpense += m_UserStatisticsData[i].llIntervalExpense;					            //系统支出
		m_llSumIntervalIncome += m_UserStatisticsData[i].llIntervalIncome.llMoneyNum;
		m_llSumIntervalExpense += m_UserStatisticsData[i].llIntervalExpense;
	}

	_sntprintf(m_szDatalogInfo , _countof(m_szDatalogInfo) ,
		TEXT("桌子号=%d, 系统时段增量=%I64d,%I64d, 系统总计增量=%I64d,%I64d, 系统时段收入=%I64d, 系统时段支出=%I64d,%I64d, 系统收入=%I64d, 系统支出=%I64d,%I64d"), 
		m_nTableID, m_llSumIntervalIncome - m_llSumIntervalExpense.llMoneyNum, -m_llSumIntervalExpense.llTicketNum,
		m_llSumIncome - m_llSumExpense.llMoneyNum, m_llSumExpense.llTicketNum,
		m_llSumIntervalIncome, m_llSumIntervalExpense.llMoneyNum, m_llSumIntervalExpense.llTicketNum, 
		m_llSumIncome, m_llSumExpense.llMoneyNum, m_llSumExpense.llTicketNum);
	//GLOGFI(LOGID_Losspool(m_nRoomID),m_szDatalogInfo);

	for (int i = 0;i < PLAY_COUNT; i++)
	{
		m_UserStatisticsData[i].llIntervalIncome.Clear();
		m_UserStatisticsData[i].llIntervalExpense.Clear();
	}
	m_llSumIntervalIncome = 0;
	m_llSumIntervalExpense.Clear();

	//经验值变化 
	LONGLONG llGradeScore = m_llGradeScore[nChairID] - m_llPreGradeScore[nChairID];
	if (!WritePreGameScore(nChairID, llPreWinScore, llPreTotalScore.llMoneyNum, llGradeScore))
	{
		return false;
	}

	//写了分数后就同步
	m_UserPreGameScore[nChairID] = m_UserGameScore[nChairID];
	m_llPreGradeScore[nChairID] = m_llGradeScore[nChairID];

	return true;
}

void CGameServer::RecordServerLog(DWORD dwChairID, int UserDBID)
{
	/*_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("桌子号=%d, DBID=%d, 怪物免伤率: 免伤判断次数=%I64d, 免伤生效次数=%I64d, 免伤获得收益=%I64d"),
	m_nTableID, UserDBID, m_nStatictisAvoidCounts, m_nStatictisAvoidValidCounts, m_llSumAvoidHitSystemGetScore);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("桌子号=%d, DBID=%d, 技能触发率: 玩家攻击次数=%d, 技能触发次数=%d,%d,%d,%d, [单个玩家数据]"),
	m_nTableID, UserDBID, m_UserStatisticsData[dwChairID].m_nStatictisHitCounts,
	m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_LianSuoShanDian], m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_HengSaoQianJun],
	m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_GaoShanLiuShui], m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_LiuXingHuoYu]);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("桌子号=%d, DBID=%d, 怪物击杀率: 杀死数=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	有效击杀=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	怪物成本: 怪物支出=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	怪物收益=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d"),
	m_nTableID, UserDBID, m_nStatictisMonsterKillCounts[mtMoleBoSi], m_nStatictisMonsterKillCounts[mtMoleDaiMeng], m_nStatictisMonsterKillCounts[mtMoleFeiXing], m_nStatictisMonsterKillCounts[mtMoleHuShi], m_nStatictisMonsterKillCounts[mtMoleKuangGong], m_nStatictisMonsterKillCounts[mtMoleKing], m_nStatictisMonsterKillCounts[mtSpider], m_nStatictisMonsterKillCounts[mtFly], m_nStatictisMonsterKillCounts[mtFlyGoblin],
	m_nStatictisMonsterIncomingCounts[mtMoleBoSi], m_nStatictisMonsterIncomingCounts[mtMoleDaiMeng], m_nStatictisMonsterIncomingCounts[mtMoleFeiXing], m_nStatictisMonsterIncomingCounts[mtMoleHuShi], m_nStatictisMonsterIncomingCounts[mtMoleKuangGong], m_nStatictisMonsterIncomingCounts[mtMoleKing], m_nStatictisMonsterIncomingCounts[mtSpider], m_nStatictisMonsterIncomingCounts[mtFly], m_nStatictisMonsterIncomingCounts[mtFlyGoblin],
	m_nMonsterPlayerCost[mtMoleBoSi], m_nMonsterPlayerCost[mtMoleDaiMeng], m_nMonsterPlayerCost[mtMoleFeiXing], m_nMonsterPlayerCost[mtMoleHuShi], m_nMonsterPlayerCost[mtMoleKuangGong], m_nMonsterPlayerCost[mtMoleKing], m_nMonsterPlayerCost[mtSpider], m_nMonsterPlayerCost[mtFly], m_nMonsterPlayerCost[mtFlyGoblin],
	m_nMonsterPlayerIncome[mtMoleBoSi], m_nMonsterPlayerIncome[mtMoleDaiMeng], m_nMonsterPlayerIncome[mtMoleFeiXing], m_nMonsterPlayerIncome[mtMoleHuShi], m_nMonsterPlayerIncome[mtMoleKuangGong], m_nMonsterPlayerIncome[mtMoleKing], m_nMonsterPlayerIncome[mtSpider], m_nMonsterPlayerIncome[mtFly], m_nMonsterPlayerIncome[mtFlyGoblin]);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);*/
}

//向GameServer即DB写捕鱼预结算分等关键信息
bool CGameServer::WritePreGameScore(DWORD dwChairID, const stUserMoney& llPreWinScore, LONGLONG llPreTotalScore, LONGLONG llGradeScore)
{
	int nResult = 0;
	tagSetFishPreScoreInfo sPreScoreInfo;

	IGamePlayer *lpPlayer = NULL;
	m_pList->GetUser(dwChairID,&lpPlayer);	

	int pID  = lpPlayer->get_ID();
	int UserDBID = lpPlayer->get_DBID();

	RecordServerLog(dwChairID, UserDBID);

	try{
		if( m_pTableChannel )
		{
			memset(&sPreScoreInfo, 0, sizeof(sPreScoreInfo));
			sPreScoreInfo.dwUserID = m_dwUserID[dwChairID];
			sPreScoreInfo.llWinScore = llPreWinScore.llMoneyNum;
			sPreScoreInfo.llRemainScore = llPreTotalScore;
			sPreScoreInfo.llGradeScore = llGradeScore;

			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Set_Fish_PreScore_Info, &sPreScoreInfo, sizeof(sPreScoreInfo));    

			// * /玩家写分
			//long lScore[5] = {0};
			//memset(lScore, 0, sizeof(lScore));
			//lScore[0] = lDeltaScore.lScore;
			// * /这里为什么还要写分？
			// * /m_pSite->WriteScore(dwChairID, (char*)lScore);

			char logdata[200];
			sprintf(logdata,"用户ID：%d   用户数据库ID: %d   写入数据库增量分数  %ld  玩家收入:%ld  玩家投入成本:%ld     " , pID, UserDBID, llPreTotalScore, llPreWinScore.llMoneyNum, llGradeScore);
			m_LogFile.WriteDataLogFile(logdata);

			m_dbUserPreWelFarePoint[dwChairID] = m_dbUserWelFarePoint[dwChairID];

			if (nResult >= S_OK)
			{// * /扩展写分成功返回
				return true;
			} 
			else
			{
				_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("ERROR:定时写预算分命令请求返回=%d."), nResult);
				m_LogFile.WriteErrorLogFile(m_szErrorInfo);
				return false;
			}
		}
		else
		{
			_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("Error:定时写预算分命令为空."));
			m_LogFile.WriteErrorLogFile(m_szErrorInfo);

			return false;
		}
	}catch (...){
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("Unknown Error as writePreScoresInfo()."));
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		TRACEDEBUG(m_szErrorInfo);
		return false;
	}

	return false;
}

void CGameServer::SendBasicInfo(DWORD byChairID, IGamePlayer *pPlayer)
{
	if (false == CheckValidChairID(byChairID) || NULL == pPlayer)
		return;

	int nLevel = 0;
	LONGLONG curExp = GetPlayerExp(byChairID, pPlayer, &nLevel);

	SY::RefreshUserFishExp pack;
	pack.set_userid(byChairID);
	pack.set_exp(curExp);
	pack.set_level(nLevel);

	char buf[200] = {0};

	if (pack.SerializeToArray(buf,sizeof(buf)))   //s13
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(), GameMessages::RefreshUserFishExp, buf,pack.ByteSize());
	}
}

// 发送金币给指定玩家
void CGameServer::SendUserMoneyCountTo(BYTE byChairID, IGamePlayer *pPlayer, bool bRealTimeMoney /* = false */)
{
	//变量定义

	if (false == CheckValidChairID(byChairID) || NULL == pPlayer)
		return;

	SY::UserMoneySync syCoinCount;
	//设置变量
	syCoinCount.set_chairid(byChairID);
	if (bRealTimeMoney)
	{
		syCoinCount.set_usermoneynum(m_UserGameScore[byChairID].llMoneyNum);  //
		syCoinCount.set_userticketnum(m_UserGameScore[byChairID].llTicketNum);
		syCoinCount.set_level(m_nUserLevel[byChairID]);
		syCoinCount.set_exp(m_llGradeScore[byChairID]);
	}
	else
	{
		syCoinCount.set_usermoneynum(GetPlayerScore(byChairID, pPlayer));  //
		syCoinCount.set_userticketnum(0);//GetPlayerTicket(byChairID, pPlayer));

		int nLevel = 0;
		LONGLONG curExp = GetPlayerExp(byChairID, pPlayer, &nLevel);
		syCoinCount.set_level(nLevel);
		syCoinCount.set_exp(curExp);
	}

	char buf[200] = {0};

	if (syCoinCount.SerializeToArray(buf,sizeof(buf)))   //s13
	{
		sendGameCmdProtBufTo(pPlayer->get_chair(),GameMessages::UserMoneySync,buf,syCoinCount.ByteSize());
	}
}

// 发送鱼轨迹信息给玩家
void CGameServer::SendAddFishCmd( int nCmd , char *pData , int nLen)
{
	GAME_CMD_DATA buf;
	buf.InitData(-1, nCmd);
	buf.AddData(pData,nLen);
	m_pServer->SendGameCmd(buf.GetBuf(), buf.GetDataLen());
}

//发送屏幕分辨率给玩家
void CGameServer::SendScreenResolution(int iWidth,int iHeight)
{
	SY::ScreenResolution syScreenResolution;
	syScreenResolution.set_client_width(iWidth);
	syScreenResolution.set_client_height(iHeight);

	char buf[200] = {0};

	if (syScreenResolution.SerializeToArray(buf,sizeof(buf)))   //s12
	{		
		sendGameCmdProtBufAll(GameMessages::ScreenResolution,buf,syScreenResolution.ByteSize());
	}
}

//移动版处理玩家上分
void CGameServer::MoblieDealUserAddScore(int ChairID)
{

	stUserMoney nAddSocre = m_UserGameScore[ChairID];

	if (nAddSocre.llMoneyNum < 0 || nAddSocre.llTicketNum < 0)
	{
		return;
	}


	////玩家购买子弹用了金币数
	//m_UserGameScore[ChairID] -= nAddSocre;
	////减去玩家总的可兑换的子弹数
	//m_UserGameScore[ChairID] -= nAddSocre;

	//购买了子弹，还未写分
	m_enUserState[ ChairID]   = enPlayerBuy;			//玩家上分状态

	//SY::AddScore syAddScore;
	//syAddScore.set_buy(true);
	//syAddScore.set_chairid(ChairID);
	//syAddScore.set_allscore(m_nUserBulletScore[ChairID]);
	//syAddScore.set_totalbasescore(m_UserGameScore[ChairID]);

	//char buf[200];

	//if (syAddScore.SerializePartialToArray(buf,sizeof(buf)))
	//{
	//	sendGameCmdProtBufAll(GameMessages::AddScore,buf,syAddScore.ByteSize());
	//}

}

//手机版处理玩家下分
void CGameServer::MoblieDealUserRemoveSocer(EnableLeaveQuery *pEnableLeaveQuery,IGamePlayer *pPlayer)
{
	if (pEnableLeaveQuery == NULL)
	{
		return;
	}

	//float StayTime = 0.0 ;

	int nChairID = pEnableLeaveQuery->m_ChairID;

	DWORD dwExitRoomTick = GetTickCount();
	DWORD dwEnterRoomTick = m_UserStatisticsData[pEnableLeaveQuery->m_ChairID].nUserEnterRoomTick;
	DWORD dwStayTime = dwExitRoomTick - dwEnterRoomTick;

	m_UserStatisticsData[pEnableLeaveQuery->m_ChairID].nUserExitRoomTick = dwExitRoomTick;

	//if(m_bLossPoolRepay)    //亏损池返还启用开关 
	//{
	//	if (m_llLossPoolCount[pEnableLeaveQuery->m_ChairID] > 0 )
	//	{
	//	
	//		if (dwStayTime > 3 * 60 * 1000)   //
	//		{
	//		
	//			if (m_llLossPoolCount[pEnableLeaveQuery->m_ChairID] < 0)
	//			{
	//				return;
	//			}		

	//			m_nUserBulletScore[pEnableLeaveQuery->m_ChairID] += m_nCellscore * m_llLossPoolCount[pEnableLeaveQuery->m_ChairID];


	//			SY::SubScore sySubScore;

	//			sySubScore.set_buy(true);
	//			sySubScore.set_buyscore(m_nUserBulletScore[pEnableLeaveQuery->m_ChairID]);
	//			sySubScore.set_allscore(0);
	//			sySubScore.set_chairid(pEnableLeaveQuery->m_ChairID);
	//			sySubScore.set_totalbasescore(m_UserGameScore[pEnableLeaveQuery->m_ChairID]);
	//			sySubScore.set_repayfromlosspool(m_nCellscore * m_llLossPoolCount[pEnableLeaveQuery->m_ChairID] + m_llSumFlyBulletScore);

	//			char buf[160];
	//			if (sySubScore.SerializePartialToArray(buf,sizeof(buf)))
	//			{
	//				sendGameCmdProtBufAll(GameMessages::SubScore,buf,sySubScore.ByteSize());
	//				OUTLOOK("玩家游戏时间超过3分钟，返还亏损池");
	//			}
	//		}
	//		else
	//		{			
	//			OUTLOOK("玩家游戏时间低于3分钟，不返还亏损池");
	//		}
	//	}
	//}


	//退出时计算玩家的分数 
	if ( enPlayerBuy == m_enUserState[pEnableLeaveQuery->m_ChairID] )
	{		
		//CalcScore( pPlayer , pEnableLeaveQuery->m_ChairID);
		CalcPreScore(pEnableLeaveQuery->m_ChairID);
		m_enUserState[pEnableLeaveQuery->m_ChairID]  = enPlayerSettled;
	}
	else
	{
		;//写分失败
	}   	


	//下分后玩家亏损池清0；
	//m_llLossPoolCount[pEnableLeaveQuery->m_ChairID] = 0;

	m_UserStatisticsData[pEnableLeaveQuery->m_ChairID].nUserExitRoomTick = 0;
	m_UserStatisticsData[pEnableLeaveQuery->m_ChairID].nUserEnterRoomTick = 0;    
}

void CGameServer::Statistic_Start(IGamePlayer* pPlayer, int nIdx)
{
	__time64_t			t = _time64(NULL);

	memset(m_Statistic + nIdx, 0, sizeof(Statistic_t));
	memset(m_Change + nIdx, 0, sizeof(Change_t));
	pPlayer->get_UserName(m_Statistic[nIdx].Init.UserName);
	m_Statistic[nIdx].Init.RoomID				= m_nRoomID;
	m_Statistic[nIdx].Init.TableID				= m_nTableID + 1;
	m_Statistic[nIdx].Init.DBID					= pPlayer->get_DBID(NULL);
	m_Statistic[nIdx].Init.InitMoney			= m_UserGameScore[nIdx].llMoneyNum;
	m_Statistic[nIdx].Init.Level				= m_nUserLevel[nIdx];
	m_Statistic[nIdx].Init.StartTime			= t;
	m_Statistic[nIdx].Accumulation.MostGain		= m_UserGameScore[nIdx].llMoneyNum;
	m_Statistic[nIdx].Accumulation.Level		= m_nUserLevel[nIdx];
	m_Statistic[nIdx].Accumulation.T			= t;
	m_Statistic[nIdx].Lasttime					= t;
	
#if 0
	Init_t		Init;

	pPlayer->get_UserName(Init.UserName);
	Init.RoomID		= m_nRoomID;
	Init.TableID	= m_nTableID;
	Init.StartTime	= _time64(NULL);
	Init.DBID		= pPlayer->get_DBID(NULL);
	Init.InitMoney	= m_UserGameScore[nIdx].llMoneyNum;
	Init.Level		= m_nUserLevel[nIdx];
//	m_pStatistic->PlayerSitdown(Init);

	// 将统计信息初始化
	memset(m_Change + nIdx, 0, sizeof(Change_t));
	m_Change[nIdx].RoomID	= m_nRoomID;
	m_Change[nIdx].TableID	= m_nTableID;
	m_Change[nIdx].DBID		= Init.DBID;
#endif
}

void CGameServer::Statistic_Change(IGamePlayer* pGamePlayer, int nIdx)
{
	int					i;
	Init_t				*pit;
	Accumulation_t		*pac;
	Delta_t				*pdl;
	Statistic_t			*pStatistic;
	Change_t&			Change = m_Change[nIdx];
	
	pStatistic	= m_Statistic + nIdx;
	pit			= &pStatistic->Init;
	pac			= &pStatistic->Accumulation;
	pdl			= &pStatistic->Delta;

	// 这里做一次汇总，
	pac->MoneyGain		+= Change.MoneyGain;				// 累加击杀总收入
	pac->MoneyCost		+= Change.MoneyCost;				// 累加击杀总支出
	pac->MissingCount	+= Change.MissingCount;				// 累加免伤判断次数
	pac->MissingValid	+= Change.MissingValid;				// 累加免伤生效次数
	pac->MissingGain	+= Change.MissingGain;				// 累加免伤系统收益

	pdl->MoneyGain		= Change.MoneyGain;					// 时段击杀总收益
	pdl->MoneyCost		= Change.MoneyCost;					// 时段击杀总支出
	pdl->MissingCount	= Change.MissingCount;				// 时段免伤判断次数
	pdl->MissingValid	= Change.MissingValid;				// 时段免伤生效次数
	pdl->MissingGain	= Change.MissingGain;				// 时段免伤系统收益

	for ( i = 0; i < SkillTypeCount; i++ )
	{
		pdl->SkillCount[i] = Change.SkillCount[i];			// 时段技能触发次数
		pac->SkillCount[i] += Change.SkillCount[i];			// 累加技能触发次数
	}

	for ( i = 0; i < MonsterTypeCount; i++ )
	{
		pdl->KillMonster[i] = Change.KillMonster[i];		// 时段击杀怪物数量(不管怪物是否掉钱)
		pdl->KillValid[i]	= Change.KillValid[i];			// 时段有效击杀数量(怪物掉钱了)
		pdl->KillCost[i]	= Change.KillCost[i];			// 时段击杀怪物支出
		pdl->KillGain[i]	= Change.KillGain[i];			// 时段击杀怪物收入
		pac->KillMonster[i] += Change.KillMonster[i];		// 累加击杀怪物数量(不管怪物是否掉钱)
		pac->KillValid[i]	+= Change.KillValid[i];			// 累加有效击杀数量(怪物掉钱了)
		pac->KillCost[i]	+= Change.KillCost[i];			// 累加击杀怪物支出
		pac->KillGain[i]	+= Change.KillGain[i];			// 累加击杀怪物收入
	}

	if (Change.Level > pac->Level)
		pac->Level = Change.Level;

	if (pac->MonsterRate <= Change.MonsterRate)				// 单只怪物最大倍率
	{
		pac->MonsterRate	= Change.MonsterRate;
		pac->MosterID		= Change.MosterID;
	}

	if (pac->MonsterMostGain < Change.MonsterMostGain)		// 单只怪物最大收益
		pac->MonsterMostGain = Change.MonsterMostGain;

	if (pac->MostGain < pit->InitMoney + pac->MoneyGain - pac->MoneyCost)		// 最大收益额（开始的时候等于初始金额）
	{
		pac->MostGain	= pit->InitMoney + pac->MoneyGain - pac->MoneyCost;		// 新的最大收益
		pac->T			= Change.T;												// 最大收益时间点
	}
	
	m_pStatistic->IncomingChange(m_Statistic[nIdx]);
	pStatistic->Lasttime = _time64(NULL);

	// 重新初始化统计信息
	memset(m_Change + nIdx, 0, sizeof(Change_t));
}

void CGameServer::Statistic_End(IGamePlayer* pPlayer, int nIdx)
{
	m_pStatistic->PlayerLeave(m_Statistic[nIdx]);
	memset(m_Statistic + nIdx, 0, sizeof(Statistic_t));
	memset(m_Change + nIdx, 0, sizeof(Change_t));
}
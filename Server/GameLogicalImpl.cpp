#include "StdAfx.h"
#include "GameLogi.h"
#include "Protocol/GameMessageTags.h"
#include "utility.h"
#include "MonsterManager.h"
#include "StatisticMgr.h"

// * /porting from FishgSea V2.5.0版本号数字定义, 22Oct.2015
// * /数据定义版本号：定义方式如下：创建数据游戏ID占n位整数×1000000000，
// * /主版本号占3位整数×1000000，次版本占3位整数×1000，最低版本3位整数；即捕鱼的2.5.0为数据为31002005000
#define CUR_FISHING_VERSION	31002008000
//任路 2014-07-25 tagFishingData共享数据新增dwLossPool保存玩家亏损值
//增加OLD_FISHING_VERSION 记录旧的版本号 ， CURR_FISHING_VERSION要提升新的版本号来达到兼容旧版本
#define OLD_FISHING_VERSION_1	31002005000				//最初的版本号
#define OLD_FISHING_VERSION_2	31002006000				//由于回退失识造成部分玩家的版本为 31002006000
#define OLD_FISHING_VERSION_3	31002007000				
#define VER_FIELD_REMAINDER      1000000000         // * /余数为记录版本号的字段，22Oct.2015

//IGameLogical
STDMETHODIMP CGameServer::SetServerSite(IServerSite * pGameSite)
{
	m_pSite = pGameSite;
	m_pSite->AddRef();

	m_pSite->GetPlayerList(&m_pList);
	m_pSite->GetServerBase(&m_pServer);

	m_quickStartelper.SetServerSite(pGameSite);

	m_dwCurTick = GetTickCount();

	m_nRoomID = m_pSite->GetGameOption();      //分段自定义

	//获取房间信息 房间ID和桌子ID
	InitRoomInfo();

	//设置房间名
	char szServerName[MAX_NAME_LEN] = {0};
	m_pServer->QueryInterface(IID_ITableChannel, (void **)&m_TableChannel);

	if (m_TableChannel != NULL)
	{
		m_TableChannel->TableDispatch(ITableChannel::IID_Get_Room_Name, szServerName, MAX_NAME_LEN);

		StatisticMgr::getInstance().SetRoomName(szServerName);
	}

	//LOGI("m_nRoomID=%d, m_nTableID=%d", m_nRoomID, m_nTableID);
	CMonsterManager::Instance().Initialize(m_nRoomID, m_nTableID);

	// 服务器统计
	m_pStatistic = const_cast<CStatistic*>(CStatistic::Instance());
	m_pStatistic->SetRoomName(szServerName);

	//设置规则号即房间号
	m_LogFile.SetGameRoomID(m_nRoomID);

	//	m_pTableChannel->TableDispael::IID_Get_TableID, (void*)&m_nTableID, 0);

	//日志记录文件初始化
	//m_LogFile.WriteErrorLogFile("Table Initialize.", LOG_OPEN);
	//m_LogFile.WriteDataLogFile("SetServerSite...", LOG_OPEN);

	return S_OK;
}

STDMETHODIMP CGameServer::SetTableID(int nTableID)
{
	m_nTableID = nTableID;

	return S_OK;
}

STDMETHODIMP_(int) CGameServer::GetRoomID()
{
	return m_nRoomID;
}

STDMETHODIMP_(int) CGameServer::GetTableID()
{
	return m_nTableID;
}

STDMETHODIMP CGameServer::ConstructScene()
{
	m_quickStartelper.Reset();

	//	在这里构造最初的游戏现场，注意ConstructScene并不意味着游戏开始
	//  TODO:想清楚自己的数据结构，这个函数在每轮开始的时候被调用。

	m_started=false;

	m_dwCurTick = GetTickCount();   // 得到当前tick

	KillGameTimer(TIME_ADD_MONSTER);													//添加鱼
	KillGameTimer(TIME_ADD_GROUP_MONSTER);												//产生鱼轨迹
	KillGameTimer(TIME_CLEAR_MONSTER);													//清除鱼轨迹
	KillGameTimer(TIME_CHANGE_SCENE);												//切换场景
	//KillGameTimer(TIME_REGULAR_FISH);												//生成规则鱼																			
	KillGameTimer(TIME_CHECK_POOL);													//加载配置文件	
	//KillGameTimer(TIME_LOAD_FILE);													//检查奖池  
	

	//static int initcount;
	//initcount++;
	//OUTLOOK("初始化次数%d", initcount);

	// 初始化游戏数据
	InitData();
	//游戏规则号，用着房间号的辨识
	//m_nRoomID = m_pSite->GetGameOption();   //TODO: 与m_nGameRoomID重复，直接替换掉

	// 更新好的刷新机制是一段间隔刷一波怪， 客户端记录所有数据， 再按自己的频率刷新出来（即并不是服务器通知就刷新怪）
	const GamePropertyInfo* infGameProperty = GameConfigMgr::getInstance().Current().GetGamePropertyInfo(m_nRoomID);
	SetGameTimer(TIME_ADD_MONSTER, infGameProperty->AddMonsterTime, INFINITE, 0L);		                    //添加鱼    3秒
	SetGameTimer(TIME_ADD_GROUP_MONSTER, infGameProperty->AddGroupMonsterTime, INFINITE, 0L);                 //组添加鱼    10秒
	SetGameTimer(TIME_CLEAR_MONSTER, infGameProperty->KillMonsterTime, INFINITE, 0L);						//清除鱼轨迹 1秒
	SetGameTimer(TIME_CHANGE_SCENE, infGameProperty->ChangeScrenceTime, INFINITE, 0L);						//改变场景   30秒
	//SetGameTimer(TIME_LOAD_FILE, infGameProperty->LoadFileTime, INFINITE, 0L);								//配置文件刷时周期 60秒
	SetGameTimer(TIME_CHECK_POOL, infGameProperty->CheckPoolTime, INFINITE, 0L);								//检查奖池 5秒
	m_pServer->StartTimer(STANDARD_TIME_ELAPSE);
	return S_OK;
}

//狂暴状态  更改刷新怪和组合怪时间  清除怪时间
void CGameServer::change_monster_time(float rate/* = 1*/)
{
	KillGameTimer(TIME_ADD_MONSTER);
	KillGameTimer(TIME_ADD_GROUP_MONSTER);
	KillGameTimer(TIME_CLEAR_MONSTER);
	const GamePropertyInfo* infGameProperty = GameConfigMgr::getInstance().Current().GetGamePropertyInfo(m_nRoomID);
	SetGameTimer(TIME_ADD_MONSTER, (DWORD)(infGameProperty->AddMonsterTime/rate), INFINITE, 0L);		                    
	SetGameTimer(TIME_ADD_GROUP_MONSTER, (DWORD)(infGameProperty->AddGroupMonsterTime/rate), INFINITE, 0L);                
	SetGameTimer(TIME_CLEAR_MONSTER, (DWORD)(infGameProperty->KillMonsterTime/rate), INFINITE, 0L);						
}
STDMETHODIMP CGameServer::GetPersonalScene(int nIndex, UINT uflags, IGameLogical::SCENE * pScene)
{
	Game_SCENE * pGameScene =(Game_SCENE *)pScene;
	player_index = nIndex;
	// TODO:填写Game_SCENE结构，接下去系统会自动把这个结构传送给客户端
	// 客户端会在OnSceneChanged当中处理这里的数据
	// 注意，这个结构通常都是变长的。

	int nGameScene = (int)m_enGamePhase;
	IGamePlayer *pPlayer = NULL;
	GAME_CMD_DATA buf;
	int nSize = 0;

	BYTE byBuffer[2048] = { 0 };
	int wSendSize = 0;

	if (FAILED(m_pList->GetUser(nIndex, &pPlayer)) || (pPlayer && FALSE == pPlayer->IsValidUser()))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("GetPersonalScene() %d NOT be valid User."), nIndex);
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		return S_OK;
	}

	m_dwCurTick = GetTickCount();

	switch (nGameScene)
	{
	case GS_FREE:		//空闲状态
	case GS_PLAYING:	//游戏状态 
		{
			//发送信息
			if (pPlayer->IsPlayer())
			{
				//获得玩家的分数即金币数
				//m_UserStatisticsData[nIndex].llUserCarryScore.llMoneyNum = GetPlayerScore(nIndex, pPlayer);
				//发送分数到客户端
				SendUserMoneyCountTo(nIndex, pPlayer);
				GameStationBase *pGameScene = (GameStationBase *)pScene->lpdata;

				if (!pGameScene)
				{
					_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("GameStationBase Create Failed    Player = %d"), nIndex);
					m_LogFile.WriteErrorLogFile(m_szErrorInfo);
					//GLOG_NORMAL(m_nRoomID*10+5,m_szErrorInfo);
					return false;
				}

				pGameScene->bPlaying = true;					//游戏是否已经开始了
				pGameScene->bySceneIndex = (BYTE)m_nSceneIndex;				//当前场景

				for (int i = 0; i<PLAY_COUNT; i++)
				{
					pGameScene->nUserMoneyNum[i] = (int)m_UserGameScore[i].llMoneyNum; // m_UserStatisticsData[i].llUserCarryScore.llMoneyNum;
					pGameScene->nUserTicketNum[i] = (int)m_UserGameScore[i].llTicketNum; // m_UserStatisticsData[i].llUserCarryScore.llTicketNum;
				}

				//填充技能和武器状态信息
				for (int i = 0; i < WEAPON_COUNT; i++)
				{
					int nWeaponCost = -1;
					const WeaponCostInfo* pInfWeapon = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, (WeaponID)(i + 1));
					if (pInfWeapon != nullptr)
					{
						nWeaponCost = pInfWeapon->Cost;
					}

					pGameScene->nWeaponCost[i] = nWeaponCost;
				}

				for (int i = 0; i < SKILL_COUNT; i++)
				{
					pGameScene->SkillInf[i].nSkillID = i + 1;
					pGameScene->SkillInf[i].SkilllState = SY::SkillStatus::Actived;  //TODO: 根据取出来的数据填充
				}

				//鱼儿赔率
				//for (int i = 0; i<26; i++)
				//{
				//    pGameScene->nFishScore[i] = m_nFishScore[i];
				//}
			}

			//发送场景
			nSize += sizeof(GameStationBase);
			pScene->cbSize = nSize + sizeof(int);
		}
		break;
	}
	SAFE_RELEASE(pPlayer);

	return S_OK;
}

STDMETHODIMP CGameServer::Start()
{
	//	在这里，游戏才开始（客户端已经开始）
	m_started=true;
	m_dwCurTick = GetTickCount();
	SetGameStatus(GS_PLAYING);
	KillGameTimer(TIME_WILD_SKILL_DELEY);
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		SetGameTimer(TIME_WILD_SKILL_DELEY,5*1000,INFINITE,0L);
		wild_skill_status = false;
		wild_skill_blank = 0;
		wild_skill_monster_map.clear();
//		timer_start_num++;
	}
//	player_sum++;
	return S_OK;
}
//延时启动
void CGameServer::wild_skill_timer_deley()
{
	//2016.10.20 hl  狂暴功能相关timer
	KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);
	KillGameTimer(TIME_WILD_SKILL_DELEY);
	//狂暴状态
	//2016.10.20 hl 关于狂暴状态计时器//如果开启就开启计时器
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		SetGameTimer(TIME_WILD_SKILL_WAIT_STEP,DEFAULT_WILD_SKILL_ELSPSE,INFINITE,0L);
		wild_skill_blank = 0;
		wild_skill_status = false;
		wild_skill_wait_step();
	}
}

STDMETHODIMP CGameServer::Stop(UINT uflags)
{
	m_quickStartelper.Reset();

	//应该仔细的处理stop的uflags，不同的flag指示了停止的理由
	switch (uflags) {
	case STOP_SERVERERROR:
		m_pSite->Dismiss("服务器停机或者网管解散");
		// TODO:如果要有额外处理，加入代码
		break;
	case STOP_NOENOUGHPLAYER:
		if (m_started) 
		{
			//找到引起该事件的人，此人逃跑
			//IGamePlayer * pPlayer =NULL;
			//for (int i=0; i<4; i++) {
			//	if (FAILED(m_pList->GetUser(i, &pPlayer))) continue;

			//	if (pPlayer->IsValidUser()) continue;
			//	//	如果不是valid,此人逃跑。注意，只可能是一个人引起的停机
			//	// TODO: 计算赢输分
			//	m_pSite->OnGameEnd();
			//}
			//m_pSite->Dismiss("有人逃跑");
		}
		else {
			//m_pSite->Dismiss(NULL);
		}
		break;
	default:	// * /add:其他原因停机解散就可以了，06May.2011
		m_pSite->Dismiss("其他原因，桌子解散！");
	}

	m_pServer->StopTimer();		// * /Add for..as you see

	//2016.10.20 hl  狂暴功能相关timer
	KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);                                        //狂暴状态
	KillGameTimer(TIME_WILD_SKILL_END);
	KillGameTimer(TIME_WILD_SKILL_WILL_END);
	wild_skill_monster_map.clear();
	return S_OK;
}

STDMETHODIMP CGameServer::Clear()
{
	m_started =false;

	//m_nRoomID = 0;
	//系统数据
	memset(m_userName, 0, sizeof(m_userName));														//置0玩家姓名
	memset(m_dwUserID, 0, sizeof(m_dwUserID));														//置0玩家ID
	memset(m_lDrawnCount, 0, sizeof(m_lDrawnCount));											//置0玩家多少局
	memset(m_enUserState, enPlayerNull, sizeof(m_enUserState));							//默认都是空状态 

	memset(m_UserStatisticsData, 0, sizeof(m_UserStatisticsData));								//游戏中使用的分数
	not_normal_dead_king.clear();
	return S_OK;
}

STDMETHODIMP CGameServer::GetScore(int nIndex, void * lpScoreBuf)
{
	ASSERT(lpScoreBuf);
	Game_SCORE * pScore =(Game_SCORE *)lpScoreBuf;

	pScore->Clear();

	return E_NOTIMPL;
}

STDMETHODIMP CGameServer::OnGameOperation(int nIndex, DWORD dwUserID, int opID, LPOPDATA oData, int nSize)
{
	WriteInfoLog(Format("CGameServer::OnGameOperation 来了消息：%d", opID));

	//游戏没开始才检查
	if (0x70 == opID && !m_started)	    //p对应asc值为0x70
	{
		if (SUCCEEDED(m_quickStartelper.OnGameOperation(nIndex, dwUserID, opID, oData))) 
		{
			m_started = true;		//捕鱼就认为开始了

			m_UserStatisticsData[nIndex].llUserCarryScore = m_UserGameScore[nIndex];
			m_UserStatisticsData[nIndex].dwLastHitTime = GetTickCount();  
			m_UserStatisticsData[nIndex].nUserEnterRoomTick = GetTickCount();     //记录玩家进入房间tick
		}

		IGamePlayer * _pPlayer = NULL;
		int nState = -1;

		//获得用户数据
		if (FAILED(m_pList->FindUser(dwUserID, &_pPlayer)) || false == CheckValidChairID(nIndex)) 
		{
			//在m_pList中不存在意味着该用户不是player
			//throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTPLAYER, "非玩家用户发出数据");
		}
		else
		{
			//发送区域信息
			{
				SY::ArenaInfo pack;
				pack.set_id(0);
				pack.set_game_id(0);
				pack.set_type(0);
				pack.set_min_money(GameConfigMgr::getInstance().Current().GetEnterLimit(m_pServer->GetGameOption()));
				pack.set_rule(m_pServer->GetGameOption());
				pack.set_level(0);
				pack.set_name("");
				
				char buf[200] = {0};

				if (pack.SerializeToArray(buf, sizeof(buf)))
				{
					sendGameCmdProtBufTo(nIndex, GameMessages::ArenaInfo, buf, pack.ByteSize());
				}
			}

			//发送基础信息
			SendBasicInfo(nIndex, _pPlayer);
			//发送同步数据
			SendAdditionalGameData(nIndex);
			//判断是否需要自动激活技能
			CheckCanAutoActiveSkill(_pPlayer, true);
			//发送共享数据
			SendUserMoneyCountTo(nIndex, _pPlayer, true);
		}

		return S_OK;
	}
	

	//出错描述
	struct OPERATION_ERROR {
		enum {
			UNDEFERROR,
			USERISNOTPLAYER,
			USERISNOTACTIVATE,
			OUTCARDISNOTEXIST
		};
		int nCode;
		char szDesc[255];

		OPERATION_ERROR(int nCode, char * desc=NULL) {
			nCode  =nCode;
			if (desc) lstrcpyn(szDesc, desc, sizeof(szDesc));
		}
	};

	IGamePlayer * pPlayer = NULL;
	int nState = -1;

	try {
		//获得用户数据
		if (FAILED(m_pList->FindUser(dwUserID, &pPlayer)) || false == CheckValidChairID(nIndex)) 
		{
			//在m_pList中不存在意味着该用户不是player
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTPLAYER, "非玩家用户发出数据");
		}

		//游戏未开始不处理玩家命令
		if (false == m_started)
		{
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTACTIVATE, "游戏未开始不处理玩家命令");
		}

		//设置用户的ID
		if (0 == m_dwUserID[nIndex])
		{
			//获得DBID<br class="Apple-interchange-newline">
			m_dwUserID[nIndex] = dwUserID;
			//获得用户名
			pPlayer->get_UserName(m_userName[nIndex]);
			m_userName[nIndex][63] = '\0';
		}

		//消息处理
		switch (opID)
		{
		case GameMessages::GameDataReq:
			//收到这个消息立即反发一个金币消息
			CheckCanReturnUserCoin(pPlayer);
			SendUserMoneyCountTo(nIndex, pPlayer, true);

			m_UserStatisticsData[nIndex].bClientReady = true;

			WriteInfoLog("处理了GameDataReq");
			break;

		case GameMessages::BuySkillReq:
			OnBuySkill(pPlayer, oData, nSize);
			break;
		case GameMessages::BuyAutoAttackReq:
			OnBuyAutoAttack(pPlayer, oData, nSize);
			break;
		case GameMessages::BuyHammerLevelReq:
			OnBuyAutoHammer(pPlayer,oData,nSize);
			break;
		case GameMessages::ChangeSkillStatusReq:
			OnChangeSkillStatus(pPlayer, oData, nSize);
			break;                          
		case GameMessages::ChangeWeaponReq:	// 切换锤子
			//OnChangeWeapon(pPlayer, oData, nSize);
			break;         
		case GameMessages::UpgradeSkillReq: // 技能升星
			OnUpgradeSkill(pPlayer, oData, nSize);
			break;
		case GameMessages::PlayerHit:		// 玩家点击
			OnPlayerHit(pPlayer, oData, nSize);
			break;
		default:
			TRACEWARNING("未处理的命令:%d", opID);
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTACTIVATE, "未知道命令");
			break;
		}
	} catch (OPERATION_ERROR err) {
		//输出错误
		/*	错误信息如下
		 *	--Error occured in OnGameOperation --- xxxxxx
		 *	----from
		 *  ------user name : xxxxx
		 *	------user db ID :	xxxx
		 *	----other information
		 *	----
		 */
		Trace("--Error occurred in OnGameOperation ---- %s", err.szDesc);
		Trace("----from");
		char playername[255];
		if (!pPlayer) m_pList->FindUser(dwUserID, &pPlayer);
		if (!pPlayer) strcpy(playername, "unknown player");
		else pPlayer->get_UserName(playername);
		Trace("------user name : %s", playername);
		if (pPlayer) {
			TABLE_USER_INFO userInfo;
			pPlayer->GetPlayer(&userInfo);
			Trace("------user db ID : %d", userInfo.lDBID);
		}

		//对于不同的错误，处理不一样
		switch (err.nCode) {
		case OPERATION_ERROR::USERISNOTPLAYER:
			//	非玩家发包，直接断线
			m_pSite->GetOff(dwUserID);
			break;
		case OPERATION_ERROR::USERISNOTACTIVATE:
			//非活动用户发包，忽略
			break;
		case OPERATION_ERROR::OUTCARDISNOTEXIST:
			//包数据错，断线/回复到上一次
			m_pSite->GetOff(dwUserID);
			break;
		// TODO: 加入其他的处理代码
		default:
			//	其他问题。断线总是保险的做法
			m_pSite->GetOff(dwUserID);
		}

		Trace("-----");
	}
	catch(...) {
		//未知错误发生
		Trace("--Unknown error occured in OnGameOperation");
	}

	SAFE_RELEASE(pPlayer);
	return S_OK;
}

//捕鱼共享数据信息
bool CGameServer::GetFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData)
{
	HRESULT hResult = S_FALSE;
	tagFishingData* pDataBuf = NULL;
//	SGameData sTmpGameData;
	tagUpdateFishingData sServerFishingData;

	try
	{
		if (m_pTableChannel != NULL)
		{
			sServerFishingData.dwUserID = dwUserID;

			hResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Get_Fishing_Data, &sServerFishingData, sizeof(tagUpdateFishingData));

			if (S_OK == hResult)
			{
				pDataBuf = (tagFishingData*)sServerFishingData.byBuffer;
				memcpy(pData, pDataBuf, sizeof(*pData));

				ValidateFishingCommonData(pData);
			}
			else
			{
				TRACEFALTAL("User=%.64s get getFishingData result be FAIL!", m_userName[dwChairID]);

				return false;
			}
		}
		else
		{
			TRACEFALTAL("User=%.64s Fail to get getFishingData Table channel be NULL!", m_userName[dwChairID]);

			return false;
		}
	}
	catch (...)
	{
		TRACEFALTAL("User=%.64s Error Catch: getFishingData catch error", m_userName[dwChairID]);

		return false;
	}

	return true;
}

bool CGameServer::SaveFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData)
{
	int nResult = 0;
	int nSize = 0;
	tagUpdateFishingData sServerFishingData;

	try
	{
		if (m_pTableChannel)
		{
			sServerFishingData.dwUserID = dwUserID;

			SGameData *pGameData = &(pData->sEnyGameData);		// * /能量条记录检查

			nSize = sizeof(pGameData->wRecEnergyBarCount) + pGameData->wRecEnergyBarCount * sizeof(SRecEnergyBar) + sizeof(SWelfareData);
			if (nSize > 512 || pGameData->wRecEnergyBarCount > CountArray(pGameData->arRecEnergyBar))
			{
				TRACEFALTAL(_T("Error: user=%.64s 保存游戏数据长度(%d)超过了512或能量条数组数(%d)超过了最大值！"), m_userName[dwChairID], nSize, (int)CountArray(pGameData->arRecEnergyBar));
				return false;
			}

			memcpy(sServerFishingData.byBuffer, pData, sizeof(sServerFishingData.byBuffer));

			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Set_Fishing_Data, &sServerFishingData, sizeof(tagUpdateFishingData));

			if (S_OK != nResult)	// * /modify >= 0, 09Jan.2012
			{
				TRACEFALTAL(_T("User=%.64s userID=%u,save saveFishingData result be FAIL!"), m_userName[dwChairID], dwUserID);

				Trace("save saveFishingData result be FAIL!");
				return false;
			}
		}
		else
		{
			TRACEFALTAL(_T("User=%.64s Fail to save saveFishingData Table channel be NULL!"), m_userName[dwChairID]);

			Trace("Fail to save saveFishingData Table channel be NULL!");
			return false;
		}
	}
	catch (...)
	{
		TRACEFALTAL(_T("User=%.64s Error: saveFishingData catch error"), m_userName[dwChairID]);

		Trace("Error: saveFishingData catch error");
	}
	return false;
}

bool CGameServer::ValidateFishingCommonData(tagFishingData* pData)
{
	if (NULL == pData)
	{
		return false;
	}

	// * /数据定义版本号：定义方式如下：创建数据游戏ID占n位整数×1000000000，
	// * /主版本号占3位整数×1000000，次版本占3位整数×1000，最低版本3位整数；即捕鱼的2.5.0为数据为31002005000
	LONGLONG llGetInitGameID = pData->llVersion / VER_FIELD_REMAINDER;
	LONGLONG llGetVersionNo = pData->llVersion % VER_FIELD_REMAINDER;

	//旧的版本号
	LONGLONG llOldVersionNo1 = OLD_FISHING_VERSION_1 % VER_FIELD_REMAINDER;
	LONGLONG llOldVersionNo2 = OLD_FISHING_VERSION_2 % VER_FIELD_REMAINDER;
	LONGLONG llOldVersionNo3 = OLD_FISHING_VERSION_3 % VER_FIELD_REMAINDER;

	//新的版本号
	LONGLONG llCurVersionNo = CUR_FISHING_VERSION % VER_FIELD_REMAINDER;


	//任路 2014-07-25 增加了亏损值，为了兼容旧的数据。需要根据版本号进行判断
	//如果当前版本号是旧版本，就修改玩家的数据为新版本和新的共享数据结构大小
	if ( llGetVersionNo == llOldVersionNo1   ||
		 llGetVersionNo == llOldVersionNo2   ||
		 llGetVersionNo == llOldVersionNo3   )
	{
		//Trace("玩家%s 临时打印 旧版本号%I64d  ",m_userName[dwChairID],pFishingData->llVersion);
		//给玩家设置新的版本
		pData->llVersion = CUR_FISHING_VERSION;
		//把共享数据结构大小给他改变为最新的大小
		pData->wDataLen = sizeof(tagFishingData);
		//如果是旧版本，那么必须要初始化他的亏损值为0
		memset(pData->arLossPool, 0 , sizeof(tagLossPool) * EXTEND_DATA_COUNT );
		
		//Trace("临时打印 玩家%s 给玩家设为新的版本号 %I64d    亏损数组中第一个的值 %d  最后一个值 %d    等级%I64d  新版本数据大小%d"  , m_userName[dwChairID],pFishingData->llVersion,pFishingData->arLossPool[0].dwLossPool ,pFishingData->arLossPool[19].dwLossPool  ,pFishingData->llGradeScore ,  pFishingData->wDataLen);
		return true;
	}
	else if ( llGetVersionNo == llCurVersionNo && sizeof(tagFishingData) == pData->wDataLen)
	{
		//Trace("当前为新版本号不用处理  %I64d"  , llCurVersionNo);
		//新版本什么事情都不用做，直接返回TRUE
		//Trace("临时打印 玩家%s  已经是最新版本号",m_userName[dwChairID] );
		return true;
	}
	// * /检查版本号与数据大小，不对就需要初始化 任路 2014-07-25 增加了亏损值，数据结构变大了，兼容以前的数据大小并修改为最新的数据大小 sizeof(tagFishingData) -sizeof(DWORD) == pFishingData->wDataLen
/*	if (llGetVersionNo == llCurVersionNo && (  (sizeof(tagFishingData)  - sizeof(DWORD) ) == pFishingData->wDataLen || sizeof(tagFishingData) == pFishingData->wDataLen) )
	{// * /数据版本与尺寸正确
		return true;
	}*/

	memset(pData, 0, sizeof(tagFishingData));
	pData->llVersion = CUR_FISHING_VERSION;
	pData->wDataLen = sizeof(tagFishingData);
	//Trace("临时打印 玩家%s 新来的玩家的版本号 %I64d      两个不同倍数的亏损值 %d   %d    等级 %I64d"  ,m_userName[dwChairID],pFishingData->llVersion,pFishingData->arLossPool[0].dwLossPool ,pFishingData->arLossPool[19].dwLossPool,pFishingData->llGradeScore);
	return false;
}

bool CGameServer::GetCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData)
{
	HRESULT hResult = S_FALSE;
	UserInteractData* pDataBuf = NULL;
//	SGameData sTmpGameData;
	tagGameToolInfo gameToolInfo;

	try
	{
		if (m_pTableChannel != NULL)
		{
			gameToolInfo.dwUserID = dwUserID;

			hResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Get_GameToolInfo, &gameToolInfo, sizeof(gameToolInfo));

			if (S_OK == hResult)
			{
				pDataBuf = (UserInteractData*)gameToolInfo.byBuffer;
				memcpy(pData, pDataBuf, sizeof(*pData));
			}
			else
			{
				TRACEFALTAL("User=%.64s get GetCustomData result be FAIL!", m_userName[dwChairID]);

				return false;
			}
		}
		else
		{
			TRACEFALTAL("User=%.64s Fail to get GetCustomData Table channel be NULL!", m_userName[dwChairID]);

			return false;
		}
	}
	catch (...)
	{
		TRACEFALTAL("User=%.64s Error Catch: GetCustomData catch error", m_userName[dwChairID]);

		return false;
	}

	return true;
}

bool CGameServer::SaveCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData)
{
	int nResult = 0;
	int nSize = 0;
	tagGameToolInfo gameToolInfo;

	try
	{
		if (m_pTableChannel)
		{
			gameToolInfo.dwUserID = dwUserID;

			memcpy(gameToolInfo.byBuffer, pData, sizeof(gameToolInfo.byBuffer));

			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Set_GameToolInfo, &gameToolInfo, sizeof(gameToolInfo));

			if (S_OK != nResult)	// * /modify >= 0, 09Jan.2012
			{
				TRACEFALTAL(_T("User=%.64s userID=%u,SaveCustomData result be FAIL!"), m_userName[dwChairID], dwUserID);

				Trace("SaveCustomData result be FAIL!");
				return false;
			}
		}
		else
		{
			TRACEFALTAL(_T("User=%.64s Fail to SaveCustomData Table channel be NULL!"), m_userName[dwChairID]);

			Trace("Fail to SaveCustomData Table channel be NULL!");
			return false;
		}
	}
	catch (...)
	{
		TRACEFALTAL(_T("User=%.64s Error: SaveCustomData catch error"), m_userName[dwChairID]);

		Trace("Error: SaveCustomData catch error");
	}
	return false;
}

// * /计算通用捕鱼等级, 27Oct.2015
int CGameServer::SendGeneralLevel(int nChairID, IGamePlayer *pPlayer)
{
	SendBasicInfo(nChairID, pPlayer);

	return 0;
}

void CGameServer::WriteInfoLog(CString szMsg)
{
	if (!m_bEnableInfoLog)
	{
		return;
	}

	CString szOutput;
	szOutput.Format("用户ID【%ld】-->%s", m_iLogID, szMsg);

	CTime tmTime(time(NULL));

	CString szFileName;
	szFileName.Format("锤耗儿跟踪日志_%d_%s", m_iLogID, tmTime.Format("%Y%m%d"));

	TRACEFALTALFILE((LPSTR)(LPCSTR)szFileName, (LPSTR)(LPCSTR)szOutput);
}

void CGameServer::WriteKillLog(CString szmsg)
{
	return ;
	CString szOutput;
	szOutput.Format("用户ID【%ld】-->%s", m_iLogID, szmsg);

	CTime tmTime(time(NULL));

	CString szFileName;
	szFileName.Format("锤耗儿金币日志_%d_%s", m_iLogID, tmTime.Format("%Y%m%d"));

	TRACEFALTALFILE((LPSTR)(LPCSTR)szFileName, (LPSTR)(LPCSTR)szOutput);
}
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
	
	m_pPrize = NULL;	// * /�ʳؽӿ�

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
	// TODO:�ͷ�������Դ

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
		// * /�йܲ�ѯ�ӿ�ʵ����
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
	// TODO:��д��Ϸ���еĹ���ʱ�䡣����������ʱ���1/5����ôϵͳ����Ʒ֡�ֱ�ӷ���E_NOTIMPL���Թر�������

	return S_OK;
}

STDMETHODIMP CGameServer::GetPlayerNum(IGameConst::PLAYERNUMBER * pNumber)
{
	ASSERT(pNumber);

	// TODO:��д��ҵ�����
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

	// TODO: ��д�ڲ��汾��
	// ����������ڱ������ʹ���ϵĿͻ����������µķ�����
	pVS->curVersion = MAKEWORD(0,1);
	pVS->maxVersion = MAKEWORD(0,1);
    pVS->minVersion = MAKEWORD(0,1);
	
	return S_OK;
}

// * /ʵ�����ʳؽӿ�
STDMETHODIMP CGameServer::SetPrizeInterface(IPrizeFund * pPrize)
{
	return S_OK;
}

void CGameServer::SetAdditionalData(DWORD dwUserID, void* apData, int size)
{

}

// * /IGameQuery�ӿڣ��йܴ����ѯ�ӿ�
//����ֵ��S_OK����ʾ��֧�ָò�ѯ
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

			//�����ﴦ������·ְ���������Ϣ
			MoblieDealUserRemoveSocer(stELQ,lpPlayer);

			stELQ->m_EnableLeave = 1;

			return S_OK;
		case IGameQuery::IID_GameEndStatus:				//������³���
		{
			return S_OK;
		}	
		case IID_GetGameSupportNo:	// * /��������Ϸ��Ҫ�������ݿ�ʱ֧��
			*((int*)lpData) = emSupptFishData;
			return S_OK;
		default:
			break;
	}
	
	return E_FAIL;
}

//�����´δ����ļ�ʱ��    //TODO
int CGameServer::GetComingTimerTick(DWORD &dwComingID)
{
	int nComingTick = STANDARD_TIME_ELAPSE;
	int nMinTick = MAX_TICK_COUNT;                   //����ʱ��������  1000000000
	int i = 0, nNextElapse = 0;
	DWORD dwCurTick = m_dwCurTick; //GetTickCount();
	//LOGI("��ǰtick %d",m_dwCurTick);

	dwComingID = 0;
	for (i = 1; i<= TIMER_COUNT; ++i)         //��ʱ������ = 12
	{
		if (TRUE == m_arTimerManage[i].bEnable && 0 != m_arTimerManage[i].dwRepeat)
		{
			nNextElapse = (m_arTimerManage[i].dwSetTick + m_arTimerManage[i].dwElapse) - dwCurTick;       //�´�ʱ����
			if (nNextElapse < nMinTick)
			{
				nMinTick = nNextElapse;             //�ҵ��´�ʱ�����е���Сֵ
				dwComingID = i;	                    //������Сֵ��Ӧ��i
			}	
		}
	}
	if (nMinTick < 200)
	{
		nMinTick = 200;                              //��С���������ܵ���200ms
	}
	if (nComingTick > nMinTick)						//
	{
		nComingTick = nMinTick;
	}
	return nComingTick;
}

// * /��ѯӦ�ü���ļ�ʱ��
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

//ģ��Դ���ʱ���ӿ�
//���ö�ʱ��
bool CGameServer::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	if (dwTimerID< 0 || dwTimerID > TIMER_COUNT || dwElapse < 0 || dwRepeat <= 0)
	{
		return false;
	}
	m_arTimerManage[dwTimerID].bEnable = TRUE;            //          ��ʱ����Ч
	m_arTimerManage[dwTimerID].dwElapse = dwElapse;					// 
	m_arTimerManage[dwTimerID].dwRepeat = dwRepeat;					//
	m_arTimerManage[dwTimerID].wBindParam = wBindParam;
	m_arTimerManage[dwTimerID].dwSetTick = m_dwCurTick;	//

	return true;
}

//�������ü�ʱ��ʱ��
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

//ɾ����ʱ��
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

void CGameServer::OnTimePass()          //��ʱ���¼�
{
	DWORD dwTimerID = CheckActiveTimer();
	DWORD wBindParam = 0;
	DWORD dwNextTick = STANDARD_TIME_ELAPSE;
	int nMinTick = STANDARD_TIME_ELAPSE;
	int i = 0;

	// 	_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("m_started %d,dwtimerID:%d"), m_started,dwTimerID);
	// 	m_LogFile.WriteErrorLogFile( m_szErrorInfo );

	//	LOGI("���ö�ʱ���¼�");

	//�ϸ�ļ�ʱ������Ч
	if (0 < dwTimerID && dwTimerID <= TIMER_COUNT)
	{
		wBindParam = m_arTimerManage[dwTimerID].wBindParam;
	}


	m_dwCurTick = GetTickCount();
	// 		1		//�����
	// 		2		//������켣
	// 		3		//�����켣
	// 		4		//�л�����
	// 		5		//���ɹ�����Ⱥ
	// 		6		//���������ļ�
	// 		7		//��齱��
	// 		8		//Ԥд�����
	try
	{
		//�¼�����
		// 						char lBuffer[512];
		// 				 		sprintf(lBuffer, "-------------------------------%s\n", lMessage.DebugString().c_str());
		// 				 		OutputDebugString(lBuffer);

		//  		char lBuffer[512];
		//  		sprintf(lBuffer, "------------�����¼����%d  tickcount %u---------\n", dwTimerID, GetTickCount());
		//  		OutputDebugString(lBuffer);

		//  		if (dwTimerID == 2 || dwTimerID ==5)     
		//  		{
		//  			dwTimerID = 1;
		//  		}

		for (int i = 0 ; i<PLAY_COUNT; i++)
		{
			// ��Ҹս���ʱ��Ϊ enPlayerEnter�� �Ϸ֣�ת��Ϊ��Ϸ�ڻ��֣����ʱ��Ϊ enPlayerBuy����������Ϸ״̬��
			// ������Ϸ״̬�£�һ��ʱ�䲻��ǹ��ǿ��������
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
		case TIME_ADD_MONSTER:			//������
			{
				CreateMonster();
				break;
			}
		case TIME_ADD_GROUP_MONSTER:	//������켣
			{
				//������켣
				CreateGroupMonsters();                 
				break;
			}
		case TIME_CLEAR_MONSTER:	//�����
			{
				OnTimerDestroyMonsters();
				break;
			}
		case TIME_CHANGE_SCENE:	//�л�����
			{
				//�����л�������Ϣ
				OnTimerChangeScene();
				break;
			}
		//case TIME_LOAD_FILE:	//���������ļ�  TODO: ��ʵ�ֲ��ָ��£������޸����Ĳ��ָ����Ĳ���
		//	{
		//		bool isModified = GameConfigMgr::getInstance().Current().ReloadConfig();
		//		if (isModified)
		//		{
		//			TRACEDEBUG("����[%d]�������ļ��޸��ˣ� ���¼��� WhacAMole.xml", m_nRoomID);

		//			GameConfigMgr::getInstance().Current().LoadConfig();

		//			StatisticMgr::getInstance().DumpConfig(m_config, m_nTableID);
		//		}	
		//		break;
		//	}
		case TIME_CHECK_POOL:	//5���齱��
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
		// �Ƿ���Ҫ�����ٴ�����ʱ����0xFeb.2011
		nMinTick = GetComingTimerTick(dwNextTick);	//����200ms
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
	//��ʱע�͹������Ŀ��	G_iRecPreDayTime  = StrToInt(sDay);
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
* ֹͣ���м�ʱ��	
*/
void CGameServer::KillAllTimer()
{
	KillGameTimer(TIME_ADD_MONSTER);													//�����
	KillGameTimer(TIME_ADD_GROUP_MONSTER);												//������켣
	KillGameTimer(TIME_CLEAR_MONSTER);											   //�����켣
	KillGameTimer(TIME_CHANGE_SCENE);                                        //�л�����
	KillGameTimer(TIME_CHECK_POOL);											//���������ļ�	
	//KillGameTimer(TIME_LOAD_FILE);											//��齱��                    

	m_pServer->StopTimer();
	return ;
}

/**
* ���ݳ�ʼ��
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

	m_nStatictisAvoidCounts = 0;		//�����жϴ���
	m_nStatictisAvoidValidCounts = 0;		//������Ч����   
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
		WriteInfoLog("CGameServer::CreateGroupMonsters ˢ��Ϲ֣�������ҿͻ��˲�δ׼���ã�");
		//return;
	}

    //LOGI("nRoomID=%d, nTableID=%d, nTableUserNum=%d", m_nRoomID, m_nTableID, m_nTableUserCount);

    /*
    CMonster* pMonster = CMonsterManager::Instance().CreateMonster(m_nTableID);
    if (pMonster == nullptr)
        return;

    //LOGI("CreateMonster : ID=%d, Type=%d, RoomID=%d, TableID=%d, nTableUserNum=%d", pMonster->id(), pMonster->monsterType(), m_nRoomID, m_nTableID, m_nTableUserCount);

    SY::MonsterTrace syTrace;
    //���ñ���

    syTrace.set_monsterid(pMonster->id());
    syTrace.set_typeid_(pMonster->monsterType());
    syTrace.set_curhp(pMonster->curHP());
    syTrace.set_maxhp(pMonster->totalHP());
    syTrace.set_speed(pMonster->moveSpeed());

    // ��������Ҫ���� RespawnIndex�� �ǵ�����Ҫ���� 3��Point��������ʱΪ��
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

    //���ñ���
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
		if(wild_skill_status)//�ѿ񱩹ּ��뵽map��ȥ
			wild_skill_monster_map[pMonster->id()] = 0;
		syTrace->set_speedrate((wild_skill_status)?get_wild_config_val(WILD_SKILL_SPEED):-1);

        // ��������Ҫ���� RespawnIndex�� �ǵ�����Ҫ���� 3��Point��������ʱΪ��
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
		WriteInfoLog("CGameServer::CreateGroupMonsters ˢ��Ϲ֣�������ҿͻ��˲�δ׼���ã�");
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
    
    //���ñ���
    for (auto it = lstMonster.begin(); it != lstMonster.end(); ++it)
    {
        CMonster* pMonster = *it;

        SY::MonsterTrace* syTrace = syGroupTrace.add_traces();
        syTrace->set_monsterid(pMonster->id());
        syTrace->set_typeid_(pMonster->monsterType());
        syTrace->set_curhp(pMonster->curHP());
        syTrace->set_maxhp(pMonster->totalHP());
        syTrace->set_speed(pMonster->moveSpeed());
		if(wild_skill_status)//�ѿ񱩹ּ��뵽map��ȥ
			wild_skill_monster_map[pMonster->id()] = 0;
		syTrace->set_speedrate((wild_skill_status)?get_wild_config_val(WILD_SKILL_SPEED):-1);
        // ��������Ҫ���� RespawnIndex�� �ǵ�����Ҫ���� 3��Point��������ʱΪ��
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
	WriteInfoLog(_T("��ʼ������Ч�ĺ���"));

	CMonsterManager::Instance().CheckMonsterToDestroy(m_nTableID);

	//for (int i = 0; i < MAX_FISH_TRACE_COUNT; i++)
	//{
	//    if (m_stFishTrace[i][0].bExist == false)
	//    {
	//        continue;
	//    }
	//    //��ȡ��ǰʱ��
	//    DWORD dwNowTime = GetTickCount();

	//    //��ǰʱ�� >= ������ɵ�ʱ��+�������ʱ��  ��Ҫ��ʱ���������
	//    if (dwNowTime >= (m_stFishTrace[i][0].nBuildTime + FISH_ALIVE_TIME))
	//    {
	//        for (int j = 0; j < 4; j++)
	//        {
	//            m_llLossPoolCount[j] += m_stFishTrace[i][0].nHitCountID[j];  //������һ����ӵ���������������
	//                                                                         // 								_sntprintf(m_szDatalogInfo , _countof(m_szDatalogInfo) ,
	//                                                                         // 									TEXT("1142 :%I64d"), 
	//                                                                         // 									m_llLossPoolCount[j]);
	//                                                                         // 								GLOGFI(1,m_szDatalogInfo);
	//            m_stFishTrace[i][0].nHitCountID[j] = 0;     //����ǲ�����һ�����
	//            m_stFishTrace[i][0].bCountedFull[j] = false;
	//            m_stFishTrace[i][0].bMustdieZero = false;
	//        }
	//        m_stFishTrace[i][0].bExist = false;
	//        //	OUTLOOK("����ʱ�䵽������Ϊ��Ч:%d",m_stFishTrace[i][0].nFishID);
	//    }
	//}
}

//�����л�������Ϣ	
void	CGameServer::OnTimerChangeScene()
{
	m_nSceneIndex++;	//����ID
	if(m_nSceneIndex>=3)
	{
		m_nSceneIndex=0;
	}
	//��������

	SY::ChangeScene syChangeScene;

	//���ñ���

	syChangeScene.set_sceneindex(m_nSceneIndex);


	//���ͳ����л���Ϣ
	char buf[200] = {0};
	if (syChangeScene.SerializeToArray(buf, sizeof(buf)))      //s11
	{
		sendGameCmdProtBufAll(GameMessages::ChangeScene, buf, syChangeScene.ByteSize());
	}

	//ɱ��������ʱ��(��ֹ���л������ͷ����µ���Ⱥ)
	//KillGameTimer(TIME_ADD_MONSTER);   //TODO: �л��������һ��ʱ���ٿ�ʼ������
	//KillGameTimer(TIME_ADD_GROUP_MONSTER);	

	//������Ⱥ
	//SetGameTimer(TIME_REGULAR_FISH,10*1000,INFINITE, 0L);

}


//2016.10.20 hl ��״̬��ʱ���ӿں�������ؽӿ� start

//��״̬����
void CGameServer::wild_skill_state_end()
{
	//֪ͨ�ͻ��˽�����  ������һ�ֿ񱩼�ʱ
	KillGameTimer(TIME_WILD_SKILL_END);
	make_tip_info(WILD_SKILL_STATUS_END,player_index);
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
		SetGameTimer(TIME_WILD_SKILL_DELEY,5*1000,INFINITE,0L);//��ʱ5��
//		SetGameTimer(TIME_WILD_SKILL_WAIT_STEP,DEFAULT_WILD_SKILL_ELSPSE,INFINITE,0L);
//		wild_skill_wait_step();
	}
	change_monster_time();
	wild_skill_blank = 0;
	wild_skill_status = false; //�񱩽��������ʻָ�����
	SetGameTimer(TIME_WILD_END_CREATE_MONSTER,1000,INFINITE,0L);//�����ڿ񱩺��û���ˣ���ʱ���ؿ�������û�֣�ǿ����һ��ˢ������
}
//��״̬��������
void CGameServer::wild_skill_will_end()
{
	//֪ͨ�ͻ��˻�ʣ�����3����
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
//��״̬����ʱ�׶�
void CGameServer::wild_skill_wait_step()
{
	if((bool)get_wild_config_val(WILD_SKILL_OPEN))
	{
//		static int wild_skill_blank = 0;
		if(0 == wild_skill_blank--)
		{
			wild_skill_blank = get_wild_config_val(WILD_SKILL_BLANK_TIME); //�������Ϊ30�� ����ʱ��Ϊ60*n
		}
//		if(!((wild_skill_blank)%2) && wild_skill_blank)
		if(2 == wild_skill_blank)
		{
			//�����������㱨ʱ ֪ͨ�ͻ���  2�������㱨��
			make_tip_info(WILD_SKILL_STATUS_BLANK,player_index,wild_skill_blank);
			return ;
		}
		else if(1 == wild_skill_blank && 1 == get_wild_config_val(WILD_SKILL_BLANK_TIME))
		{
			//�������ֻ��1���Ӿͽ���񱩣����ڴ�֪ͨ
			make_tip_info(WILD_SKILL_STATUS_BLANK,player_index,wild_skill_blank);
		}

		else if(0 == wild_skill_blank)
		{	//֪ͨ�ͻ��˽����״̬
			KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);
			SetGameTimer(TIME_WILD_SKILL_WILL_END,(get_wild_config_val(WILD_SKILL_CONTINUE_TIME)-DEFAULT_WILD_SKILL_PRETIP_TIME)*1000,INFINITE,0L);
			make_tip_info(WILD_SKILL_STATUS_START,player_index);
			wild_skill_monster_map.clear();//����ϴο񱩹�
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
//2016.10.20 hl ��״̬��ʱ���ӿں�������ؽӿ� end
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

	//����Ƿ��µ�һ��
	if (!IsSameDay(m_LastTime, time(0)))
	{
		buildGameNo();
		UpdateRecordLogPath();
		//��־��¼�ļ���ʼ��
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

	// ��ȡ����״̬������Ѿ������Ǿ�˵�������߼�����  ��ͻ���δ�жϻ���Ҵ������׿�����
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


	// ������ҵȼ��ж�����ǰ������򣩣����ǵ��ȼ��ļ���
	int curLevel = m_nUserLevel[ChairID];
	const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
	if (pSkillCostInfo == nullptr)
	{
		TRACEFALTAL("OnBuySkill, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d", m_nRoomID, skillID);
		return false;
	}

	bool isBuy = false; // ����������Ҫ����
	bool canActive = false;

	int nActiveLevel = pSkillCostInfo->ActiveLevel;
	int nBuyCost = pSkillCostInfo->BuyCost;
	if (curLevel < nActiveLevel)
	{
		// ����, ���ж�Ǯ�Ƿ��㹻
		if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
		{
			TRACEFALTAL("OnBuySkill, Money NOT ENOUGHT!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
			return false;
		}

		// �ȿ�Ǯ���ټ���
		m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
		if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

		m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;


		isBuy = true;
		canActive = true;

		TRACEDEBUG("BuySkill[%d] With Money, UserID=%d, BuyCost=%d, Level=%d", skillID, pPlayer->get_DBID(), nBuyCost, curLevel);
	}
	else
	{
		// ֱ�Ӽ���
		canActive = true;

		TRACEDEBUG("BuySkill[%d] For Free, UserID=%d, BuyCost=%d, Level=%d", skillID, pPlayer->get_DBID(), nBuyCost, curLevel);
	}

	if (canActive)
	{
		// �����ʱ�򣬾����ù���״̬��Ĭ�ϼ��1��
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
			TEXT("������: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_Buyed), activeStatus=0x%x(SY::Actived), star=0x%x(1)"),
			pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillBuyStatus, m_UserInteractData[ChairID].nSkillActiveStatus, m_UserInteractData[ChairID].nSkillLevel);
		TRACEDEBUG(m_szDatalogInfo);
	}

	//TODO: �ȼ������󣬻᷵�ش˽��
	SendAdditionalGameData(ChairID);

	// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID
	SY::BuySkillRsp syBuySkillRsp;
	syBuySkillRsp.set_chairid(ChairID);
	syBuySkillRsp.set_skillid(skillID);
	syBuySkillRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuySkillRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuySkillRsp, buf, syBuySkillRsp.ByteSize());
	}

	//���ͷ������ͻ���
	SendUserMoneyCountTo(ChairID, pPlayer, true);

	return true;
}
//�����Զ����ȼ�
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
	// Ĭ�ϼ��3��
	if (buyStatus == 0)
		buyStatus = auto_hammer_default_lv;

	if (nAutoHammerLv <= buyStatus)
	{
		TRACEFALTAL("OnBuyAutoAttack, Already BUYED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoHammerLv, buyStatus);
		return false;
	}

	// ����Խ������
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

	// ����, ���ж�Ǯ�Ƿ��㹻
	int nBuyCost = pAutoHammerCostInfo->active_cost;
	if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
	{
		TRACEFALTAL("OnBuyAutoAttack, Money NOT ENOUGHT!!!, UserID=%d, nAutoAttackLevel=%d", pPlayer->get_DBID(), nAutoHammerLv);
		return false;
	}

	// �ȿ�Ǯ���ټ���
	m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
	if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

	m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), TEXT("BuyAutoAttack[%d] With Money, UserID=%d, BuyCost=%d, buyStatus=%d"), nAutoAttackLevel, pPlayer->get_DBID(), nBuyCost, buyStatus);

	m_UserInteractData[ChairID].auto_hammer_lv = buyStatus + 1;
	m_UserPreInteractData[ChairID].auto_hammer_lv = m_UserInteractData[ChairID].auto_hammer_lv;

	//SetAdditionalGameData(ChairID, enADT_AutoAttackLevel, m_UserInteractData[ChairID].nAutoAttackLevel);

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("�����Զ��һ��ȼ�: UserID=%d, nAutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), (int)nAutoAttackLevel, (int)buyStatus));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID);

	// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID
	SY::BuyHammerLevelRsp syBuyAutoHammerRsp;
	syBuyAutoHammerRsp.set_chairid(ChairID);
	syBuyAutoHammerRsp.set_hammerlevel(nAutoHammerLv);
	syBuyAutoHammerRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuyAutoHammerRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuyHammerLevelRsp, buf, syBuyAutoHammerRsp.ByteSize());
	}

	//���ͷ������ͻ���
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
	// Ĭ�ϼ��3��
	if (buyStatus == 0)
		buyStatus = nAutoAttackDefaultLevel;

	if (nAutoAttackLevel <= buyStatus)
	{
		TRACEFALTAL("OnBuyAutoAttack, Already BUYED!!!, UserID=%d, AutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), nAutoAttackLevel, buyStatus);
		return false;
	}

	// ����Խ������
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

	// ����, ���ж�Ǯ�Ƿ��㹻
	int nBuyCost = pAutoAttackCostInfo->ActiveCost;
	if (m_UserGameScore[ChairID].llMoneyNum < nBuyCost)
	{
		TRACEFALTAL("OnBuyAutoAttack, Money NOT ENOUGHT!!!, UserID=%d, nAutoAttackLevel=%d", pPlayer->get_DBID(), nAutoAttackLevel);
		return false;
	}

	// �ȿ�Ǯ���ټ���
	m_UserGameScore[ChairID].llMoneyNum -= nBuyCost;
	if (m_UserGameScore[ChairID].llMoneyNum < 0)    m_UserGameScore[ChairID].llMoneyNum = 0;

	m_UserStatisticsData[ChairID].llBuySkillCost += nBuyCost;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), TEXT("BuyAutoAttack[%d] With Money, UserID=%d, BuyCost=%d, buyStatus=%d"), nAutoAttackLevel, pPlayer->get_DBID(), nBuyCost, buyStatus);

	m_UserInteractData[ChairID].nAutoAttackLevel = buyStatus + 1;
	m_UserPreInteractData[ChairID].nAutoAttackLevel = m_UserInteractData[ChairID].nAutoAttackLevel;

	//SetAdditionalGameData(ChairID, enADT_AutoAttackLevel, m_UserInteractData[ChairID].nAutoAttackLevel);

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("�����Զ��һ��ȼ�: UserID=%d, nAutoAttackLevel=%d, buyStatus=%d", pPlayer->get_DBID(), (int)nAutoAttackLevel, (int)buyStatus));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID);

	// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID
	SY::BuyAutoAttackRsp syBuyAutoAttackRsp;
	syBuyAutoAttackRsp.set_chairid(ChairID);
	syBuyAutoAttackRsp.set_autoattacklevel(nAutoAttackLevel);
	syBuyAutoAttackRsp.set_result(0);
	char buf[200] = { 0 };
	if (syBuyAutoAttackRsp.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(ChairID, GameMessages::BuyAutoAttackRsp, buf, syBuyAutoAttackRsp.ByteSize());
	}

	//���ͷ������ͻ���
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

	// ֻ������/����״̬ʱ�ŷ����ͣ��ʼ������ȱ���������״̬
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
			TEXT("�л���������״̬: UserID=%d, SkillID=%d, activeStatus=0x%x(%d)"),
			pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillActiveStatus, status);
		//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);
	}

	SendAdditionalGameData(ChairID, 112);

	// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID   �ͻ����ڽ��պ󣬻�ˢ�½���
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

	// ��������������ҵĵ�ǰ����
	int clientChairID = lMessage.chairid();
	int ChairID = pPlayer->get_chair();
	int WeaponID = lMessage.weaponid();

	TRACEDEBUG("OnChangeWeapon: ChairID=%d, SkillID=%d,  clientChairID=%d", ChairID, WeaponID, clientChairID);

	// ����Ϣ����Ҫ�����ͻ����������

	//
	//// ʧ�ܲŷ�����Ϣ
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

	// ��ȡ����״̬������Ѿ������Ǿ�˵�������߼�����  ��ͻ���δ�жϻ���Ҵ������׿�����
	SY::SkillStatus activeStatus = m_UserInteractData[ChairID].GetSkillActiveStatus(skillID);
	if (activeStatus == SY::InActive)
	{
		TRACEFALTAL("OnUpgradeSkill, Skill UNACTIVE!!!, UserID=%d, SkillID=%d", pPlayer->get_DBID(), skillID);
		return false;
	}


	// �����Ǽ�
	unsigned char SkillLevel = m_UserInteractData[ChairID].GetSkillLevel(skillID);
	// ����Ѿ��������Ͳ�����������
	if (SkillLevel >= 5)
	{
		TRACEWARNING("OnUpgradeSkill, SkillLevel[%d] is Invalid", SkillLevel);
		return false;
	}
	else if (SkillLevel == 0)
	{
		// ������˵�������ڼ���ʱ���Զ���Ϊ1�ǣ� ������������һ��
		TRACEWARNING("OnUpgradeSkill, SkillLevel[%d] is Invalid", SkillLevel);
		SkillLevel = 1;
	}


	// ������ҵȼ��ж�����ǰ������򣩣����ǵ��ȼ��ļ���
	int curLevel = m_nUserLevel[ChairID];
	const SkillCostInfo* pSkillCostInfo = GameConfigMgr::getInstance().Current().GetSkillCostInfo(m_nRoomID, skillID);
	if (pSkillCostInfo == nullptr)
	{
		TRACEFALTAL("OnUpgradeSkill, GetSkillCostInfo Failed, RoomID=%d, SkillID=%d", m_nRoomID, skillID);
		return false;
	}

	// �Ӽ���ȼ���ʼ��ÿ2������һ��
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
		TEXT("��������: UserID=%d, SkillID=%d, star=0x%x(%d)"),
		pPlayer->get_DBID(), skillID, m_UserInteractData[ChairID].nSkillLevel, SkillLevel);
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	SendAdditionalGameData(ChairID, 113);

	// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID
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
	// �ж��Ƿ񱩻�
	int	nMonsterCount = PlayerHit.monsteridlist_size();
	if (nMonsterCount <= 1)	// �����б�Ϊ�ջ���ֻ��һֻ��������Ƿ���������ʱ�򲻴�������
		return false;

	//�ж��ǲ��ǿ�������
	{
		const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, mtMoleKing);
		if (!pMonsterProperty)
		{
			return FALSE;
		}

		if (pMonsterProperty->RespawnProb <= 0)
		{
			TRACEFALTALFILE(_T("���������嵥"), _T("����ʹ����ң����Ӻ�: %d, �û�ID: %ld"), m_nTableID, pPlayer->get_DBID(NULL));

			m_pServer->ForceUserOffline(pPlayer->get_ID(NULL));

			return FALSE;
		}
	}

	//�жϵ�һֻ�ǲ�������
	{
		int nMonsterID = PlayerHit.monsteridlist(0);
		MonsterType monsterType = CMonsterManager::Instance().GetMonsterType(m_nTableID, nMonsterID);

		if (monsterType != mtMoleKing)
		{
			if (monsterType != mtNone && monsterType != MonsterTypeCount)
			{
				TRACEFALTALFILE(_T("���������嵥"), _T("����ʹ����ң����Ӻ�: %d, �û�ID: %ld"), m_nTableID, pPlayer->get_DBID(NULL));

				m_pServer->ForceUserOffline(pPlayer->get_ID(NULL));
			}

			return FALSE;
		}
	}

	// �ж������Ƿ�����
	const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)mtMoleKing);
	if (infMonsterProperty == nullptr)
		return false;

	// �������ˣ���������ˣ�ʵ����ϵͳ�����棩
	if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
	{
		//δ����������Ҫ�������ļ��㣬��������ֻ��һ�����ˣ���������Ҫ�ӣ���Ҫ�ӣ�
		StatisticMgr::getInstance().NormalHit(mtMoleKing, 0, 0, 0, 0, 1);

		return -1;
	}

	// ���涼���ˣ���ʼ���㱩����
	double dRate = -1;

#ifdef DEBUG
	const KingConfig* king_config = GameConfigMgr::getInstance().Current().GetKingConfig(m_nRoomID);
	if (nullptr != king_config)
		dRate = (double)king_config->king_critical_rate;

	if (dRate < 0)											// ������û�ж�ȡ�����͸�����Ļ�ϵĹ��������������
		dRate = 10000.0 / (nMonsterCount-1);
#else
	dRate = 10000.0 / (nMonsterCount-1);
#endif
	

	if ((genrand_real2() * 10000) >= dRate)
		return FALSE;

	return TRUE;
}

// ��һ�������
void CGameServer::_PlayerHitKing(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit)
{
	// �ж��Ƿ񱩻�
	Change_t& Change	= m_Change[pPlayer->get_chair(NULL)];
	int	nMonsterCount	= PlayerHit.monsteridlist_size();
	WeaponID eWeaponID	= (WeaponID)PlayerHit.weaponid();
	const WeaponCostInfo* infWeaponCost = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, eWeaponID);
	if (infWeaponCost == nullptr)
	{
		TRACEFALTAL("��Ҳ������� ����ID[%d]δ�ҵ���", eWeaponID);
		return;
	}
	
	// ���㱩���ܽ��
	int nChairID			= pPlayer->get_chair(NULL);
	int	nGainMoneyNum		= 0;						
	int nGainTickNum		= 0;
	int nWeaponKingCost		= infWeaponCost->Cost;	// ���λ�������
	int nWeaponCostTotal	= 0;
	bool is_wild_skill_monster = false;
	float wild_skill_pay_rate = 1.0;
	float wild_skill_get_rate = 1.0;
	stUserMoney UserIncoming;
	SY::PlayerHitIncoming syPlayerHitIncoming;		// ����ÿ�����������
	syPlayerHitIncoming.set_chairid(nChairID);		// ��λ��

#ifdef	_DEBUG
	long long GameScore = m_UserGameScore[nChairID].llMoneyNum;
#endif
#ifdef LOG_TRACE
	CString info("");
	info.Format("��������id ");
	WriteKillLog(info);
#endif

	for (int i = 0; i < nMonsterCount; ++i)
	{
		int nMonsterID = PlayerHit.monsteridlist(i);
		//�����Ƿ��ǿ񱩹�
		std::map<DWORD,int>::iterator itmap = wild_skill_monster_map.find(nMonsterID);
		if(itmap != wild_skill_monster_map.end())
		{//�ڴ˴������п񱩹ִ�map��ɾ����ֱ�����´ο񱩿�ʼǰ����ϴο񱩹� 2016.10.26 hl
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
			// ����������жϣ�����ֻ��һ��Ǯ��ֻҪ������һ�ο� �ͷ���
			if(m_UserGameScore[nChairID].llMoneyNum < nWeaponKingCost * wild_skill_pay_rate)
			{
				send_weapon_cost_ex(nWeaponKingCost,nChairID);
				_PlayerNotCriticalHit(pPlayer, PlayerHit);
				return;
			}

			// �ڴ˴����������� ֱ�ӵ����γ�һ�������¼
			if(0 != (int)(infWeaponCost->Cost * wild_skill_pay_rate - infWeaponCost->Cost))
			{
				SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
				pMonsterIncoming->set_monsterid(nMonsterID);
				pMonsterIncoming->set_killrate(-1);
				pMonsterIncoming->set_usermoneynum(0);
				pMonsterIncoming->set_userticketnum(0);
				pMonsterIncoming->set_weaponcostex((int)(infWeaponCost->Cost * wild_skill_pay_rate - infWeaponCost->Cost));
			}
			UserIncoming.llMoneyNum -= infWeaponCost->Cost * wild_skill_pay_rate;	// ȥ����������(�û����������ѣ��������ﱩ���Ͳ�������������)
			Change.KillCost[mtMoleKing] += infWeaponCost->Cost * wild_skill_pay_rate;
			nWeaponKingCost = infWeaponCost->Cost * wild_skill_pay_rate;			// ��״̬�»����������ۿ񱩵���������
			nWeaponCostTotal += nWeaponKingCost;

			StatisticMgr::getInstance().NormalHit(mtMoleKing, nWeaponKingCost, 0, 1, 0, 0);
		}

		MonsterType monsterType = CMonsterManager::Instance().GetMonsterType(m_nTableID, nMonsterID);
		if (monsterType == mtNone)
			continue;
		
		if (monsterType == mtFlyGoblin)		// ����С����ʱ����ͨ
			continue;

		if(monsterType == MonsterTypeCount)
			continue;
		const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
		if (infMonsterProperty == nullptr)
			continue;
		
		// �������ˣ���������ˣ�ʵ����ϵͳ�����棩
		Change.KillMonster[monsterType]++;
		Change.MissingCount++;
		if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
		{
			StatisticMgr::getInstance().KingHit(monsterType, 0, 1, 0, 1);

			Change.MissingValid++;
			continue;
		}
		
		// ��û�ɱ����
		int nKillRate = CMonsterManager::Random(infMonsterProperty->KillRateMin, infMonsterProperty->KillRateMax + 1);
		CMonster* monster = CMonsterManager::Instance().GetMonster(m_nTableID, nMonsterID);
		if (monster != nullptr)
			monster->Destroy();

		// �����Ƿ�������
		bool havIncoming = CMonsterManager::Prob(1, nKillRate);
		if (!havIncoming)	// ����������
		{
			StatisticMgr::getInstance().KingHit(monsterType, 0, 1, 1, 0);

			continue;
		}

		// ��ȡ��Ϸ��
		nGainMoneyNum = nKillRate * infWeaponCost->Cost * wild_skill_get_rate;
		StatisticMgr::getInstance().KingHit(monsterType, nGainMoneyNum, 1, 0, 0);
		UserIncoming.llMoneyNum += nGainMoneyNum;

		// ��¼ÿ����������棬֮�󷵻ظ��ͻ��ˣ����ű���Ҷ���
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
			TRACEWARNING("��GK���ܽ�%lld������ɱǰ�ܶ%lld������ɱ���%d�����������͡�%d������ɱ���ʡ�%d����������桾%d��",
				GameScore + nGainMoneyNum, GameScore, nMonsterID, monsterType, nKillRate, nGainMoneyNum);
			GameScore += nGainMoneyNum;
#endif
		}

		{	// ���²�����ͳ����Ϣ������Ϸ�߼��޹�
			Change.KillGain[monsterType] += nGainMoneyNum;
			Change.KillValid[monsterType]++;

			if ( nKillRate > Change.MonsterRate )
			{
				Change.MonsterRate = nKillRate;		// ��ɱ����
				Change.MosterID = monsterType;		// ����ʵĹ���id
			}

			if (nGainMoneyNum > Change.MonsterMostGain)
			{
				Change.MonsterMostGain = nGainMoneyNum;	// ��ֻ�����������
			}

			//////////////////////////////////////////////////////////////////////////
			// ͳ�����ݣ�
			m_UserStatisticsData[nChairID].llUserIncome.llMoneyNum += nGainMoneyNum;
			m_UserStatisticsData[nChairID].llIntervalExpense.llMoneyNum += nGainMoneyNum;
			m_nMonsterPlayerIncome[monsterType] += nGainMoneyNum;
			m_nStatictisMonsterIncomingCounts[monsterType]++;
			//////////////////////////////////////////////////////////////////////////
			m_nStatictisMonsterKillCounts[monsterType] += 1;
		}
	}
	// �����������ܿ�����������
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

	// �㲥֪ͨ����������Ϣ
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

	// ��ȡ��ҵȼ�
	GetPlayerExp(nChairID, pPlayer, &Change.Level);

	// ���;���ֵ
	SendGeneralLevel(nChairID, pPlayer);

	// ��Ϸ����ͬ�����������
	if (syPlayerHitIncoming.incominglist_size() > 0)
	{
		char buf[1024] = { 0 };
		if (syPlayerHitIncoming.SerializeToArray(buf, sizeof(buf)))
		{
			sendGameCmdProtBufAll(GameMessages::PlayerHitIncomingSync, buf, syPlayerHitIncoming.ByteSize());
		}
	}

	// �������¼������ͻ���
	SendCriticalStrikeTo(pPlayer, UserIncoming, PlayerHit, nWeaponKingCost);
}

void CGameServer::_OnPlayerHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, BOOL isCriticalHit)
{
	int				ChairID = pPlayer->get_chair();  
	Change_t&		Change = m_Change[pPlayer->get_chair(NULL)];
    vector<stDeadMonster>		vecAllDeadMonster[MonsterTypeCount];				// ͳ�����д����Ĺ���MonsterID����������������TypeΪ0�����򱣴�

	stPlayerHit playerHit;     
	playerHit.nWeaponID		= PlayerHit.weaponid();
	playerHit.nMonsterID	= PlayerHit.monsterid();
	playerHit.isRobot		= PlayerHit.isrobot();
	int nKingID				= -1;

	SY::PlayerHitIncoming syPlayerHitIncoming;
	syPlayerHitIncoming.set_chairid(ChairID);     //��λ��

	if (PlayerHit.monsteridlist_size() > 0)
		nKingID = PlayerHit.monsteridlist(0);

    // ��Ҫ��¼��ÿֻ�ָ��Ի�õ�Ǯ�����ڿͻ�����ʾ
    vector<stMonsterIncoming> vecMonsterIncoming[MonsterTypeCount];		// ͳ����ʵ�ʲ�������ļ��㣬��������������TypeΪ0�����򱣴�
    vector<stMonsterIncoming> vecMonsterMissing[MonsterTypeCount];      // ͳ�Ʊ����˵Ĺ����������������TypeΪ0�����򱣴�

	int temp_weapon_cost = 0;
	bool is_wild_monster = false;

    // ����������ף��ж�����Ƿ���ʹ�ô������� ��Ǯ�Ƿ��㹻ʹ��
    const WeaponCostInfo* pWeaponCostInfo = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, (WeaponID)playerHit.nWeaponID);
    if (!pWeaponCostInfo)
    {
        TRACEFALTAL("�������ݴ���, UserID=%d, WeaponID=%d", pPlayer->get_ID(), playerHit.nWeaponID);
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
			if (isCriticalHit > 0 && nKingID == nMonsterID)	// �����ܻ����������Ͳ�ͳ����
			{
#ifdef LOG_TRACE
				CString info("");
				info.Format("����������ɱid{%d}",nMonsterID);
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
				info.Format("���ܻ�ɱid{%d}",nMonsterID);
					WriteKillLog(info);
#endif
            }
			else /*if(monsterType == MonsterTypeCount)*///���˸���Ч��������Ϊ������Ҳ����ֵķ���ֵ  ����������  ����һ���Ķ�̫�� 2016.11.9 hl
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
					info.Format("���ܷ����δ�ҵ��ֲ��Ҽ��ܻ�ɱ�б���δ����,��ɱid{%d}",nMonsterID);
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
				info.Format("�����δ�ҵ���(����),��ɱid{%d}",nMonsterID);
				WriteKillLog(info);
#endif
			}
        }

        playerHit.lstSkilTrigger.push_back(skillHit);


        if (0 < skillHit.nSkillID && skillHit.nSkillID <= SKILL_COUNT)
        {
            m_UserStatisticsData[ChairID].m_nStatictisSkillTriggerCounts[skillHit.nSkillID]++;
			Change.SkillCount[skillHit.nSkillID]++;		// ͳ�Ƽ���ʩ�Ŵ���
        }
    }

    {
       	if (!(nKingID == playerHit.nMonsterID && isCriticalHit > 0))
		{
			MonsterType monsterType = CMonsterManager::Instance().PlayerHit(m_nTableID, playerHit.nMonsterID,wild_skill_status);	// ���������һ�ι���
			if(monsterType != mtNone && monsterType != MonsterTypeCount && PlayerHit.haspayweaponcost())
			{

					is_wild_monster = wild_monster(playerHit.nMonsterID);
					vecAllDeadMonster[monsterType].push_back(stDeadMonster(playerHit.nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && playerHit.nMonsterID == nKingID));
					vecAllDeadMonster[0].push_back(stDeadMonster(playerHit.nMonsterID, monsterType,is_wild_monster,0, isCriticalHit < 0 && monsterType == mtMoleKing && playerHit.nMonsterID == nKingID));
					temp_weapon_cost += pWeaponCostInfo->Cost * (is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1);
#ifdef LOG_TRACE
					CString info("");
					info.Format("��ɱid{%d}",playerHit.nMonsterID/*,m_UserGameScore[ChairID].llMoneyNum*/);
					WriteKillLog(info);
#endif
					
			}
			else if((monsterType == MonsterTypeCount || monsterType == mtNone )&& PlayerHit.haspayweaponcost())//���˸���Ч��������Ϊ������Ҳ����ֵķ���ֵ  ����������  ����һ���Ķ�̫�� 2016.11.9 hl
			{
				SY::MonsterIncoming* pMonsterIncomingMiss = syPlayerHitIncoming.add_incominglist();
				pMonsterIncomingMiss->set_monsterid(playerHit.nMonsterID);
				pMonsterIncomingMiss->set_killrate(0);
				pMonsterIncomingMiss->set_usermoneynum(0);
				pMonsterIncomingMiss->set_userticketnum(0);
				pMonsterIncomingMiss->set_weaponcostex(-pWeaponCostInfo->Cost);
#ifdef LOG_TRACE
				CString info("");
				info.Format("�����δ�ҵ���,��ɱid{%d}",playerHit.nMonsterID);
				WriteKillLog(info);
#endif
			}
		}
    }
  //�ͻ��������˿�״̬��һ�£����¿ͻ���Ǯ���ۣ�������˲����ۣ�������²�������򣬷����ͻ����ѿ���������2016.11.1 hl
    if (m_UserGameScore[ChairID].llMoneyNum < temp_weapon_cost && vecAllDeadMonster[0].size())
    {
        //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "�����˼��ܣ���ҷ�������֧���˴�������ģ�UserID=%d, UserMoney=%d, WeaponID=%d, TargetNum=%d", pPlayer->get_ID(), m_UserGameScore[ChairID].llMoneyNum, playerHit.nWeaponID, vecAllDeadMonster[0].size());
        //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "=====KillOffTest : OnPlayerHit, Player=%d, curMoney=%d, preMoney=%d", pPlayer->get_DBID(), m_UserGameScore[ChairID].llMoneyNum, m_UserPreGameScore[ChairID].llMoneyNum);
        //m_pSite->GetOff(pPlayer->get_ID());
		send_weapon_cost_ex(pWeaponCostInfo->Cost * vecAllDeadMonster->size(),ChairID);
		TRACEDEBUG("�����۷Ѳ������ͻ������Ľ�һ���! UserID=%d", pPlayer->get_ID());
        return;
    }
	m_UserStatisticsData[ChairID].m_nStatictisHitCounts++;
    m_UserStatisticsData[ChairID].dwLastHitTime = ::GetTickCount();

    //GLOGFI(LOGID_Trace(m_nRoomID), "��ҡ�%d����������%d�������˹��%d�������ι���ɱ��%d��ֻ", pPlayer->get_DBID(), playerHit.nWeaponID, playerHit.nMonsterID, vecAllDeadMonster[0].size());

    // Ϊ0����û��������������������������˵��ǰ�ߣ�
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

	//��ҷ�������
	if (m_UserGameScore[ChairID].llMoneyNum <= 0 )
	{	
        TRACEDEBUG("��ҷ���������������, UserID=%d", pPlayer->get_ID());
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

        // ͳ�Ƽ�ɱ��¼
        int nKillNum = vecAllDeadMonster[monsterType].size();
        if (nKillNum > 0)
        {
			Change.KillMonster[monsterType] += nKillNum;			// ͳ�ƻ�ɱ��������
            if (CMonsterManager::IsMole((MonsterType)monsterType))
            {
                if (monsterType == mtMoleKing)
                    m_UserInteractData[ChairID].llMoleKingKillNum += nKillNum;
                //�󹤵������ݺϲ�����ͨ������
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
            // �������ˣ���������ˣ�ʵ����ϵͳ�����棩
            if (CMonsterManager::Prob(infMonsterProperty->MissRate, PrecisionNumber))
            {
                // ���ˣ���Ҳ��������棬����۵�������Ҫ������ĵĽ��
                vecMonsterMissing[monsterType].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
                vecMonsterMissing[0].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
            }
            else
            {
                // δ������
                vecMonsterIncoming[monsterType].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
                vecMonsterIncoming[0].push_back(stMonsterIncoming(it->monster_id, it->monster_type,it->wild_king_client_not_die,it->be_wild_monster, it->king_prob));
            }
        }
    }

	Change.MissingCount += vecMonsterMissing[0].size() + vecMonsterIncoming[0].size();		// �����жϴ���
	Change.MissingValid += vecMonsterMissing[0].size();										// ������Ч����

	vector<SY::MonsterIncoming> lstReback;

    // ����������棨Ҳ���ܻ��ǮӴ��
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

    // ���ڱ����˵Ĺ֣��۵�������Ҫ������ĵĽ�� 
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

		//������Ҫ��������ǲ��Ǳ������ˣ�����ǵĻ�����Ҫ����
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

	//����
	for (vector<SY::MonsterIncoming>::iterator itFind = lstReback.begin(); itFind != lstReback.end(); itFind++)
	{
		SY::MonsterIncoming* pMonsterIncoming = syPlayerHitIncoming.add_incominglist();
		*pMonsterIncoming = *itFind;
	}

    // ���ͱ��η����仯���ͻ��� ���� ͬ����ǰ�����أ�  Ŀǰ���õ��Ǻ��� 
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

	//���;���ֵ
	SendGeneralLevel(ChairID, pPlayer);
    // ���÷ַ����Լ�
    SendUserMoneyCountTo(ChairID, pPlayer, true);

    // ��Ϸ����ͬ�����������
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

// ֪ͨ�ͻ�������δ���������Լ���ˢ��
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

// ֪ͨ�ͻ�������������
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
	syKingCriticalHit.set_usermoneygain(UserMoney.llMoneyNum + nWeaponKingCost); // Ӧ�üӻ��������ѣ��������Ǵ���������
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

//��ҷ����ӵ�	
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
	// �ж���������
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
	case emKingCriticalHit:			// ��������
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		_PlayerHitKing(pPlayer, lMessage);
		break;
	case emNoCriticalHit:			// ����δ����
		_PlayerNotCriticalHit(pPlayer, lMessage);
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		break;
	case emNormal:					// һ����
		_OnPlayerHit(pPlayer, lMessage, isCriticalHit);
		break;
	}

	if(m_UserGameScore[nChairID].llMoneyNum < GameConfigMgr::getInstance().Current().GetEnterLimitMin(m_nRoomID) ||
		m_UserGameScore[nChairID].llMoneyNum >= GameConfigMgr::getInstance().Current().GetEnterLimitMax(m_nRoomID))
	{
		m_pSite->GetOff(pPlayer->get_ID());	// ��ҳ������޻����ޣ�����
	}

	// add end
}

void CGameServer::AddUserExp(IGamePlayer* pPlayer, int nExpAdd)
{
	int nChairID = pPlayer->get_chair();

	bool isLevelUp = GameConfigMgr::getInstance().Current().AddExp(m_nUserLevel[nChairID], m_llGradeScore[nChairID], nExpAdd);
	//���µ����
	pPlayer->put_Exp(m_llGradeScore[nChairID]);

	if (isLevelUp)
	{
		//�ж��Ƿ���Ҫ����
		CheckCanReturnUserCoin(pPlayer);
		//TODO: �����Ϸ�����������ж��Ƿ���Ҫ�Զ�����ܣ�
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
			//ֱ��д����
			m_UserGameScore[nChairID].llMoneyNum += pSkillCostInfo->BuyCost;

			// �޸Ľ�������
			m_UserInteractData[nChairID].SetSkillBuyStatus(skillID, enSBS_Payed);    
			m_UserPreInteractData[nChairID].nSkillBuyStatus = m_UserInteractData[nChairID].nSkillBuyStatus;  

			SendAdditionalGameData(nChairID, enADT_SkillBuyStatus);

			m_UserStatisticsData[nChairID].llBuySkillReturn += pSkillCostInfo->BuyCost;

			_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
				TEXT("�ʼ����������ܽ��: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_Payed), returnMoney=%d, activeLevel=%d, UserLevel=%d"),
				pPlayer->get_DBID(), skillID, m_UserInteractData[nChairID].nSkillBuyStatus, pSkillCostInfo->BuyCost, pSkillCostInfo->ActiveLevel, m_nUserLevel[nChairID]);
			m_LogFile.WriteDataLogFile(m_szDatalogInfo);

			SendBasicInfo(nChairID, pPlayer);

			// ֪ͨ�ͻ���
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
				TEXT("�Զ������: UserID=%d, SkillID=%d, buyStatus=0x%x(enSBS_None), activeStatus=0x%x(SY::Actived), star=0x%x(1)"),
				pPlayer->get_DBID(), skillID, m_UserInteractData[nChairID].nSkillBuyStatus, m_UserInteractData[nChairID].nSkillActiveStatus, m_UserInteractData[nChairID].nSkillLevel);
			//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);     

			SendAdditionalGameData(nChairID);

			if (!bFromeInit)
			{
				// ����ID: 0��ʾ�ɹ��� >0��ʾ����ID
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
	// ��֤�ͻ��˴������Ĳ���
	const WeaponCostInfo* infWeaponCost = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, weaponID);
	if (infWeaponCost == nullptr)
	{
		TRACEFALTAL("��Ҳ������� ����ID[%d]δ�ҵ���", weaponID);
		return;
	}

	int ChairID = pPlayer->get_chair();
	int iKingList = PlayerHit.monsteridlist_size();

	int nCount = lstMonsterIncoming.size();
	int weaponCost = infWeaponCost->Cost; // ���λ�������  
	float wild_skill_pay_rate = 1.0;
	float wild_skill_get_rate = 1.0;
	for(auto &it = lstMonsterIncoming.begin(); it != lstMonsterIncoming.end(); ++it)
	{
		if (isIncoming && it->nMonsterType == mtMoleKing && iKingList <= 1)
		{
			it->weapon_cost_ex = 0;
			WriteInfoLog(_T("�����䵥"));
		}
		else
		{
			wild_skill_pay_rate = (it->is_wild_monster?get_wild_config_val(WILD_SKILL_PAY_RETE):1);

			Change.KillCost[monsterType] += weaponCost * wild_skill_pay_rate;		// ͳ�ƴ����͹����ɱ֧��

			Change.MoneyCost += weaponCost * wild_skill_pay_rate;	               // ͳ�ƹ����ɱ��֧��
			it->weapon_cost_ex =weaponCost*(wild_skill_pay_rate - 1.0);
		}
	}
// 	for(auto &it =lstMonsterIncoming.begin(); it != lstMonsterIncoming.end();++it)  //ÿ����֧�б�����Ƿ���Ҫ����֧��������� ������û���� ��Ҫ���� 2016.10.28 hl
// 	{
// 		it->weapon_cost_ex =weaponCost*(wild_skill_pay_rate - 1.0);
// 	}
//	lstMonsterIncoming[0].weapon_cost_ex += (Change.KillCost[monsterType] - weaponCost * nCount); //ͳ�ƿͻ�����Ҫ����֧���������� 2016.10.28 hl
	if (isIncoming)
	{
		// �Ƿ����ˣ�   δ�����˵Ĺ�
		const MonsterPropertyInfo* infMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
		if (infMonsterProperty == nullptr)
			return;

		// �����ɱ�ʣ�ʵ�ʾ����Ƿ��ܻ�ô˴ν���    ��������nCount��
		for (int i = 0; i < nCount; ++i)
		{
			int nKillRate = CMonsterManager::Random(infMonsterProperty->KillRateMin, infMonsterProperty->KillRateMax + 1);
			if (nKillRate == 0) continue;

			bool havIncoming = false;   // �Ƿ��ܻ������
			LONGLONG GainTicketNum = 0;
			LONGLONG GainMoneyNum = 0;
			wild_skill_get_rate = lstMonsterIncoming[i].is_wild_monster?get_wild_config_val(WILD_SKILL_GET_RATE):1;
			if (monsterType == mtFlyGoblin)
			{
				havIncoming = CMonsterManager::Prob(nKillRate, 100);

				UserIncoming.lMiss += havIncoming ? 0 : 1;

				if (havIncoming)
				{
					// ����С�� ����ʹ������ �����ȡ��Ʊ
					const WeaponTicketIncoming* pWeaponTicketIncomingInfo = GameConfigMgr::getInstance().Current().GetWeaponTicketIncoming(m_nRoomID, weaponID);
					if (pWeaponTicketIncomingInfo)
					{
						GainTicketNum = CMonsterManager::Random(pWeaponTicketIncomingInfo->TicketNumMin, pWeaponTicketIncomingInfo->TicketNumMax + 1);
						GainTicketNum += infMonsterProperty->TicketNum * wild_skill_get_rate;//��Ʊ����

						UserIncoming.llTicket += GainTicketNum;
						lstMonsterIncoming[i].incoming.llTicketNum = GainTicketNum;

						//////////////////////////////////////////////////////////////////////////
						// ͳ�����ݣ�
						m_UserStatisticsData[ChairID].llUserIncome.llTicketNum += GainTicketNum;
						m_UserStatisticsData[ChairID].llIntervalExpense.llTicketNum += GainTicketNum;
						m_nMonsterPlayerIncome[monsterType] += GainTicketNum;
						m_nStatictisMonsterIncomingCounts[monsterType]++;

						//////////////////////////////////////////////////////////////////////////
						Change.KillValid[monsterType]++;		// ������Ч��ɱ
						Change.KillGain[monsterType] += GainTicketNum;
					}
					else
					{
						TRACEFALTAL("����[%d] WeaponTicketIncoming���ô���δ�ҵ���Ӧ����, weaponID=%d", m_nRoomID, weaponID);
					}
				}
			}
			else
			{
				// �Ƿ��ɱ�ɹ����ܷ��ȡ������
				havIncoming = CMonsterManager::Prob(1, nKillRate);

				UserIncoming.lMiss += havIncoming ? 0 : 1;

				if (havIncoming)
				{
					// ��ȡ��Ϸ��  
					GainMoneyNum = nKillRate * weaponCost * wild_skill_get_rate; //���ڼ��ٴ���������
					UserIncoming.llCoin += GainMoneyNum;

					lstMonsterIncoming[i].nKillRate = nKillRate;
					lstMonsterIncoming[i].incoming.llMoneyNum = GainMoneyNum;

					//////////////////////////////////////////////////////////////////////////
					// ͳ�����ݣ�
					m_UserStatisticsData[ChairID].llUserIncome.llMoneyNum += GainMoneyNum;
					m_UserStatisticsData[ChairID].llIntervalExpense.llMoneyNum += GainMoneyNum;
					m_nMonsterPlayerIncome[monsterType] += GainMoneyNum;
					m_nStatictisMonsterIncomingCounts[monsterType]++;
					//////////////////////////////////////////////////////////////////////////

					//////////////////////////////////////////////////////////////////////////
					// ͳ����־ add by Ԭ���� 2016-10-13
					Change.KillValid[monsterType]++;		// ������Ч��ɱ
					if ( nKillRate > Change.MonsterRate )
					{
						Change.MonsterRate = nKillRate;		// ��ɱ����
						Change.MosterID = monsterType;		// ����ʵĹ���id
					}

					if (GainMoneyNum > Change.MonsterMostGain)
					{
						Change.MonsterMostGain = GainMoneyNum;	// ��ֻ�����������
					}

					Change.KillGain[monsterType] += GainMoneyNum;
					Change.MoneyGain += GainMoneyNum;			// �ۼӵ�������
					//////////////////////////////////////////////////////////////////////////

					// �㲥ͨ��
					const BroadcastInfo* infBroadcast = GameConfigMgr::getInstance().Current().GetBroadcastInfo(m_nRoomID);
					if (infBroadcast == nullptr)
					{
						TRACEFALTAL("����[%d]δ���ö�Ӧ�㲥��Ϣ", m_nRoomID);
						continue;
					}

					// �ж��Ƿ���Ҫ����
					if (nKillRate >= infBroadcast->ServerLimit)
					{
						for (auto it = infBroadcast->lstBroadLevelInfo.rbegin(); it != infBroadcast->lstBroadLevelInfo.rend(); ++it)
						{
							if (nKillRate < it->Limit)
								continue;

							// �������ݣ� ��ʽ��ΪANSI�� ������ҪתΪUTF8���  �� ����ֱ�Ӹ�ʽ��ΪUTF8
							// [%s]һ������������%d����%s�����%d���       ===>      [����ǳ�]һ������������xx���������������ơ����xxxxxxx���

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

			//GLOGFI(LOGID_Statictis(m_nRoomID), "UserID=%d, �Ƿ�����=��, �Ƿ�������=%d, ��������ID=%d, ����ID=%d, ��ɱ����=%d, ��֧=+%I64d-%d,+%I64d, ��ǰ���=%I64d,%I64d",
			//    pPlayer->get_DBID(), havIncoming, monsterType, weaponID, nKillRate, GainMoneyNum, weaponCost, GainTicketNum, m_UserGameScore[ChairID].llMoneyNum, m_UserGameScore[ChairID].llTicketNum);

			////TODO: ��������
			//UserIncoming.llTicketNum++;
			//lstMonsterIncoming[i].incoming.llTicketNum += 1;
		}

		m_nStatictisMonsterKillCounts[monsterType] += nCount;
	}
	else
	{
		//GLOGFI(LOGID_Statictis(m_nRoomID), "UserID=%d, �Ƿ�����=��, ��������ID=%d, ����ID=%d, ��������=%d, ��֧=-%d,+0, ��ǰ���=%I64d,%I64d",
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

	// �۵�������
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
	// ͳ�����ݣ�
	m_nStatictisAvoidCounts += nCount;

	m_UserStatisticsData[ChairID].llUserCost.llMoneyNum += nWeaponCostTotal;



	m_UserStatisticsData[ChairID].llIntervalIncome.llMoneyNum += nWeaponCostTotal;
	m_nMonsterPlayerCost[monsterType] += nWeaponCostTotal;
	//////////////////////////////////////////////////////////////////////////

	// ���ĵĽ��ת��Ϊ��������
	const GameIncomingInfo* pGameInComingInfo = GameConfigMgr::getInstance().Current().GetGameIncomingInfo(m_nRoomID);
	if (pGameInComingInfo)
	{
		m_dbUserWelFarePoint[ChairID] += nWeaponCostTotal * 0.01 * pGameInComingInfo->WelfarePointFactor * 0.01;
	}
	else
	{
		TRACEFALTAL("����[%d]����������������δ�ҵ���Ӧ����", m_nRoomID);
	}

	AddUserExp(pPlayer, (int)(nWeaponCostTotal * pGameInComingInfo->ExpFactor * 0.01));
}

void CGameServer::BroadcastMessage(IGamePlayer* pPlayer, const char* txt, const char* userName, WeaponID weaponID, int nKillRate, const char* monsterName, int rewardMoney, int rewardTicket)
{
	char cBrBuf[200] = { 0 };

	int ChairID = pPlayer->get_chair();

	if (GetTickCount() - m_nBroadcastTime[ChairID] <= Max_Broadcast_TIME)   //���β������ʱ�䲻��С��Max_Broadcast_TIME��
	{
		return;
	}

	char lBuffer[MAX_MSG_LEN] = { 0 };

	//���Ͷ���ҵ�������
	{
		char Utf8Name[128] = { 0 };
		CharSetHelper::ANSIToUTF8("��", Utf8Name, sizeof(Utf8Name) - 1);

		wsprintf(lBuffer, txt, Utf8Name, nKillRate, monsterName, rewardMoney);

		SY::BroadcastS2C syBroadcastS2C;
		syBroadcastS2C.set_text(lBuffer);
		syBroadcastS2C.set_username(userName);
		syBroadcastS2C.set_weaponid(weaponID);
		syBroadcastS2C.set_monstername(monsterName);
		syBroadcastS2C.set_rewardmoneynum(rewardMoney);
		syBroadcastS2C.set_rewardticketnum(rewardTicket);

		if (GetTickCount() - m_nBroadcastTime[ChairID] > Max_Broadcast_TIME)   //���β������ʱ�䲻��С��Max_Broadcast_TIME��
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
			_sntprintf(m_szErrorInfo, CountArray(m_szErrorInfo), TEXT("Catch Error: Table=%d, �㲥�ӿ�����Ϊ��."), m_nTableID);
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
	if (m_nGameIndx>2500) //ֻ��Ϊ������Ψһ�ӵ�һ����ʶ
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

// * / ������Ϣ���緿���������Ӻŵȸ�ֵ��26Apr.2011
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
			_sntprintf(m_zsServerName.szServerName, _countof(m_zsServerName.szServerName), TEXT("���㷿����"));
			nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_Get_Room_Name, &m_zsServerName, sizeof(m_zsServerName));    //TODO: ������ʵ��δ����
			m_pTableChannel->TableDispatch(ITableChannel::IID_Get_TableID, (void*)&m_nTableID, 0);
		}
		else
		{
			TRACEDEBUG("������Ϣ�ӿ�����Ϊ��");
		}
	}
	catch(...)
	{
		TRACEDEBUG("Unknown Error as Broadcast bonus information.");
	}

}

//������Ϸ״̬
enGamePhase	CGameServer::SetGameStatus(enGamePhase emGamePase)
{
	return m_enGamePhase = emGamePase;
}

//��Ч�����Ӻ�
bool CGameServer::CheckValidChairID(int nChairID)
{
	if (0 <= nChairID && nChairID < PLAY_COUNT)
	{
		return true;
	}

	return false;
}

//�����ҵķ���
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

//�������
bool CGameServer::CalcScore(IGamePlayer *pPlayer, int nChairID)
{
	//��Ϊ�յ��������Ч���
	if ( enPlayerNull == m_enUserState[ nChairID ] || !m_UserStatisticsData[nChairID].bClientReady )
	{
		return false;
	}

	//д����ֵ
	stUserMoney llWriteScore = m_UserGameScore[nChairID] - m_UserStatisticsData[nChairID].llUserCarryScore;

	//˰��
	LONGLONG llTax = llWriteScore.llMoneyNum;//llWriteScore.llMoneyNum;
	//Ϊ�����ͱ�Ϊ����
	/*if ( llTax < 0)
	llTax =  -llTax;*/


	// ���ᱻִ�е�������

	//д��ʧ�ܷ���
	if (false == WriteExitGameScore(pPlayer, nChairID, m_lDrawnCount[nChairID], llWriteScore, -llTax, m_UserStatisticsData[nChairID].llUserCost.llMoneyNum))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s WriteExitGameScore  WriteScore = %I64d,%I64d    TotalScore =  %I64d,%I64d"), 
			m_userName[nChairID], llWriteScore.llMoneyNum, llWriteScore.llTicketNum, m_UserGameScore[nChairID].llMoneyNum, m_UserGameScore[nChairID].llTicketNum);
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		return false;
	}

	//д�˷������ͬ��
	m_UserPreGameScore[nChairID] = m_UserGameScore[nChairID];
	return true;
}

//ʹ����չд�ֽӿڣ���¼˰�ʹ��׺ͳ��ͷ֣�10Nov.2011
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
		sExtGameScore.stExtScore.lDrawn = lDrawn;			// * /����Ϊƽ���ۼ�
		sExtGameScore.stExtScore.llTax = llTax;
		sExtGameScore.stExtScore.llFreqPile = llFreqScore;
		nResult = m_pTableChannel->TableDispatch(ITableChannel::IID_EXTEND_GAME_SCORE, &sExtGameScore, sizeof(sExtGameScore));

		TRACEDEBUG("=====WriteExitGameScoreTest : Player=%d, curMoney=%d, diffMoney=%d", pPlayer->get_DBID(), m_UserGameScore[nChairID], llWinScore.llMoneyNum);
	}

	TRACEDEBUG("writeExtGameScore() ITableChannel::IID_EXTEND_GAME_SCORE Result = %d.", nResult);

	if (nResult == S_OK)
	{
		//д�벶��ȼ�
		m_FishingData[nChairID].llGradeScore = pPlayer->get_Exp();

		SaveFishingCommonData(nChairID, m_dwUserID[nChairID], &m_FishingData[nChairID]);
		SaveCustomData(nChairID, m_dwUserID[nChairID], &m_UserInteractData[nChairID]);
	}


	//��չд�ֳɹ�����
	_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s д�ֳɹ�  ����Я�� %I64d,%I64d ���  ��Ӯ��� = %I64d,%I64d  ������� %I64d,%I64d"),
		m_userName[nChairID],										            //�ǳ�
		m_UserStatisticsData[nChairID].llUserCarryScore.llMoneyNum,
		m_UserStatisticsData[nChairID].llUserCarryScore.llTicketNum,			//����Я��
		llWinScore.llMoneyNum, llWinScore.llTicketNum,													//��Ӯ���
		m_UserGameScore[nChairID].llMoneyNum, m_UserGameScore[nChairID].llTicketNum			        //�������
		);
	//д������־
	m_LogFile.WriteDataLogFile(m_szErrorInfo);


	// д�������ݵ���־
	m_llSumIncome += m_UserStatisticsData[nChairID].llIntervalIncome.llMoneyNum;						//ϵͳ����
	m_llSumExpense += m_UserStatisticsData[nChairID].llIntervalExpense;					            //ϵͳ֧��
	m_llSumIntervalIncome += m_UserStatisticsData[nChairID].llIntervalIncome.llMoneyNum;
	m_llSumIntervalExpense += m_UserStatisticsData[nChairID].llIntervalExpense;

	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: ���Ӻ�=%d, UserDBID=%d, ϵͳʱ������=%I64d, ϵͳʱ��֧��=%I64d,%I64d, ϵͳ����=%I64d, ϵͳ֧��=%I64d,%I64d, �����ʹ���ϵͳ��ȡ�����=%I64d"),
	//    m_nTableID, pPlayer->get_DBID(), m_llSumIntervalIncome, m_llSumIntervalExpense.llMoneyNum, m_llSumIntervalExpense.llTicketNum,
	//    m_llSumIncome, m_llSumExpense.llMoneyNum, m_llSumExpense.llTicketNum, m_llSumAvoidHitSystemGetScore);
	//GLOGFI(LOGID_Losspool(m_nRoomID), m_szDatalogInfo);


	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: ���Ӻ�=%d, UserDBID=%d, ��ұ�������=%I64d, ��ұ�������=%I64d,%I64d, ��ҹ�������=%I64d, ϵͳ�������=%I64d, ��ҽ���ʱ���=%I64d, ��ҵ�ǰ���=%I64d"),
	//    m_nTableID, pPlayer->get_DBID(), m_UserStatisticsData[nChairID].llUserCost, m_UserStatisticsData[nChairID].llUserIncome.llMoneyNum, m_UserStatisticsData[nChairID].llUserIncome.llTicketNum,
	//    m_UserStatisticsData[nChairID].llBuySkillCost, m_UserStatisticsData[nChairID].llBuySkillReturn,
	//    m_UserStatisticsData[nChairID].llUserCarryScore, m_UserGameScore[nChairID].llMoneyNum);
	//GLOGFI(LOGID_Losspool(m_nRoomID), m_szDatalogInfo);


	//_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	//    TEXT("ExitScore: ���ܴ���, UserDBID=%d, ��ҹ�������=%d, ���ܴ�������=%d,%d,%d,%d"),
	//    pPlayer->get_DBID(), m_UserStatisticsData[nChairID].m_nStatictisHitCounts,
	//    m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_LianSuoShanDian], m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_HengSaoQianJun],
	//    m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_GaoShanLiuShui], m_UserStatisticsData[nChairID].m_nStatictisSkillTriggerCounts[Skill_LiuXingHuoYu]);
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	m_UserStatisticsData[nChairID].Clear();

	//��ʱû�еȼ���,����Ϊ0
	if (false == WritePreGameScore(nChairID, llWinScore, m_UserGameScore[nChairID].llMoneyNum, 0))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("%.64s Ԥ��ȡд��ʧ�� ��Ӯ���� =%I64d  �ܷ��� =  %I64d  "),
			m_userName[nChairID],
			llWinScore.llMoneyNum,
			m_UserGameScore[nChairID].llMoneyNum);
		m_LogFile.WriteErrorLogFile(m_szErrorInfo);
		return false;
	}

	return true;
}

//TODO: Ԥ������ҵ÷�
bool CGameServer::CalcPreScore(int nChairID)
{
	//������˻��������˶��ٷ���
	stUserMoney llPreWinScore = m_UserGameScore[nChairID] - m_UserPreGameScore[nChairID] ;

	//����ܷ���
	stUserMoney llPreTotalScore = m_UserGameScore[nChairID];			   //m_UserGameScoreȫ����ʱ�ɽӿڶ���

	for (int i = 0; i < PLAY_COUNT; i++) 
	{
		m_llSumIncome += m_UserStatisticsData[i].llIntervalIncome.llMoneyNum;						//ϵͳ����
		m_llSumExpense += m_UserStatisticsData[i].llIntervalExpense;					            //ϵͳ֧��
		m_llSumIntervalIncome += m_UserStatisticsData[i].llIntervalIncome.llMoneyNum;
		m_llSumIntervalExpense += m_UserStatisticsData[i].llIntervalExpense;
	}

	_sntprintf(m_szDatalogInfo , _countof(m_szDatalogInfo) ,
		TEXT("���Ӻ�=%d, ϵͳʱ������=%I64d,%I64d, ϵͳ�ܼ�����=%I64d,%I64d, ϵͳʱ������=%I64d, ϵͳʱ��֧��=%I64d,%I64d, ϵͳ����=%I64d, ϵͳ֧��=%I64d,%I64d"), 
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

	//����ֵ�仯 
	LONGLONG llGradeScore = m_llGradeScore[nChairID] - m_llPreGradeScore[nChairID];
	if (!WritePreGameScore(nChairID, llPreWinScore, llPreTotalScore.llMoneyNum, llGradeScore))
	{
		return false;
	}

	//д�˷������ͬ��
	m_UserPreGameScore[nChairID] = m_UserGameScore[nChairID];
	m_llPreGradeScore[nChairID] = m_llGradeScore[nChairID];

	return true;
}

void CGameServer::RecordServerLog(DWORD dwChairID, int UserDBID)
{
	/*_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("���Ӻ�=%d, DBID=%d, ����������: �����жϴ���=%I64d, ������Ч����=%I64d, ���˻������=%I64d"),
	m_nTableID, UserDBID, m_nStatictisAvoidCounts, m_nStatictisAvoidValidCounts, m_llSumAvoidHitSystemGetScore);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("���Ӻ�=%d, DBID=%d, ���ܴ�����: ��ҹ�������=%d, ���ܴ�������=%d,%d,%d,%d, [�����������]"),
	m_nTableID, UserDBID, m_UserStatisticsData[dwChairID].m_nStatictisHitCounts,
	m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_LianSuoShanDian], m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_HengSaoQianJun],
	m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_GaoShanLiuShui], m_UserStatisticsData[dwChairID].m_nStatictisSkillTriggerCounts[Skill_LiuXingHuoYu]);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);

	_sntprintf(m_szDatalogInfo, _countof(m_szDatalogInfo),
	TEXT("���Ӻ�=%d, DBID=%d, �����ɱ��: ɱ����=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	��Ч��ɱ=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	����ɱ�: ����֧��=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, \
	��������=%I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d, %I64d"),
	m_nTableID, UserDBID, m_nStatictisMonsterKillCounts[mtMoleBoSi], m_nStatictisMonsterKillCounts[mtMoleDaiMeng], m_nStatictisMonsterKillCounts[mtMoleFeiXing], m_nStatictisMonsterKillCounts[mtMoleHuShi], m_nStatictisMonsterKillCounts[mtMoleKuangGong], m_nStatictisMonsterKillCounts[mtMoleKing], m_nStatictisMonsterKillCounts[mtSpider], m_nStatictisMonsterKillCounts[mtFly], m_nStatictisMonsterKillCounts[mtFlyGoblin],
	m_nStatictisMonsterIncomingCounts[mtMoleBoSi], m_nStatictisMonsterIncomingCounts[mtMoleDaiMeng], m_nStatictisMonsterIncomingCounts[mtMoleFeiXing], m_nStatictisMonsterIncomingCounts[mtMoleHuShi], m_nStatictisMonsterIncomingCounts[mtMoleKuangGong], m_nStatictisMonsterIncomingCounts[mtMoleKing], m_nStatictisMonsterIncomingCounts[mtSpider], m_nStatictisMonsterIncomingCounts[mtFly], m_nStatictisMonsterIncomingCounts[mtFlyGoblin],
	m_nMonsterPlayerCost[mtMoleBoSi], m_nMonsterPlayerCost[mtMoleDaiMeng], m_nMonsterPlayerCost[mtMoleFeiXing], m_nMonsterPlayerCost[mtMoleHuShi], m_nMonsterPlayerCost[mtMoleKuangGong], m_nMonsterPlayerCost[mtMoleKing], m_nMonsterPlayerCost[mtSpider], m_nMonsterPlayerCost[mtFly], m_nMonsterPlayerCost[mtFlyGoblin],
	m_nMonsterPlayerIncome[mtMoleBoSi], m_nMonsterPlayerIncome[mtMoleDaiMeng], m_nMonsterPlayerIncome[mtMoleFeiXing], m_nMonsterPlayerIncome[mtMoleHuShi], m_nMonsterPlayerIncome[mtMoleKuangGong], m_nMonsterPlayerIncome[mtMoleKing], m_nMonsterPlayerIncome[mtSpider], m_nMonsterPlayerIncome[mtFly], m_nMonsterPlayerIncome[mtFlyGoblin]);
	GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), m_szDatalogInfo);*/
}

//��GameServer��DBд����Ԥ����ֵȹؼ���Ϣ
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

			// * /���д��
			//long lScore[5] = {0};
			//memset(lScore, 0, sizeof(lScore));
			//lScore[0] = lDeltaScore.lScore;
			// * /����Ϊʲô��Ҫд�֣�
			// * /m_pSite->WriteScore(dwChairID, (char*)lScore);

			char logdata[200];
			sprintf(logdata,"�û�ID��%d   �û����ݿ�ID: %d   д�����ݿ���������  %ld  �������:%ld  ���Ͷ��ɱ�:%ld     " , pID, UserDBID, llPreTotalScore, llPreWinScore.llMoneyNum, llGradeScore);
			m_LogFile.WriteDataLogFile(logdata);

			m_dbUserPreWelFarePoint[dwChairID] = m_dbUserWelFarePoint[dwChairID];

			if (nResult >= S_OK)
			{// * /��չд�ֳɹ�����
				return true;
			} 
			else
			{
				_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("ERROR:��ʱдԤ����������󷵻�=%d."), nResult);
				m_LogFile.WriteErrorLogFile(m_szErrorInfo);
				return false;
			}
		}
		else
		{
			_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("Error:��ʱдԤ�������Ϊ��."));
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

// ���ͽ�Ҹ�ָ�����
void CGameServer::SendUserMoneyCountTo(BYTE byChairID, IGamePlayer *pPlayer, bool bRealTimeMoney /* = false */)
{
	//��������

	if (false == CheckValidChairID(byChairID) || NULL == pPlayer)
		return;

	SY::UserMoneySync syCoinCount;
	//���ñ���
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

// ������켣��Ϣ�����
void CGameServer::SendAddFishCmd( int nCmd , char *pData , int nLen)
{
	GAME_CMD_DATA buf;
	buf.InitData(-1, nCmd);
	buf.AddData(pData,nLen);
	m_pServer->SendGameCmd(buf.GetBuf(), buf.GetDataLen());
}

//������Ļ�ֱ��ʸ����
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

//�ƶ��洦������Ϸ�
void CGameServer::MoblieDealUserAddScore(int ChairID)
{

	stUserMoney nAddSocre = m_UserGameScore[ChairID];

	if (nAddSocre.llMoneyNum < 0 || nAddSocre.llTicketNum < 0)
	{
		return;
	}


	////��ҹ����ӵ����˽����
	//m_UserGameScore[ChairID] -= nAddSocre;
	////��ȥ����ܵĿɶһ����ӵ���
	//m_UserGameScore[ChairID] -= nAddSocre;

	//�������ӵ�����δд��
	m_enUserState[ ChairID]   = enPlayerBuy;			//����Ϸ�״̬

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

//�ֻ��洦������·�
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

	//if(m_bLossPoolRepay)    //����ط������ÿ��� 
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
	//				OUTLOOK("�����Ϸʱ�䳬��3���ӣ����������");
	//			}
	//		}
	//		else
	//		{			
	//			OUTLOOK("�����Ϸʱ�����3���ӣ������������");
	//		}
	//	}
	//}


	//�˳�ʱ������ҵķ��� 
	if ( enPlayerBuy == m_enUserState[pEnableLeaveQuery->m_ChairID] )
	{		
		//CalcScore( pPlayer , pEnableLeaveQuery->m_ChairID);
		CalcPreScore(pEnableLeaveQuery->m_ChairID);
		m_enUserState[pEnableLeaveQuery->m_ChairID]  = enPlayerSettled;
	}
	else
	{
		;//д��ʧ��
	}   	


	//�·ֺ���ҿ������0��
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

	// ��ͳ����Ϣ��ʼ��
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

	// ������һ�λ��ܣ�
	pac->MoneyGain		+= Change.MoneyGain;				// �ۼӻ�ɱ������
	pac->MoneyCost		+= Change.MoneyCost;				// �ۼӻ�ɱ��֧��
	pac->MissingCount	+= Change.MissingCount;				// �ۼ������жϴ���
	pac->MissingValid	+= Change.MissingValid;				// �ۼ�������Ч����
	pac->MissingGain	+= Change.MissingGain;				// �ۼ�����ϵͳ����

	pdl->MoneyGain		= Change.MoneyGain;					// ʱ�λ�ɱ������
	pdl->MoneyCost		= Change.MoneyCost;					// ʱ�λ�ɱ��֧��
	pdl->MissingCount	= Change.MissingCount;				// ʱ�������жϴ���
	pdl->MissingValid	= Change.MissingValid;				// ʱ��������Ч����
	pdl->MissingGain	= Change.MissingGain;				// ʱ������ϵͳ����

	for ( i = 0; i < SkillTypeCount; i++ )
	{
		pdl->SkillCount[i] = Change.SkillCount[i];			// ʱ�μ��ܴ�������
		pac->SkillCount[i] += Change.SkillCount[i];			// �ۼӼ��ܴ�������
	}

	for ( i = 0; i < MonsterTypeCount; i++ )
	{
		pdl->KillMonster[i] = Change.KillMonster[i];		// ʱ�λ�ɱ��������(���ܹ����Ƿ��Ǯ)
		pdl->KillValid[i]	= Change.KillValid[i];			// ʱ����Ч��ɱ����(�����Ǯ��)
		pdl->KillCost[i]	= Change.KillCost[i];			// ʱ�λ�ɱ����֧��
		pdl->KillGain[i]	= Change.KillGain[i];			// ʱ�λ�ɱ��������
		pac->KillMonster[i] += Change.KillMonster[i];		// �ۼӻ�ɱ��������(���ܹ����Ƿ��Ǯ)
		pac->KillValid[i]	+= Change.KillValid[i];			// �ۼ���Ч��ɱ����(�����Ǯ��)
		pac->KillCost[i]	+= Change.KillCost[i];			// �ۼӻ�ɱ����֧��
		pac->KillGain[i]	+= Change.KillGain[i];			// �ۼӻ�ɱ��������
	}

	if (Change.Level > pac->Level)
		pac->Level = Change.Level;

	if (pac->MonsterRate <= Change.MonsterRate)				// ��ֻ���������
	{
		pac->MonsterRate	= Change.MonsterRate;
		pac->MosterID		= Change.MosterID;
	}

	if (pac->MonsterMostGain < Change.MonsterMostGain)		// ��ֻ�����������
		pac->MonsterMostGain = Change.MonsterMostGain;

	if (pac->MostGain < pit->InitMoney + pac->MoneyGain - pac->MoneyCost)		// ���������ʼ��ʱ����ڳ�ʼ��
	{
		pac->MostGain	= pit->InitMoney + pac->MoneyGain - pac->MoneyCost;		// �µ��������
		pac->T			= Change.T;												// �������ʱ���
	}
	
	m_pStatistic->IncomingChange(m_Statistic[nIdx]);
	pStatistic->Lasttime = _time64(NULL);

	// ���³�ʼ��ͳ����Ϣ
	memset(m_Change + nIdx, 0, sizeof(Change_t));
}

void CGameServer::Statistic_End(IGamePlayer* pPlayer, int nIdx)
{
	m_pStatistic->PlayerLeave(m_Statistic[nIdx]);
	memset(m_Statistic + nIdx, 0, sizeof(Statistic_t));
	memset(m_Change + nIdx, 0, sizeof(Change_t));
}
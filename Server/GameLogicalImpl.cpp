#include "StdAfx.h"
#include "GameLogi.h"
#include "Protocol/GameMessageTags.h"
#include "utility.h"
#include "MonsterManager.h"
#include "StatisticMgr.h"

// * /porting from FishgSea V2.5.0�汾�����ֶ���, 22Oct.2015
// * /���ݶ���汾�ţ����巽ʽ���£�����������ϷIDռnλ������1000000000��
// * /���汾��ռ3λ������1000000���ΰ汾ռ3λ������1000����Ͱ汾3λ�������������2.5.0Ϊ����Ϊ31002005000
#define CUR_FISHING_VERSION	31002008000
//��· 2014-07-25 tagFishingData������������dwLossPool������ҿ���ֵ
//����OLD_FISHING_VERSION ��¼�ɵİ汾�� �� CURR_FISHING_VERSIONҪ�����µİ汾�����ﵽ���ݾɰ汾
#define OLD_FISHING_VERSION_1	31002005000				//����İ汾��
#define OLD_FISHING_VERSION_2	31002006000				//���ڻ���ʧʶ��ɲ�����ҵİ汾Ϊ 31002006000
#define OLD_FISHING_VERSION_3	31002007000				
#define VER_FIELD_REMAINDER      1000000000         // * /����Ϊ��¼�汾�ŵ��ֶΣ�22Oct.2015

//IGameLogical
STDMETHODIMP CGameServer::SetServerSite(IServerSite * pGameSite)
{
	m_pSite = pGameSite;
	m_pSite->AddRef();

	m_pSite->GetPlayerList(&m_pList);
	m_pSite->GetServerBase(&m_pServer);

	m_quickStartelper.SetServerSite(pGameSite);

	m_dwCurTick = GetTickCount();

	m_nRoomID = m_pSite->GetGameOption();      //�ֶ��Զ���

	//��ȡ������Ϣ ����ID������ID
	InitRoomInfo();

	//���÷�����
	char szServerName[MAX_NAME_LEN] = {0};
	m_pServer->QueryInterface(IID_ITableChannel, (void **)&m_TableChannel);

	if (m_TableChannel != NULL)
	{
		m_TableChannel->TableDispatch(ITableChannel::IID_Get_Room_Name, szServerName, MAX_NAME_LEN);

		StatisticMgr::getInstance().SetRoomName(szServerName);
	}

	//LOGI("m_nRoomID=%d, m_nTableID=%d", m_nRoomID, m_nTableID);
	CMonsterManager::Instance().Initialize(m_nRoomID, m_nTableID);

	// ������ͳ��
	m_pStatistic = const_cast<CStatistic*>(CStatistic::Instance());
	m_pStatistic->SetRoomName(szServerName);

	//���ù���ż������
	m_LogFile.SetGameRoomID(m_nRoomID);

	//	m_pTableChannel->TableDispael::IID_Get_TableID, (void*)&m_nTableID, 0);

	//��־��¼�ļ���ʼ��
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

	//	�����ﹹ���������Ϸ�ֳ���ע��ConstructScene������ζ����Ϸ��ʼ
	//  TODO:������Լ������ݽṹ�����������ÿ�ֿ�ʼ��ʱ�򱻵��á�

	m_started=false;

	m_dwCurTick = GetTickCount();   // �õ���ǰtick

	KillGameTimer(TIME_ADD_MONSTER);													//�����
	KillGameTimer(TIME_ADD_GROUP_MONSTER);												//������켣
	KillGameTimer(TIME_CLEAR_MONSTER);													//�����켣
	KillGameTimer(TIME_CHANGE_SCENE);												//�л�����
	//KillGameTimer(TIME_REGULAR_FISH);												//���ɹ�����																			
	KillGameTimer(TIME_CHECK_POOL);													//���������ļ�	
	//KillGameTimer(TIME_LOAD_FILE);													//��齱��  
	

	//static int initcount;
	//initcount++;
	//OUTLOOK("��ʼ������%d", initcount);

	// ��ʼ����Ϸ����
	InitData();
	//��Ϸ����ţ����ŷ���ŵı�ʶ
	//m_nRoomID = m_pSite->GetGameOption();   //TODO: ��m_nGameRoomID�ظ���ֱ���滻��

	// ���ºõ�ˢ�»�����һ�μ��ˢһ���֣� �ͻ��˼�¼�������ݣ� �ٰ��Լ���Ƶ��ˢ�³������������Ƿ�����֪ͨ��ˢ�¹֣�
	const GamePropertyInfo* infGameProperty = GameConfigMgr::getInstance().Current().GetGamePropertyInfo(m_nRoomID);
	SetGameTimer(TIME_ADD_MONSTER, infGameProperty->AddMonsterTime, INFINITE, 0L);		                    //�����    3��
	SetGameTimer(TIME_ADD_GROUP_MONSTER, infGameProperty->AddGroupMonsterTime, INFINITE, 0L);                 //�������    10��
	SetGameTimer(TIME_CLEAR_MONSTER, infGameProperty->KillMonsterTime, INFINITE, 0L);						//�����켣 1��
	SetGameTimer(TIME_CHANGE_SCENE, infGameProperty->ChangeScrenceTime, INFINITE, 0L);						//�ı䳡��   30��
	//SetGameTimer(TIME_LOAD_FILE, infGameProperty->LoadFileTime, INFINITE, 0L);								//�����ļ�ˢʱ���� 60��
	SetGameTimer(TIME_CHECK_POOL, infGameProperty->CheckPoolTime, INFINITE, 0L);								//��齱�� 5��
	m_pServer->StartTimer(STANDARD_TIME_ELAPSE);
	return S_OK;
}

//��״̬  ����ˢ�¹ֺ���Ϲ�ʱ��  �����ʱ��
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
	// TODO:��дGame_SCENE�ṹ������ȥϵͳ���Զ�������ṹ���͸��ͻ���
	// �ͻ��˻���OnSceneChanged���д������������
	// ע�⣬����ṹͨ�����Ǳ䳤�ġ�

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
	case GS_FREE:		//����״̬
	case GS_PLAYING:	//��Ϸ״̬ 
		{
			//������Ϣ
			if (pPlayer->IsPlayer())
			{
				//�����ҵķ����������
				//m_UserStatisticsData[nIndex].llUserCarryScore.llMoneyNum = GetPlayerScore(nIndex, pPlayer);
				//���ͷ������ͻ���
				SendUserMoneyCountTo(nIndex, pPlayer);
				GameStationBase *pGameScene = (GameStationBase *)pScene->lpdata;

				if (!pGameScene)
				{
					_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("GameStationBase Create Failed    Player = %d"), nIndex);
					m_LogFile.WriteErrorLogFile(m_szErrorInfo);
					//GLOG_NORMAL(m_nRoomID*10+5,m_szErrorInfo);
					return false;
				}

				pGameScene->bPlaying = true;					//��Ϸ�Ƿ��Ѿ���ʼ��
				pGameScene->bySceneIndex = (BYTE)m_nSceneIndex;				//��ǰ����

				for (int i = 0; i<PLAY_COUNT; i++)
				{
					pGameScene->nUserMoneyNum[i] = (int)m_UserGameScore[i].llMoneyNum; // m_UserStatisticsData[i].llUserCarryScore.llMoneyNum;
					pGameScene->nUserTicketNum[i] = (int)m_UserGameScore[i].llTicketNum; // m_UserStatisticsData[i].llUserCarryScore.llTicketNum;
				}

				//��似�ܺ�����״̬��Ϣ
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
					pGameScene->SkillInf[i].SkilllState = SY::SkillStatus::Actived;  //TODO: ����ȡ�������������
				}

				//�������
				//for (int i = 0; i<26; i++)
				//{
				//    pGameScene->nFishScore[i] = m_nFishScore[i];
				//}
			}

			//���ͳ���
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
	//	�������Ϸ�ſ�ʼ���ͻ����Ѿ���ʼ��
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
//��ʱ����
void CGameServer::wild_skill_timer_deley()
{
	//2016.10.20 hl  �񱩹������timer
	KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);
	KillGameTimer(TIME_WILD_SKILL_DELEY);
	//��״̬
	//2016.10.20 hl ���ڿ�״̬��ʱ��//��������Ϳ�����ʱ��
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

	//Ӧ����ϸ�Ĵ���stop��uflags����ͬ��flagָʾ��ֹͣ������
	switch (uflags) {
	case STOP_SERVERERROR:
		m_pSite->Dismiss("������ͣ���������ܽ�ɢ");
		// TODO:���Ҫ�ж��⴦���������
		break;
	case STOP_NOENOUGHPLAYER:
		if (m_started) 
		{
			//�ҵ�������¼����ˣ���������
			//IGamePlayer * pPlayer =NULL;
			//for (int i=0; i<4; i++) {
			//	if (FAILED(m_pList->GetUser(i, &pPlayer))) continue;

			//	if (pPlayer->IsValidUser()) continue;
			//	//	�������valid,�������ܡ�ע�⣬ֻ������һ���������ͣ��
			//	// TODO: ����Ӯ���
			//	m_pSite->OnGameEnd();
			//}
			//m_pSite->Dismiss("��������");
		}
		else {
			//m_pSite->Dismiss(NULL);
		}
		break;
	default:	// * /add:����ԭ��ͣ����ɢ�Ϳ����ˣ�06May.2011
		m_pSite->Dismiss("����ԭ�����ӽ�ɢ��");
	}

	m_pServer->StopTimer();		// * /Add for..as you see

	//2016.10.20 hl  �񱩹������timer
	KillGameTimer(TIME_WILD_SKILL_WAIT_STEP);                                        //��״̬
	KillGameTimer(TIME_WILD_SKILL_END);
	KillGameTimer(TIME_WILD_SKILL_WILL_END);
	wild_skill_monster_map.clear();
	return S_OK;
}

STDMETHODIMP CGameServer::Clear()
{
	m_started =false;

	//m_nRoomID = 0;
	//ϵͳ����
	memset(m_userName, 0, sizeof(m_userName));														//��0�������
	memset(m_dwUserID, 0, sizeof(m_dwUserID));														//��0���ID
	memset(m_lDrawnCount, 0, sizeof(m_lDrawnCount));											//��0��Ҷ��پ�
	memset(m_enUserState, enPlayerNull, sizeof(m_enUserState));							//Ĭ�϶��ǿ�״̬ 

	memset(m_UserStatisticsData, 0, sizeof(m_UserStatisticsData));								//��Ϸ��ʹ�õķ���
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
	WriteInfoLog(Format("CGameServer::OnGameOperation ������Ϣ��%d", opID));

	//��Ϸû��ʼ�ż��
	if (0x70 == opID && !m_started)	    //p��ӦascֵΪ0x70
	{
		if (SUCCEEDED(m_quickStartelper.OnGameOperation(nIndex, dwUserID, opID, oData))) 
		{
			m_started = true;		//�������Ϊ��ʼ��

			m_UserStatisticsData[nIndex].llUserCarryScore = m_UserGameScore[nIndex];
			m_UserStatisticsData[nIndex].dwLastHitTime = GetTickCount();  
			m_UserStatisticsData[nIndex].nUserEnterRoomTick = GetTickCount();     //��¼��ҽ��뷿��tick
		}

		IGamePlayer * _pPlayer = NULL;
		int nState = -1;

		//����û�����
		if (FAILED(m_pList->FindUser(dwUserID, &_pPlayer)) || false == CheckValidChairID(nIndex)) 
		{
			//��m_pList�в�������ζ�Ÿ��û�����player
			//throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTPLAYER, "������û���������");
		}
		else
		{
			//����������Ϣ
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

			//���ͻ�����Ϣ
			SendBasicInfo(nIndex, _pPlayer);
			//����ͬ������
			SendAdditionalGameData(nIndex);
			//�ж��Ƿ���Ҫ�Զ������
			CheckCanAutoActiveSkill(_pPlayer, true);
			//���͹�������
			SendUserMoneyCountTo(nIndex, _pPlayer, true);
		}

		return S_OK;
	}
	

	//��������
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
		//����û�����
		if (FAILED(m_pList->FindUser(dwUserID, &pPlayer)) || false == CheckValidChairID(nIndex)) 
		{
			//��m_pList�в�������ζ�Ÿ��û�����player
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTPLAYER, "������û���������");
		}

		//��Ϸδ��ʼ�������������
		if (false == m_started)
		{
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTACTIVATE, "��Ϸδ��ʼ�������������");
		}

		//�����û���ID
		if (0 == m_dwUserID[nIndex])
		{
			//���DBID<br class="Apple-interchange-newline">
			m_dwUserID[nIndex] = dwUserID;
			//����û���
			pPlayer->get_UserName(m_userName[nIndex]);
			m_userName[nIndex][63] = '\0';
		}

		//��Ϣ����
		switch (opID)
		{
		case GameMessages::GameDataReq:
			//�յ������Ϣ��������һ�������Ϣ
			CheckCanReturnUserCoin(pPlayer);
			SendUserMoneyCountTo(nIndex, pPlayer, true);

			m_UserStatisticsData[nIndex].bClientReady = true;

			WriteInfoLog("������GameDataReq");
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
		case GameMessages::ChangeWeaponReq:	// �л�����
			//OnChangeWeapon(pPlayer, oData, nSize);
			break;         
		case GameMessages::UpgradeSkillReq: // ��������
			OnUpgradeSkill(pPlayer, oData, nSize);
			break;
		case GameMessages::PlayerHit:		// ��ҵ��
			OnPlayerHit(pPlayer, oData, nSize);
			break;
		default:
			TRACEWARNING("δ���������:%d", opID);
			throw OPERATION_ERROR(OPERATION_ERROR::USERISNOTACTIVATE, "δ֪������");
			break;
		}
	} catch (OPERATION_ERROR err) {
		//�������
		/*	������Ϣ����
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

		//���ڲ�ͬ�Ĵ��󣬴���һ��
		switch (err.nCode) {
		case OPERATION_ERROR::USERISNOTPLAYER:
			//	����ҷ�����ֱ�Ӷ���
			m_pSite->GetOff(dwUserID);
			break;
		case OPERATION_ERROR::USERISNOTACTIVATE:
			//�ǻ�û�����������
			break;
		case OPERATION_ERROR::OUTCARDISNOTEXIST:
			//�����ݴ�����/�ظ�����һ��
			m_pSite->GetOff(dwUserID);
			break;
		// TODO: ���������Ĵ������
		default:
			//	�������⡣�������Ǳ��յ�����
			m_pSite->GetOff(dwUserID);
		}

		Trace("-----");
	}
	catch(...) {
		//δ֪������
		Trace("--Unknown error occured in OnGameOperation");
	}

	SAFE_RELEASE(pPlayer);
	return S_OK;
}

//���㹲��������Ϣ
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

			SGameData *pGameData = &(pData->sEnyGameData);		// * /��������¼���

			nSize = sizeof(pGameData->wRecEnergyBarCount) + pGameData->wRecEnergyBarCount * sizeof(SRecEnergyBar) + sizeof(SWelfareData);
			if (nSize > 512 || pGameData->wRecEnergyBarCount > CountArray(pGameData->arRecEnergyBar))
			{
				TRACEFALTAL(_T("Error: user=%.64s ������Ϸ���ݳ���(%d)������512��������������(%d)���������ֵ��"), m_userName[dwChairID], nSize, (int)CountArray(pGameData->arRecEnergyBar));
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

	// * /���ݶ���汾�ţ����巽ʽ���£�����������ϷIDռnλ������1000000000��
	// * /���汾��ռ3λ������1000000���ΰ汾ռ3λ������1000����Ͱ汾3λ�������������2.5.0Ϊ����Ϊ31002005000
	LONGLONG llGetInitGameID = pData->llVersion / VER_FIELD_REMAINDER;
	LONGLONG llGetVersionNo = pData->llVersion % VER_FIELD_REMAINDER;

	//�ɵİ汾��
	LONGLONG llOldVersionNo1 = OLD_FISHING_VERSION_1 % VER_FIELD_REMAINDER;
	LONGLONG llOldVersionNo2 = OLD_FISHING_VERSION_2 % VER_FIELD_REMAINDER;
	LONGLONG llOldVersionNo3 = OLD_FISHING_VERSION_3 % VER_FIELD_REMAINDER;

	//�µİ汾��
	LONGLONG llCurVersionNo = CUR_FISHING_VERSION % VER_FIELD_REMAINDER;


	//��· 2014-07-25 �����˿���ֵ��Ϊ�˼��ݾɵ����ݡ���Ҫ���ݰ汾�Ž����ж�
	//�����ǰ�汾���Ǿɰ汾�����޸���ҵ�����Ϊ�°汾���µĹ������ݽṹ��С
	if ( llGetVersionNo == llOldVersionNo1   ||
		 llGetVersionNo == llOldVersionNo2   ||
		 llGetVersionNo == llOldVersionNo3   )
	{
		//Trace("���%s ��ʱ��ӡ �ɰ汾��%I64d  ",m_userName[dwChairID],pFishingData->llVersion);
		//����������µİ汾
		pData->llVersion = CUR_FISHING_VERSION;
		//�ѹ������ݽṹ��С�����ı�Ϊ���µĴ�С
		pData->wDataLen = sizeof(tagFishingData);
		//����Ǿɰ汾����ô����Ҫ��ʼ�����Ŀ���ֵΪ0
		memset(pData->arLossPool, 0 , sizeof(tagLossPool) * EXTEND_DATA_COUNT );
		
		//Trace("��ʱ��ӡ ���%s �������Ϊ�µİ汾�� %I64d    ���������е�һ����ֵ %d  ���һ��ֵ %d    �ȼ�%I64d  �°汾���ݴ�С%d"  , m_userName[dwChairID],pFishingData->llVersion,pFishingData->arLossPool[0].dwLossPool ,pFishingData->arLossPool[19].dwLossPool  ,pFishingData->llGradeScore ,  pFishingData->wDataLen);
		return true;
	}
	else if ( llGetVersionNo == llCurVersionNo && sizeof(tagFishingData) == pData->wDataLen)
	{
		//Trace("��ǰΪ�°汾�Ų��ô���  %I64d"  , llCurVersionNo);
		//�°汾ʲô���鶼��������ֱ�ӷ���TRUE
		//Trace("��ʱ��ӡ ���%s  �Ѿ������°汾��",m_userName[dwChairID] );
		return true;
	}
	// * /���汾�������ݴ�С�����Ծ���Ҫ��ʼ�� ��· 2014-07-25 �����˿���ֵ�����ݽṹ����ˣ�������ǰ�����ݴ�С���޸�Ϊ���µ����ݴ�С sizeof(tagFishingData) -sizeof(DWORD) == pFishingData->wDataLen
/*	if (llGetVersionNo == llCurVersionNo && (  (sizeof(tagFishingData)  - sizeof(DWORD) ) == pFishingData->wDataLen || sizeof(tagFishingData) == pFishingData->wDataLen) )
	{// * /���ݰ汾��ߴ���ȷ
		return true;
	}*/

	memset(pData, 0, sizeof(tagFishingData));
	pData->llVersion = CUR_FISHING_VERSION;
	pData->wDataLen = sizeof(tagFishingData);
	//Trace("��ʱ��ӡ ���%s ��������ҵİ汾�� %I64d      ������ͬ�����Ŀ���ֵ %d   %d    �ȼ� %I64d"  ,m_userName[dwChairID],pFishingData->llVersion,pFishingData->arLossPool[0].dwLossPool ,pFishingData->arLossPool[19].dwLossPool,pFishingData->llGradeScore);
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

// * /����ͨ�ò���ȼ�, 27Oct.2015
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
	szOutput.Format("�û�ID��%ld��-->%s", m_iLogID, szMsg);

	CTime tmTime(time(NULL));

	CString szFileName;
	szFileName.Format("���Ķ�������־_%d_%s", m_iLogID, tmTime.Format("%Y%m%d"));

	TRACEFALTALFILE((LPSTR)(LPCSTR)szFileName, (LPSTR)(LPCSTR)szOutput);
}

void CGameServer::WriteKillLog(CString szmsg)
{
	return ;
	CString szOutput;
	szOutput.Format("�û�ID��%ld��-->%s", m_iLogID, szmsg);

	CTime tmTime(time(NULL));

	CString szFileName;
	szFileName.Format("���Ķ������־_%d_%s", m_iLogID, tmTime.Format("%Y%m%d"));

	TRACEFALTALFILE((LPSTR)(LPCSTR)szFileName, (LPSTR)(LPCSTR)szOutput);
}
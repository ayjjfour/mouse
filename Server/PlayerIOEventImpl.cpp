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
	case 54134725:	//ʯ��
	case 53737291:	//����
	case 34203756:	//�·�
	case 52697881:	//��־��
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

	if (CString(Utf8Name).Find("ran_") == 0)	//��������Ƚ����ϵ���˺�
	{
		m_bEnableInfoLog = true;
		m_iLogID = pGamePlayer->get_DBID();
		CMonsterManager::Instance().Attach(m_nTableID, this);
	}

	if (m_bEnableInfoLog)
	{
		m_bEnableInfoLog = GameConfigMgr::getInstance().Current().GetGamePropertyInfo(m_nRoomID)->bEnableLog;
	}
	
	WriteInfoLog("��ҽ�����Ϸ");

	CharSetHelper::UTF8ToANSI(Utf8Name, AnsiName, sizeof(AnsiName));
	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), "OnUserEnter: DBID=%d, UserName=%s, RoomID=%d, TableID=%d, Idx=%d", pGamePlayer->get_DBID(), AnsiName, m_nRoomID, m_nTableID, nIdx);

	//m_pTableChannel->TableDispatch(ITableChannel::IID_Get_TableID, (void*)&m_nTableID, 0);

	//��ʼ����ҽ��뷿��Я������
	m_UserStatisticsData[nIdx].Clear();

	// ͬ������������ݣ� nIdx>=0ʱ����ֲ�ͬ����<0ʱͬ����������
	m_pSite->OnSceneChanged(nIdx);     //ȥ��m_started�ж�ֱ�ӵ��ó����л�   2016/1/22

	//��Ϸδ��ʼ �Ϳ�ʼ������ʱ�� ��ʼ��Ϸ
	if (!m_bIsPlaying)
	{
		//��ʼ��Ϸconscons
		//��ʱע�� StartGame();
	}

	//int iTempPercent = AuthPermissions(nIdx);

	//memset(&m_arFishingData[nIdx], 0, sizeof(tagFishingData));
	//if (false == getFishingData(nIdx, pGamePlayer->get_DBID(), &m_arFishingData[nIdx]))
	//{
	//    _sntprintf(m_szErrorInfo, CountArray(m_szErrorInfo), TEXT("Error:%.64s Fail to GetFishingData."), m_userName[nIdx]);
	//    m_LogFile.WriteErrorLogFile(m_szErrorInfo);
	//    return E_FAIL;
	//}

	// ����������
	//DeserializeUserInteractData(nIdx);

	//��ҽ������¼�����������ϵĽ��������������������ӵ� /100*100��֤�ӵ��������ڻ�ı���
	m_dwUserID[nIdx] = pGamePlayer->get_ID();							//DBID��Ϊ0
	m_UserGameScore[nIdx].llMoneyNum = GetPlayerScore(nIdx, pGamePlayer);    // m_llUserTotalScore
	m_UserGameScore[nIdx].llTicketNum = 0;//GetPlayerTicket(nIdx, pGamePlayer);
	m_UserPreGameScore[nIdx] = m_UserGameScore[nIdx];
	m_dbUserWelFarePoint[nIdx] = 0;// ������
	m_dbUserPreWelFarePoint[nIdx] = 0;// ������
	m_lDrawnCount[nIdx] = 0;							//ƽ�ִ���
	m_enUserState[nIdx] = enPlayerEnter;		//��ǽ���״̬
	m_nTableUserCount++;

	// ��Ҿ���ȼ�
	GetFishingCommonData(nIdx, m_dwUserID[nIdx], &m_FishingData[nIdx]);
	GetCustomData(nIdx, m_dwUserID[nIdx], &m_UserInteractData[nIdx]);
	m_UserPreInteractData[nIdx] = m_UserInteractData[nIdx];

	m_UserStatisticsData[nIdx].llUserCarryScore = m_UserGameScore[nIdx];
	m_UserStatisticsData[nIdx].dwLastHitTime = GetTickCount();  
	m_UserStatisticsData[nIdx].nUserEnterRoomTick = GetTickCount();     //��¼��ҽ��뷿��tick

	//�����û�Exp
	pGamePlayer->put_Exp(m_FishingData[nIdx].llGradeScore);

	m_llGradeScore[nIdx] = GetPlayerExp(nIdx, pGamePlayer, &m_nUserLevel[nIdx]);
	m_llPreGradeScore[nIdx] = m_llGradeScore[nIdx];

	// �ж��Ƿ���Ҫ�Զ������
	//CheckCanAutoActiveSkill(pGamePlayer);

	// ���뷿��ʱ�������Ƿ���Ҫ��֮ǰ����ļ��ܽ��
	//CheckCanReturnUserCoin(pGamePlayer);

	//��ҽ��뼴�Ϸ�
	MoblieDealUserAddScore(nIdx);	

	//////////////////////////////////////////////////////////////////////////
	// ��ʼͳ�������Ϣ add by Ԭ���� 2016-10-13
	Statistic_Start(pGamePlayer, nIdx);
	
	//////////////////////////////////////////////////////////////////////////

	//���������Ӧд�ֶ�ʱ��
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
		//��Щ���뾭����ʱ����Կ����ȶ�����
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "losspool", 0, true, LOGID_Losspool(m_nRoomID));
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "trace", 0, true, LOGID_Trace(m_nRoomID));
		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "Statictis", 0, LOGID_Statictis(m_nRoomID));

		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath, "scorebugtrace", 0, true, LOGID_ScoreBugTrace(m_nRoomID));

		//ILog4zManager::CreateInstance()->DynamicCreateLoggerSpecifiedLogId(m_RecordLogPath,"EnetrNoGun",0,true,100);

	}

	//m_pServer->StartTimer(STANDARD_TIME_ELAPSE);

	//m_LogFile.WriteDataLogFile("��ҽ���");
	return S_OK;
}

STDMETHODIMP CGameServer::OnUserExit(int nIdx, IGamePlayer * pGamePlayer)
{

	if (NULL == pGamePlayer || FALSE == pGamePlayer->IsPlayer())
		return E_NOTIMPL;

	//GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), "OnUserExit: DBID=%d, RoomID=%d, TableID=%d, Idx=%d, state=%d", pGamePlayer->get_DBID(), m_nRoomID, m_nTableID, nIdx, m_enUserState[nIdx]);

	//�˳�ʱ������ҵķ���,ֻ��ûд��ʱ��д��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
	if (enPlayerBuy == m_enUserState[nIdx])
	{
		CalcScore(pGamePlayer, nIdx);
		m_enUserState[nIdx] = enPlayerSettled;
	}

	memset(&(m_UserInteractData[nIdx]), 0, sizeof(UserInteractData));
	memset(&(m_UserPreInteractData[nIdx]), 0, sizeof(UserInteractData));

	m_dwUserID[nIdx] = 0;									//DBID��Ϊ0
	m_lDrawnCount[nIdx] = 0;								//ƽ�ִ���
	m_enUserState[nIdx] = enPlayerNull;				        //��������״̬
	m_nTableUserCount--;
	memset(m_userName, 0, sizeof(m_userName));
	m_UserStatisticsData[nIdx].Clear();

	m_dbUserWelFarePoint[nIdx] = 0;            //ͬ��
	m_dbUserPreWelFarePoint[nIdx] = 0;            //ͬ��

	//��ñ����ڵ���Ҹ���
	int nUserCount = m_pList->GetValidUserNumber();
	//������û������˾͹ر����е�TIMER
	if (0 == nUserCount)
	{
		//ɱ�����м�ʱ��
		//	KillAllTimer();       2015.5.6
		m_bIsPlaying = false;
		//��Ϸ���� �������ӵĽ���д�����ý�����
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

	WriteInfoLog("����뿪��Ϸ");

	m_bEnableInfoLog = false;
	m_iLogID = 0;
	CMonsterManager::Instance().Detach(m_nTableID);

	return S_OK;
}

STDMETHODIMP CGameServer::OnUserOffline(int nIdx, IGamePlayer * pGamePlayer)
{
	TRACEDEBUG("Player=%d OnUserOffline", nIdx);
	//OnUserExit(nIdx, pGamePlayer);	// * /LeonLv add:�������˳�����25Feb.2011
	return E_NOTIMPL;
}

STDMETHODIMP CGameServer::OnUserReplay(int nIdx, IGamePlayer * pGamePlayer)
{
	TRACEDEBUG("Player=%d OnUserReplay", nIdx);

	// ���ŵ�¼ʱ������ˢ�µ�ǰ�������
	SendUserMoneyCountTo(nIdx, pGamePlayer, true);

	return S_OK;	// * /OnUserEnter(nIdx, pGamePlayer);
}

STDMETHODIMP CGameServer::OnUserReady(int nIdx, IGamePlayer * pGamePlayer)
{
	return S_OK;
}
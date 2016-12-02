#include "stdafx.h"
#include <algorithm>
#include "serverSite.h"
#include "Gameserver.h"
#include "GameLogi.h"
#include "CharSetHelper.h"
#include "ShareStruct.h"
#include "GameUtility.h"
#include "Protocol/GameMessageTags.h"
#include "Protocol/GameMessage.pb.h"
#include "TableServer.h"
#include <map>

// * /protocol buffer use
STDMETHODIMP CGameServer::GetPersonalSceneProtBuf(int nIndex, UINT uflags, IGameLogical::SCENE * pScene)
{
	Game_SCENE * pGameScene =(Game_SCENE *)pScene;

	// TODO:填写Game_SCENE结构，接下去系统会自动把这个结构传送给客户端
	// 客户端会在OnSceneChanged当中处理这里的数据
	// 注意，这个结构通常都是变长的。

	int nGameScene = (int)m_enGamePhase;
	IGamePlayer *pPlayer = NULL;
	IGamePlayer *pPlayer1 = NULL;
	GAME_CMD_DATA buf;
	int nSize = 0;

	BYTE byBuffer[2048]={0};
	int wSendSize=0;



	if (FAILED(m_pList->GetUser(nIndex, &pPlayer)) || (pPlayer && FALSE == pPlayer->IsValidUser()))
	{
		_sntprintf(m_szErrorInfo, _countof(m_szErrorInfo), TEXT("GetPersonalScene() %d NOT be valid User."), nIndex);
		m_LogFile.WriteErrorLogFile( m_szErrorInfo );

		return S_OK;
	}
	//遍历玩家在线状态
	for (int i=0; i<PLAY_COUNT; i++) 
	{ 
		m_pList->GetUser(i, &pPlayer1);
		if (pPlayer1->IsValidUser())
		{
			m_bOnlineChair[i] = true;
		}
		else
		{
			m_bOnlineChair[i] = false;
		}
	}

	m_dwCurTick = GetTickCount();		

	SY::GameStationBase *pStationBase =new SY::GameStationBase; 

	SY::ServerPersonalSceneRsp lScene;

	lScene.set_reg_type(0);
	lScene.set_allocated_stationbase(pStationBase);	

	nGameScene = 1;    //temptest
	switch (nGameScene)
	{
	case GS_FREE:		//空闲状态
	case GS_PLAYING:	//游戏状态
		{
			//发送信息
			if (pPlayer->IsPlayer())
			{
				//获得玩家的分数
				//m_UserGameScore[nIndex].llMoneyNum = GetPlayerScore(nIndex, pPlayer);
				//m_UserGameScore[nIndex].llTicketNum = GetPlayerTicket(nIndex, pPlayer);

				//记录下进入的金币
				//m_UserStatisticsData[nIndex].llUserCarryScore = m_UserGameScore[nIndex] ;
				//发送分数到客户端

				//SendUserMoneyCountTo(nIndex, pPlayer);	
				//GameStationBase *pGameScene =(GameStationBase *)pScene->lpdata;
				//发送屏幕分辨率给所有玩家
				SendScreenResolution(CLIENT_WIDTH,CLIENT_HEIGHT);

				if ( !pGameScene )
				{
					_sntprintf(m_szErrorInfo , _countof(m_szErrorInfo) ,TEXT("GameStationBase Create Failed    Player = %d"), nIndex);
					m_LogFile.WriteErrorLogFile(m_szErrorInfo);
					return false;
				}

				//TODO: 数据待填充
				pStationBase->set_playing(true);         //游戏是否已经开始
				m_nSceneIndex = 0;              //temptest  
				pStationBase->set_sceneindex(m_nSceneIndex);        //当前场景
				for (int i=0; i<PLAY_COUNT; i++)
				{
					pStationBase->add_onlinechair(m_bOnlineChair[i]);     //玩家是否在线状态
					pStationBase->add_usermoneynum(m_UserGameScore[i].llMoneyNum);
					pStationBase->add_userticketnum(m_UserGameScore[i].llTicketNum);
				}

				for (int i = 0; i < WEAPON_COUNT; i++)
				{
					int nWeaponCost = -1;
					const WeaponCostInfo* pInfWeapon = GameConfigMgr::getInstance().Current().GetWeaponCostInfo(m_nRoomID, (WeaponID)(i + 1));
					if (pInfWeapon != nullptr)
					{
						nWeaponCost = pInfWeapon->Cost;
					}

					pStationBase->add_weaponcost(nWeaponCost); 
				}

				for (int i = 0; i < SKILL_COUNT; i++)
				{
					SY::SkillInfo* pSkillINfo = pStationBase->add_skillinf();
					pSkillINfo->set_skillid(i+1);
					pSkillINfo->set_status(SY::Actived); //TODO: 根据取出来的数据填充
				}

				//for(int i =0; i<26; i++)
				//{
				//	pStationBase->add_nfishscore(m_nFishScore[i]); //鱼儿赔率
				//}
			}

			//发送场景
			if(lScene.SerializeToArray(pScene->lpdata,sizeof(pScene->lpdata)))
			{
				pScene->cbSize = lScene.ByteSize();
			}
		}
		break;	
	}
	SAFE_RELEASE(pPlayer);
	return S_OK;
}


// * /protocol buffer send
bool CGameServer::sendGameCmdProtBufAll(int aCmdId, void *apData, int aLen)
{
	GameDataProtBuf ProtBufData;
	ProtBufData.InitData(aCmdId);
	if(apData && aLen > 0){
		ProtBufData.AddData(apData, aLen);
	}

	return m_pServer->SendGameCmdToAllPlayer(ProtBufData.GetBuf(), ProtBufData.GetDataLen()) != FALSE;

}
bool CGameServer::sendGameCmdProtBufExcept(int nInx,int cmdId, void* apData,int nLen)
{
	IGamePlayer *pPlayer = 0;
	m_pList->GetUser(nInx, &pPlayer);
	if(!pPlayer)
	{
		TRACEFALTAL("sendGameCmdProtBufExcept: user:%d not found!", nInx);
		return false;
	}
	GameDataProtBuf ProtBufData;
	ProtBufData.InitData(cmdId);
	if(apData && nLen > 0){
		ProtBufData.AddData(apData, nLen);
	}

	return m_pServer->SendGameCmdExcept(pPlayer->get_ID(), ProtBufData.GetBuf(), ProtBufData.GetDataLen()) != FALSE;
}

bool CGameServer::sendGameCmdProtBufTo(int nInx,int cmdId, void* apData,int nLen)
{
	IGamePlayer *pPlayer = 0;
	m_pList->GetUser(nInx, &pPlayer);
	if(!pPlayer)
	{
		TRACEFALTAL("sendGameCmdProtBufTo: user:%d not found!", nInx);
		return false;
	}

	GameDataProtBuf ProtBufData;
	ProtBufData.InitData(cmdId);
	if(apData && nLen > 0){
		ProtBufData.AddData(apData, nLen);
	}

	return m_pServer->SendGameCmdTo(pPlayer->get_ID(), ProtBufData.GetBuf(), ProtBufData.GetDataLen()) != FALSE;
}

CStringA _GameLogic_ConvertFromHex(CStringA szValue)
{
	long lRet = 0;

	for (int i = 0; i < szValue.GetLength(); i++)
	{
		CStringA::XCHAR chValue = szValue[i];

		int iValue = 0;

		if (chValue >= '0' && chValue <= '9')
		{
			iValue = (int)(chValue - '0');
		}
		else if (chValue >= 'a' && chValue <= 'f')
		{
			iValue = (int)(chValue - 'a') + 10;
		}
		else if (chValue >= 'A' && chValue <= 'F')
		{
			iValue = (int)(chValue - 'A') + 10;
		}
		else
		{
			break;
		}

		lRet *= 16;
		lRet += iValue;
	}

	szValue.Format("%ld", lRet);

	return szValue;
}

bool CGameServer::SendAdditionalGameData(int nInx)
{
	UserInteractData& pData = m_UserInteractData[nInx];

	SY::AdditionalGameDataRsp pack;
	pack.set_result(0);
	pack.set_game_id(GAME_ID);
	pack.set_user_dbid(0);
	
	CStringA szAddtionData;
	CStringA szValue;
	
	//enADT_SkillBuyStatus
	szAddtionData.Append(_T("111:"));
	for (int i = SkillTypeCount - 1; i > Skill_None; i--)
	{
		szValue.AppendFormat(_T("%d"), pData.GetSkillBuyStatus((SkillID)i));
	}
	szAddtionData.Append(_GameLogic_ConvertFromHex(szValue)); szValue.Empty();
	
	//enADT_SkillActiveStatus
	szAddtionData.Append(_T("|112:"));
	for (int i = SkillTypeCount - 1; i > Skill_None; i--)
	{
		szValue.AppendFormat(_T("%d"), pData.GetSkillActiveStatus((SkillID)i));
	}
	szAddtionData.Append(_GameLogic_ConvertFromHex(szValue)); szValue.Empty();

	//enADT_SkillLevel
	szAddtionData.Append(_T("|113:"));
	for (int i = SkillTypeCount - 1; i > Skill_None; i--)
	{
		szValue.AppendFormat(_T("%d"), pData.GetSkillLevel((SkillID)i));
	}
	szAddtionData.Append(_GameLogic_ConvertFromHex(szValue)); szValue.Empty();

	//enADT_AutoAttackLevel
	szAddtionData.AppendFormat("|121:%hhu", pData.nAutoAttackLevel);

	pack.set_additional_data((LPSTR)(LPCTSTR)szAddtionData, szAddtionData.GetLength());

	//enADT_AutoHammerLevel 131 自动锤等级 2016.10.18 hl

	szAddtionData.AppendFormat("|131:%hhu",pData.auto_hammer_lv);
	pack.set_additional_data((LPSTR)(LPCTSTR)szAddtionData,szAddtionData.GetLength());


	char buf[2048] = {0};

	if (pack.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(nInx, GameMessages::AdditionalGameDataRsp, buf, pack.ByteSize());
	}


	return true;
}

bool CGameServer::SendAdditionalGameData(int nInx, byte type)
{
	UserInteractData& pData = m_UserInteractData[nInx];

	SY::HallGsAdditionalGameData1Rsp pack;
	pack.set_index(nInx);
	pack.set_user_dbid(0);
	pack.set_type(type);
	pack.set_size(0);

	CStringA szValue;

	switch (type)
	{
	case 111:
		for (int i = SkillTypeCount - 1; i > Skill_None; i--)
		{
			szValue.AppendFormat(_T("%d"), pData.GetSkillBuyStatus((SkillID)i));
		}
		break;
	case 112:
		for (int i = SkillTypeCount - 1; i > Skill_None; i--)
		{
			szValue.AppendFormat(_T("%d"), pData.GetSkillActiveStatus((SkillID)i));
		}
		break;
	case 113:
		for (int i = SkillTypeCount - 1; i > Skill_None; i--)
		{
			szValue.AppendFormat(_T("%d"), pData.GetSkillLevel((SkillID)i));
		}
		break;
	}

	szValue = _GameLogic_ConvertFromHex(szValue);

	pack.set_add_game_data((LPSTR)(LPCTSTR)szValue, szValue.GetLength());

	char buf[2048] = {0};
	if (pack.SerializeToArray(buf, sizeof(buf)))
	{
		sendGameCmdProtBufTo(nInx, GameMessages::HallGsAdditionalGameData1Rsp, buf, pack.ByteSize());
	}

	return true;
}

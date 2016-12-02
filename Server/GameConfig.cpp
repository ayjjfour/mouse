#include "stdafx.h"
#include "GameConfig.h"
#include "Trace.h"

using namespace tinyxml2;

CGameConfig::CGameConfig(void)
{
}


CGameConfig::~CGameConfig(void)
{
	auto_hammer_config.clear();
}

void CGameConfig::ClearCache()
{
	m_cfgCommonProperty.clear();
	m_cfgRoom.clear();
	m_AutoAttackConfig.clear();
	//2016.10.18 hl
	auto_hammer_config.clear();

	m_UserLevel.clear();
}

bool CGameConfig::LoadConfig()
{
	//CString szFilePath = AfxGetApplicationDir();   //����·��
	CString szGameConfigPath = GameConfigMgr::ConfigFile();

	tinyxml2::XMLDocument m_xml;    

	//////////////////////////////////////////////////////////////////////////
	// ���صȼ�����
	if (XML_NO_ERROR != m_xml.LoadFile(szGameConfigPath))
	{
		TRACEFALTAL("����ʧ�ܣ�δ�ҵ��ļ��ڴ�·��:%s", szGameConfigPath.GetString());
		return false;
	}

	ClearCache();
	
	XMLElement* pRoot = m_xml.RootElement();
	if (pRoot == nullptr)
	{
		TRACEFALTAL("��ȡUserLevelRoot�ڵ�ʧ��");
		return false;
	}

	XMLElement* pLevelLimitProperty = pRoot->FirstChildElement("LevelLimit");
	m_nMaxLevel = pLevelLimitProperty->IntAttribute("MaxLevel");

	for (XMLNode* pItLevelNode = pLevelLimitProperty->FirstChildElement("Level");
		pItLevelNode != nullptr;
		pItLevelNode = pItLevelNode->NextSibling())
	{
		const XMLElement*  pLevelNode = pItLevelNode->ToElement();
		CommonMonsterProperty infCommonMonsterProperty;

		int nLevel = pLevelNode->IntAttribute("ID");
		const char* szExp = pLevelNode->Attribute("EXP");
		LONGLONG llExp = std::stoll(szExp);           

		m_UserLevel.push_back(llExp);
	}

	//////////////////////////////////////////////////////////////////////////
	// ����ͳ������
	XMLElement* pStatistic = pRoot->FirstChildElement("Statistic");

	if (pStatistic == nullptr)
	{
		m_StatConfig = StatisticConfig();
	}
	else
	{
		m_StatConfig.Target = pStatistic->Attribute("Target");
		m_StatConfig.Frequency = pStatistic->IntAttribute("Frequency");
	}


	//////////////////////////////////////////////////////////////////////////
	// ������Ϸ����   
	XMLElement* pAutoAttackProperty = pRoot->FirstChildElement("AutoAttackConfig");
	m_nAutoAttackDefaultLevel = pAutoAttackProperty->IntAttribute("AutoAttackDefaultLevel");

	for (XMLNode* pItAutoAttackNode = pAutoAttackProperty->FirstChildElement("AutoAttack");
		pItAutoAttackNode != nullptr;
		pItAutoAttackNode = pItAutoAttackNode->NextSibling())
	{
		const XMLElement*  pAutoAttackNode = pItAutoAttackNode->ToElement();
		CommonMonsterProperty infCommonMonsterProperty;

		AutoAttackConfig config;
		config.AutoAttackLevel = pAutoAttackNode->IntAttribute("ID");
		const char* szActiveCost = pAutoAttackNode->Attribute("ActiveCost");
		config.ActiveCost = std::stoi(szActiveCost);

		m_AutoAttackConfig.push_back(config);
	}

	//��ȡ�Զ������� 2016.10.18 hl
	XMLElement* auto_hammer_property_ptr = pRoot->FirstChildElement("AutoHammerConfig");
	if (auto_hammer_property_ptr != nullptr)
	{
		m_nAutoAttackDefaultLevel = auto_hammer_property_ptr->IntAttribute("AutoHammerDefaultLevel");

		for (XMLNode* pItAutoHammerNode = auto_hammer_property_ptr->FirstChildElement("Hammerlv");
				pItAutoHammerNode != nullptr;
				pItAutoHammerNode = pItAutoHammerNode->NextSibling())
		{
			const XMLElement*  pAutoHammerNode = pItAutoHammerNode->ToElement();

			AutoHammerConfig config;
			config.auto_hammer_lv = pAutoHammerNode->IntAttribute("ID");
			const char* szActiveCost = pAutoHammerNode->Attribute("ActiveCost");
			config.active_cost = std::stoi(szActiveCost);

			auto_hammer_config.push_back(config);
		}
	}

	// ͨ������
	XMLElement* pCommonMonsterProperty = pRoot->FirstChildElement("CommonMonsterProperty");
	for (XMLNode* pItMonsterNode = pCommonMonsterProperty->FirstChildElement("Monster");
		pItMonsterNode != nullptr;
		pItMonsterNode = pItMonsterNode->NextSibling())
	{
		const XMLElement*  pMonsterNode = pItMonsterNode->ToElement();
		CommonMonsterProperty infCommonMonsterProperty;

		MonsterType monsterType = (MonsterType)pMonsterNode->IntAttribute("ID");
		infCommonMonsterProperty.monsterType = monsterType;
		infCommonMonsterProperty.MonsterName = pMonsterNode->Attribute("Name");
		infCommonMonsterProperty.MaxHP = pMonsterNode->IntAttribute("HP");
		infCommonMonsterProperty.fMoveSpeed = pMonsterNode->FloatAttribute("MoveSpeed");
		infCommonMonsterProperty.fWaitToEscapeTimeMin = pMonsterNode->FloatAttribute("WaitToEscapeTimeMin");
		infCommonMonsterProperty.fWaitToEscapeTimeMax = pMonsterNode->FloatAttribute("WaitToEscapeTimeMax");
		infCommonMonsterProperty.fEscapeTimeMin = pMonsterNode->FloatAttribute("EscapeTimeMin");
		infCommonMonsterProperty.fEscapeTimeMax = pMonsterNode->FloatAttribute("EscapeTimeMax");

		m_cfgCommonProperty[monsterType] = infCommonMonsterProperty;
	}


	if (m_cfgCommonProperty.size() != MonsterTypeCount-1)
	{
		TRACEFALTAL("MonsterProperty��������������������Ӧ: [%d]-[%d]", m_cfgCommonProperty.size(), MonsterTypeCount-1);
		return false;
	}


	// ÿ��Rooom������
	for (XMLElement* pRoomNode = pRoot->FirstChildElement("Room");
		pRoomNode != nullptr;
		pRoomNode = pRoomNode->NextSiblingElement("Room"))
	{
		// RoomID
		int nRoomID = pRoomNode->IntAttribute("ID");
		if (nRoomID <= 0)
		{
			TRACEFALTAL("��ȡRooot->ID�ڵ�ʧ��, RoomID=%d", nRoomID);
			continue;
		}

		RoomConfig config;

		//RoomConfig config;
		config.RoomID = nRoomID;
		config.EnterLimit = pRoomNode->IntAttribute("EnterLimit");

		XMLElement * coin_limit_config_ptr = pRoomNode->FirstChildElement("CoinLimit");
		if (coin_limit_config_ptr)
		{
			config.CoinLimit.LimitMin = coin_limit_config_ptr->IntAttribute("min");
			config.CoinLimit.LimitMax = coin_limit_config_ptr->IntAttribute("max");
		}

		//2016.10.19 hl��Ӷ�ȡ���������ļ��Ƿ����񱩹��� �������ļ�δ���ø����� ��Ĭ�ϲ�������0�� ����ֵΪ����
		XMLElement * wild_skill_property_ptr = pRoomNode->FirstChildElement("WildSkillProperty");
		if(NULL != wild_skill_property_ptr)
		{
			config.wild_skill = wild_skill_property_ptr->IntAttribute("wild_skill");
			if(config.wild_skill)
			{
				config.wild_skill_blank_time = wild_skill_property_ptr->IntAttribute("wild_skill_blank_time");
				if(config.wild_skill_blank_time <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_blank_time�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_blank_time);
					return false;
				}
				config.wild_skill_continue_time = wild_skill_property_ptr->IntAttribute("wild_skill_continue_time");
				if(config.wild_skill_continue_time <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_continue_time�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_continue_time);
					return false;
				}
				config.wild_skill_pay_rate = wild_skill_property_ptr->FloatAttribute("wild_skill_pay_rate");
				if(config.wild_skill_pay_rate <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_pay_rate�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_pay_rate);
					return false;
				}
				config.wild_skill_monster_speed = wild_skill_property_ptr->FloatAttribute("wild_skill_monster_speed");
				if(config.wild_skill_monster_speed <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_monster_speed�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_monster_speed);
					return false;
				}
				config.wild_skill_get_rate = wild_skill_property_ptr->FloatAttribute("wild_skill_get_rate");
				if(config.wild_skill_get_rate <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_get_rate�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_get_rate);
					return false;
				}
				config.wild_skill_create_monster_speed = wild_skill_property_ptr->FloatAttribute("wild_skill_create_monster_speed");
				if(config.wild_skill_create_monster_speed <= 0)
				{
					TRACEFALTAL("WildSkillProperty����wild_skill_create_monster_speed�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.wild_skill_create_monster_speed);
					return false;
				}
			}
		}
		else
		{
			TRACEFALTAL("δ�ҵ������ԵĽڵ�");
	//		return false;
		}
		//���ˢ��Ч������
		XMLElement * create_monster_speed_rate = pRoomNode->FirstChildElement("MonsterCreateSpeedRate");
		if(NULL != create_monster_speed_rate)
		{
			config.create_monster_speed_rate = create_monster_speed_rate->FloatAttribute("create_monster_speed_rate");
			if(config.create_monster_speed_rate <= 0)
			{
				TRACEFALTAL("MonsterCreateSpeedRate����create_monster_speed_rate�������ü��ʱ�䲻��С�ڵ���0: [%d]", config.create_monster_speed_rate);
				return false;
			}
		}
		else
		{
			TRACEFALTAL("δ�ҵ�MonsterCreateSpeedRate�Ľڵ�");
	//		return false;
		}
		// �������������������
		XMLElement * king_config_ptr = pRoomNode->FirstChildElement("KingConfig");
		if(NULL != king_config_ptr)
		{
			config.king_config.king_critical_rate	= king_config_ptr->IntAttribute("critical_rate");
			config.king_config.anim_mid_last		= king_config_ptr->IntAttribute("anim_mid_last");
			config.king_config.anim_end_last		= king_config_ptr->IntAttribute("anim_end_last");
		}

		// GameProperty
		XMLElement* pGameProperty = pRoomNode->FirstChildElement("GameProperty");
		config.infGameProperty.AddMonsterTime = pGameProperty->IntAttribute("AddMonsterTime");
		config.infGameProperty.AddGroupMonsterTime = pGameProperty->IntAttribute("AddGroupMonsterTime");
		config.infGameProperty.KillMonsterTime = pGameProperty->IntAttribute("KillMonsterTime");
		config.infGameProperty.ChangeScrenceTime = pGameProperty->IntAttribute("ChangeScrenceTime");
		config.infGameProperty.CheckPoolTime = pGameProperty->IntAttribute("CheckPoolTime");
		config.infGameProperty.LoadFileTime = pGameProperty->IntAttribute("LoadFileTime");
		config.infGameProperty.bEnableLog = pGameProperty->BoolAttribute("EnableLog");


		// MonsterProperty
		XMLElement* pMonsterProperty = pRoomNode->FirstChildElement("MonsterProperty");
		for (XMLNode* pItMonsterNode = pMonsterProperty->FirstChildElement("Monster");
			pItMonsterNode != nullptr;
			pItMonsterNode = pItMonsterNode->NextSibling())
		{
			XMLElement* pMonsterNode = pItMonsterNode->ToElement();
			MonsterType monsterType = (MonsterType)pMonsterNode->IntAttribute("ID");
			const CommonMonsterProperty& infCommonMonsterProperty = m_cfgCommonProperty[monsterType];  
			MonsterPropertyInfo infMonsterProperty(infCommonMonsterProperty);

			infMonsterProperty.RespawnProb = pMonsterNode->IntAttribute("RespawnProb");
			infMonsterProperty.MaxOnScreen = pMonsterNode->IntAttribute("MaxOnScreen");

			infMonsterProperty.KillRateMin = pMonsterNode->IntAttribute("KillRateMin");
			infMonsterProperty.KillRateMax = pMonsterNode->IntAttribute("KillRateMax");
			infMonsterProperty.MissRate = pMonsterNode->IntAttribute("MissRate");
			infMonsterProperty.TicketNum = pMonsterNode->IntAttribute("TicketNum");

			config.mapMonsterProperty[monsterType] = infMonsterProperty;
		}

		if (config.mapMonsterProperty.size() != MonsterTypeCount-1)
		{
			TRACEFALTAL("MonsterProperty��������������������Ӧ: [%d]-[%d]", config.mapMonsterProperty.size(), MonsterTypeCount-1);
			return false;
		}


		// GameIncoming
		XMLElement* pGameIncomingProperty = pRoomNode->FirstChildElement("GameIncoming");
		if (pGameIncomingProperty == nullptr)
		{
			TRACEFALTAL("δ�ҵ�GameIncoming�Ľڵ�");
			return false;
		}
		config.infGameIncoming.WelfarePointFactor = pGameIncomingProperty->IntAttribute("WelfarePointFactor");
		config.infGameIncoming.ExpFactor = pGameIncomingProperty->IntAttribute("ExpFactor");


		// MonsterPoolConfig
		XMLElement* pMonsterPoolProperty = pRoomNode->FirstChildElement("MonsterPoolConfig");
		if (pMonsterPoolProperty == nullptr)
		{
			TRACEFALTAL("δ�ҵ�MonsterPoolConfig�Ľڵ�");
			return false;
		}
		config.infMonsterPool.defaultSize = (WORD)pMonsterPoolProperty->IntAttribute("DefaultSize");
		config.infMonsterPool.maxSize = (WORD)pMonsterPoolProperty->IntAttribute("MaxSize");
		config.infMonsterPool.offsetSize = (WORD)pMonsterPoolProperty->IntAttribute("OffsetSize");


		// WeaponCostProperty
		XMLElement* pWeaponCostProperty = pRoomNode->FirstChildElement("WeaponCost");
		for (XMLNode* pItWeaponNode = pWeaponCostProperty->FirstChildElement("Weapon");
			pItWeaponNode != nullptr;
			pItWeaponNode = pItWeaponNode->NextSibling())
		{
			XMLElement* pWeaponProperty = pItWeaponNode->ToElement();

			WeaponCostInfo infWeaponProperty;
			infWeaponProperty.weaponID = (WeaponID)pWeaponProperty->IntAttribute("ID");
			infWeaponProperty.WeaponName = pWeaponProperty->Attribute("Name");
			infWeaponProperty.Cost = pWeaponProperty->IntAttribute("Cost");
			infWeaponProperty.MoneyLimit = pWeaponProperty->IntAttribute("MoneyLimit");

			config.mapWeaponCost[infWeaponProperty.weaponID] = infWeaponProperty;
		}

		if (config.mapWeaponCost.size() != WeaponTypeCount-1)
		{
			TRACEFALTAL("WeaponCost��������������������Ӧ: [%d]-[%d]", config.mapWeaponCost.size(), WeaponTypeCount-1);
			return false;
		}

		// WeaponCostProperty
		XMLElement* pWeaponTicketIncomingProperty = pRoomNode->FirstChildElement("WeaponTicketIncoming");
		for (XMLNode* pItWeaponTicketIncomingNode = pWeaponTicketIncomingProperty->FirstChildElement("Weapon");
			pItWeaponTicketIncomingNode != nullptr;
			pItWeaponTicketIncomingNode = pItWeaponTicketIncomingNode->NextSibling())
		{
			XMLElement* pWeaponProperty = pItWeaponTicketIncomingNode->ToElement();

			WeaponTicketIncoming infWeaponTicketIncoming;
			infWeaponTicketIncoming.weaponID = (WeaponID)pWeaponProperty->IntAttribute("ID");
			infWeaponTicketIncoming.TicketNumMin = pWeaponProperty->IntAttribute("TicketNumMin");
			infWeaponTicketIncoming.TicketNumMax = pWeaponProperty->IntAttribute("TicketNumMax");

			config.mapWeaponTicketIncoming[infWeaponTicketIncoming.weaponID] = infWeaponTicketIncoming;
		}

		if (config.mapWeaponTicketIncoming.size() != WeaponTypeCount-1)
		{
			TRACEFALTAL("WeaponTicketIncoming��������������������Ӧ: [%d]-[%d]", config.mapWeaponTicketIncoming.size(), WeaponTypeCount-1);
			return false;
		}


		// SkillCostProperty
		XMLElement* pSkillCostProperty = pRoomNode->FirstChildElement("SkillCost");
		for (XMLNode* pItSkillNode = pSkillCostProperty->FirstChildElement("Skill");
			pItSkillNode != nullptr;
			pItSkillNode = pItSkillNode->NextSibling())
		{
			XMLElement* pSkillProperty = pItSkillNode->ToElement();

			SkillCostInfo infSkillProperty;
			infSkillProperty.skillID = (SkillID)pSkillProperty->IntAttribute("ID");
			infSkillProperty.SkillName = pSkillProperty->Attribute("Name");
			infSkillProperty.BaseProb = pSkillProperty->IntAttribute("BaseProb");
			infSkillProperty.MaxProb = pSkillProperty->IntAttribute("MaxProb");
			infSkillProperty.MaxTarget = pSkillProperty->IntAttribute("MaxTarget");
			infSkillProperty.ActiveLevel = pSkillProperty->IntAttribute("ActiveLevel");
			infSkillProperty.UpgradeLevel = pSkillProperty->IntAttribute("UpgradeLevel");
			infSkillProperty.BuyCost = pSkillProperty->IntAttribute("BuyCost");

			config.mapSkillCost[infSkillProperty.skillID] = infSkillProperty;
		}

		if (config.mapSkillCost.size() != SkillTypeCount-1)
		{
			TRACEFALTAL("SkillCost��������������������Ӧ: [%d]-[%d]", config.mapSkillCost.size(), SkillTypeCount-1);
			return false;
		}


		// BroadcastInfo
		XMLElement* pBroadcastProperty = pRoomNode->FirstChildElement("Broadcast");
		if (pBroadcastProperty == nullptr)
		{
			TRACEFALTAL("δ�ҵ�BroadcastInfo�Ľڵ�");
			return false;
		}
		config.infBroadcast.ServerLimit = pBroadcastProperty->IntAttribute("ServerLimit");

		for (XMLNode* pItBroadLevelNode = pBroadcastProperty->FirstChildElement("BroadLevel");
			pItBroadLevelNode != nullptr;
			pItBroadLevelNode = pItBroadLevelNode->NextSibling())
		{
			XMLElement* pBroadLevelProperty = pItBroadLevelNode->ToElement();

			BroadLevelInfo infBroadLevel;
			infBroadLevel.Limit = pBroadLevelProperty->IntAttribute("Limit");
			infBroadLevel.Text = pBroadLevelProperty->Attribute("Text");

			config.infBroadcast.lstBroadLevelInfo.push_back(infBroadLevel);
		}

		XMLElement* pBroadKingProperty = pRoomNode->FirstChildElement("BroadKing");
		if (pBroadKingProperty) 
		{
			config.infBroadcast.lstBroadKingInfo.Limit	= pBroadKingProperty->IntAttribute("Limit");
			config.infBroadcast.lstBroadKingInfo.Text	= pBroadKingProperty->Attribute("Text");
		}

		m_cfgRoom[nRoomID] = config;
	}

	return true;
}

bool CGameConfig::ReloadConfig()
{
	return true;

	//bool isModified = false;

	//WIN32_FIND_DATA FindFileData;
	//HANDLE hFile;

	//CString szFilePath = AfxGetApplicationDir();   //����·��
	//CString szGameConfigPath = szFilePath + szGameConfigFilename;
	//hFile = FindFirstFile(szGameConfigPath, &FindFileData);

	//if (hFile == INVALID_HANDLE_VALUE) {
	//	TRACEFALTAL("δ�ҵ������ļ�:%s, ����ID=%d", szGameConfigPath, GetLastError());
	//	return false;
	//}
	//else 
	//{
	//	FILETIME ftLocal;
	//	SYSTEMTIME stLastModifyTime;
	//	if (CompareFileTime(&FindFileData.ftLastWriteTime, &m_ftLastModifyTime) > 0)
	//	{
	//		// Convert the last-write time to local time.
	//		if (FileTimeToLocalFileTime(&FindFileData.ftLastWriteTime, &ftLocal))
	//		{
	//			// Convert the local file time from UTC to system time.
	//			FileTimeToSystemTime(&ftLocal, &stLastModifyTime);

	//			// Build a string showing the date and time.
	//			TRACEDEBUG("�µ��޸�ʱ�䣺%02d/%02d/%d  %02d:%02d",
	//				stLastModifyTime.wDay, stLastModifyTime.wMonth, stLastModifyTime.wYear,
	//				stLastModifyTime.wHour, stLastModifyTime.wMinute);

	//			isModified = true;
	//		}
	//	}

	//	FindClose(hFile);
	//}

	//return isModified;
}

/// ����ָ�����������
//const RoomConfig* CGameConfig::GetRoomConfig(int nRoomID) const
//{
//    if (nRoomID <= 0)
//    {
//        LOGE("RoomID����: %d", nRoomID);
//        return nullptr;
//    }
//
//    auto it = m_cfgRoom.find(nRoomID);
//    if (it == m_cfgRoom.end())
//        return nullptr;
//
//    return &(it->second);
//}

const int CGameConfig::GetEnterLimit(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetEnterLimit, nRoomID����: %d", nRoomID);
		return -1;
	}

	unsigned long long Min = itRoom->second.CoinLimit.LimitMin;

	return Min;
}

const unsigned long long CGameConfig::GetEnterLimitMin(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetEnterLimit, nRoomID����: %d", nRoomID);
		return -1;
	}

	unsigned long long Min = itRoom->second.CoinLimit.LimitMin;

	return Min;
}

const unsigned long long CGameConfig::GetEnterLimitMax(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetEnterLimit, nRoomID����: %d", nRoomID);
		return -1;
	}

	unsigned long long Max = itRoom->second.CoinLimit.LimitMax;

	return Max;
}

const MonsterPoolConfig* CGameConfig::GetMonsterPoolConfig(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("MonsterPoolConfig, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	return &(itRoom->second.infMonsterPool);
}

const CommonMonsterProperty* CGameConfig::GetMonsterCommonProperty(MonsterType monsterType) const
{
	if (monsterType >= MonsterTypeCount)
	{
		TRACEFALTAL("GetMonsterCommonProperty, monsterType����: %d", monsterType);
		return nullptr;
	}

	auto it = m_cfgCommonProperty.find(monsterType);
	if (it == m_cfgCommonProperty.end())
	{
		TRACEFALTAL("GetMonsterCommonProperty, monsterType����: %d", monsterType);
		return nullptr;
	}

	return &(it->second);
}

const MonsterPropertyInfo* CGameConfig::GetMonsterPropertyInfo(int nRoomID, MonsterType monsterType) const
{
	if (monsterType >= MonsterTypeCount)
	{
		TRACEFALTAL("GetMonsterProperty, monsterType����: %d", monsterType);
		return nullptr;
	}

	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetMonsterProperty, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	const RoomConfig& roomConfig = itRoom->second;
	auto itMonster = roomConfig.mapMonsterProperty.find(monsterType);
	if (itMonster == roomConfig.mapMonsterProperty.end())
	{
		TRACEFALTAL("GetMonsterProperty, monsterType����: %d", monsterType);
		return nullptr;
	}
	return &(itMonster->second);
}

const GamePropertyInfo* CGameConfig::GetGamePropertyInfo(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GamePropertyInfo, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	return &(itRoom->second.infGameProperty);   
}

const RoomConfig * CGameConfig::get_room_config_int_info(const int room_id) const
{
	auto it_room = m_cfgRoom.find(room_id);
	if(it_room == m_cfgRoom.end())
	{
		TRACEFALTAL("GamePropertyInfo, room_id error : %d",room_id);
		return nullptr;
	}

	return &(it_room->second);
}
const GameIncomingInfo* CGameConfig::GetGameIncomingInfo(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GameIncomingInfo, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	return &(itRoom->second.infGameIncoming);
}

const BroadcastInfo* CGameConfig::GetBroadcastInfo(int nRoomID) const
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("BroadcastInfo, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	return &(itRoom->second.infBroadcast);
}

const WeaponCostInfo* CGameConfig::GetWeaponCostInfo(int nRoomID, WeaponID weaponID) const
{
	if (weaponID >= WeaponTypeCount)
	{
		TRACEFALTAL("GetWeaponCostInfo, weaponID����: %d", weaponID);
		return nullptr;
	}

	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetWeaponCostInfo, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	const RoomConfig& roomConfig = itRoom->second;
	auto itWeaponCost = roomConfig.mapWeaponCost.find(weaponID);
	if (itWeaponCost == roomConfig.mapWeaponCost.end())
	{
		TRACEFALTAL("GetWeaponCostInfo, ���ô���weaponID: %d", weaponID);
		return nullptr;
	}
	return &(itWeaponCost->second);
}

const WeaponTicketIncoming* CGameConfig::GetWeaponTicketIncoming(int nRoomID, WeaponID weaponID) const
{
	if (weaponID >= WeaponTypeCount)
	{
		TRACEFALTAL("GetWeaponTicketIncoming, weaponID����: %d", weaponID);
		return nullptr;
	}

	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetWeaponTicketIncoming, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	const RoomConfig& roomConfig = itRoom->second;
	auto itWeaponCost = roomConfig.mapWeaponTicketIncoming.find(weaponID);
	if (itWeaponCost == roomConfig.mapWeaponTicketIncoming.end())
	{
		TRACEFALTAL("GetWeaponTicketIncoming, ���ô���weaponID: %d", weaponID);
		return nullptr;
	}
	return &(itWeaponCost->second);
}

const SkillCostInfo* CGameConfig::GetSkillCostInfo(int nRoomID, SkillID skillID) const
{
	if (skillID >= SkillTypeCount)
	{
		TRACEFALTAL("GetSkillCostInfo, skillID����: %d", skillID);
		return nullptr;
	}

	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetSkillCostInfo, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	const RoomConfig& roomConfig = itRoom->second;
	auto itSkillCost = roomConfig.mapSkillCost.find(skillID);
	if (itSkillCost == roomConfig.mapSkillCost.end())
	{
		TRACEFALTAL("GetSkillCostInfo, ���ô���skillID����:", skillID);
		return nullptr;
	}
	return &(itSkillCost->second);
}

const StatisticConfig* CGameConfig::GetStatisticConfig() const
{
	return &m_StatConfig;
}

const int CGameConfig::GetAutoAttackDefaultLevel() const
{
	return (m_nAutoAttackDefaultLevel < 3)? 3 : m_nAutoAttackDefaultLevel;
}

//2016.10.18 hl��ȡ�Զ���Ĭ�ϵȼ�
const int CGameConfig::get_auto_hammer_default_lv() const
{
	return (auto_hammer_default_lv < 3)? 3 : auto_hammer_default_lv;
}
//2016.10.18 hl  ��ȡ�Զ��������ļ�

const AutoHammerConfig * CGameConfig::get_auto_hammer_info(int hammer_lv) const
{
	if (hammer_lv <= 0 || hammer_lv > auto_hammer_config.size())
	{
		TRACEFALTAL("GetAutoHammerInfo, nAutoHammerLevel����: %d", hammer_lv);
		return nullptr;
	}

	return &(auto_hammer_config[hammer_lv-1]);
}
const AutoAttackConfig* CGameConfig::GetAutoAttackInfo(int nAutoAttackLevel) const
{
	if (nAutoAttackLevel <= 0 || nAutoAttackLevel > m_AutoAttackConfig.size())
	{
		TRACEFALTAL("GetAutoAttackInfo, nAutoAttackLevel����: %d", nAutoAttackLevel);
		return nullptr;
	}

	return &(m_AutoAttackConfig[nAutoAttackLevel-1]);
}

const KingConfig* CGameConfig::GetKingConfig(int nRoomID)
{
	auto itRoom = m_cfgRoom.find(nRoomID);
	if (itRoom == m_cfgRoom.end())
	{
		TRACEFALTAL("GetEnterLimit, nRoomID����: %d", nRoomID);
		return nullptr;
	}

	return &(itRoom->second.king_config);
}

int CGameConfig::CalcLevel(LONGLONG curExp)
{
	LONGLONG nMaxExp = m_UserLevel[m_nMaxLevel];
	if (curExp >= nMaxExp)
	{
		return m_nMaxLevel;
	}

	for (auto it = m_UserLevel.begin(); it != m_UserLevel.end(); ++it)
	{
		if (curExp < *it)
			return (it - m_UserLevel.begin() - 1);
	}

	return 0;
}

bool CGameConfig::AddExp(int& curLevel, LONGLONG& curExp, LONGLONG addExp)
{
	if (curLevel < 0 || curLevel >= m_UserLevel.size())
	{
		TRACEFALTAL("AddExp, curLevel����: %d", curLevel);
		return false;
	}

	LONGLONG nMaxExp = m_UserLevel[m_nMaxLevel];
	curExp = min(INT64_MAX-54775807, curExp + addExp);   // ע�⣺ �����ͻ�Ƽ���ľ������ޣ������𲻻ᳬ������

	// �ﵽ���һ��
	if (curLevel >= m_UserLevel.size() - 1)
	{
		return false;
	}

	bool bHit = false;

	while (true)
	{
		LONGLONG nextLvlExp = m_UserLevel[curLevel+1];
		if (curExp >= nextLvlExp)
		{
			curLevel++;
			bHit = true;
		}
		else
		{
			break;
		}
	}

	return bHit;
}

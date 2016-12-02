#pragma once
#include "tinyxml2.h"
#include "Common.h"
#include <map>
#include <list>
#include <vector>

/// ��������
enum MonsterType
{
	mtNone,                      // �����ͣ�ռλ��
	mtMoleBoSi,                  // ��˹���� 1
	mtMoleDaiMeng,               // ���ȵ��� 2
	mtMoleFeiXing,               // ���е��� 3
	mtMoleHuShi,                 // ��ʿ���� 4
	mtMoleKuangGong,             // �󹤵��� 5
	mtMoleKing,                  // ����     6
	mtSpider,                    // ֩��     7
	mtFly,                       // ��Ӭ     8
	mtFlyGoblin,                 // ����С�� 9
	MonsterTypeCount
};

#define mtMonsterTypeFirst mtMoleBoSi // �������������

/// ��������
enum WeaponID
{
	Hammer_None,
	Hammer_Wooden,               // ľ��    1
	Hammer_Stone,                // ʯ��    2
	Hammer_BlackIron,            // ����    3
	Hammer_Silver,               // ����    4
	Hammer_Golden,               // ��    5
	WeaponTypeCount
};

/// ��������
enum SkillID
{
	Skill_None,                  
	Skill_LianSuoShanDian,       // ��������  1
	Skill_HengSaoQianJun,        // ��ɨǧ��  2
	Skill_GaoShanLiuShui,        // ��ɽ��ˮ  3
	Skill_LiuXingHuoYu,          // ���ǻ���  4
	//Skill_DiMaoTuCi,           // ��éͻ��  5
	SkillTypeCount
};

/// ��Ϸ�趨�������
struct GamePropertyInfo
{
	int AddMonsterTime;         // ��ֻˢ�ּ�������룩
	int AddGroupMonsterTime;    // �й���ˢ��Ⱥ�ּ�������룩
	int KillMonsterTime;        // ��ʱ���������룩
	int ChangeScrenceTime;      // �л�������ʱ��(����)
	int CheckPoolTime;          // ��齱�����(��)  ��Ҫ����д��־��
	int LoadFileTime;           // ˢ�������ļ�ʱ��
	bool bEnableLog;

	GamePropertyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// �������,��Ϊͨ�����Ժͷ����������
struct CommonMonsterProperty
{
	MonsterType monsterType;     // ������1 - 9
	std::string MonsterName;     // ����
	int MaxHP;                   // Ѫ�������԰�����
	float fMoveSpeed;            // �ƶ��ٶ�
	float fWaitToEscapeTimeMin;  // ���ֵ�����ʱ�䣬��
	float fWaitToEscapeTimeMax;  // ���ֵ�����ʱ�䣬��
	float fEscapeTimeMin;        // ���ܵ�����ʱ�䣬��
	float fEscapeTimeMax;        // ���ܵ�����ʱ�䣬��

	CommonMonsterProperty()
	{
		monsterType = mtNone;
		MaxHP = 0;
		fMoveSpeed = 0.0f;
		fWaitToEscapeTimeMin = 0.0f;
		fEscapeTimeMin = 0.0f;
		fWaitToEscapeTimeMax = 0.0f;
		fEscapeTimeMax = 0.0f;
	}
};

// ȫ����
struct MonsterPropertyInfo : CommonMonsterProperty
{
	int RespawnProb;            // ����ˢ�»��ʣ�ǧ�ֱȣ� �����ڲ�ͬ���������ò�ͬ��ˢ�¼���
	int MaxOnScreen;            // ���ͬ������

	// ��ɱ�ʣ�  R[]��ʾ��ĳ��ֵ��Χ�����һ��������1�������������Ϊ�õ�������Ļ�ɱ��
	int KillRateMin;            // KillRate ��ɱ��
	int KillRateMax;            // KillRate ��ɱ��
	int MissRate;               // ������ 
	int TicketNum;             // �ɹ���ɱ������ܻ�õ���Ʊ��

	MonsterPropertyInfo()
	{
		monsterType = mtNone;
		MaxHP = 0;
		fMoveSpeed = 0.0f;
		fWaitToEscapeTimeMin = 0.0f;
		fEscapeTimeMin = 0.0f;
		fWaitToEscapeTimeMax = 0.0f;
		fEscapeTimeMax = 0.0f;

		RespawnProb = 0;
		MaxOnScreen = 0;

		KillRateMin = 1;
		KillRateMax = 1;
		MissRate = 0;
		TicketNum = 0;
	}

	MonsterPropertyInfo(const CommonMonsterProperty& commonProperty)
	{
		monsterType = commonProperty.monsterType;
		MonsterName = commonProperty.MonsterName;
		MaxHP = commonProperty.MaxHP;
		fMoveSpeed = commonProperty.fMoveSpeed;
		fWaitToEscapeTimeMin = commonProperty.fWaitToEscapeTimeMin;
		fWaitToEscapeTimeMax = commonProperty.fWaitToEscapeTimeMax;
		fEscapeTimeMin = commonProperty.fEscapeTimeMin;
		fEscapeTimeMax = commonProperty.fEscapeTimeMax;

		RespawnProb = 0;
		MaxOnScreen = 0;

		KillRateMin = 1;
		KillRateMax = 1;
		MissRate = 0;
	}
};

/// ��Ϸ�������
struct GameIncomingInfo
{
	int WelfarePointFactor;             // ��������ϵ��
	int ExpFactor;                      // ����ϵ��

	GameIncomingInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// ��������
struct WeaponCostInfo
{
	WeaponID weaponID;
	std::string WeaponName;
	int Cost;                   // ÿ�λ�������
	int MoneyLimit;             // Я���˴��Ľ�ҳ�������Cost*��ֵ��

	WeaponCostInfo()
	{
		weaponID = Hammer_Wooden;
		Cost = 0;
		MoneyLimit = 0;
	}
};

struct WeaponTicketIncoming
{
	WeaponID weaponID;
	int TicketNumMin;
	int TicketNumMax;

	WeaponTicketIncoming()
	{
		weaponID = Hammer_Wooden;
		TicketNumMin = 0;
		TicketNumMax = 0;
	}
};

/// ���ܻ���
struct SkillCostInfo
{
	SkillID skillID;
	std::string SkillName;
	int BaseProb;               // ������������
	int MaxProb;                // �����������ޣ��ﵽ����ȼ���ÿ��2������1%ֱ�����ޣ�
	int MaxTarget;              // ��������������Ϊ0��ʾ�������ޣ�
	int ActiveLevel;            // Ĭ�ϼ���ȼ�
	int UpgradeLevel;           // ÿN��������
	int BuyCost;                // ��ǰ�������Ľ��

	SkillCostInfo()
	{
		skillID = SkillTypeCount;
		BaseProb = 0;
		MaxProb = 0;
		MaxTarget = 0;
		ActiveLevel = 0;
		UpgradeLevel = 0;
		BuyCost = 0;
	}
};

// ������������
struct MonsterPoolConfig
{
	WORD defaultSize;    // �����Ĭ��ֵ          
	WORD maxSize;        // �������������Ϊ0����ʾ���޴�С
	WORD offsetSize;     // ÿ������Ĵ�С  

	MonsterPoolConfig()
	{
		defaultSize = 0;
		maxSize = 0;
		offsetSize = 0;
	}
};

struct BroadLevelInfo
{
	BroadLevelInfo()
	{
		Limit = -1;
		Text = "";
	}

	unsigned int	Limit;
	std::string		Text;
};

struct BroadcastInfo
{
	int         ServerLimit;
	std::list<BroadLevelInfo> lstBroadLevelInfo;
	BroadLevelInfo	lstBroadKingInfo;
};

struct AutoAttackConfig
{
	int         AutoAttackLevel;
	int         ActiveCost;

	AutoAttackConfig()
	{
		AutoAttackLevel = 0;
		ActiveCost = 0;
	}
};

struct AutoHammerConfig
{
	int   auto_hammer_lv;
	int   active_cost;

	AutoHammerConfig():auto_hammer_lv(0),
						active_cost(0)
	{

	}
};

struct CoinLimitConfig
{
	CoinLimitConfig()
	{
		LimitMin = 10;
		LimitMax = 2100000000;
	}

	unsigned long long LimitMin;
	unsigned long long LimitMax;
};

struct KingConfig
{
	KingConfig()
	{
		king_critical_rate = -1;
		anim_mid_last = 500;		// 500����
		anim_end_last = 500;		// 500����
	}

	unsigned int king_critical_rate;
	unsigned int anim_mid_last;
	unsigned int anim_end_last;
};

// ÿ��Rooom������
struct RoomConfig
{
	int                                         RoomID;
	int                                         EnterLimit;             // ����������
	CoinLimitConfig								CoinLimit;				// ������ƣ��������޺����ޣ�
	GamePropertyInfo                            infGameProperty;
	GameIncomingInfo                            infGameIncoming;
	MonsterPoolConfig                           infMonsterPool;
	BroadcastInfo                               infBroadcast;
	std::map<MonsterType, MonsterPropertyInfo>  mapMonsterProperty;
	std::map<WeaponID, WeaponCostInfo>          mapWeaponCost;
	std::map<WeaponID, WeaponTicketIncoming>    mapWeaponTicketIncoming;
	std::map<SkillID, SkillCostInfo>            mapSkillCost;
	int                                        wild_skill;//�Ƿ��п񱩹���  0Ϊ������  ����Ϊ���� Ĭ�ϲ����� 2016.10.19 hl
	int                                        wild_skill_blank_time; //�񱩹���Ĭ��ʱ����  Ĭ��2����
	int                                        wild_skill_continue_time;//�񱩼��ܳ���ʱ��    Ĭ��10��
	float                                      wild_skill_pay_rate;//��״̬����
	float                                      wild_skill_monster_speed; //��״̬ˢ�� ���ﶯ������
	float                                      wild_skill_get_rate;//��״̬��һ�ȡ����
	float                                      wild_skill_create_monster_speed;//��ˢ���ٶ�
	float                                      create_monster_speed_rate;//ˢ����������
	KingConfig									king_config; //�������������
	RoomConfig():wild_skill(0),
		wild_skill_blank_time(2),
		wild_skill_continue_time(10),
		wild_skill_pay_rate(1.0),
		wild_skill_monster_speed(1.0),
		wild_skill_get_rate(1.0),
		wild_skill_create_monster_speed(1.0),
		create_monster_speed_rate(1.0)
	{
		RoomID = 0;
		EnterLimit = 0;
	}
};

struct StatisticConfig
{
	std::string Target;
	DWORD Frequency;

	StatisticConfig()
	{
		Target = "";
		Frequency = 0;
	}
};

class CGameConfig
{
public:
	CGameConfig();
	~CGameConfig();

	// ������������
	bool LoadConfig();
	// ���ж������ļ��Ƿ����޸ģ����޸��ټ��أ� ����true��ʾ���޸�
	bool ReloadConfig();    

	// ����ָ����������ã����û�仯�����ټ����ļ������Ǵӻ����ж�ȡ
	//const RoomConfig* GetRoomConfig(int nRoomID) const;

	const MonsterPoolConfig* GetMonsterPoolConfig(int nRoomID) const;
	const CommonMonsterProperty* GetMonsterCommonProperty(MonsterType monsterType) const;
	const MonsterPropertyInfo* GetMonsterPropertyInfo(int nRoomID, MonsterType monsterType) const;
	const GamePropertyInfo* GetGamePropertyInfo(int nRoomID) const;
	const GameIncomingInfo* GetGameIncomingInfo(int nRoomID) const;
	const BroadcastInfo* GetBroadcastInfo(int nRoomID) const;
	const WeaponCostInfo* GetWeaponCostInfo(int nRoomID, WeaponID weaponID) const;
	const WeaponTicketIncoming* GetWeaponTicketIncoming(int nRoomID, WeaponID weaponID) const;
	const SkillCostInfo* GetSkillCostInfo(int nRoomID, SkillID skillID) const;
	const int GetEnterLimit(int nRoomID) const;
	const unsigned long long GetEnterLimitMin(int nRoomID) const;
	const unsigned long long GetEnterLimitMax(int nRoomID) const;
	const KingConfig* GetKingConfig(int nRoomID);

	//const MonsterPropertyInfo& GetMonsterProperty(MonsterType monsterType);
	//const RoomConfig* GetRoomConfig(int RoomID) const;

	const int GetAutoAttackDefaultLevel() const;
	const AutoAttackConfig* GetAutoAttackInfo(int nAutoAttackLevel) const;

	const int get_auto_hammer_default_lv() const;//2016.10.17 hl
	const AutoHammerConfig * get_auto_hammer_info(int hammer_lv) const;

	const RoomConfig * get_room_config_int_info(const int room_id) const; 

	const StatisticConfig* GetStatisticConfig() const;

	// ���ؼ���
	int CalcLevel(LONGLONG curExp);
	// �����Ƿ����������ܻ��޸ĵ�ǰ����;���
	bool AddExp(int& curLevel, LONGLONG& curExp, LONGLONG addExp);

private:
	void ClearCache();  // ����Ѽ��صĻ���

private:
	//tinyxml2::XMLDocument m_xml; 
	std::map<int, CommonMonsterProperty> m_cfgCommonProperty; //����TypeΪ����
	std::map<int, RoomConfig> m_cfgRoom; //RooomIDΪ����

	int m_nAutoAttackDefaultLevel;        // Ĭ���Զ��һ����ŵȼ�
	std::vector<AutoAttackConfig> m_AutoAttackConfig;

	int auto_hammer_default_lv;			//Ĭ���Զ����ȼ�
	std::vector<AutoHammerConfig> auto_hammer_config; //�����Զ����ȼ����ѱ� 

	int m_nMaxLevel;              // ���ŵ����ȼ���
	std::vector<LONGLONG> m_UserLevel; // �ȼ��ֶ�

	StatisticConfig m_StatConfig;
};

class GameConfigMgr
{
public:
	static GameConfigMgr& getInstance();

	static bool Initialize(int iRoomID);

	static const CString ConfigFile();

private:
	GameConfigMgr();
	~GameConfigMgr();

public:
	CGameConfig& Current();

	bool ReloadConfig();

private:
	int m_iRoomID;

	//˫������ƣ���֤�ⲿ���úͶ��̵߳��õĿɿ���
	CGameConfig m_Config1;
	CGameConfig m_Config2;

	//˫�������ָ��
	CGameConfig* m_pCurConfig;

private:
	CRITICAL_SECTION m_Lock;
	HANDLE m_hThread;
	bool m_bStop;

	CString szMD5;

	void StartThread();
	void StopThread();

	static void _LoadConfigProc(void* apParam);
};
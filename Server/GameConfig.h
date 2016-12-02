#pragma once
#include "tinyxml2.h"
#include "Common.h"
#include <map>
#include <list>
#include <vector>

/// 怪物类型
enum MonsterType
{
	mtNone,                      // 空类型，占位用
	mtMoleBoSi,                  // 波斯地鼠 1
	mtMoleDaiMeng,               // 呆萌地鼠 2
	mtMoleFeiXing,               // 飞行地鼠 3
	mtMoleHuShi,                 // 护士地鼠 4
	mtMoleKuangGong,             // 矿工地鼠 5
	mtMoleKing,                  // 鼠王     6
	mtSpider,                    // 蜘蛛     7
	mtFly,                       // 苍蝇     8
	mtFlyGoblin,                 // 盗宝小妖 9
	MonsterTypeCount
};

#define mtMonsterTypeFirst mtMoleBoSi // 重命名方便遍历

/// 武器类型
enum WeaponID
{
	Hammer_None,
	Hammer_Wooden,               // 木锤    1
	Hammer_Stone,                // 石锤    2
	Hammer_BlackIron,            // 铁锤    3
	Hammer_Silver,               // 银锤    4
	Hammer_Golden,               // 金锤    5
	WeaponTypeCount
};

/// 技能类型
enum SkillID
{
	Skill_None,                  
	Skill_LianSuoShanDian,       // 连锁闪电  1
	Skill_HengSaoQianJun,        // 横扫千军  2
	Skill_GaoShanLiuShui,        // 高山流水  3
	Skill_LiuXingHuoYu,          // 流星火雨  4
	//Skill_DiMaoTuCi,           // 地茅突刺  5
	SkillTypeCount
};

/// 游戏设定相关配置
struct GamePropertyInfo
{
	int AddMonsterTime;         // 单只刷怪间隔（毫秒）
	int AddGroupMonsterTime;    // 有规则刷新群怪间隔（毫秒）
	int KillMonsterTime;        // 定时清理怪物（毫秒）
	int ChangeScrenceTime;      // 切换场景的时间(分钟)
	int CheckPoolTime;          // 检查奖池情况(秒)  主要是在写日志？
	int LoadFileTime;           // 刷新配置文件时间
	bool bEnableLog;

	GamePropertyInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 怪物相关,分为通用属性和房间相关属性
struct CommonMonsterProperty
{
	MonsterType monsterType;     // 索引，1 - 9
	std::string MonsterName;     // 名字
	int MaxHP;                   // 血量，可以挨几下
	float fMoveSpeed;            // 移动速度
	float fWaitToEscapeTimeMin;  // 出现到逃跑时间，秒
	float fWaitToEscapeTimeMax;  // 出现到逃跑时间，秒
	float fEscapeTimeMin;        // 逃跑到销毁时间，秒
	float fEscapeTimeMax;        // 逃跑到销毁时间，秒

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

// 全属性
struct MonsterPropertyInfo : CommonMonsterProperty
{
	int RespawnProb;            // 怪物刷新机率，千分比， 可以在不同场景下设置不同的刷新几率
	int MaxOnScreen;            // 最大同屏数量

	// 击杀率：  R[]表示在某数值范围内随机一个倍数，1除以这个倍数即为该地鼠或怪物的击杀率
	int KillRateMin;            // KillRate 击杀率
	int KillRateMax;            // KillRate 击杀率
	int MissRate;               // 免伤率 
	int TicketNum;             // 成功击杀后，最大能获得的礼票数

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

/// 游戏收益相关
struct GameIncomingInfo
{
	int WelfarePointFactor;             // 福利产出系数
	int ExpFactor;                      // 经验系数

	GameIncomingInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 武器花费
struct WeaponCostInfo
{
	WeaponID weaponID;
	std::string WeaponName;
	int Cost;                   // 每次击晕消耗
	int MoneyLimit;             // 携带此锤的金币持有量（Cost*此值）

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

/// 技能花费
struct SkillCostInfo
{
	SkillID skillID;
	std::string SkillName;
	int BaseProb;               // 基础触发几率
	int MaxProb;                // 触发几率上限（达到激活等级后，每升2级提升1%直到上限）
	int MaxTarget;              // 最大命中数（如果为0表示不限上限）
	int ActiveLevel;            // 默认激活等级
	int UpgradeLevel;           // 每N级可升星
	int BuyCost;                // 提前激活消耗金币

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

// 生物对象池配置
struct MonsterPoolConfig
{
	WORD defaultSize;    // 对象池默认值          
	WORD maxSize;        // 对象池最大，如果设为0，表示不限大小
	WORD offsetSize;     // 每次扩充的大小  

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
		anim_mid_last = 500;		// 500毫秒
		anim_end_last = 500;		// 500毫秒
	}

	unsigned int king_critical_rate;
	unsigned int anim_mid_last;
	unsigned int anim_end_last;
};

// 每个Rooom的设置
struct RoomConfig
{
	int                                         RoomID;
	int                                         EnterLimit;             // 进入金币限制
	CoinLimitConfig								CoinLimit;				// 金币限制（包括上限和下限）
	GamePropertyInfo                            infGameProperty;
	GameIncomingInfo                            infGameIncoming;
	MonsterPoolConfig                           infMonsterPool;
	BroadcastInfo                               infBroadcast;
	std::map<MonsterType, MonsterPropertyInfo>  mapMonsterProperty;
	std::map<WeaponID, WeaponCostInfo>          mapWeaponCost;
	std::map<WeaponID, WeaponTicketIncoming>    mapWeaponTicketIncoming;
	std::map<SkillID, SkillCostInfo>            mapSkillCost;
	int                                        wild_skill;//是否有狂暴功能  0为不开启  其他为开启 默认不开启 2016.10.19 hl
	int                                        wild_skill_blank_time; //狂暴功能默认时间间隔  默认2分钟
	int                                        wild_skill_continue_time;//狂暴技能持续时间    默认10秒
	float                                      wild_skill_pay_rate;//狂暴状态赔率
	float                                      wild_skill_monster_speed; //狂暴状态刷怪 怪物动作加速
	float                                      wild_skill_get_rate;//狂暴状态金币获取倍率
	float                                      wild_skill_create_monster_speed;//狂暴刷怪速度
	float                                      create_monster_speed_rate;//刷怪速率配置
	KingConfig									king_config; //鼠王暴相关配置
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

	// 加载所有配置
	bool LoadConfig();
	// 会判断配置文件是否有修改，有修改再加载， 返回true表示有修改
	bool ReloadConfig();    

	// 加载指定房间的配置，如果没变化，不再加载文件，而是从缓存中读取
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

	// 返回级别
	int CalcLevel(LONGLONG curExp);
	// 返回是否升级，可能会修改当前级别和经验
	bool AddExp(int& curLevel, LONGLONG& curExp, LONGLONG addExp);

private:
	void ClearCache();  // 清除已加载的缓存

private:
	//tinyxml2::XMLDocument m_xml; 
	std::map<int, CommonMonsterProperty> m_cfgCommonProperty; //怪物Type为索引
	std::map<int, RoomConfig> m_cfgRoom; //RooomID为索引

	int m_nAutoAttackDefaultLevel;        // 默认自动挂机开放等级
	std::vector<AutoAttackConfig> m_AutoAttackConfig;

	int auto_hammer_default_lv;			//默认自动锤等级
	std::vector<AutoHammerConfig> auto_hammer_config; //激活自动锤等级消费表 

	int m_nMaxLevel;              // 开放的最大等级数
	std::vector<LONGLONG> m_UserLevel; // 等级分段

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

	//双缓存机制，保证外部调用和多线程调用的可靠性
	CGameConfig m_Config1;
	CGameConfig m_Config2;

	//双缓存核心指针
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
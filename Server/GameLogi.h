// GameLogi.h: interface for the CGameServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLESERVER_H__B9B928D4_0E4A_42E0_956E_A0EA13AC458B__INCLUDED_)
#define AFX_SAMPLESERVER_H__B9B928D4_0E4A_42E0_956E_A0EA13AC458B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "site_i.h"
#include "..\transStruct.h"
#include "LogicalHelper.h"
#include "CMD_Game.h"
#include "GameConfig.h"
#include "Log.h"
#include "Statistic.h"
#include "StatisticMgr.h"

// * /房间名
struct tagServerName
{
	char szServerName[MAX_NAME_LEN];//32 == MAX_NAME_LEN
};

//系统收入、系统支出、飞行子弹数、亏损池炮数总额、亏损池平均数、免伤率功能提取金币数。

// 统计当前房间所有桌子的收支
struct SRoomTotal
{
	stUserMoney  llSysIncome;						//系统收入
	stUserMoney  llSysExpense;						//系统支出
	LONGLONG     llSumAvoidHitGetScore;			    //免伤率功能金币数
	LONGLONG     llBuySkillCost;					//购买技能、挂机等功能消耗（整场游戏消费）
	LONGLONG     llBuySkillReturn;					//购买技能返还（挂机不返还）

	SRoomTotal()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 用户统计数据
struct PlayerStatisticsData
{
	stUserMoney	llUserCost;		        //玩家本次消费（整场游戏消费，不包括购买消耗）
	stUserMoney	llUserIncome;           //玩家本次收入
	stUserMoney	llIntervalIncome;		//系统时间段收入（PreWriteScore间隔）
	stUserMoney	llIntervalExpense;		//系统时间段支出
	LONGLONG	llAvoidHitSystemGetScore;   //免伤功能系统提取金币数 

	LONGLONG    llBuySkillCost;         //购买技能、挂机等功能消耗（整场游戏消费）
	LONGLONG    llBuySkillReturn;       //购买技能返还（挂机不返还）

	stUserMoney	llUserCarryScore;		//玩家进入时携带的金币

	DWORD		nUserEnterRoomTick;		//玩家进入房间打tick,目的在于记录玩家退出房间时是否超过3分钟
	DWORD		nUserExitRoomTick;		//玩家退出房间tick，用处同上

	DWORD       dwLastHitTime;            // 上次打击时间


	//此数据由客户端发送，所以统计在角色上
	int			m_nStatictisHitCounts;                          //攻击次数
	int			m_nStatictisSkillTriggerCounts[SKILL_COUNT + 1];	//各技能触发次数

	bool		bClientReady;

	PlayerStatisticsData()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}
};


enum enSkillBuyStatus
{
	enSBS_None,     // 未提前购买
	enSBS_Buyed,    // 已提前购买
	enSBS_Payed,    // 提前购买且已返还
};

// SY::SkillStatus
//enum enSkillActiveStatus
//{
//    enSAS_Unactived,     // 未激活
//    enSAS_Actived,       // 已激活
//    enSAS_Disabled,      // 已激活但禁用
//};

// 玩家交互数据（自定义数据）
struct UserInteractData
{
	ULONGLONG llMoleNormalKillNum;        // uint 普通地鼠
	ULONGLONG llMoleKuangGongKillNum;     // uint 矿工地鼠
	ULONGLONG llFlyGoblinKillNum;         // uint 盗宝树懒
	ULONGLONG llSpiderKillNum;            // uint 蜘蛛
	ULONGLONG llFlyKillNum;               // uint 苍蝇
	ULONGLONG llMoleKingKillNum;          // uint 鼠王

	// 以下数据每一位表示一个技能的值
	unsigned int nSkillBuyStatus;      // enSkillBuyStatus    0未购买，1已购买， 2已返还
	unsigned int nSkillActiveStatus;   // enSkillActiveStatus 0未激活，1已激活， 2玩家已禁用（此状态暂时不保存在服务器）
	unsigned int nSkillLevel;          // byte 0-4，最大支持0-15

	unsigned char nAutoAttackLevel;    // unsigned byte 0-10 自动挂机开放等级

	unsigned char auto_hammer_lv;      //unsigned byte  0-10 自动锤开放等级


//	unsigned char auto_hammer_active_status; // 0未激活 1已激活  2已禁用

	UserInteractData()
	{
		memset(this, 0, sizeof(*this));
	}

	const int get_auto_hammer_lv() const
	{

	}
	enSkillBuyStatus GetSkillBuyStatus(SkillID nSkillID) const
	{
		if (nSkillID == Skill_None) return enSBS_None;
		return (enSkillBuyStatus)((nSkillBuyStatus >> ((nSkillID - 1) * 4)) & 0xF);
	}

	void SetSkillBuyStatus(SkillID nSkillID, enSkillBuyStatus status)
	{
		if (nSkillID == Skill_None) return ;
		enSkillBuyStatus oldStatus = GetSkillBuyStatus(nSkillID);
		nSkillBuyStatus += (status - oldStatus) << ((nSkillID - 1) * 4);
	}


	SY::SkillStatus GetSkillActiveStatus(SkillID nSkillID) const
	{
		if (nSkillID == Skill_None) return SY::SkillStatus::InActive;
		return (SY::SkillStatus)((nSkillActiveStatus >> ((nSkillID - 1) * 4)) & 0xF);
	}

	void SetSkillActiveStatus(SkillID nSkillID, SY::SkillStatus status)
	{
		if (nSkillID == Skill_None) return ;
		SY::SkillStatus oldStatus = GetSkillActiveStatus(nSkillID);
		nSkillActiveStatus += (status - oldStatus) << ((nSkillID - 1) * 4);
	}


	unsigned char GetSkillLevel(SkillID nSkillID) const
	{
		if (nSkillID == Skill_None) return 0;
		return (nSkillLevel >> ((nSkillID - 1) * 4)) & 0xF;
	}

	void SetSkillLevel(SkillID nSkillID, unsigned char level)
	{
		if (nSkillID == Skill_None) return ;
		unsigned char oldLevel = GetSkillLevel(nSkillID);
		nSkillLevel += (level - oldLevel) << ((nSkillID - 1) * 4);
	}
};

struct stIncoming
{
	LONGLONG llCoin;
	LONGLONG llTicket;
	LONGLONG llWeapon;
	long lMiss;

	stIncoming()
	{
		ZeroMemory(this, sizeof(*this));
	}
};


//-----------------------------------------------------------------
//初始默认的枪数概率
static int G_iMustShotProbability[5][3] = {{0,0,100},{0,30,70},{0,40,60},{60,40,0},{80,20,0}};
//狂暴状态标识 2016.10.20 hl 
enum WILD_SKILL_STATUS
{
	WILD_SKILL_STATUS_BLANK = 1, //狂暴状态准备期间 1分钟间隔
	WILD_SKILL_STATUS_WILL_START,  //最后30秒进入狂暴状态
	WILD_SKILL_STATUS_START,       //进入狂暴状态
	WILD_SKILL_STATUS_WILL_END,    //狂暴即将结束（最后3秒）
	WILD_SKILL_STATUS_END,         //狂暴结束
};

enum GET_CONFIG_VAL
{
	WILD_SKILL_OPEN = 1,
	WILD_SKILL_BLANK_TIME,
	WILD_SKILL_CONTINUE_TIME,
	WILD_SKILL_SPEED,
	WILD_SKILL_PAY_RETE,
	WILD_SKILL_GET_RATE,
	WILD_SKILL_CREATE_MONSTER_SPEED,
};
class CGameServer : 
				public IGameLogical
				, public IPlayerIOEvent
				, public INewTimer			// * /set INewTimer in Site_i.h
				, public IGameQuery			// * /代打查询接口，04May.2012
{
public:			//IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return (m_dwRef++);
	}
    virtual ULONG STDMETHODCALLTYPE Release( void) {
		--m_dwRef;
		if(m_dwRef == 0)
		{	// * /add
			delete this;
		}
		return m_dwRef;
	}
private:
	ULONG m_dwRef;

public:				// IGameConst
	STDMETHOD(GetEstimateTime)(DWORD * pMillesecond);
	STDMETHOD(GetPlayerNum)(IGameConst::PLAYERNUMBER * pNumber);
	STDMETHOD(GetGameIdentifier)(char * lpszIdentifier);
	STDMETHOD(CanQuickStart)();								//retrun S_OK if supported
	STDMETHOD(CanSaveGame)();									//retrun S_OK if supported
	STDMETHOD(GetVersion)(IGameConst::VERSION_STRUCT * pVS);

public:		//IGameLogical
	STDMETHOD(SetServerSite)(IServerSite * pGameSite);
	STDMETHOD(ConstructScene)();
	STDMETHOD(GetPersonalScene)(int nIndex, UINT uflags, IGameLogical::SCENE * pScene);
	STDMETHOD(Start)();
	STDMETHOD(Stop)(UINT uflags);
	STDMETHOD(Clear)();
	STDMETHOD(GetScore)(int nIndex, void * lpScoreBuf);
	STDMETHOD(OnGameOperation)(int nIndex, DWORD dwUserID, int opID, LPOPDATA oData, int nSize);
	STDMETHOD(SetTableID)(int nTableID);	// * /LeonLv add:获取桌子ID的接口, 22Aug.2012

	// 获取房间和桌子ID，add by 袁俊杰 2016-10-12
	STDMETHOD_(int, GetRoomID)();
	STDMETHOD_(int, GetTableID)();


	// * /protocol buffer using interface
	STDMETHOD(GetPersonalSceneProtBuf)(int nIndex, UINT uflags, SCENE * pScene);	// * /获取特定玩家的Protocol buffer游戏现场

public:		//IPlayerIOEvent // * /
	STDMETHOD(OnUserEnter)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserExit)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserOffline)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserReplay)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserReady)(int nIdx, IGamePlayer * pGamePlayer);	// * /Add Ready interface


public:			// * /INewTimer
	int			GetComingTimerTick(DWORD &dwComingID);						//即将下次触发的计时器
	DWORD		CheckActiveTimer();							//查询应该激活的计时器
	bool		SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam);	//设置定时器
	bool		ReSetGameTimer(DWORD dwTimerID, DWORD dwElapse);							// * /重新设置计时器时延
	bool		KillGameTimer(DWORD dwTimerID);				//删除定时器
	virtual void OnTimePass();

	bool IsSameDay( time_t t1, time_t t2 )
	{
		tm tm1;
		tm tm2;
		localtime_s(&tm1, &t1);
		localtime_s(&tm2, &t2);
		if ( tm1.tm_year == tm2.tm_year
			&& tm1.tm_yday == tm2.tm_yday)
		{
			return true;
		}
		return false;
	}

public:	// * /IGameQuery接口：支持代打查询，04May.2012
	//返回值非S_OK，表示不支持该查询
	HRESULT QueryGameInfo(DWORD dwMark, LPVOID lpData);

public:	// * /彩池查询接口
	STDMETHOD(SetPrizeInterface)(IPrizeFund * pPrize);
	virtual void SetAdditionalData( DWORD dwUserID,void* apData, int size );

private:
	IServerSite * m_pSite;
	CServerBase * m_pServer;
	ITableChannel* m_TableChannel;
	IUserList * m_pList;
	ITableChannel			*m_pTableChannel;						// * /供停机、彩池、查询房间名和桌子号的接口，27Apr.2011

	bool	m_started;

	DWORD					m_dwCurTick;					// * /当前时间戳，毫秒
	int				m_nPrivTimerFlag;				// * /正在运行的计时器标记
	STimerManage	m_arTimerManage[TIMER_COUNT + 1];     //服务端TIMER管理结构体

	clock_t  Curclock, Preclock;    

	IPrizeFund	* m_pPrize;		// * /彩池变量
	int           player_index;

public:
	CGameServer();
	virtual ~CGameServer();

private:		//游戏数据，所谓现场，就是由这些数据组成
	int                     m_nTableUserCount;          //记录桌子玩家数，如果是空的，有些逻辑就可以不用执行了（用于快速比较）
	//int	m_nGameState;			//游戏状态GAME_STARTED, GAME_STOPPED, GAME_CONSTRUCT
	//int m_nActivePlayer;		//活动用户，一般只有一个。活动用户就是可以出牌的人。
								//记录椅子号(1-4)有效

	TCHAR					m_szErrorInfo[512];			//调试用的错误信息
	TCHAR					m_szDatalogInfo[512];       //数据读写信息
	int						m_nTableID;					// 桌子ID
	int				        m_nRoomID;					//游戏房间ID
	LogicalHelper m_quickStartelper;
	enGamePhase		        m_enGamePhase;
	private:												//系统数据
		char					m_userName[PLAY_COUNT][64];								//玩家姓名
		DWORD					m_dwUserID[PLAY_COUNT];										//玩家的ID值
		enPlayerState			m_enUserState[PLAY_COUNT];									// 玩家的状态

		UserInteractData        m_UserPreInteractData[PLAY_COUNT];          // 上次保存时的交互数据
		UserInteractData        m_UserInteractData[PLAY_COUNT];             // 实时的交互数据
		tagFishingData			m_FishingData[PLAY_COUNT];					// 捕鱼共享数据

		stUserMoney				m_UserPreGameScore[PLAY_COUNT];			    // 玩家的上次统计时的金币数
		stUserMoney				m_UserGameScore[PLAY_COUNT];			    // 玩家的实时金币数  

		double					m_dbUserPreWelFarePoint[PLAY_COUNT];        // 玩家的上次福利点  
		double					m_dbUserWelFarePoint[PLAY_COUNT];           // 玩家的实时福利点  

		int                     m_nUserLevel[PLAY_COUNT];                // 玩家等级
		LONGLONG                m_llPreGradeScore[PLAY_COUNT];           // 玩家打击消耗的游戏币统计（对应转换为经验，1：1）
		LONGLONG                m_llGradeScore[PLAY_COUNT];              // 玩家打击消耗的游戏币统计（对应转换为经验，1：1）


		tagServerName		    m_zsServerName;														//房间名
		long					m_lDrawnCount[PLAY_COUNT];								//多少局

private:							//日志记录性数据
	CLog					m_LogFile;																	//日志文件记录

	// 个人相关数据统计 
	PlayerStatisticsData        m_UserStatisticsData[PLAY_COUNT];                 // 统计数据用

	// 数据统计总计
	LONGLONG	m_llSumIntervalIncome;			//系统时断收入（统计当前桌）
	stUserMoney	m_llSumIntervalExpense;   			//系统时断支出（统计当前桌）
	LONGLONG	m_llSumIncome;                    //系统收入（统计当前桌）
	stUserMoney	m_llSumExpense;					//系统支出（统计当前桌）
	LONGLONG	m_llSumAvoidHitSystemGetScore;  //系统免伤获得收益（统计当前桌）


	//概率统计      
	LONGLONG	m_nStatictisAvoidCounts;		    //免伤判断次数
	LONGLONG	m_nStatictisAvoidValidCounts;		//免伤有效次数（计算免伤率）
	LONGLONG	m_nStatictisMonsterKillCounts[MonsterTypeCount];		//怪物击杀总次数（不考虑被免伤的怪）
	LONGLONG	m_nStatictisMonsterIncomingCounts[MonsterTypeCount];	//怪物获得收益次数（计算计杀率）

	// 亏损池功能暂时保留，以观后效
	//每一种怪玩家投入成本
	LONGLONG					m_nMonsterPlayerCost[MonsterTypeCount];
	//每一种怪玩家收益				
	LONGLONG					m_nMonsterPlayerIncome[MonsterTypeCount];


private:                    // 配置相关
	int                     wild_skill_blank;
	int						m_nSceneIndex;			//背景索引	
	bool					m_bIsPlaying;		    //游戏是否开始

	bool                    m_bAlarmUser[PLAY_COUNT];    //危险用户 
	bool					m_bOnlineChair[PLAY_COUNT];			// 在线玩家     
	int                     m_nBroadcastTime[PLAY_COUNT];          // 上次播报时间
	bool                    wild_skill_status;                   //是否是狂暴状态
	//
	time_t m_LastTime; 
	char m_GameNo[28]; 
	int m_nGameIndx;              
	char m_RecordLogPath[MAX_PATH];  

	bool m_bEnableInfoLog;
	long m_iLogID;

private:					// 统计日志
	CStatistic*				m_pStatistic;				
	
public:
	void WriteInfoLog(CString szMsg);

	void WriteKillLog(CString szmsg);

public:
	/// 记录服务端启动的时间	
	void	RecordStartTime();

	///刷新记录日志路径
	void UpdateRecordLogPath();
	//生成日志名
	bool GenerateRecordLogName(std::string& aName);

	///获取日志路径
	//void GetRecordLogPath( char* apFile, char* apPath, int aSize );

	/// 杀所有计时器	
	void	KillAllTimer();
	/// 数据初始化	
	void	InitData();

private:
	void CreateMonster();           // 随机产生一定数量的怪物
	void CreateGroupMonsters();     // 产生一组怪物                       

	// 定时销毁怪物
	void OnTimerDestroyMonsters();
	// 发送切换场景消息	
	void OnTimerChangeScene();
	// 检查奖池
	void OnTimerCheckPool();

	//狂暴状态相关计时器接口和其他接口 start 2016.10.20 hl
	//狂暴状态结束
	void wild_skill_state_end();
	//狂暴状态即将结束
	void wild_skill_will_end();
	//狂暴状态倒计时阶段
	void wild_skill_wait_step();

	//狂暴阶段定时器延时启动

	void wild_skill_timer_deley();

	//狂暴期间和正常期间刷怪时间控制

	void change_monster_time(float rate = 1);
	//提示消息char2utf8
	void make_tip_info(const int wild_skill_status,const int player_index_id,const int remain_time = 0);
	//狂暴期间武器花费不够
	void send_weapon_cost_ex(const int modify_cost,const int player_index_id);

	//获取是否狂暴状态怪
	bool wild_monster(const int monster_id);

	//狂暴状态相关计时器接口和其他接口 end 2016.10.20 hl
	//激活技能	
	bool OnBuySkill(IGamePlayer *pPlayer, void *pData, int nSize);
	//购买自动攻击等级	
	bool OnBuyAutoAttack(IGamePlayer *pPlayer, void *pData, int nSize);

	//获取狂暴配置  返回
	float get_wild_config_val(GET_CONFIG_VAL type);
	//购买锤子等级 2016.10.18 hl

	bool OnBuyAutoHammer(IGamePlayer * pPlayer,void * pData,int nSize);


	//修改技能状态
	bool OnChangeSkillStatus(IGamePlayer *pPlayer, void *pData, int nSize);
	//切换武器	
	bool OnChangeWeapon(IGamePlayer *pPlayer, void *pData, int nSize);
	//技能升重	
	bool OnUpgradeSkill(IGamePlayer *pPlayer, void *pData, int nSize);
	// 玩家打击怪物
	void OnPlayerHit(IGamePlayer *pPlayer,void *pBuffer ,int nSize);
	// 增加经验，可能会升级
	void AddUserExp(IGamePlayer* pPlayer, int nExpAdd);
	// 判断是否可以自动激活技能
	void CheckCanAutoActiveSkill(IGamePlayer* pPlayer, bool bFromeInit = false);
	//判断是否需要返还金币
	void CheckCanReturnUserCoin(IGamePlayer* pPlayer);

	// 玩家收益计算，一次性为同一类型怪物计算出所有收益
	// WeaponCost： 武器基本消耗
	// nCount： 此类怪物计算的次数
	// isIncoming： true，表示需要计算收益， false，表示直接扣道具消费即可
	void CalcPlayerIncoming(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, MonsterType monsterType, WeaponID weaponID, vector<stMonsterIncoming>& lstMonsterIncoming, 
							bool isIncoming, stIncoming& UserIncoming, Change_t& Change,
							vector<SY::MonsterIncoming>& lstMonsterReback);

	void BroadcastMessage(IGamePlayer* pPlayer, const char* txt, const char* userName, WeaponID weaponID, int nKillRate, const char* monsterName, int rewardMoney, int rewardTicket);

	//产生游戏编号
	void buildGameNo();


	//捕鱼共享数据信息
	bool GetFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData);
	bool SaveFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData);
	bool ValidateFishingCommonData(tagFishingData* pData);

	//游戏私有数据
	bool GetCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData);
	bool SaveCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData);
	
	//玩家击晕鼠王逻辑
	BOOL _is_CriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit);
	void _OnPlayerHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, BOOL isCriticalHit);
	void _PlayerHitKing(IGamePlayer *pPlayer, SY::PlayerHit& playerhit);
	void _PlayerNotCriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit);

	//------------------------------------------------------游戏逻辑相关-----------------------------------------------------
public:			
	//获取房间信息
	void InitRoomInfo();
	//设置游戏状态
	enGamePhase	SetGameStatus(enGamePhase emGamePase);	
	//有效的椅子号
	bool CheckValidChairID(int nChairID);
	//获得玩家的分数
	LONGLONG GetPlayerScore(int nChair, IGamePlayer *pPlayer);
	// 获取玩家经验、等级
	LONGLONG GetPlayerExp(int nChair, IGamePlayer *pPlayer, int* pLevel);
	//计算分数
	bool CalcScore(IGamePlayer *pPlayer, int nChairID);
	//使用扩展写分接口，记录税率贡献和常客分
	bool WriteExitGameScore(IGamePlayer *pPlayer, int nChairID, long lDrawn, const stUserMoney& llWinScore, LONGLONG llTax, LONGLONG llFreqScore);
	//预结算玩家得分
	bool CalcPreScore(int nChairID);
	void RecordServerLog(DWORD dwChairID, int UserDBID);
	//向GameServer即DB写捕鱼预结算分等关键信息
	bool WritePreGameScore(DWORD dwChairID, const stUserMoney& llPreWinScore, LONGLONG llPreTotalScore, LONGLONG llGradeScore);

	//------------------------------------------------------游戏消息发送相关-----------------------------------------------------
public:																				
	// 发送金币给指定玩家
	// 显示实时数据
	void SendUserMoneyCountTo(BYTE byChairID, IGamePlayer *pPlayer, bool bRealTimeMoney = false);
	// 发送鼠王暴击事件给玩家
	void SendCriticalStrikeTo(IGamePlayer *pPlayer, const stUserMoney& UserMoney, SY::PlayerHit& PlayerHit, int nWeaponKingCost);
	// 发送鱼轨迹信息给玩家
	void SendAddFishCmd( int nCmd , char *pData , int nLen);
	//发送屏幕分辨率给玩家
	void SendScreenResolution(int iWidth,int iHeight);
	//发送用户基础信息
	void SendBasicInfo(DWORD byChairID, IGamePlayer *pPlayer);
	// * /计算通用捕鱼等级, 27Oct.2015
	int SendGeneralLevel(int nChairID, IGamePlayer *pPlayer);

	//------------------------------------------------------protobuf消息发送接收相关-----------------------------------------------------

public:
	// * /protocol buffer framework send
	bool sendGameCmdProtBufAll(int aCmdId, void *apData, int aLen);                        //发送消息到全部玩家
	bool sendGameCmdProtBufTo(int nInx,int cmdId, void* apData,int nLen);					 //发送消息到指定玩家
	bool sendGameCmdProtBufExcept(int nInx, int cmdId, void* apData,int nLen);				//发送消息到除此以外的玩家

	bool SendAdditionalGameData(int nInx);
	bool SendAdditionalGameData(int nInx, byte type);

	//手机版处理上分
	void MoblieDealUserAddScore(int ChairID);
	//手机版处理玩家下分
	void MoblieDealUserRemoveSocer(EnableLeaveQuery *pEnableLeaveQuery,IGamePlayer *pPlayer);

	//------------------------------------------------------统计日志相关相关-----------------------------------------------------
public:	
	void Statistic_Start(IGamePlayer* pGamePlayer, int nIdx);
	void Statistic_Change(IGamePlayer* pGamePlayer, int nIdx);
	void Statistic_End(IGamePlayer* pGamePlayer, int nIdx);

private:
	Statistic_s		m_Statistic[PLAY_COUNT];
	Change_t		m_Change[PLAY_COUNT];
	std::map<DWORD,int> wild_skill_monster_map;
	std::map<DWORD,int> not_normal_dead_king;
};

#endif // !defined(AFX_SAMPLESERVER_H__B9B928D4_0E4A_42E0_956E_A0EA13AC458B__INCLUDED_)

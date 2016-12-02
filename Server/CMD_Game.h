#pragma once

#include "Protocol/GameMessage.pb.h"
#include <list>

const static int PLAY_COUNT = 1;		// * /最大的游戏玩家数
const static int SKILL_COUNT = 4;       // 技能数          SkillTypeCount
const static int WEAPON_COUNT = 5;      // 锤子种类数      WeaponTypeCount

#define ROOM_COUNT 5        //房间数
#define PI          3.1415926    //
//#define FISH_TYPE_COUNT 25    //鱼类型数 
#define AlarmlScore   5000    //报警分数，如果每次写分的时候绝对值大于此值 那么就认为此玩家在刷分将被踢出

//金捕鱼的命令
//#define GAME_PATH							30000600						/** 名字 ID */
static char szTempStr[MAX_PATH] =		{0};			/** 暂存字符串，用来获取 dll_name 等的时候使用 */
#define GAME_PATH							"ServerWhacAMole\\WhacAMole"						/** 名字 ID */
#define GAME_SERVER_FOLDER(A)				(sprintf(A,"%s",GAME_PATH),strcat(A,""))


/////////////////服务端TIMER////////////////////////
#define STANDARD_TIME_ELAPSE		1000					//标准时间间隔
#define MAX_TICK_COUNT				1000000000		//最大计时器毫秒数

#define DEFAULT_WILD_SKILL_ELSPSE   60*1000  //默认30秒一次，主要是因为在最后30秒要特别提示
#define DEFAULT_WILD_SKILL_PRETIP_TIME  3  //默认提前通知狂暴状态结束时间4秒
enum TimerID
{
	TIMER_NONE = 0,
	TIME_ADD_MONSTER,					    //添加一只怪
	TIME_ADD_GROUP_MONSTER,					//产生一组怪
	TIME_CLEAR_MONSTER,						//定时清除需要销毁的怪
	TIME_CHANGE_SCENE,						//切换场景
	TIME_LOAD_FILE,							//加载配置文件
	TIME_CHECK_POOL,						//检查奖池
	TIME_INTERVAL_PLAYER_ONE,		    	//玩家1时段存分
	TIME_INTERVAL_PLAYER_TWO,		    	//玩家2时段存分
	TIME_INTERVAL_PLAYER_THREE,		    	//玩家3时段存分
	TIME_INTERVAL_PLAYER_FOUR,		    	//玩家4时段存分

	TIME_WILD_SKILL_DELEY,                    //狂暴状态客户端第一次总是收不到消息 延时5秒开始进入狂暴
	TIME_WILD_SKILL_WAIT_STEP,                  //狂暴技能触发记时 默认间隔1分钟 hl 2016.10.20
	TIME_WILD_SKILL_WILL_END,               //狂暴状态持续时间倒计时（提前?秒通知客户端）hl 2016.10.20  
	TIME_WILD_SKILL_END,                    //狂暴状态持续时间结束
	TIME_WILD_END_CREATE_MONSTER,           //狂暴结束后强制走一波刷怪

	TIME_COUNT                              //计时器个数  10 
};

const static int TIMER_COUNT = TIME_COUNT;  //计时器个数

////////////////////////////////////////////////////////////////////////

static const int CLIENT_WIDTH = 1920; // 1280;	//屏幕宽
static const int CLIENT_HEIGHT = 1080; // 720;	//屏幕宽

const int DIFF_WIDTH =20;		//宽的差值
const int DIFF_HEIGHT = 10;   //高的差值

// * /LeonLv add for pack , 19Feb.2011
#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)


//------------------------------------------------------------------------------------
//初始轨迹点和角度
//struct FishTrace
//{
//	float x;														//鱼儿X坐标
//	float y;														//鱼儿Y坐标
//	float fRotation;											//当前角度
//	double fMoveTime;										//移动时间保持这个轨迹运动的时间  
//	float fChangeTime;									//改变时间，从之前的角度改变到现在角度规定在这个时间内完成
//	bool  bExist ;										//坐标是否过期，默认为过期就是这个坐标默认不存在
//	unsigned int   nFishID;												//这个坐标对应的FISH的ID号
//	int   nFishType;											//这个坐标对应的FISH的类型
//	int   m_nPtIndex;										//鱼游动坐标的索引
//	int   nBuildTime;										//鱼生成时间
//	int   nHitCount;											//击中次数
//	bool  bFirstHit[PLAY_COUNT];									//玩家首次击中
//	int	  nHitCountID[PLAY_COUNT];								// 玩家击中次数
//	int	  nLastMultiple[PLAY_COUNT];						//最后一次击打鱼时的游戏倍数
//	float fAccamulateShootCountRatio[PLAY_COUNT];				//已累计炮数比例
//	int   nHitBaseMaxCount;									//必须击中的最大次数
//	bool  bCountedFull[PLAY_COUNT];										//累积击打分数已满 
//	int   nTakeFishType;									//类型为21和22的鱼可以附带小鱼
//	int   nSpeed;												//速度
//	bool  bMustdieZero;											//0分必中炮数鱼
//	int   debugCodeRow;                                         //用于调试
//	unsigned int   iGroupIndex;                                          //同组鱼索引，如果一组鱼，其索引值相同
//	bool  bOneNetAllDie;										//一网打尽
//	bool  bInScreen;                                             //在屏幕上
//
//};

struct SkillInfo
{
	BYTE                nSkillID;
	SY::SkillStatus     SkilllState;
};

//------------------------------------------------------------------------------------
// 游戏基础数据包，其他共有的数据继承该结构体数据包	
struct GameStationBase
{
	bool				bPlaying;								//游戏是否已经开始
	BYTE     			bySceneIndex;							//当前场景
	bool				bOnLineChair[PLAY_COUNT];				//在线状态
	int					nUserMoneyNum[PLAY_COUNT];				//玩家游戏币
	int					nUserTicketNum[PLAY_COUNT];			    //玩家礼票
	int					nWeaponCost[WEAPON_COUNT];				//武器消耗
	SkillInfo			SkillInf[SKILL_COUNT];					//技能状态
};
////----------------------------------------------------------------------------------------------
////上分和下分
//struct CMD_UserScore
//{
//	bool							bBuy;										//上分是否成功
//	BYTE							byChairID;									//上分操作的玩家
//	int  						    nBuyScore;									//上分数
//	int							nAllScore;										//购买的总子弹数
//	LONGLONG				llTotalBaseScore;							        //还可购买多少分（即玩家的剩余分数）
//};
//----------------------------------------------------------------------------------------------

//struct SkillHit
//{
//    BYTE            bySkillID;              // 触发的技能
//    BYTE            nMonsterCount;          // 击中的怪物个数
//    int             MonsterList[0];         // 击中的怪物ID
//};
//
////发射炮弹	
//struct CMD_UserShoot
//{
//	BYTE			byChairID;					//玩家座位号
//	BYTE    		byWeaponID;					//攻击的武器类型
//	int 			nMonsterID;					//攻击目标
//	bool			bIsRobot;					//判断是否是机器人
//    BYTE            nSkillCount;                //触发的技能个数
//    SkillHit*       SkillHitList;               //技能列表
//};

////----------------------------------------------------------------------------------------------
////设置炮火倍率	
//struct CMD_ChangeWeapon
//{
//	BYTE			byChairID;					//玩家桌子号
//	BYTE			WeaponID;					//切换的目标武器
//};
//
////--------------------------------------------------------------------------------------
////切换场景	
//struct CMD_ChangeScene
//{
//    BYTE     		wSceneIndex;				//当前场景
//    int             wRmoveID;                   //预留这里
//};
////------------------------------------------------------------------------------------------------------
////鱼儿轨迹	
//struct CMD_FishTrace
//{
//	bool									bRegular;								//规则标识
//	FishTrace                          m_fishtrace[5];
//};
////----------------------------------------------------------------------------------------
////金币数目
//struct CMD_UserMoneyCount
//{
//	BYTE			byChairID;								//玩家椅子
//	LONGLONG		llMoneyCount;							//金币数目
//    LONGLONG		llTicketCount;							//金币数目
//};

//////////////////////////////////////////////////////////////////////////
// 服务器内部使用结构

// 技能命中
struct stSkillHit
{
	int nSkillID;
	list<int> lstMonster;

	stSkillHit()
	{
		nSkillID = 0;
	}
};

// 玩家打击
struct stPlayerHit
{
	int nChairID;
	int nWeaponID;
	int nMonsterID;
	bool isRobot;
	bool is_wild_monster;
	std::list<stSkillHit> lstSkilTrigger;

	stPlayerHit():is_wild_monster(false)
	{
		nChairID = 0;
		nWeaponID = 0;
		nMonsterID = 0;
		isRobot = false;
	}
};

struct stDeadMonster
{
	int monster_id;
	int monster_type;
	bool be_wild_monster;
	int	 wild_king_client_not_die;//狂暴期间鼠王客户端没死，服务端死了，多扣一个武器的钱
	bool king_prob;
	stDeadMonster(int id, int type,bool is_wild_monster,int extern_money, bool isKingProb):monster_id(id), monster_type(type)
							,be_wild_monster(is_wild_monster),
							wild_king_client_not_die(extern_money),
							king_prob(isKingProb)
	{

	}
};

// 玩家的钱币总类
struct stUserMoney
{
	LONGLONG    llMoneyNum;             //金币（积分）
	LONGLONG    llTicketNum;           //礼票数

	stUserMoney()
	{
		Clear();
	}

	stUserMoney(LONGLONG _llMoney, LONGLONG _llTicket)
	{
		llMoneyNum = _llMoney;
		llTicketNum = _llTicket;
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	stUserMoney& operator +=(const stUserMoney& value)
	{
		llMoneyNum += value.llMoneyNum;
		llTicketNum += value.llTicketNum;
		return *this;
	}
};

static stUserMoney operator + (const stUserMoney& lhs, const stUserMoney& rhs)
{
	return stUserMoney(lhs.llMoneyNum + rhs.llMoneyNum, lhs.llTicketNum + rhs.llTicketNum);
}

static stUserMoney operator - (const stUserMoney& lhs, const stUserMoney& rhs)
{
	return stUserMoney(lhs.llMoneyNum - rhs.llMoneyNum, lhs.llTicketNum - rhs.llTicketNum);
}

struct stMonsterIncoming
{
	int nMonsterID;
	int nMonsterType;
	int nKillRate;
	int weapon_cost_ex;
	bool is_wild_monster;
	stUserMoney incoming;
	bool bKingProb;

	stMonsterIncoming(int _nMonsterID, int nType,int extern_cost ,bool wild_monster, bool bKingProb) : nMonsterID(_nMonsterID), nMonsterType(nType), nKillRate(0),weapon_cost_ex(extern_cost),is_wild_monster(wild_monster),bKingProb(bKingProb) {}
};

/********************************************************************************/


// 玩家游戏中的阶段定义
enum enPlayerState 
{
	enPlayerNull = 0,					// 没有玩家
	enPlayerEnter,						//玩家进入
	enPlayerBuy,							//已购买子弹
	enPlayerSettled,					//已结算
};

// 保存玩家的自定义数据
enum enAdditionData_Type
{
	enADT_None = 0,

	// 为保证和以前定义的不冲突，所以从101开始

	enADT_MoleNormalKillNum = 101,        // uint 普通地鼠
	enADT_MoleKuangGongKillNum,     // uint 矿工地鼠
	enADT_FlyGoblinKillNum,         // uint 盗宝树懒
	enADT_SpiderKillNum,            // uint 蜘蛛
	enADT_FlyKillNum,               // uint 苍蝇
	enADT_MoleKingKillNum,          // uint 鼠王

	enADT_SkillBuyStatus = 111,      // byte 0未购买，1已购买， 2已返还（bit）
	enADT_SkillActiveStatus,         // byte 0未激活，1已激活         （bit）
	enADT_SkillLevel,                // byte 0-4                    （bit）

	enADT_AutoAttackLevel = 121,     // byte 0-10 自动挂机开放等级

	enADT_AutoHammerLevel = 131,    //byte   0-10 自动锤开放等级   
};


/************************************************************************/

#pragma warning (pop)
#pragma pack(pop)
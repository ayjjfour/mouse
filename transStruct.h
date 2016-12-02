#ifndef __TRANS_STRUCT_H__
#define __TRANS_STRUCT_H__

#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)

#define GAME_ID 83	// * /游戏ID号
#define GAME_NAME "锤耗儿"

#define PLAYER_COUNT	4 // * /最大的游戏玩家数

//消息ID必须小于128。128以上的ID被系统保留
enum MSG_ID
{
	MSG_GAME_READY = 10,
};

// * /frame size
#define GAME_ZONE_WIDTH		1026	//1024	// * /frame边框会占用2个像素
#define GAME_ZONE_HEIGHT	597		//595	// * /其中，工具栏高度44（原43）

// * /海景有效区域尺寸为：1024×576, 10Sep.2012
#define SEA_SCENE_WITDH		1024
#define SEA_SCENE_HEIGHT	564	//不是563：炮台上下的偏移要相等

#define IsLegalChair(x) (0 <= (x) && (x) < PLAY_COUNT)

#define MAX_SHOW_FISH_COUNT		100// *V2012 /80				// * /客户端最大显示鱼的数量
#define MAX_BULLET_COUNT				20										//最大子弹数   
#define MAX_FISH_TRACE_COUNT			500										//最大鱼儿轨迹数
#define MAX_FISH_KIND					25							//最多鱼种类数
#define UserCellMultipleRate            100                      //转换单元分到游戏倍数
#define EXERCIZE_ROOM                    1                       //练习房
#define HUNDRED_ROOM                     2						//百炮房
#define THOUSAND_ROOM					 3						//千炮房
#define TEN_THOUSAND_ROOM                4						//万炮房
#define MIN_SHOOT_TIME                   200                   //射击时最短间隔时间(单位:200ms)
#define Max_Broadcast_TIME               3*1000                //最大播报间隔时间

#define DEBUG_SCORE                      1                      //
#define DEBUG_SHOOT                      1                     //

#define EXTEND_DATA_COUNT			20			//用于扩展的数据个数

// * /LeonLv add Start:捕鱼类共享数据游戏接口，26Oct.2012
#define NUM_FISHING_DATA_LEN	1024		// * /数据库存储捕鱼类共享数据的大小

// * /游戏支持的查询数字，用于支持GameServer查询
enum{
	emSupptNoCheck	= -1,	// * /未检查
	emSupptZero		=  0,	// * /不支持
	emSupptFishData =  1,	// * /支持捕鱼类2.5.0之后的共享数据
};

///////////////////////////////////////////////////////////
////游戏规则号定义
////          1         0         1         1
//          未使用    未使用    底分系数  底分幂数
//   
//    例如：上面规则号1011，底分=底分系数(1)×10^底分幂数(1) = 1×10 = 10
///////////////////////////////////////////////////////////


// 不要删除这个定义，用于金币类游戏的
const unsigned char GAMENOTIFY_CLIENT_SHOULD_CLOSE=0x73;
const unsigned char GAMENOTIFY_CLIENT_GAME_INFO = 0x74;	// * /游戏金币等提示信息

struct GN_CLIENT_SHOULD_CLOSE {
	char msg[255];
};

struct Robot_SCENE
{
	int cbSize;
	char lpdata[4000];
};

struct Game_SCENE {
public:		// attributes
	int cbSize;				//整个结构的大小，必须是所有scene的第一个元素

public:			// methods
	void init() {
		// TODO: 添加你自己的初始化代码
	}
};

//消息ID必须小于128。128以上的ID被系统保留

//游戏使用的分数结构
struct Game_SCORE {
	long lScore;					//积分, 在金币游戏中就是金币数
	long lWins;						//赢
	long lLose;						//输
	long lDrawn;					//平
	long lRunOut;					//逃跑

	void Clear() {
		ZeroMemory(this, sizeof(Game_SCORE));
	}
};

//游戏使用的经验
struct Game_EXP 
{
	//int nLevel;						//等级
	LONGLONG llExp;					//经验值

	Game_EXP()
	{
		Clear();
	}

	void Clear() {
		ZeroMemory(this, sizeof(Game_EXP));
	}
};

//捕鱼游戏定时写分数等关键预结算记录
struct tagSetFishPreScoreInfo 
{
	tagSetFishPreScoreInfo()
	{
		dwUserID = 0;
		llWinScore = 0;
		llRemainScore = 0;
		llGradeScore = 0;
	}
	DWORD		dwUserID;
	LONGLONG	llWinScore;				//预赢金币数
	LONGLONG	llRemainScore;			//预结算余额
	LONGLONG	llGradeScore;			//玩家等级分
};















enum emPropNameID
{
    enPropNameNone		= 0,
    enPropNameFullCan	= 1,	// * /全屏炮ID
    enPropNameGoldSkCan	= 2,	// * /炼金炮ID
    enPropNamePointGoldCan	= 3,	// * /点金炮ID
	enPropNameSkillBuy	= 4,	// 锤耗儿技能购买状态
	enPropNameSkillActive	= 5,	// 锤耗儿技能激活状态
	enPropNameSkillLevel	= 6,	// 锤耗儿技能等级
	enPropNameAutoAttack	= 7,	// 自动挂机等级
    enPropNameEnd,				// * /20Feb.2013
};

// * /LeonLv add:能量条记录，17May.2011
struct SRecEnergyBar 
{
    WORD wCellScore;		// 倍数
    WORD wEnergyBar1st;
    WORD wEnergyBar2nd;
    WORD wEnergyBar3rd;
    DWORD dwLastGameTime;	// * /记录最后玩次倍数的时间点
};

// * /福利房间保存数据，26Jul.2011
struct SWelfareData 
{
    DWORD dwLastSaveTime;	// * /
    WORD wGetWellCount;		// * /领取福利的次数
};

// * /存储到数据库的玩家记录，GameServer接口为每个玩家提供512BYTE的缓存供游戏自定义使用，17May.2011
struct SGameData 
{
    WORD			wRecEnergyBarCount;		// * /此玩家拥有的能量条记录数，既有效的倍数总数
    SRecEnergyBar	arRecEnergyBar[20];		// * /记录是个变长数组
    SWelfareData	sWelData;				// * /福利房间数据记录
};

// * /道具记录（特殊炮等）
struct tagPropRec 
{
	tagPropRec()
	{
		enPropID = (emPropNameID)0;
		byStatus = 0;
	}
	emPropNameID	enPropID;		// * /道具ID
	BYTE			byStatus;		// * /道具状态
};

//不同倍数的亏损值保存
struct tagLossPool
{
	tagLossPool()
	{
		bUse = false;
		dwLossPool = 0;
		lCellScore = 0;
	}

	bool				bUse;						//是否使用
	DWORD			dwLossPool;			//记录玩家的亏损值
	LONG				lCellScore;				//倍数
};

//捕鱼类游戏支持共享数据
struct  tagFishingData
{
	LONGLONG	llVersion;				//数据定义版本号：定义方式如下：创建数据游戏ID占n位整数×1000000000，主版本号占3位整数×1000000，次版本占3位整数×1000，最低版本3位整数；即捕鱼的2.5.0为数据为31002005000
	WORD			wDataLen;					//共享数据的长度：每个版本定义的都应该是固定值，否则为错误数据
	SGameData	sEnyGameData;		//V2.5.0之前使用的能量条记录
	LONGLONG	llGradeScore;		//等级分记录，需要转换成等级使用
	tagPropRec	arPropRec[EXTEND_DATA_COUNT];		// * /道具记录：暂时定义20个道具
	tagLossPool  arLossPool[EXTEND_DATA_COUNT];		//玩家的亏损值保存表
};

// * /游戏使用捕鱼共享数据接口
struct tagUpdateFishingData
{
	tagUpdateFishingData()
	{
		dwUserID = 0;
		memset(byBuffer, 0, NUM_FISHING_DATA_LEN);
	}

	DWORD dwUserID;
	BYTE byBuffer[NUM_FISHING_DATA_LEN];
};

// add by shijian at 2016.9.1, 增加游戏私有数据接口
#define NUM_GAMEINFO_LEN	512
struct tagGameToolInfo
{
	tagGameToolInfo()
	{
		dwUserID = 0;
		memset(byBuffer, 0, NUM_GAMEINFO_LEN);
	}
	DWORD dwUserID;
	BYTE byBuffer[NUM_GAMEINFO_LEN];
};

#pragma warning (pop)
#pragma pack(pop)

#endif //__TRANS_STRUCT_H__
#ifndef _SHARE_STRUCTS_H_
#define _SHARE_STRUCTS_H_
#include <string>
#include <vector>

//对于所有游戏的一个公用
//这个主要是框架用到的地方

#define MAHJONG_ID		(112)	//麻将gameID
#define NIUNIU_ID		(113)	//牛牛gameID
#define SHIDIANBAN_ID	(114)	//十点半GameID
#define CHEXUAN_ID		(115)	//扯旋GameID

#define Multiple_Chips  (100)  //筹码放大倍数

#define MAX_FREE_MONEY (2500) //最大免费领取金币数量

const static int USER_CHARGE_ACCUMLATE_TASK_ACTION_ID			= 1;	// 累计充值
const static int USER_CHARGE_SINGLE_TASK_ACTION_ID				= 2;	// 单笔充值
const static int USER_CHARGE_TIMES_ACTION_ID					= 3;	// 玩家充值次数

const static int USER_TAKE_BENIFIT_TASK_ACTION_ID				= 120;	// 领取救济金

const static int USER_BIND_PHONE_ACTION_ID						= 200;	// 绑定手机
const static int USER_BIND_EMAIL_ACTION_ID						= 201;	// 绑定邮件
const static int USER_MODIFY_ACCOUNT_ACTION_ID					= 202;	// 修改账号
const static int USER_MODIFY_NICK_NAME_ACTION_ID				= 203;	// 修改昵称
const static int USER_MODIFY_PASSWORD_ACTION_ID					= 204;	// 修改密码

const static double gTELEVISION_TIME_INTERVAL = 5.0;


#define WORLD_MESSAGE_NOTIFY "WORLD_MESSAGE_NOTIFY"
#define SYSTEM_MESSAGE_NOTIFY "SYSTEM_MESSAGE_NOTIFY"
#define GAME_TV_MESSAGE_NOTIFY "GAME_TV_MESSAGE_NOTIFY"

#define NETWORK_DISABLE_NOTIFY "NETWORK_DISABLE_NOTIFY"
#define TELEVISION_MESSAGE_NOTIFY "TELEVISION_MESSAGE_NOTIFY"
#define INGAME_MESSAGE_NOTIFY "INGAME_MESSAGE_NOTIFY"
#define PLAYER_BROKE_NOTIFY "PLAYER_BROKE_NOTIFY"

#define REFRESH_MOENY_NOTIFY "REFRESH_MOENY_NOTIFY"
#define ENTER_BACKGROUND_NOTIFY "ENTER_BACKGROUND_NOTIFY"
#define ENTER_FOREGROUND_NOTIFY "ENTER_FOREGROUND_NOTIFY"
#define SETTING_UPDATE_NOTIFY "SETTING_UPDATE_NOTIFY"

#define SERVER_KICK_OFF_NOTIFY "SERVER_KICK_OFF_NOTIFY"
#define SHOW_OLD_VERSION_MESSAGEBOX "SHOW_OLD_VERSION_MESSAGEBOX"

#define GF_NOTIFY_VIP_INFORMATION "GF_NOTIFY_VIP_INFORMATION"
#define GF_NOTIFY_TASK_UPDATED "GF_NOTIFY_TASK_UPDATED"   // 参数为usertaskID
#define GF_NOTIFY_TASK_REWARDED "GF_NOTIFY_TASK_REWARDED" // 参数为usertaskID
#define GF_NOTIFY_CANNOT_ENTER_ROOM "GF_NOTIFY_CANNOT_ENTER_ROOM"  // 不能进入房间的通知

#define GF_NOTIFY_MAIL_RECV_NEW "GF_NOTIFY_MAIL_RECV_NEW" // 参数为新邮件Id
#define GF_NOTIFY_MAIL_DELETED "GF_NOTIFY_MAIL_DELETED" // 参数为删除邮件Id
#define GF_NOTIFY_MAIL_OPERATION "GF_NOTIFY_MAIL_OPERATION"
/// @brief 游戏中玩家金币更新使用，游戏中不响应REFRESH_MOENY_NOTIFY
#define REFRESH_IN_GAME_MOENY_NOTIFY "REFRESH_IN_GAME_MOENY_NOTIFY"

#define GF_NOTIFY_SWITCH_TABLE_RSP "GF_NOTIFY_SWITCH_TABLE_RSP" // 换桌结果

#define GF_NOTIFY_USER_TABLE_CHANGED "GF_NOTIFY_USER_TABLE_CHANGED" // 换桌通知

#define GF_NOTIFY_UPDATE_TABLE_BONUS "GF_NOTIFY_UPDATE_TABLE_BONUS" // 通知更新桌子奖金池信息

#define  GF_NOTIFY_LOGIN_OK  "GF_NOTIFY_LOGIN_OK"  //登陆成功

#define GF_NOTIFY_GEMDIM_RECEIVE_PLAY_RESULT "GF_NOTIFY_GEMDIM_RECEICE_PLAY_RESULT" //接收对方玩家游戏结果数据

#define GF_NOTIFY_VIP_LEVEL_CHANGED "GF_NOTIFY_VIP_LEVEL_CHANGED" // 通知玩家VIP等级发生变化

#define GF_NOTIFY_CHARGE_INFORMATION "GF_NOTIFY_CHARGE_INFORMATION" // 充值信息

#define  GF_NOTIFY_HALLPAY_RESPONSE "GF_NOTIFY_HALLPAY_RESPONSE" // 充值返回

#define CMN_NOTIFY_VIP_CLOSE_INTO_SHOP "CMN_NOTIFY_VIP_CLOSE_INTO_SHOP"

#define CMN_NOTIFY_ALL_FREE_COIN_INFO_GOT "CMN_NOTIFY_ALL_FREE_COIN_INFO_GOT" // 每日登录/救济金奖励领取次数发生变化

#define GF_NOTIFY_DAILY_LOGIN_FREE_COIN_GOT "GF_NOTIFY_DAILY_LOGIN_FREE_COIN_GOT" // 领取每日金币成功

#define GF_NOTIFY_BENIFIT_FREE_COIN_GOT "GF_NOTIFY_BENIFIT_FREE_COIN_GOT" // 领取救济金成功

#define GF_NOTIFY_GET_USER_BIND_PHONE_RST "GF_NOTIFY_GET_USER_BIND_PHONE_RST" // 通知取得绑定手机结果

#define GF_NOTIFY_GET_USER_BIND_EMAIL_RST "GF_NOTIFY_GET_USER_BIND_EMAIL_RST" // 取得绑定邮箱结果

#define GF_NOTIFY_SET_USER_BIND_MAIL_RST "GF_NOTIFY_SET_USER_BIND_MAIL_RST" // 通知修改绑定邮箱

#define GF_NOTIFY_SET_USER_BIND_PHONE_RST "GF_NOTIFY_SET_USER_BIND_PHONE_RST" // 通知修改绑定手机结果

#define GF_NOTIFY_UPDATE_ROOM_BONUS "GF_NOTIFY_UPDATE_ROOM_BONUS" // 通知房间奖金池数字变化

#define  GF_NOTIFY_LOAD_TABLE_VIEW "GF_NOTIFY_USER_ENTER_TABLE"  //通知用户进入房间

#define GF_NOTIFY_PLAYER_EXTRA_INFO_GOT "GF_NOTIFY_PLAYER_EXTRA_INFO_GOT" //其他玩家额外信息，vip等级，自定义头像等等

#define GF_NOTIFY_SELF_EXTRA_INFO_GOT "GF_NOTIFY_SELF_EXTRA_INFO_GOT" //其他玩家额外信息，vip等级，自定义头像等等

#define GF_NOTIFY_MONTHLY_SUBSCRIPTION_INFO "GF_NOTIFY_MONTHLY_SUBSCRIPTION_INFO" // 包月礼包信息得到
#define GF_NOTIFY_MONTHLY_SUB_REWARD_RST "GF_NOTIFY_MONTHLY_SUB_REWARD_RST" // 领取包月每日额度结果
#define GF_NOTIFY_USER_WELFARE_LEVEL_UPDATED "GF_NOTIFY_USER_WELFARE_LEVEL_UPDATED" //福利点等级更新
#define GF_NOTIFY_USER_LEFT_LUCKY_TIMES_RESPONES "GF_NOTIFY_USER_LEFT_LUCKY_TIMES_RESPONES" //剩余抽奖次数

#define GF_NOTIFY_KICKED_OUT_BY_ADMIN "GF_NOTIFY_KICKED_OUT_BY_ADMIN"

#define GF_NOTIFY_SELF_ADDITIONAL_DATA "GF_NOTIFY_SELF_ADDITIONAL_DATA"

#define GF_NOTIFY_TASK_PROGRESS_GOT "GF_NOTIFY_TASK_PROGRESS_GOT"

#define GF_NOTIFY_FREE_MONEY_TIME_GOT "GF_NOTIFY_FREE_MONEY_TIME_GOT"

#define GF_NOTIFY_DOWNLOAD_FILE_RESULT "GF_NOTIFY_DOWNLOAD_FILE_RESULT"
#define GF_NOTIFY_DOWNLOAD_FILE_SIZE "GF_NOTIFY_DOWNLOAD_FILE_SIZE"

#define GF_NOTIFY_HTTP_DOWNLOAD_RESPONSE "GF_NOTIFY_HTTP_DOWNLOAD_RESPONSE"

enum UserStatus
{
	Offline,				//用户在房间中，已经断线状态
	Out,
	Logout,
	Delegated,
	InArena,			//用户在场中等待客户端发来进入房间消息
	InRoom,			    //用户在房间中等待客户端发来入座的消息
	WaitReEnter,		//用户重入等待客户端发来入座消息
	Sit,				//用户在房间中，已经坐下状态
	Ready,				//用户在房间中，已经Ready状态
	Playing,			//用户在房间中，已经游戏状态
};

//这个做一个扩展，加上可以区分玩法的一个type
enum ArenaType
{
	VisitorArena,
	RegisteredArena,
	SingleArena,
	HundredPeoplePlayArena, // 多人玩法的一个区域

};

enum GameStatus
{
	Disconnected,
	Connected,
	LoggedIn,
};

enum UserType
{
	Single,
	Visitor,
	Registered
};

enum ChatType
{
	World,
	Private,
	Announcement,
	Television, // 玩家某局成绩突出
	VIPMessage,
	FaceMessage,
};


//int tenthirtyNum;
//int fiveSamllNum;
//int smallFiveNum;
//int kingNum;
enum  AddDataType
{
	MAX_FAN_TYPE = 1,
	MAX_WIN_TYPE = MAX_FAN_TYPE,
	TENTHIRTY_NUM = 2,
	FIVESAMLL_NUM = 3,
	SMALLFIVE_NUM = 4,
	KING_NUM = 5,

	
};

enum ArenaLevel
{
	Beginner = 1,
	Intermediate,
	Advanced
};

struct AddDataInfoPair
{
	int type;
	std::string value;
};

struct PlayerAddDataInfos
{
	int chairID;
	int DBID;
	std::vector<AddDataInfoPair> AddDataInfoPairs;
};

#pragma pack(push)
#pragma pack(1)

struct ArenaInfo
{
	unsigned short m_ID;
	unsigned short m_NameLength;
	int m_MinMoney;
	long long m_Rule;
	int m_Type;
	int m_GameID;
	int Level; //房间等级 
};

struct ArenaDetail
{
	ArenaDetail()
		:m_pName(0)
	{
	}
	~ArenaDetail()
	{
		delete[] m_pName;
	}
	ArenaInfo m_Info;
	char* m_pName;
};

struct TableInfo
{
	unsigned short m_ArenaID;
	unsigned short m_RoomID;
	unsigned short m_ID;
	unsigned short m_NameLength;
	int m_MinMoney;
	long long m_Rule;
	int m_Type;
	int m_MaxPlayerCount;
	int m_PlayerCount;// 发送给client时填写
	int m_GameID;
	unsigned short m_SortID;
};

struct TableDetail
{
	TableDetail()
		:m_pName(0)
	{
	}
	~TableDetail()
	{
		delete[] m_pName;
	}

	bool operator==(const TableDetail& aOther) const
	{
		return this->m_Info.m_ArenaID == aOther.m_Info.m_ArenaID
				&& this->m_Info.m_RoomID == aOther.m_Info.m_RoomID
				&& this->m_Info.m_ID == aOther.m_Info.m_ID;
	}

	bool operator<(const TableDetail& aOther) const
	{
		return this->m_Info.m_ArenaID < aOther.m_Info.m_ArenaID
				|| this->m_Info.m_RoomID < aOther.m_Info.m_RoomID
				|| this->m_Info.m_ID < aOther.m_Info.m_ID
				|| this->m_Info.m_GameID < aOther.m_Info.m_GameID;
	}

	TableInfo m_Info;
	char* m_pName;
};

struct GFChargePropsData
{
	GFChargePropsData() {
		memset(this, 0, sizeof(GFChargePropsData));
	}
	~GFChargePropsData() {
		delete [] PropName;
		delete [] IconName;
		delete [] MemoName;
	}

	int PropID;
	char* PropName;
	char* IconName;
	char* MemoName;
	int Price;
	long long ConvertCoin;
	bool Hot;
	int SortNum;
};

struct GameTaskActionNotify
{
	int ActionID;
	int UserID;
	int Amount;
	short GameID;
};

// 游戏逻辑生成的波波消息
struct GameBroadcastMsg
{
	GameBroadcastMsg()
		: GameID(0)
		, GameExtraLen(0)
	{
		memset(Message, 0, sizeof(Message));
	}

	short GameID;
	short GameExtraLen;
	char Message[512];
};

const static int EXTRA_TELEVISION_TYPE_WIN = 1;		 // 输赢
const static int EXTRA_TELEVISION_TYPE_HAND_PATTEN = 2; // 牌型
const static int EXTRA_TELEVISION_TYPE_PREFERENCIAL = 3;// 特惠大礼包
const static int EXTRA_TELEVISION_TYPE_LOTTERY = 4;// 乐透信息
const static int EXTRA_TELEVISION_TYPE_MONTH = 5; // 包月
const static int EXTRA_TELEVISION_TYPE_BANKER_WIN = 6; // 庄家赢取
const static int EXTRA_TELEVISION_TYPE_PLAYER_WIN = 7; // 闲家赢取
const static int EXTRA_TELEVISION_TYPE_RED_PACKET = 8; // 发放红包

struct ExtraTelevisionMsg
{
	char Type;			// 播报类型
	char HandPattern;	// 牌型
	short VIPDays;
	long long CoinNumber;	// 输赢数字
	short NameLen;		// 使用ExtraLen时，NameLen将是所有的扩展长度，注意区分
	//long long Reserved;	// 额外占8个，扩展使用
	short ExtraLen1;
	short ExtraLen2;
	short ExtraLen3;
	short ExtraLen4;
	// 昵称
};

enum ChatContType
{
    ContBigWinner			= 0,
    ContBonusPool			= 1,
	ContBankerBigWinner		= 2,
    ContTuHaoReward			= 7,
    ContPrefrentialGift		= 8,    // * /特惠大礼包
    ContMonthRecharge		= 9,      // * /月充值

    ContMahjongWinner       = 10,   // * /
};

// * /chat section type that send to chat server for broadcasting to client in protocol buffer game server
enum ChatSctType
{
    SctStandardWords	= 0,   // * /this section do not require special show
    SctUserName			= 1,
    SctBankerName		= 2,
    SctCoinNumber		= 3,
    SctHandPattern		= 4,
    SctTuHaoReward		= 7,
    SctTableName		= 8,
    SctVIPDay			= 9,

    SctDoubleCount      = 10,   // * /Mahjong double win
    SctFanCount         = 11,   // * /Mahjong win's Fan
};


#pragma pack(pop)

#endif
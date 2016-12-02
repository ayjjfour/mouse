#ifndef _SHARE_STRUCTS_H_
#define _SHARE_STRUCTS_H_
#include <string>
#include <vector>

//����������Ϸ��һ������
//�����Ҫ�ǿ���õ��ĵط�

#define MAHJONG_ID		(112)	//�齫gameID
#define NIUNIU_ID		(113)	//ţţgameID
#define SHIDIANBAN_ID	(114)	//ʮ���GameID
#define CHEXUAN_ID		(115)	//����GameID

#define Multiple_Chips  (100)  //����Ŵ���

#define MAX_FREE_MONEY (2500) //��������ȡ�������

const static int USER_CHARGE_ACCUMLATE_TASK_ACTION_ID			= 1;	// �ۼƳ�ֵ
const static int USER_CHARGE_SINGLE_TASK_ACTION_ID				= 2;	// ���ʳ�ֵ
const static int USER_CHARGE_TIMES_ACTION_ID					= 3;	// ��ҳ�ֵ����

const static int USER_TAKE_BENIFIT_TASK_ACTION_ID				= 120;	// ��ȡ�ȼý�

const static int USER_BIND_PHONE_ACTION_ID						= 200;	// ���ֻ�
const static int USER_BIND_EMAIL_ACTION_ID						= 201;	// ���ʼ�
const static int USER_MODIFY_ACCOUNT_ACTION_ID					= 202;	// �޸��˺�
const static int USER_MODIFY_NICK_NAME_ACTION_ID				= 203;	// �޸��ǳ�
const static int USER_MODIFY_PASSWORD_ACTION_ID					= 204;	// �޸�����

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
#define GF_NOTIFY_TASK_UPDATED "GF_NOTIFY_TASK_UPDATED"   // ����ΪusertaskID
#define GF_NOTIFY_TASK_REWARDED "GF_NOTIFY_TASK_REWARDED" // ����ΪusertaskID
#define GF_NOTIFY_CANNOT_ENTER_ROOM "GF_NOTIFY_CANNOT_ENTER_ROOM"  // ���ܽ��뷿���֪ͨ

#define GF_NOTIFY_MAIL_RECV_NEW "GF_NOTIFY_MAIL_RECV_NEW" // ����Ϊ���ʼ�Id
#define GF_NOTIFY_MAIL_DELETED "GF_NOTIFY_MAIL_DELETED" // ����Ϊɾ���ʼ�Id
#define GF_NOTIFY_MAIL_OPERATION "GF_NOTIFY_MAIL_OPERATION"
/// @brief ��Ϸ����ҽ�Ҹ���ʹ�ã���Ϸ�в���ӦREFRESH_MOENY_NOTIFY
#define REFRESH_IN_GAME_MOENY_NOTIFY "REFRESH_IN_GAME_MOENY_NOTIFY"

#define GF_NOTIFY_SWITCH_TABLE_RSP "GF_NOTIFY_SWITCH_TABLE_RSP" // �������

#define GF_NOTIFY_USER_TABLE_CHANGED "GF_NOTIFY_USER_TABLE_CHANGED" // ����֪ͨ

#define GF_NOTIFY_UPDATE_TABLE_BONUS "GF_NOTIFY_UPDATE_TABLE_BONUS" // ֪ͨ�������ӽ������Ϣ

#define  GF_NOTIFY_LOGIN_OK  "GF_NOTIFY_LOGIN_OK"  //��½�ɹ�

#define GF_NOTIFY_GEMDIM_RECEIVE_PLAY_RESULT "GF_NOTIFY_GEMDIM_RECEICE_PLAY_RESULT" //���նԷ������Ϸ�������

#define GF_NOTIFY_VIP_LEVEL_CHANGED "GF_NOTIFY_VIP_LEVEL_CHANGED" // ֪ͨ���VIP�ȼ������仯

#define GF_NOTIFY_CHARGE_INFORMATION "GF_NOTIFY_CHARGE_INFORMATION" // ��ֵ��Ϣ

#define  GF_NOTIFY_HALLPAY_RESPONSE "GF_NOTIFY_HALLPAY_RESPONSE" // ��ֵ����

#define CMN_NOTIFY_VIP_CLOSE_INTO_SHOP "CMN_NOTIFY_VIP_CLOSE_INTO_SHOP"

#define CMN_NOTIFY_ALL_FREE_COIN_INFO_GOT "CMN_NOTIFY_ALL_FREE_COIN_INFO_GOT" // ÿ�յ�¼/�ȼý�����ȡ���������仯

#define GF_NOTIFY_DAILY_LOGIN_FREE_COIN_GOT "GF_NOTIFY_DAILY_LOGIN_FREE_COIN_GOT" // ��ȡÿ�ս�ҳɹ�

#define GF_NOTIFY_BENIFIT_FREE_COIN_GOT "GF_NOTIFY_BENIFIT_FREE_COIN_GOT" // ��ȡ�ȼý�ɹ�

#define GF_NOTIFY_GET_USER_BIND_PHONE_RST "GF_NOTIFY_GET_USER_BIND_PHONE_RST" // ֪ͨȡ�ð��ֻ����

#define GF_NOTIFY_GET_USER_BIND_EMAIL_RST "GF_NOTIFY_GET_USER_BIND_EMAIL_RST" // ȡ�ð�������

#define GF_NOTIFY_SET_USER_BIND_MAIL_RST "GF_NOTIFY_SET_USER_BIND_MAIL_RST" // ֪ͨ�޸İ�����

#define GF_NOTIFY_SET_USER_BIND_PHONE_RST "GF_NOTIFY_SET_USER_BIND_PHONE_RST" // ֪ͨ�޸İ��ֻ����

#define GF_NOTIFY_UPDATE_ROOM_BONUS "GF_NOTIFY_UPDATE_ROOM_BONUS" // ֪ͨ���佱������ֱ仯

#define  GF_NOTIFY_LOAD_TABLE_VIEW "GF_NOTIFY_USER_ENTER_TABLE"  //֪ͨ�û����뷿��

#define GF_NOTIFY_PLAYER_EXTRA_INFO_GOT "GF_NOTIFY_PLAYER_EXTRA_INFO_GOT" //������Ҷ�����Ϣ��vip�ȼ����Զ���ͷ��ȵ�

#define GF_NOTIFY_SELF_EXTRA_INFO_GOT "GF_NOTIFY_SELF_EXTRA_INFO_GOT" //������Ҷ�����Ϣ��vip�ȼ����Զ���ͷ��ȵ�

#define GF_NOTIFY_MONTHLY_SUBSCRIPTION_INFO "GF_NOTIFY_MONTHLY_SUBSCRIPTION_INFO" // ���������Ϣ�õ�
#define GF_NOTIFY_MONTHLY_SUB_REWARD_RST "GF_NOTIFY_MONTHLY_SUB_REWARD_RST" // ��ȡ����ÿ�ն�Ƚ��
#define GF_NOTIFY_USER_WELFARE_LEVEL_UPDATED "GF_NOTIFY_USER_WELFARE_LEVEL_UPDATED" //������ȼ�����
#define GF_NOTIFY_USER_LEFT_LUCKY_TIMES_RESPONES "GF_NOTIFY_USER_LEFT_LUCKY_TIMES_RESPONES" //ʣ��齱����

#define GF_NOTIFY_KICKED_OUT_BY_ADMIN "GF_NOTIFY_KICKED_OUT_BY_ADMIN"

#define GF_NOTIFY_SELF_ADDITIONAL_DATA "GF_NOTIFY_SELF_ADDITIONAL_DATA"

#define GF_NOTIFY_TASK_PROGRESS_GOT "GF_NOTIFY_TASK_PROGRESS_GOT"

#define GF_NOTIFY_FREE_MONEY_TIME_GOT "GF_NOTIFY_FREE_MONEY_TIME_GOT"

#define GF_NOTIFY_DOWNLOAD_FILE_RESULT "GF_NOTIFY_DOWNLOAD_FILE_RESULT"
#define GF_NOTIFY_DOWNLOAD_FILE_SIZE "GF_NOTIFY_DOWNLOAD_FILE_SIZE"

#define GF_NOTIFY_HTTP_DOWNLOAD_RESPONSE "GF_NOTIFY_HTTP_DOWNLOAD_RESPONSE"

enum UserStatus
{
	Offline,				//�û��ڷ����У��Ѿ�����״̬
	Out,
	Logout,
	Delegated,
	InArena,			//�û��ڳ��еȴ��ͻ��˷������뷿����Ϣ
	InRoom,			    //�û��ڷ����еȴ��ͻ��˷�����������Ϣ
	WaitReEnter,		//�û�����ȴ��ͻ��˷���������Ϣ
	Sit,				//�û��ڷ����У��Ѿ�����״̬
	Ready,				//�û��ڷ����У��Ѿ�Ready״̬
	Playing,			//�û��ڷ����У��Ѿ���Ϸ״̬
};

//�����һ����չ�����Ͽ��������淨��һ��type
enum ArenaType
{
	VisitorArena,
	RegisteredArena,
	SingleArena,
	HundredPeoplePlayArena, // �����淨��һ������

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
	Television, // ���ĳ�ֳɼ�ͻ��
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
	int Level; //����ȼ� 
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
	int m_PlayerCount;// ���͸�clientʱ��д
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

// ��Ϸ�߼����ɵĲ�����Ϣ
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

const static int EXTRA_TELEVISION_TYPE_WIN = 1;		 // ��Ӯ
const static int EXTRA_TELEVISION_TYPE_HAND_PATTEN = 2; // ����
const static int EXTRA_TELEVISION_TYPE_PREFERENCIAL = 3;// �ػݴ����
const static int EXTRA_TELEVISION_TYPE_LOTTERY = 4;// ��͸��Ϣ
const static int EXTRA_TELEVISION_TYPE_MONTH = 5; // ����
const static int EXTRA_TELEVISION_TYPE_BANKER_WIN = 6; // ׯ��Ӯȡ
const static int EXTRA_TELEVISION_TYPE_PLAYER_WIN = 7; // �м�Ӯȡ
const static int EXTRA_TELEVISION_TYPE_RED_PACKET = 8; // ���ź��

struct ExtraTelevisionMsg
{
	char Type;			// ��������
	char HandPattern;	// ����
	short VIPDays;
	long long CoinNumber;	// ��Ӯ����
	short NameLen;		// ʹ��ExtraLenʱ��NameLen�������е���չ���ȣ�ע������
	//long long Reserved;	// ����ռ8������չʹ��
	short ExtraLen1;
	short ExtraLen2;
	short ExtraLen3;
	short ExtraLen4;
	// �ǳ�
};

enum ChatContType
{
    ContBigWinner			= 0,
    ContBonusPool			= 1,
	ContBankerBigWinner		= 2,
    ContTuHaoReward			= 7,
    ContPrefrentialGift		= 8,    // * /�ػݴ����
    ContMonthRecharge		= 9,      // * /�³�ֵ

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
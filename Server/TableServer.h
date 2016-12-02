/*
 * CTableServer	：桌服务器基类，给Server提供回调接口,2000.10.30
 *					2001.10.11,定义新的CTableServer接口OnDispatchInterface(),Delete(),增加请求版本号和返回dll版本号
 */

#if !defined(AFX_TABLESERVER_H__198C012A_AE41_11D4_A045_0080AD315721__INCLUDED_)
#define AFX_TABLESERVER_H__198C012A_AE41_11D4_A045_0080AD315721__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"
#include "ITrace.h"
#include "../transStruct.h"

#define INVALID_TABLE_USER_ID	0xFFFF
#define INVALID_USER_ID			0xFFFF
#define MAX_TABLE_USER_ID		512
#define MAX_TABLE_PLAYER		6		// 服务器最大支持的人数
#define MAX_SCORE_BUF_LEN		20

inline BOOL IsValidChair(int nChair)
{
	return nChair >= 0 && nChair < MAX_TABLE_PLAYER;
};

inline BOOL IsValidUserID(DWORD dwUserID)
{
	return (WORD)dwUserID != INVALID_USER_ID && (WORD)dwUserID < MAX_TABLE_USER_ID;
}

inline CString GetTime()
{
	CTime time;
	CString str;
	time = CTime::GetCurrentTime();
	str.Format("%d.%d.%d, %d:%d:%d",time.GetYear(),time.GetMonth(),time.GetDay(),
		time.GetHour(),time.GetMinute(),time.GetSecond());
	return str;
}


//	Player state at Server site, do not confused with PlayerState
enum USER_STATUS // 参见BaseGame的CUserManager.h
{
	sGetOut = 0,		// 0,离开了
	sFree,				// 1,在房间站立
	sWaitSit,			// 2,等待Server回应
	sSit,				// 3,坐在椅子上，没按开始
	sReady,				// 4,同意游戏开始
	sPlaying,			// 5,正在玩
	sOffLine,			// 6,断线等待续玩
	sLookOn				// 7,旁观
};

class TABLE_USER_INFO
{
public:
	DWORD	dwUserID;					// 玩家标识
	int		nChair;						// 玩家所在的椅子号，范围为0到3有效
	int		nState;						// 玩家的状态
	int		nFaceID;					// 用户图标
	long	lDBID;						// 用户数据库ID
	DWORD	dwRightLevel;				// 用户权限
	char	szUserName[MAX_NAME_LEN];	// 玩家名
	
	// 积分
	char	lScoreBuf[MAX_SCORE_BUF_LEN];
	Game_EXP UserExp;				    // 经验

	bool bUseProtBuf;   // * /protocol buffer use

	BOOL IsValid()
	{
		return (dwUserID!=INVALID_USER_ID);
	};

	BOOL IsLookOnPeople()
	{
		return (nState==sLookOn);
	};

	BOOL IsReady()
	{
		return (nState==sReady);
	};

	BOOL IsPlaying()
	{
		return (nState==sPlaying || nState==sOffLine);
	};

	BOOL IsOffline()
	{
		return (nState==sOffLine);
	};
};

#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)

struct GAME_CMD_DATA			// 填写游戏数据的包装
{ // 游戏数据格式
  // 大批量数据自行处理
	GAME_CMD_DATA()
	{
		cChair = -1;
		cCmdID = -1;
		nDataLen = 0;
	};
	GAME_CMD_DATA(int nChair,char cmd)
	{
		InitData(nChair,cmd);
	}
	void InitData(int nChair,char cmd)
	{
		cChair = (char)nChair;
		cCmdID = cmd;
		nDataLen = 0;
	}
	BOOL SetData(LPCVOID buf, int nLen)
	{
		if(nLen > sizeof(data)/sizeof(char))
		{
			TRACE("Error : GAME_CMD_DATA::SetData() : nLen = %d\n",nLen);
			return FALSE;
		}
		memcpy(data,buf,nLen);
		nDataLen = nLen;
		return TRUE;
	}
	BOOL AddData(LPCVOID buf, int nLen)
	{
		if(nLen + nDataLen > sizeof(data)/sizeof(char))
		{
			TRACE("Error : GAME_CMD_DATA::AddData() : nLen = %d,nDataLen = %d\n",nLen,nDataLen);
			return FALSE;
		}
		memcpy(data + nDataLen,buf,nLen);
		nDataLen += nLen;
		return TRUE;
	}
	LPCSTR GetBuf()
	{
		return (LPCSTR)this;
	}
	int GetDataLen()
	{
		return nDataLen + 2;
	}

	char cChair;
	char cCmdID;
	char data[4096];
	int  nDataLen;
};

struct ItionalGameData
{
	int FanCount;
};



#pragma warning (pop)
#pragma pack(pop)

/*
 * Server提供接口CServerBase，供游戏服务器CTableServer回调
 * 功能：用于CTableServer调用Server的提供的回调函数
 * 注意：不能改变这些函数的顺序
 * 返回：如果Server返回TRUE，说明调用成功，否则说明调用出现错误
 */

class CServerBase  
{
public:
	CServerBase(){};
	~CServerBase(){};

	virtual BOOL SendGameCmd(LPCSTR buf,int nLen) = NULL
	{ // 向本桌所有人发送游戏数据(包括旁观者)
		return TRUE;
	}
	virtual BOOL SendGameCmdExcept(DWORD dwUserID,LPCSTR buf,int nLen) = NULL
	{ // 向本桌除dwUserID外其它玩家发送游戏数据
		return TRUE;
	}
	virtual BOOL SendGameCmdTo(DWORD dwUserID,LPCSTR buf,int nLen) = NULL
	{ // 向本桌指定玩家发送游戏数据
		return TRUE;
	}
	virtual BOOL SendGameCmdToMyLookOnUsers(int nChair,LPCSTR buf,int nLen) = NULL
	{ // 向本桌所有旁观椅子玩家发送游戏数据
		return TRUE;
	}
	virtual BOOL SetGameEnd() = NULL
	{ // 设置游戏结束
		return TRUE;
	}
	virtual BOOL SetGameScore(DWORD dwUserID,long lDBID,char ScoreBuf[20]) = NULL
	{ // 本局游戏成绩
		return TRUE;
	}
	virtual BOOL KickUserOnError(DWORD dwUserID) = NULL
	{ // 如果游戏发现错误数据包，要求把此人踢出游戏，此命令只能踢不在游戏中的玩家
		return TRUE;
	}
	virtual BOOL ForceUserOffline(DWORD dwUserID) = NULL
	{ // 如果游戏发现错误数据包，要求强制此人断线，他断线续玩后可以继续玩，此命令只能踢不在游戏中的玩家
		return TRUE;
	}
	virtual BOOL DismissGame() = NULL
	{ // 如果游戏发现错误，要求解散这局游戏
		return TRUE;
	}
	virtual BOOL ReportEvent(LPCSTR buf) = NULL
	{ // 如果游戏发现错误，要求记录此事件
		return TRUE;
	}

	/////////////////RFC functions////////////////////////////////

	#define CAPS_CANSETTIMER	0x00000001
	#define CAPS_CANNOTIFYEMPTY	0x00000002

	virtual BOOL GetCaps(UINT * pflags) PURE;
	//获取主服务器能力值, 该值必须是一定值

	virtual BOOL SendGameCmdToAllPlayer(LPCSTR buf, int nLen) PURE;
	// 向本桌所有玩家发送游戏数据

	virtual BOOL SetGameStart() PURE;
	//逻辑服务器通知主服务器启动游戏，主服务器应当再次调用CTableServer::OnGameBegin

	virtual BOOL StartTimer(int nInterval) PURE;
	virtual BOOL StopTimer() PURE;
	//启用/停止主服务器时钟, 取决于caps值，该方法不一定有效

	virtual DWORD	GetGameOption() PURE;
	// 获取游戏配置号

	// * /LeonLv add Start:统一到GameServer定义接口中, 04Jun.2013
	//记录每局成绩
	virtual void LogGameSet(void *pSetData)	=	NULL;

	//查询是否需要记录每局成绩
	virtual void GetWriteGameSetSetting(BOOL &bIsWrite,long &lScoreLimit)	=	NULL;

	//金币游戏含税的成绩
	virtual void SetGameScoreWithTax(DWORD dwUserID,long lDBID,void *ScoreBuf,long lTax)	=	NULL;

	//获得GAMESERVER
	virtual void * __cdecl GetGameServer()	=	NULL;

	//设置用户成绩
	virtual BOOL __cdecl NewSetGameScore(DWORD dwUserID,long lDBID,void* pScore)	=	NULL;

	//*****************************************************************
	//add by huangzhi 2007-1-2
	//增加UDP发送消息函数
	virtual BOOL SendUDPGameCmd(LPCSTR buf,int nLen) PURE;
	virtual BOOL SendUDPGameCmdTo(DWORD dwUserID,LPCSTR buf,int nLen) PURE;
	//*****************************************************************

	virtual BOOL QueryInterface(GUID iid, void **ppObj) PURE;
	// * /LeonLv add End, 04Jun.2013
};

#define IMETHOD(methodTag) virtual BOOL methodTag
#define IMETHOD_(returnType, mathodTag) virtual returnType mathodTag

interface __declspec(novtable) IMatchServerExtend {
	//TRUE表示比赛服务有效。本接口并不表示一定是比赛服务器
	IMETHOD(IsMatchExtendEnabled)() PURE;
	//	Notify: 比赛一盘结束，同时写入比赛分数以及需要记录的数据
	IMETHOD(OnSetOver)(void * lpContext, int sizeofContext) PURE;
	//	Notify: 比赛结束。pScoresBuf包括所有人的分数
	struct MATCHSCORE {
		int	nPlayers;
		long personal_score[1][5];			//这将是一个变长的数组
	};
	IMETHOD(OnMatchOver)(MATCHSCORE * pScoresBuf) PURE;
	//获得订位
	IMETHOD(GetSeat)(UINT nChair, long &ldbUserID) PURE;
	//获得比赛数据
	struct MATCHCONTEXT {
		int	NumOfSets;
		int	sizeofContext;
		void * pContextBuf;
	};
	IMETHOD(GetMatchContext)(MATCHCONTEXT * pBuf) PURE;
	//获得比赛专用的配置参数，通常用于比赛盘数
	IMETHOD_(DWORD, GetMatchOption)() PURE;
	//向数据库写入盘结果
	IMETHOD(WriteSetData)(MATCHSCORE * pScoresBuf,int nSetDataLen,char *SetData) PURE;
	//通知比赛一盘开始，停止定时检查的时钟
	IMETHOD(OnSetStart)() PURE;


};

interface __declspec(novtable) IMatchTableExtend {
	//	Notify: 定时检查，用于了解用户开始的状况
	IMETHOD(OnCheckStart)() PURE;
};

interface __declspec(novtable) ICoinServerExtend
{
	//TRUE表示是否金币服务器
	IMETHOD(IsCoinExtendEnabled)() PURE;

	IMETHOD(GetDuijuCoins)(long& lcoins) PURE;
	
	IMETHOD(GetCoins)(long lDBID,long &lcoins) PURE;

	virtual void SetCoinServer(BOOL bIsCoinServer = TRUE) PURE;
	virtual void SetDuijuCoins(const long lcoins) PURE;

	virtual void SystemMessage(DWORD dwUserID, char * szMsg = NULL) PURE;

	//写数据库
	virtual void WriteCoins(long lDBID,long &lcoins) PURE;
	//设置在游戏结束时自动写金币
	//用于俄罗斯方块等没有ServerSite的游戏
	virtual void SetAutoWrite(BOOL bAutoWrite = FALSE) PURE;
	virtual BOOL IsAutoWrite() PURE;
	
	virtual void ForceWriteScore(DWORD dwUserID,long lDBID,char *scores) PURE;
};

interface IHamsterBallServer {
	virtual char * getExtendBuf(DWORD dwUserID) PURE;
	virtual void writethrough(DWORD dwUserID) PURE;
};

#define VERSION_TABLE_SERVER_INTERFACE	0x0101

// * /LeonLv add Start:定义AI代打支持接口，04May.2012
// {104D8489-8590-4509-8282-8FAAB66352F2}
//DEFINE_GUID(IID_IGameQuery, 
//0x104d8489, 0x8590, 0x4509, 0x82, 0x82, 0x8f, 0xaa, 0xb6, 0x63, 0x52, 0xf2);
static const GUID IID_IGameQuery = 
{ 0x104d8489, 0x8590, 0x4509, { 0x82, 0x82, 0x8f, 0xaa, 0xb6, 0x63, 0x52, 0xf2 } };

struct EnableLeaveQuery
{
	long m_ChairID;
	long m_DBID;
	long m_EnableLeave; // true 可以离开    有问题写0,没问题写1
};

struct StatisticBuffer
{
	StatisticBuffer()
		: msgLen(0)
	{

	}

	int msgLen;
	char formatedMsg[1024];
	char gameData[1024];

};

struct IGameQuery
{
	enum
	{
		IID_GetCurrentPlayer	= 1,	//查询当前活动玩家椅子号
		IID_GetGamePhase		= 2,	//查询当前游戏阶段
		IID_GameEndStatus		= 3,		//查询游戏是否已经结束
		IID_IsEnableLeave		= 4,	//用户是否可以离开，发起方GameServer
		IID_GetGameSupportNo	= 7,	// * /LeonLv add for Fishing Kind checked, 26Oct.2012
		IID_CheckAICanReplace  = 12,	// * /LeonLv add:检查AI是否可以替指定玩家代打，参数为tagReplacedUserInfo, 08Apr.2013
		IID_GetPlayerEndTime = 13,		// 获取玩家一局结束时间(完成游戏时间，比如胡牌，查叫等等)，亲朋之星使用添加，17Jan.2014
	};
	//为了扩展，改用查询方式，根据传入的标识不同，返回数据类型不同，可扩展定义
	//返回值非S_OK，表示不支持该查询
	virtual HRESULT QueryGameInfo(DWORD dwMark, LPVOID lpData) = 0;
};
//增加一个接口供游戏服务器dll调用，用于彩池功能
interface __declspec(novtable) IPrizeFund
{
	//玩家中奖消息，由游戏服务器触发，执行GetMoney存储过程
	virtual void OnGetPrizeFund(DWORD dwUserID, BYTE byRuleID) PURE;
	virtual void OnGetPrizeFund(DWORD dwUserID, BYTE byRuleID, LONGLONG llGetCoins) PURE;
};
// * /LeonLv add End, 05May.2012

struct CTableServer  
{
	// 收到游戏消息
	virtual BOOL OnGameMessage(DWORD dwUserID,int nChair,LPCSTR buf, int nLen) = NULL;
	// dwUserID向指定玩家dwDestUserID转发命令，nChair为发此命令的玩家即dwUserID的椅子，dwDestUserID为数据将要发给的对象
	// buf和nLen为即要转发的数据
	virtual BOOL OnPostData(DWORD dwUserID,int nChair,DWORD dwDestUserID,LPCSTR buf, int nLen) = NULL;
	// 有一个人加入桌子
	virtual BOOL OnGameUserEnter(TABLE_USER_INFO *pUser) = NULL;
	// 有一个人离开桌子
	virtual BOOL OnGameUserExit(DWORD dwUserID) = NULL;
	// 用户准备完毕
	virtual BOOL OnGameUserReady(DWORD dwUserID) = NULL;
	// 本桌游戏开始
	virtual BOOL OnGameBegin() = NULL;
	// 用户断线
	virtual BOOL OnGameUserOffLine(DWORD dwUserID) = NULL;
	// 用户断线重入
	virtual BOOL OnGameUserReplay(DWORD dwUserID) = NULL;
	// 服务器强制结本桌游戏，但所有玩家仍在此桌
	virtual BOOL OnGameSetGameEnd() = NULL;
	// 定时器到达
	virtual BOOL OnTimePass() = NULL;
	// 桌上人已经走空，借此可以初始化自已的一些变量
	virtual BOOL OnTableEmpty() = NULL;

	enum {
		IDD_MATCHEXTEND =1	,			// the dwParam is the point to struct MATCHEXTENDPARAM
		IDD_GOBETWEENEXTEND	,			// 撮合的扩展
		IDD_CHECKVERSION	,			// 检查版本
		IDD_HAMSTERBALLSPEC	,			// 仓鼠球版本扩展
		IDD_IPRIZEFUND		=6,			// * /LeonLv:彩池接口
		IDD_IGAMEQUERY		=8,			// * /LeonLv:支持AI代打
	};
	struct MATCHEXTENDPARAM {
		IMatchServerExtend * pServerExtend;		//in
		IMatchTableExtend  * pTableExtend;		// out
	};
	struct HAMSTERBALLSPEC{
		IHamsterBallServer * pHamServer;
	};

	/*
	 *	说明: 取ICheckFileVersion的方法:
			ICheckFileVersion * pCheckFileVersion = NULL;
			OnDispatchInterface(IDD_CHECKVERSION,(DWORD)&pCheckFileVersion);
	 */
	virtual BOOL OnDispatchInterface(DWORD dwID, DWORD dwParam) = NULL;
	virtual void Delete() = NULL;

	/*
	 *	功能: 由游戏逻辑指定客户端显示的分数方面的内容包括什么
		返回: 参见IScoreDefs.h中enum Score_Type_Client定义
	 */
	virtual int __cdecl OnGetClientScoreType() = 0;
};

// 游戏服务器期望dll中CTableServer接口的版本号
#define VERSION_ITABLESERVER_GAME_SERVER_QUERY	0x0101
// CTableServer最小版本号
#define VERSION_ITABLESERVER_MIN				0x0101

// 服务器创建游戏服务器辅助类
class CTableServerHelper
{
public:
	typedef BOOL (*ProcCreateTableServer)(DWORD dwServerVersion,int nGameID,int nTableID,CServerBase *pServerBase,ITrace *pChatter, /*out*/CTableServer  **ppTableServer, /*out*/DWORD& dwTableServerVersion);
	CTableServerHelper()
	{
		m_dllTableServer = NULL;
		m_szErrorMsg[0] = 0;
	}
	~CTableServerHelper()
	{
		Close();
	}
	BOOL Create(LPCSTR szTableServerDllName,int nGameID,int nTableID,CServerBase *pServerBase,ITrace *pChatter, /*out*/CTableServer  **ppTableServer, /*out*/DWORD& dwVersionITableServer)
	{
		try
		{
			if(m_dllTableServer == NULL)
			{
				m_dllTableServer = ::LoadLibrary(szTableServerDllName);
				if(!m_dllTableServer)
				{
					char szMsg[1024];
					wsprintf(szMsg,"Can not load %s",szTableServerDllName);
					throw szMsg;
				}
			}
			ProcCreateTableServer proc;
			proc = (ProcCreateTableServer)GetProcAddress(m_dllTableServer, "CreateTableServer");
			if(proc == NULL)
				throw "Can not GetProcAddress('CreateTableServer')";

			if(!proc(VERSION_ITABLESERVER_GAME_SERVER_QUERY,nGameID,nTableID,pServerBase,pChatter, ppTableServer,dwVersionITableServer))
				throw "CreateTableServer() error";
			if((dwVersionITableServer & 0xFF00) < (VERSION_ITABLESERVER_MIN & 0xFF00))
			{
				char szMsg[1024];
				wsprintf(szMsg,"ITableServer interface version = %04X,Version = %04X wanted",dwVersionITableServer,VERSION_ITABLESERVER_MIN);
				throw szMsg;
			}
			return TRUE;
		}
		catch(LPCSTR szMsg)
		{
			lstrcpyn(m_szErrorMsg,szMsg,1023);
			TRACE(m_szErrorMsg); TRACE("\r\n");
			return FALSE;
		}
		catch(...)
		{
			strcpy(m_szErrorMsg,"CTableServerHelper::Create() : catch Error");
			TRACE(m_szErrorMsg); TRACE("\r\n");
			return FALSE;
		}
	}
	
	void Close()
	{
		if(m_dllTableServer)
		{
			FreeLibrary(m_dllTableServer);
			m_dllTableServer = NULL;
		}
	}
	LPCSTR GetErrorMsg()
	{
		return m_szErrorMsg;
	}
private:
	HINSTANCE		m_dllTableServer;
	char			m_szErrorMsg[1024];
};


// * /LeonLv add:检查AI是否可以替指定玩家代打的玩家信息，接口IID_CheckAICanReplace, 08Apr.2013
struct tagReplacedUserInfo 
{
	DWORD	dwUserID;
	int		nChairID;
};
enum{
	enReplaceTrust = 0,		// * /需要托管进入
	enReplaceNone,
};
// * /LeonLv add end


// * /LeonLv add:添加的向GameServer查询接口, 04Jun.2013
// {2C99B3EF-B403-4f27-B262-5EBC97A9E75B}
static const GUID IID_ITableChannel = 
{ 0x2c99b3ef, 0xb403, 0x4f27, { 0xb2, 0x62, 0x5e, 0xbc, 0x97, 0xa9, 0xe7, 0x5b } };
struct ITableChannel	//这是TableServer支持的接口
{
	enum
	{
		IID_TableLogicSetChanged		= 1,	//桌子逻辑设置改变了，GameLogic发起

		//剪刀石头布的查询是否比赛时间操作
		//lpData为int指针，本次操作结束后，将修改该变量为0或1，0--非比赛，1--比赛中
		IID_SJB_IsMatchTime				= 2,	

		//刷新分数操作，主要目的是为了不退出房间而获取新冲值所得的比赛币
		//lpData为指向dwUserID的DWORD*类型，刷新后的结果，将由GameServer通过IGameQuery通知游戏逻辑
		IID_SJB_RefreshCoin				= 3,	//
		//查询指定椅子号的玩家是不是机器人，采用tagQueryIsRobot结构   add by huangyiguo 2010-9-28
		IID_Query_IsRobot				=4,
		//游戏逻辑服务器发现某种情况需要主服务器停止 （目前只为深海捕鱼）add by pengfei 2011-04-14
		IID_StopServer					=5,
		//在本房间广播消息, add for 深海捕鱼游戏捕捉到黄金鲨鱼后的本房间内广播，18Apr.2011
		IID_ShowMSGInOneRoom			=6,
		//更新某玩家于某游戏的特别游戏数据 BYTE buffer[512]
		IID_UpdateGameData				=7,
		//获取房间服务器的一些信息	// * /目前有房间名，桌子号，26Apr.2011
		IID_Get_Room_Info				=8,
		//获取房间名
		IID_Get_Room_Name				=9,
		//查询某玩家所在的table id
		IID_Get_TableID					=10,
		//【查询某玩家的银行金币数】		// * /捕鱼游戏使用接口，18Aug.2011
		IID_GET_USER_BANK_AMOUNT		=11,
		// * /LeonLv add:扩展写分接口，除分数与局数外，同时写入贡献度、常客积分与扩展，10Nov.2011
		IID_EXTEND_GAME_SCORE			=12,
		// * /LeonLv add:获取捕鱼类共享数据接口，26Oct.2012
		IID_Get_Fishing_Data			=13,
		// * /LeonLv add:写捕鱼类共享数据接口，26Oct.2012
		IID_Set_Fishing_Data			=14,
		// * /LeonLv add:写捕鱼道具更新请求接口，26Oct.2012
		IID_Set_Fishing_Prop			=15,
		//查询局间等待时间，
		IID_GET_GAME_WAIT_TIME			=16,
		// * /LeonLv add:捕鱼游戏定时写玩家预结算分数等关键数据到数据库，21Dec.2012
		IID_Set_Fish_PreScore_Info		=17,
		// * /LeonLv add:查询捕鱼比赛信息, 05Mar.2013
		IID_Get_Fish_Match_Info			=18,
		// add by shijian at 2016.9.1, 增加游戏私有数据查询接口
		IID_Get_GameToolInfo			=19,
		// add by shijian at 2016.9.1, 增加游戏私有数据写入接口
		IID_Set_GameToolInfo			=20,
	};
	virtual HRESULT TableDispatch(DWORD dwMark, LPVOID lpData, int iDataLen) = 0;
};

// * /LeonLv End 04Jun.2013

//用户信息结构
struct tagUserData
{
	//用户属性
	char								szName[64];					//用户名字

	//用户积分
	LONG								lInsureScore;						//消费金币
	LONG								lGameGold;							//游戏金币
	LONG								lScore;								//用户分数
	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目
	LONG								lExperience;						//用户经验

	//用户状态
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	BYTE								cbUserStatus;						//用户状态

};

//服务端用的TIMER数据
struct STimerManage
{
	DWORD dwTimerID;
	DWORD dwElapse;
	DWORD dwRepeat;
	DWORD dwSetTick;
	WPARAM wBindParam;
	BOOL  bEnable;
};

//游戏状态
enum enGamePhase
{
	GS_FREE,			//空闲状态
	GS_PLAYING,			//游戏状态
	GS_END              //游戏结束状态
};

// * /用于计算福利点等级的玩家原始结算数据
struct GameOriginScore
{
	GameOriginScore()
	{
		clear();
	}
	void clear()
	{
		memset(this, 0, sizeof(GameOriginScore));
	}
	short ChairID;
	int UserDBID;
	LONGLONG OriginScore;
	LONGLONG Tax;
};

struct UpdateUserFishExp
{
	int UserID;
	int UserDBID;
	short RoomID;
	short Level;
	LONGLONG llExpAdd;	// 经验增量
};

struct ReturnUserCoin
{
	int UserID;
	int UserDBID;
	int GameID;
	LONGLONG llCoinAdd;
};

#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)

// * /新写分数结构，为扩展每局记录配套，04Jun.2013
struct Game_Ext_SCORE 
{
	int nChairID;
	LONGLONG llWinLose;				// * /输赢的积分
	long lWins;						// * /赢
	long lLose;						// * /输
	long lDrawn;					// * /平
	long lRunOut;					// * /逃跑
	LONGLONG llTax;					// * /贡献度
	LONGLONG llFreqPile;			// * /常客积分
	LONGLONG llExt1st;				// * /扩展未使用1
	LONGLONG llExt2nd;
};
// * /扩展写分数接口，04Jun.2013
struct tagExtGameScore 
{
	DWORD			dwUserID;
	Game_Ext_SCORE	stExtScore;
};

// * /protocol buffer send package, 07Jul.2014
struct GameDataProtBuf
{
	GameDataProtBuf()
	{
		cChair = -1;
		nTag = -1;
		nDataLen = 0;
	};
	GameDataProtBuf(int nCmd)
	{
		InitData(nCmd);
	}
	void InitData(int nCmd)
	{
		cChair = -1;
		nTag = nCmd;
		nDataLen = 0;
	}
	BOOL SetData(LPCVOID buf, int nLen)
	{
		if(nLen > sizeof(data)/sizeof(char))
		{
			TRACE("Error : GameDataProtBuf::SetData() : nLen = %d\n",nLen);
			return FALSE;
		}
		memcpy(data, buf, nLen);
		nDataLen = nLen;
		return TRUE;
	}
	BOOL AddData(LPCVOID buf, int nLen)
	{
		if(nLen + nDataLen > sizeof(data)/sizeof(char))
		{
			TRACE("Error : GameDataProtBuf::AddData() : nLen = %d,nDataLen = %d\n",nLen,nDataLen);
			return FALSE;
		}
		memcpy(data + nDataLen, buf, nLen);
		nDataLen += nLen;
		return TRUE;
	}
	LPCSTR GetBuf()
	{
		return (LPCSTR)this;
	}
	int GetDataLen()
	{
		return nDataLen + sizeof(nTag) + sizeof(cChair);
	}

	char cChair;
	char nTag;
	char data[4096];
	int  nDataLen;
};

struct tagUpdateGameData
{
	tagUpdateGameData()
	{
		dwUserID = 0;
		memset(buffer, 0, sizeof(buffer));
	}

	DWORD dwUserID;
	BYTE buffer[1024];	// * /如果最后一位(buffer[511])值为1，表示游戏请求是来读这个数据
};

#pragma warning (pop)
#pragma pack(pop)

#endif // !defined(AFX_TABLESERVER_H__198C012A_AE41_11D4_A045_0080AD315721__INCLUDED_)

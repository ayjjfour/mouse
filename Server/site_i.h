#ifndef __SITE_INTERFACE_H__
#define __SITE_INTERFACE_H__

#include "game.h"
#include "SafeLink.h"
#include "TableServer.h"
#include <unknwn.h>

/*
 *	SiteObj interface定义
 *		created by hhh, 2001.12.5
 */

//	modified by hhh 20030709
//	add following interfaces
//	IGenetic, ITimerNotify, ITimerNotify2, ITimerNotifyEx, ITimerObject, IEventHost, IGameView
#define MAX_SCORE_FIELD_COUNT		5	//游戏成绩的最大栏数


// 游戏命令码

// post data相关标识

// 定义游戏最多人数（对于象棋、围棋，值为2）
#define	MAX_PLAYER					4	//游戏的最多玩家数

#pragma pack(push)
#pragma pack(1)

typedef struct tagUSER_INFO
{
	WORD	wUserID;					// 玩家标识
	char	szUserName[MAX_NAME_LEN];	// 玩家名
	WORD	wFaceID;					// 玩家脸谱
	WORD	wNetTimelag;				// 网络延时
	char	cChair;						// 玩家所在的椅子号，范围为0到3有效
	int		nState;						// 玩家的状态
	
	// 积分
	long	lScoreBuf[MAX_SCORE_FIELD_COUNT];

	char	szLookOnUserName[MAX_NAME_LEN];// 当此玩家在旁观时，否则无此项


	inline BOOL IsValidUser()
	{
		return wUserID != INVALID_USER_ID && wUserID < MAX_USER_ID && cChair >= 0;// && cChair < MAX_PLAYER;
	}

}USER_INFO;

typedef struct tagUSER_EXT_INFO
{
	DWORD	dwPing;			// Ping的GetTickCount值，如果为0，表示没有使用
}USER_EXT_INFO;


struct CLIENT_USER_INFO : public tagUSER_INFO {
	DWORD		dwDataBaseID;					// 数据库唯一标识
	DWORD		dwRightLevel;					// 用户权限
	char		szRightName[MAX_NAME_LEN];		// 用户权限说明
	char		szGroupName[MAX_NAME_LEN];		// 用户所属的社团名
	FRIEND_TYPE	friendType;						// 朋友级别，包括普通、敌人、朋友

	void Init(USER_INFO *pInfo)
	{
		ZeroMemory(this, sizeof(CLIENT_USER_INFO));
		if (pInfo) memcpy(this, pInfo, sizeof(USER_INFO));
	};

	BOOL IsValid() {
		return (wUserID!=INVALID_USER_ID);
	};
/*
	BOOL IsLookOnPeople() {
		return ((nState&sLookOn) == sLookOn);
	};
*/
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
#pragma pack(pop)


//	IGenetic定义与IUnknown完全一样，此处定义IGenetic只是为了与老式代码兼容
#define IGenetic IUnknown

typedef IUnknown * LPIGenetic;

const GUID IID_IGenetic =IID_IUnknown;

#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID __declspec(selectany) name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

/*
 *	辅助interface定义
 *		created by hhh, 2001.12.5
 */

#define second(m)		(m*1000)
#define minute(m)		(m*60000)

//	IITimerNotify
// {8A16353D-8A07-42aa-9408-50C101980BFA}
DEFINE_GUID(IID_ITimerNotify, 
0x8a16353d, 0x8a07, 0x42aa, 0x94, 0x8, 0x50, 0xc1, 0x1, 0x98, 0xb, 0xfa);
MIDL_INTERFACE("{8A16353D-8A07-42aa-9408-50C101980BFA}")
ITimerNotify : public IUnknown
{
public:		//ITimerNotify
	STDMETHOD(OnTimePass)(int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)() PURE;
};

//	ITimerNotify2
// {5103B569-282D-487d-85C7-7D1FA09E3CC1}
DEFINE_GUID(IID_ITimerNotify2, 
0x5103b569, 0x282d, 0x487d, 0x85, 0xc7, 0x7d, 0x1f, 0xa0, 0x9e, 0x3c, 0xc1);
MIDL_INTERFACE("{5103B569-282D-487d-85C7-7D1FA09E3CC1}")
ITimerNotify2 : public IUnknown
{
public:		//ITimerNotify2
	STDMETHOD(OnTimePass)(int nID, int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)(int nID) PURE;
};

//	ITimerNotifyEx
// {F98FEED5-A9E9-4106-87F7-1C69638FBDBE}
DEFINE_GUID(IID_ITimerNotifyEx, 
0xf98feed5, 0xa9e9, 0x4106, 0x87, 0xf7, 0x1c, 0x69, 0x63, 0x8f, 0xbd, 0xbe);
MIDL_INTERFACE("{F98FEED5-A9E9-4106-87F7-1C69638FBDBE}")
ITimerNotifyEx : public IUnknown
{
public:		//ITimerNotifyEx
	STDMETHOD(OnTimePass)(IUnknown * pUnkwn/*this is the timerObj's IUnknown*/, int nRemains, int nInterval) PURE;
	STDMETHOD(OnTimeOut)(IUnknown * pUnkwn/*this is the timerObj's IUnknown*/) PURE;
};

//	ITimerObject
// {E2CF84DF-5DB8-4259-9E7C-FA6D90CA0605}
DEFINE_GUID(IID_ITimerObject, 
0xe2cf84df, 0x5db8, 0x4259, 0x9e, 0x7c, 0xfa, 0x6d, 0x90, 0xca, 0x6, 0x5);
MIDL_INTERFACE("{E2CF84DF-5DB8-4259-9E7C-FA6D90CA0605}")
ITimerObject : public IUnknown
{
public:		//ITimerObject
	STDMETHOD(attach)(ITimerNotify * pNotify, int nlevel=9) PURE;
	STDMETHOD(detach)(HANDLE hEvent) PURE;

	STDMETHOD(Start)(int nElapse, int nInterval) PURE;
	STDMETHOD(Stop)() PURE;
};

/*
 *	this interface is a lite version of IConnectionXXX family.
 *	这个接口不认识所连通的事件，更完整的处理应该用IConnectionXXX
 */
//	IEventHost
// {2999C5C8-F8A2-48b1-9D17-FF1A9E2E76AD}
DEFINE_GUID(IID_IEventHost, 
0x2999c5c8, 0xf8a2, 0x48b1, 0x9d, 0x17, 0xff, 0x1a, 0x9e, 0x2e, 0x76, 0xad);
MIDL_INTERFACE("{2999C5C8-F8A2-48b1-9D17-FF1A9E2E76AD}")
IEventHost : public IUnknown
{
public:			// IEventHost
	STDMETHOD(attach)(void * pNotify, int nlevel =9) PURE;
	STDMETHOD(detach)(void * pNotify) PURE;
	STDMETHOD(enableNotify)(void * pNotify, BOOL bEnabled =TRUE) PURE;
};

//	GameViewHelper
// {701CAAF4-144D-4ad6-8709-160D6B4A6E33}
DEFINE_GUID(IID_IGameView, 
0x701caaf4, 0x144d, 0x4ad6, 0x87, 0x9, 0x16, 0xd, 0x6b, 0x4a, 0x6e, 0x33);
MIDL_INTERFACE("{701CAAF4-144D-4ad6-8709-160D6B4A6E33}")
IGameView
{
public:
	virtual void RecvExtUserInfo(char * buf, int nLen) =0;
	virtual BOOL RecvJudge(char *buf, int nLen) =0;
	virtual BOOL RecvGameOptions(char* buf, int nLen) =0;
	virtual void EnterSinglePlayerMode() =0;
	virtual BOOL OnQuitGame() =0;
	virtual void UserListEnd() =0;
	virtual BOOL UserEnter(USER_INFO *pUser) =0;
	virtual void OnButtonSetup() =0;
	virtual void EnableUserLookOn(WORD wUserID,BOOL bIsEnable) =0;
	virtual BOOL GameMessage(char *buf, int nLen) =0;
	virtual BOOL RecvPostData(char *buf, int nLen) =0;
	virtual BOOL UserReady(WORD wUserID) =0;
	virtual BOOL UserReplay(WORD wUserID) =0;
	virtual BOOL UserOffLine(WORD wUserID) =0;
	virtual BOOL UserGameEnd(WORD wUserID) =0;
	virtual BOOL UserGameOver(WORD wUserID) =0;
	virtual BOOL UserBegin(WORD wUserID) =0;
	virtual BOOL UserExit(WORD wUserID) =0;
	virtual BOOL RecvMyInfo(char *buf, int nLen) =0;
	virtual BOOL UpdateUserScore(char *buf, int nLen) =0;
};
/*
 *	ServerSite Interfaces goes here
 */

/*
 *	Common Interfaces goes here
 */
interface IUserList;
interface IGameLogical;

//	CTableServer
// {8F1AC20B-79C8-42aa-91B4-A350BA63E40D}
DEFINE_GUID(IID_CTableServer, 
0x8f1ac20b, 0x79c8, 0x42aa, 0x91, 0xb4, 0xa3, 0x50, 0xba, 0x63, 0xe4, 0xd);
//	this is a dummy interface that reference to CTableServer

//	IGameConst
// {E88C69EA-2BE4-4948-89B9-61B9DDA25531}
DEFINE_GUID(IID_IGameConst, 
0xe88c69ea, 0x2be4, 0x4948, 0x89, 0xb9, 0x61, 0xb9, 0xdd, 0xa2, 0x55, 0x31);
MIDL_INTERFACE("{E88C69EA-2BE4-4948-89B9-61B9DDA25531}")
IGameConst : public IUnknown
{
public:			// types
	struct PLAYERNUMBER{
		int nMax;						//最大许可人数
		int nStart;						//最少开始人数
		int nMaintenance;				//最少游戏人数(某些游戏允许中途退出，因此该值可能比nStart小)
	} ;

	struct VERSION_STRUCT {
		int curVersion;					// 当前版本号
		int minVersion;					// 允许的最小版本号
		int maxVersion;					// 允许的最大版本号
	};

public:		//	IGameConst
	/*	static functions. 
		下列函数每次必须返回相同的值*/

	STDMETHOD(GetEstimateTime)(DWORD * pMillesecond) PURE;
	STDMETHOD(GetPlayerNum)(IGameConst::PLAYERNUMBER * pNumber) PURE;
	STDMETHOD(GetGameIdentifier)(char * lpszIdentifier) PURE;
	STDMETHOD(CanQuickStart)() PURE;								//retrun S_OK if supported
	STDMETHOD(CanSaveGame)() PURE;									//retrun S_OK if supported
	STDMETHOD(GetVersion)(VERSION_STRUCT * pVS) PURE;
};

//	IGameConstEx
// {4D4B5D75-C423-477d-B921-029773C74B54}
DEFINE_GUID(IID_IGameConstEx, 
0x4d4b5d75, 0xc423, 0x477d, 0xb9, 0x21, 0x2, 0x97, 0x73, 0xc7, 0x4b, 0x54);
MIDL_INTERFACE("{4D4B5D75-C423-477d-B921-029773C74B54}")
IGameConstEx : public IUnknown
{
	struct ESTIMATED_TIME {
		DWORD NormalTime;
		DWORD MinTime;
		DWORD AbnormalTime;
	};

public:			//IGameConstEx
	STDMETHOD(GetEstimateTimeEx)(IGameConstEx::ESTIMATED_TIME * pEstTime) PURE;	//the extended version of GetEstimateTime
	STDMETHOD(CanDirectStart)() PURE;								// if return S_OK, the OnGameStart will start game again even when the game is running
	STDMETHOD_(int, EnumReadyPlayer)() PURE;						// return the num of ready player
};

//	IGameStorage, this interface is not properly supported
// {0B6125B7-7F0A-4623-9A01-55C6B3F945BB}
DEFINE_GUID(IID_IGameStorage, 
0xb6125b7, 0x7f0a, 0x4623, 0x9a, 0x1, 0x55, 0xc6, 0xb3, 0xf9, 0x45, 0xbb);
MIDL_INTERFACE("{0B6125B7-7F0A-4623-9A01-55C6B3F945BB}")
IGameStorage : public IUnknown
{
public:		// IGameStorage
	STDMETHOD(GetSaveStorage)(void * pStore, int * pcbSize) PURE;			//获取存盘数据
};

//	IGameDurative
// {A18730A4-553F-4f14-83E5-EB792FDE4C50}
DEFINE_GUID(IID_IGameDurative, 
0xa18730a4, 0x553f, 0x4f14, 0x83, 0xe5, 0xeb, 0x79, 0x2f, 0xde, 0x4c, 0x50);
MIDL_INTERFACE("{A18730A4-553F-4f14-83E5-EB792FDE4C50}")
IGameDurative : public IUnknown
{
public:		//IGameDurative
	STDMETHOD(CheckStart)() PURE;										// 测试游戏是否能开始					
	STDMETHOD(CheckDurative)() PURE;									// 测试游戏是否能继续（用户出入时调用）
	STDMETHOD(CheckReplay)() PURE;										// 测试游戏是否要等待用户回来（游戏结束时用户断线调用）
};

//	IGamePlayer
// {5E89EB58-D008-4c8c-AB61-3E2283D9BBCB}
DEFINE_GUID(IID_IGamePlayer, 
0x5e89eb58, 0xd008, 0x4c8c, 0xab, 0x61, 0x3e, 0x22, 0x83, 0xd9, 0xbb, 0xcb);
MIDL_INTERFACE("{5E89EB58-D008-4c8c-AB61-3E2283D9BBCB}")
IGamePlayer : public IUnknown
{
	STDMETHOD(GetPlayer)(void * pPlayerInfo) PURE;

	//	user data manager

	STDMETHOD(put_UserStatus)(int nStatus) PURE;					// 设置UserStatus属性
	STDMETHOD_(int, get_UserStatus)(int * pStatus=NULL) PURE;		// 读取UserStatus属性
	STDMETHOD(put_UserData)(int nValue) PURE;						// 设置UserData属性
	STDMETHOD_(int, get_UserData)(int * pValue=NULL) PURE;			// 读取UserData属性
	STDMETHOD(put_UserDataBlock)(void * lpdata) PURE;				// 设置UserDataBlock属性
	STDMETHOD(get_UserDataBlock)(void ** ppdata) PURE;				// 读取UserDataBlock属性

	STDMETHOD(GetLookOnPeopleList)(IUserList **ppUserList) PURE;	// 获取旁观者列表

	//	helper functions

	STDMETHOD_(BOOL, IsValidUser)() PURE;						// 用户是否有效
	STDMETHOD_(BOOL, IsPlayer)() PURE;							// 是不是玩家

	STDMETHOD(put_State)(int sState) PURE;						// 设置State属性
	STDMETHOD_(int, get_State)(int * pState=NULL) PURE;				// 读取State属性
	
	STDMETHOD(put_EnableLookOn)(BOOL bEnable) PURE;				// 设置EnableLookOn属性
	STDMETHOD_(BOOL, get_EnableLookOn)(BOOL * pEnable=NULL) PURE;	// 读取EnableLookOn属性
	STDMETHOD_(long, get_DBID)(long * pID=NULL) PURE;			// 读取dwUserDBID属性
	STDMETHOD_(DWORD, get_ID)(DWORD * pID=NULL) PURE;			// 读取dwUserID属性
	STDMETHOD_(int, get_chair)(int * chair=NULL) PURE;			// 读取nChair属性
	STDMETHOD(get_UserName)(char * lpName) PURE;				// 获取用户名
	STDMETHOD_(DWORD, get_FaceID)(DWORD * pFaceID=NULL) PURE;		// 获取用户头像ID
	STDMETHOD_(DWORD, get_UserRight)(DWORD * pRight=NULL) PURE;	// 获取用户权利
	STDMETHOD(get_GroupName)(char * lpName) PURE;				// 获取用户所属社团名
	STDMETHOD(get_ScoreBuf)(long * lpScoreBuf) PURE;			// 获取用户成绩缓冲区

	STDMETHOD_(BOOL, IsRunAway)() PURE;
	STDMETHOD_(char*, get_hamster_extend_buf)() PURE;

	// * /LeonLv add:普通更新写分接口导致分数缓存不能随写分更新，添加此接口用于写分时单独更新分数缓存；普通写分方式不能使用此接口，22Sep.2012
	STDMETHOD(update_ScoreBuf)(long * lpAltScore) PURE;
	STDMETHOD_(LONGLONG, get_Exp)(LONGLONG *pExp = NULL) PURE;                    // 获取玩家经验值
	STDMETHOD(put_Exp)(LONGLONG nValue) PURE;						// 设置UserData属性
};

//	IPlayerIOEvent
// {BDFFAE75-DAB6-42d3-BCE2-25A7A41B5CBC}
DEFINE_GUID(IID_IPlayerIOEvent, 
0xbdffae75, 0xdab6, 0x42d3, 0xbc, 0xe2, 0x25, 0xa7, 0xa4, 0x1b, 0x5c, 0xbc);
MIDL_INTERFACE("{BDFFAE75-DAB6-42d3-BCE2-25A7A41B5CBC}")
IPlayerIOEvent : public IUnknown
{
public:		//IPlayerIOEvent
	STDMETHOD(OnUserEnter)(int nIdx, IGamePlayer * pGamePlayer) PURE;
	STDMETHOD(OnUserExit)(int nIdx, IGamePlayer * pGamePlayer) PURE;
	STDMETHOD(OnUserOffline)(int nIdx, IGamePlayer * pGamePlayer) PURE;
	STDMETHOD(OnUserReplay)(int nIdx, IGamePlayer * pGamePlayer) PURE;
	STDMETHOD(OnUserReady)(int nIdx, IGamePlayer * pGamePlayer) PURE;	// * /LeonLv add for match, 05Mar.2013
};

//	IServerSite
// {88DD5242-12EC-4aae-B9C8-FA2ADC8039FC}
DEFINE_GUID(IID_IServerSite, 
0x88dd5242, 0x12ec, 0x4aae, 0xb9, 0xc8, 0xfa, 0x2a, 0xdc, 0x80, 0x39, 0xfc);
MIDL_INTERFACE("{88DD5242-12EC-4aae-B9C8-FA2ADC8039FC}")
IServerSite : public IUnknown
{
public:		//IServerSite
	STDMETHOD(OnGameStart)() PURE;							// 基本逻辑，游戏启动
	STDMETHOD(OnGameEnd)() PURE;							// 基本逻辑，游戏结束
	STDMETHOD(OnSceneChanged)(int nIndex=-1) PURE;			// 基本逻辑，游戏状态改变

	STDMETHOD(CreateTimer)(ITimerObject ** ppTimer) PURE;							// 创建定时器

	STDMETHOD(GetServerBase)(/* out */CServerBase ** ppServer) PURE;				// 直接获取CServerBase指针，注意，可能使ServerSite失去同步
	STDMETHOD(ForceKickOff)(int nIdx) PURE;								// 强行踢人，在游戏进行中是断线，游戏结束后是踢人
	STDMETHOD(GetOff)(DWORD dwUserID) PURE;								// 强行踢人，在游戏进行中是断线，游戏结束后是踢人
	STDMETHOD(WriteScore)(int nIdx, char * lpScoreBuf) PURE;			// 写分数，游戏可以在任何时候写分数

	STDMETHOD(GetPlayerList)(/* out */IUserList ** ppList) PURE;		// 获取用户列表
	STDMETHOD(GetLogicalServer)(/* out */ IGameLogical ** ppLogical) PURE;		// 获得游戏逻辑服务器

	STDMETHOD(WriteLog)(char * szReason, char * szDescription) PURE;		// 写事件纪录
	STDMETHOD(Dismiss)(char * szReason, BOOL bDismissTable=FALSE) PURE;		// 解散

	STDMETHOD_(DWORD, GetGameOption)(DWORD * pOption=NULL) PURE;			// 获得游戏配置

	STDMETHOD(GetTableMaster)(IGamePlayer ** ppPlayer) PURE;				// 该函数未能很好的实现，不要用!

	//	比赛用扩展
	STDMETHOD(GetServerMatchExtend)(IMatchServerExtend ** ppExtend) PURE;	//guan提供的比赛扩展
	STDMETHOD_(DWORD, GetMatchOption)(DWORD * pOption =NULL) PURE;			//比赛专用配置项，将用于比赛盘数配置
	STDMETHOD(GetLastSetScore)(long * pScores) PURE;						//辅助，获得刚结束的一盘的各家得分
	STDMETHOD(GetMatchContext)(IMatchServerExtend::MATCHCONTEXT ** ppContext) PURE;	//辅助，获得比赛现场，是IMatchServerExtend中同名函数的shortcut
	STDMETHOD_(BOOL, IsMatchEnabled)() PURE;								//比赛是否启用

	
	//	时钟
	STDMETHOD(SetTimer)(DWORD nID, int nInterval, int nEllapse=-1) PURE;
	STDMETHOD(KillTimer)(DWORD nID) PURE;

};

//	IEnumUser
// {E32CA2EA-3F14-4483-AC86-4BD5DD4D421E}
DEFINE_GUID(IID_IEnumUser, 
0xe32ca2ea, 0x3f14, 0x4483, 0xac, 0x86, 0x4b, 0xd5, 0xdd, 0x4d, 0x42, 0x1e);
MIDL_INTERFACE("{E32CA2EA-3F14-4483-AC86-4BD5DD4D421E}")
IEnumUser : public IUnknown
{
public:		// IEnumUser
	STDMETHOD(Next)(ULONG celt, IGamePlayer **ppElements, ULONG *pceltFetched) PURE;
	STDMETHOD(Reset)(void) PURE;
	STDMETHOD(Skip)(ULONG celt) PURE;
	STDMETHOD(Clone)(IEnumUser **ppEnum) PURE;
};

//	IUserList
// {45B9BB90-D2B3-4544-AE61-FF987F8AF435}
DEFINE_GUID(IID_IUserList, 
0x45b9bb90, 0xd2b3, 0x4544, 0xae, 0x61, 0xff, 0x98, 0x7f, 0x8a, 0xf4, 0x35);
MIDL_INTERFACE("{45B9BB90-D2B3-4544-AE61-FF987F8AF435}")
IUserList : public IEnumUser
{
public:			//IUserList
	STDMETHOD(BeginEnum)() PURE;									// 枚举函数，开始枚举
	STDMETHOD(EnumNext)(IGamePlayer ** ppPlayer) PURE;				// 枚举函数，枚举下一个

	STDMETHOD(GetUser)(int nIdx, IGamePlayer ** ppPlayer) PURE;	// 索引函数，索引nIdx处的User
	STDMETHOD_(int, GetUserNumber)() PURE;						// 索引函数，索引范围
	
	STDMETHOD(FindUser)(DWORD dwUserID, IGamePlayer ** ppPlayer) PURE;	// 辅助函数，搜索指定的User
	STDMETHOD_(int, GetValidUserNumber)() PURE;							// 辅助函数，获取当前有效User数量

	//	added by hhh 20030605
	STDMETHOD(ensureClearPlayer)(DWORD dwUserID, long lDBID) PURE;			//确保dwUserID指定的用户被清除
};

#define LPOPDATA	void *

#define SCENE_WHOLE		0x0001
#define SCENE_MYSELF	0x0100
#define SCENE_UPDATE	0x0002

#define STOP_SERVERERROR		0x01
#define STOP_NOENOUGHPLAYER		0x02

//	IGameLogical
// {2D39792D-8241-4094-AB06-7C8B42AB129E}
DEFINE_GUID(IID_IGameLogical, 
0x2d39792d, 0x8241, 0x4094, 0xab, 0x6, 0x7c, 0x8b, 0x42, 0xab, 0x12, 0x9e);
MIDL_INTERFACE("{2D39792D-8241-4094-AB06-7C8B42AB129E}")
IGameLogical : public IGameConst
{
public:
#pragma pack(push)

#pragma pack(1)
	struct SCENE{
		int cbSize;
		char lpdata[4000];
	} ;

#pragma pack(pop)

public:		//IGameLogical
	STDMETHOD(SetServerSite)(IServerSite * pGameSite) PURE;

	// * /LeonLv add:提供给GameServer查询彩池接口，04May.2012
	STDMETHOD(SetPrizeInterface)(IPrizeFund * pPrize) PURE;
	// * /LeonLv add End

	/*	主逻辑函数	*/

	STDMETHOD(ConstructScene)() PURE;									//创建游戏现场
	STDMETHOD(GetPersonalScene)(int nIndex, UINT uflags, SCENE * pScene) PURE;			//获取特定玩家的游戏现场
	STDMETHOD(Start)() PURE;											//开始游戏
	STDMETHOD(Stop)(UINT uflags) PURE;									//强制结束游戏
	STDMETHOD(Clear)() PURE;											//清除所有数据
	STDMETHOD(GetScore)(int nIndex, void * lpScoreBuf) PURE;

	STDMETHOD(OnGameOperation)(int nIndex, DWORD dwUserID, int opID, LPOPDATA oData, int nSize) PURE;			//主操作函数

	// * /LeonLv add Start:获取桌子ID的接口, 22Aug.2012
	STDMETHOD(SetTableID)(int nTableID) PURE;

	// 获取房间和桌子ID，add by 袁俊杰 2016-10-12
	STDMETHOD_(int, GetRoomID)() PURE;
	STDMETHOD_(int, GetTableID)() PURE;
};

//	IGameLogical2
// {30D5EF5E-C1EB-47c1-9419-5E528E4AC01E}
DEFINE_GUID(IID_IGameLogical2, 
0x30d5ef5e, 0xc1eb, 0x47c1, 0x94, 0x19, 0x5e, 0x52, 0x8e, 0x4a, 0xc0, 0x1e);
MIDL_INTERFACE("{30D5EF5E-C1EB-47c1-9419-5E528E4AC01E}")
IGameLogical2 : public IGameLogical
{
public:			//IGameLogical2
	STDMETHOD(GetScene)(UINT flags, SCENE * pScene) PURE;						//获取整个游戏现场，不一定要支持
};

// {763BFA39-ECE9-4ed5-B2BB-9C69F8B1B17E}
DEFINE_GUID(IID_IGameLogicalWithTimer, 
0x763bfa39, 0xece9, 0x4ed5, 0xb2, 0xbb, 0x9c, 0x69, 0xf8, 0xb1, 0xb1, 0x7e);
MIDL_INTERFACE("{763BFA39-ECE9-4ed5-B2BB-9C69F8B1B17E}")
IGameLogicalWithTimer : public IGameLogical
{
public:			//IGameLogicalWithTimer
	virtual void __stdcall OnTimer(int nTimerID, int nRemain, int nInterval) =0;
	virtual void __stdcall OnTimerOut(int nTimerID) =0;
};

//	IGameLogicalWithNPC
// {412FD2E7-1C1B-4d1f-BC12-3C70D88F1044}
DEFINE_GUID(IID_IGameLogicalWithNPC, 
0x412fd2e7, 0x1c1b, 0x4d1f, 0xbc, 0x12, 0x3c, 0x70, 0xd8, 0x8f, 0x10, 0x44);
MIDL_INTERFACE("{412FD2E7-1C1B-4d1f-BC12-3C70D88F1044}")
IGameLogicalWithNPC :public IGameLogical
{
public:
	virtual BOOL idleProcess() PURE;				// idle循环，在该函数内禁止调用Ｓｌｅｅｐ之类的函数
};

//	this is the extend stop reason
#define STOP_SOMEONENOTSTART	0x03

// {6569FAEE-9475-4f35-BB41-6912C879F394}
DEFINE_GUID(IID_IMatchLogical, 
0x6569faee, 0x9475, 0x4f35, 0xbb, 0x41, 0x69, 0x12, 0xc8, 0x79, 0xf3, 0x94);
MIDL_INTERFACE("{6569FAEE-9475-4f35-BB41-6912C879F394}")
IMatchLogical : public IUnknown
{
public:
	STDMETHOD_(BOOL, meIsSetOver)() PURE;			//比赛，一盘是否结束
	STDMETHOD_(BOOL, meIsMatchOver)(/*in*/IMatchServerExtend::MATCHCONTEXT * pContext) PURE;	//比赛，一局是否结束
	STDMETHOD(meGetMatchContext)(/*in, out*/IMatchServerExtend::MATCHCONTEXT * pContext) PURE;	//比赛，获得盘间数据
	STDMETHOD(meCalcMatchScore)(/*in*/IMatchServerExtend::MATCHCONTEXT * pMatchContext, /*out*/void *scoreBuf) PURE;	//比赛，计算比赛分数
	STDMETHOD(meMatchStop)(UINT uStopReason) PURE;	//比赛，扩展的stop，除了原有的原因，增加STOP_SOMEONENOTSTART
};

/*
 *	ClientSite Interfaces goes here
 */

interface IGameRender;

//	IClientSite
// {3A20DC77-D99D-4f33-A4FF-AB99C3499016}
DEFINE_GUID(IID_IClientSite, 
0x3a20dc77, 0xd99d, 0x4f33, 0xa4, 0xff, 0xab, 0x99, 0xc3, 0x49, 0x90, 0x16);
MIDL_INTERFACE("{3A20DC77-D99D-4f33-A4FF-AB99C3499016}")
IClientSite : public IUnknown  
{
public:		//IClientSite
	STDMETHOD(SendMsg)(unsigned char msgID, void * buf, int cbSize) PURE;	// 发送消息

	STDMETHOD_(int, map)(int nChair) PURE;							// 映射
	STDMETHOD(GetMap)(int ** ppMap) PURE;							// 获取映射表

	//	helper
	
	STDMETHOD(GetPlayerList)(/* out */IUserList ** ppList) PURE;		// 获取用户列表
	STDMETHOD(GetMyself)(/* out */IGamePlayer ** ppGamePlayer) PURE;	// 获取自己

	STDMETHOD(GetCommLink)(/* out */ void ** ppLink) PURE;			// 获取通信层
	STDMETHOD(GetRender)(/* out */ IGameRender ** ppRender) PURE;		// 获取游戏表示层

	STDMETHOD_(BYTE, GetGameOption)() PURE;							// 获得游戏配置
	STDMETHOD(GetMaster)(/* out */IGamePlayer ** ppGamePlayer) PURE;	// 获取桌主，不一定存在
};

//	IGameRender
// {B3F43E50-B043-44a5-B6F6-3EEEB9B572D1}
DEFINE_GUID(IID_IGameRender, 
0xb3f43e50, 0xb043, 0x44a5, 0xb6, 0xf6, 0x3e, 0xee, 0xb9, 0xb5, 0x72, 0xd1);
MIDL_INTERFACE("{B3F43E50-B043-44a5-B6F6-3EEEB9B572D1}")
IGameRender : public IGameConst
{
public:
#pragma pack(push)
#pragma warning(push)

#pragma warning(disable: 4200)
#pragma pack(1)
	struct SCENE{
		int cbSize;
		char lpdata[];
	} ;

#pragma warning(pop)
#pragma pack(pop)

public:
	STDMETHOD(SetClientSite)(IClientSite * pClientSite) PURE;
	STDMETHOD(OnInitialize)() PURE;							// 初始化

	STDMETHOD(OnWaitingStart)() PURE;						// 基本逻辑，等待启动
	STDMETHOD(OnGameStart)() PURE;							// 基本逻辑，游戏启动
	STDMETHOD(OnGameEnd)(void * scorebuf) PURE;				// 基本逻辑，游戏结束
	STDMETHOD(OnSceneChanged)(SCENE *lpScene ) PURE;			// 基本逻辑，游戏状态改变

	STDMETHOD(Stop)(char * lpszReason) PURE;				// 游戏强行结束
	
	STDMETHOD(OnPlayerStateChanged)(int nIdx, int state, IGamePlayer * pPlayer) PURE;		//玩家状态改变

	STDMETHOD(OnGameOperation)(int opID, LPOPDATA oData, int nSize) PURE;			//操作码函数

	STDMETHOD(RequestConfirmQuit)() PURE;					// 询问用户是否一定要退出
	STDMETHOD(Clear)() PURE;								// 清空数据

	STDMETHOD(EnterSinglePlayerMode)() PURE;				// 启动为单机模式

	virtual void OnButtonSetup() PURE;
};


// * /LeonLv Add start, for New Timer OnTimePass(), 16July.2010
// {C8A209F0-709F-4df5-A363-969248387A35}
DEFINE_GUID(IID_INewTimer, 
			0xc8a209f0, 0x709f, 0x4df5, 0xa3, 0x63, 0x96, 0x92, 0x48, 0x38, 0x7a, 0x35);

struct INewTimer
{
	virtual void OnTimePass() = 0;
};

// * /LeonLv Add End, 16July.2010
#ifdef _CLIENT
#include "GameCreator.h"
#else
#include "TraceOut.h"
#endif

#endif		//__SITE_INTERFACE_H__

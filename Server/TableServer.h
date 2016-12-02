/*
 * CTableServer	�������������࣬��Server�ṩ�ص��ӿ�,2000.10.30
 *					2001.10.11,�����µ�CTableServer�ӿ�OnDispatchInterface(),Delete(),��������汾�źͷ���dll�汾��
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
#define MAX_TABLE_PLAYER		6		// ���������֧�ֵ�����
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
enum USER_STATUS // �μ�BaseGame��CUserManager.h
{
	sGetOut = 0,		// 0,�뿪��
	sFree,				// 1,�ڷ���վ��
	sWaitSit,			// 2,�ȴ�Server��Ӧ
	sSit,				// 3,���������ϣ�û����ʼ
	sReady,				// 4,ͬ����Ϸ��ʼ
	sPlaying,			// 5,������
	sOffLine,			// 6,���ߵȴ�����
	sLookOn				// 7,�Թ�
};

class TABLE_USER_INFO
{
public:
	DWORD	dwUserID;					// ��ұ�ʶ
	int		nChair;						// ������ڵ����Ӻţ���ΧΪ0��3��Ч
	int		nState;						// ��ҵ�״̬
	int		nFaceID;					// �û�ͼ��
	long	lDBID;						// �û����ݿ�ID
	DWORD	dwRightLevel;				// �û�Ȩ��
	char	szUserName[MAX_NAME_LEN];	// �����
	
	// ����
	char	lScoreBuf[MAX_SCORE_BUF_LEN];
	Game_EXP UserExp;				    // ����

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

struct GAME_CMD_DATA			// ��д��Ϸ���ݵİ�װ
{ // ��Ϸ���ݸ�ʽ
  // �������������д���
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
 * Server�ṩ�ӿ�CServerBase������Ϸ������CTableServer�ص�
 * ���ܣ�����CTableServer����Server���ṩ�Ļص�����
 * ע�⣺���ܸı���Щ������˳��
 * ���أ����Server����TRUE��˵�����óɹ�������˵�����ó��ִ���
 */

class CServerBase  
{
public:
	CServerBase(){};
	~CServerBase(){};

	virtual BOOL SendGameCmd(LPCSTR buf,int nLen) = NULL
	{ // ���������˷�����Ϸ����(�����Թ���)
		return TRUE;
	}
	virtual BOOL SendGameCmdExcept(DWORD dwUserID,LPCSTR buf,int nLen) = NULL
	{ // ������dwUserID��������ҷ�����Ϸ����
		return TRUE;
	}
	virtual BOOL SendGameCmdTo(DWORD dwUserID,LPCSTR buf,int nLen) = NULL
	{ // ����ָ����ҷ�����Ϸ����
		return TRUE;
	}
	virtual BOOL SendGameCmdToMyLookOnUsers(int nChair,LPCSTR buf,int nLen) = NULL
	{ // ���������Թ�������ҷ�����Ϸ����
		return TRUE;
	}
	virtual BOOL SetGameEnd() = NULL
	{ // ������Ϸ����
		return TRUE;
	}
	virtual BOOL SetGameScore(DWORD dwUserID,long lDBID,char ScoreBuf[20]) = NULL
	{ // ������Ϸ�ɼ�
		return TRUE;
	}
	virtual BOOL KickUserOnError(DWORD dwUserID) = NULL
	{ // �����Ϸ���ִ������ݰ���Ҫ��Ѵ����߳���Ϸ��������ֻ���߲�����Ϸ�е����
		return TRUE;
	}
	virtual BOOL ForceUserOffline(DWORD dwUserID) = NULL
	{ // �����Ϸ���ִ������ݰ���Ҫ��ǿ�ƴ��˶��ߣ��������������Լ����棬������ֻ���߲�����Ϸ�е����
		return TRUE;
	}
	virtual BOOL DismissGame() = NULL
	{ // �����Ϸ���ִ���Ҫ���ɢ�����Ϸ
		return TRUE;
	}
	virtual BOOL ReportEvent(LPCSTR buf) = NULL
	{ // �����Ϸ���ִ���Ҫ���¼���¼�
		return TRUE;
	}

	/////////////////RFC functions////////////////////////////////

	#define CAPS_CANSETTIMER	0x00000001
	#define CAPS_CANNOTIFYEMPTY	0x00000002

	virtual BOOL GetCaps(UINT * pflags) PURE;
	//��ȡ������������ֵ, ��ֵ������һ��ֵ

	virtual BOOL SendGameCmdToAllPlayer(LPCSTR buf, int nLen) PURE;
	// ����������ҷ�����Ϸ����

	virtual BOOL SetGameStart() PURE;
	//�߼�������֪ͨ��������������Ϸ����������Ӧ���ٴε���CTableServer::OnGameBegin

	virtual BOOL StartTimer(int nInterval) PURE;
	virtual BOOL StopTimer() PURE;
	//����/ֹͣ��������ʱ��, ȡ����capsֵ���÷�����һ����Ч

	virtual DWORD	GetGameOption() PURE;
	// ��ȡ��Ϸ���ú�

	// * /LeonLv add Start:ͳһ��GameServer����ӿ���, 04Jun.2013
	//��¼ÿ�ֳɼ�
	virtual void LogGameSet(void *pSetData)	=	NULL;

	//��ѯ�Ƿ���Ҫ��¼ÿ�ֳɼ�
	virtual void GetWriteGameSetSetting(BOOL &bIsWrite,long &lScoreLimit)	=	NULL;

	//�����Ϸ��˰�ĳɼ�
	virtual void SetGameScoreWithTax(DWORD dwUserID,long lDBID,void *ScoreBuf,long lTax)	=	NULL;

	//���GAMESERVER
	virtual void * __cdecl GetGameServer()	=	NULL;

	//�����û��ɼ�
	virtual BOOL __cdecl NewSetGameScore(DWORD dwUserID,long lDBID,void* pScore)	=	NULL;

	//*****************************************************************
	//add by huangzhi 2007-1-2
	//����UDP������Ϣ����
	virtual BOOL SendUDPGameCmd(LPCSTR buf,int nLen) PURE;
	virtual BOOL SendUDPGameCmdTo(DWORD dwUserID,LPCSTR buf,int nLen) PURE;
	//*****************************************************************

	virtual BOOL QueryInterface(GUID iid, void **ppObj) PURE;
	// * /LeonLv add End, 04Jun.2013
};

#define IMETHOD(methodTag) virtual BOOL methodTag
#define IMETHOD_(returnType, mathodTag) virtual returnType mathodTag

interface __declspec(novtable) IMatchServerExtend {
	//TRUE��ʾ����������Ч�����ӿڲ�����ʾһ���Ǳ���������
	IMETHOD(IsMatchExtendEnabled)() PURE;
	//	Notify: ����һ�̽�����ͬʱд����������Լ���Ҫ��¼������
	IMETHOD(OnSetOver)(void * lpContext, int sizeofContext) PURE;
	//	Notify: ����������pScoresBuf���������˵ķ���
	struct MATCHSCORE {
		int	nPlayers;
		long personal_score[1][5];			//�⽫��һ���䳤������
	};
	IMETHOD(OnMatchOver)(MATCHSCORE * pScoresBuf) PURE;
	//��ö�λ
	IMETHOD(GetSeat)(UINT nChair, long &ldbUserID) PURE;
	//��ñ�������
	struct MATCHCONTEXT {
		int	NumOfSets;
		int	sizeofContext;
		void * pContextBuf;
	};
	IMETHOD(GetMatchContext)(MATCHCONTEXT * pBuf) PURE;
	//��ñ���ר�õ����ò�����ͨ�����ڱ�������
	IMETHOD_(DWORD, GetMatchOption)() PURE;
	//�����ݿ�д���̽��
	IMETHOD(WriteSetData)(MATCHSCORE * pScoresBuf,int nSetDataLen,char *SetData) PURE;
	//֪ͨ����һ�̿�ʼ��ֹͣ��ʱ����ʱ��
	IMETHOD(OnSetStart)() PURE;


};

interface __declspec(novtable) IMatchTableExtend {
	//	Notify: ��ʱ��飬�����˽��û���ʼ��״��
	IMETHOD(OnCheckStart)() PURE;
};

interface __declspec(novtable) ICoinServerExtend
{
	//TRUE��ʾ�Ƿ��ҷ�����
	IMETHOD(IsCoinExtendEnabled)() PURE;

	IMETHOD(GetDuijuCoins)(long& lcoins) PURE;
	
	IMETHOD(GetCoins)(long lDBID,long &lcoins) PURE;

	virtual void SetCoinServer(BOOL bIsCoinServer = TRUE) PURE;
	virtual void SetDuijuCoins(const long lcoins) PURE;

	virtual void SystemMessage(DWORD dwUserID, char * szMsg = NULL) PURE;

	//д���ݿ�
	virtual void WriteCoins(long lDBID,long &lcoins) PURE;
	//��������Ϸ����ʱ�Զ�д���
	//���ڶ���˹�����û��ServerSite����Ϸ
	virtual void SetAutoWrite(BOOL bAutoWrite = FALSE) PURE;
	virtual BOOL IsAutoWrite() PURE;
	
	virtual void ForceWriteScore(DWORD dwUserID,long lDBID,char *scores) PURE;
};

interface IHamsterBallServer {
	virtual char * getExtendBuf(DWORD dwUserID) PURE;
	virtual void writethrough(DWORD dwUserID) PURE;
};

#define VERSION_TABLE_SERVER_INTERFACE	0x0101

// * /LeonLv add Start:����AI����֧�ֽӿڣ�04May.2012
// {104D8489-8590-4509-8282-8FAAB66352F2}
//DEFINE_GUID(IID_IGameQuery, 
//0x104d8489, 0x8590, 0x4509, 0x82, 0x82, 0x8f, 0xaa, 0xb6, 0x63, 0x52, 0xf2);
static const GUID IID_IGameQuery = 
{ 0x104d8489, 0x8590, 0x4509, { 0x82, 0x82, 0x8f, 0xaa, 0xb6, 0x63, 0x52, 0xf2 } };

struct EnableLeaveQuery
{
	long m_ChairID;
	long m_DBID;
	long m_EnableLeave; // true �����뿪    ������д0,û����д1
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
		IID_GetCurrentPlayer	= 1,	//��ѯ��ǰ�������Ӻ�
		IID_GetGamePhase		= 2,	//��ѯ��ǰ��Ϸ�׶�
		IID_GameEndStatus		= 3,		//��ѯ��Ϸ�Ƿ��Ѿ�����
		IID_IsEnableLeave		= 4,	//�û��Ƿ�����뿪������GameServer
		IID_GetGameSupportNo	= 7,	// * /LeonLv add for Fishing Kind checked, 26Oct.2012
		IID_CheckAICanReplace  = 12,	// * /LeonLv add:���AI�Ƿ������ָ����Ҵ��򣬲���ΪtagReplacedUserInfo, 08Apr.2013
		IID_GetPlayerEndTime = 13,		// ��ȡ���һ�ֽ���ʱ��(�����Ϸʱ�䣬������ƣ���еȵ�)������֮��ʹ����ӣ�17Jan.2014
	};
	//Ϊ����չ�����ò�ѯ��ʽ�����ݴ���ı�ʶ��ͬ�������������Ͳ�ͬ������չ����
	//����ֵ��S_OK����ʾ��֧�ָò�ѯ
	virtual HRESULT QueryGameInfo(DWORD dwMark, LPVOID lpData) = 0;
};
//����һ���ӿڹ���Ϸ������dll���ã����ڲʳع���
interface __declspec(novtable) IPrizeFund
{
	//����н���Ϣ������Ϸ������������ִ��GetMoney�洢����
	virtual void OnGetPrizeFund(DWORD dwUserID, BYTE byRuleID) PURE;
	virtual void OnGetPrizeFund(DWORD dwUserID, BYTE byRuleID, LONGLONG llGetCoins) PURE;
};
// * /LeonLv add End, 05May.2012

struct CTableServer  
{
	// �յ���Ϸ��Ϣ
	virtual BOOL OnGameMessage(DWORD dwUserID,int nChair,LPCSTR buf, int nLen) = NULL;
	// dwUserID��ָ�����dwDestUserIDת�����nChairΪ�����������Ҽ�dwUserID�����ӣ�dwDestUserIDΪ���ݽ�Ҫ�����Ķ���
	// buf��nLenΪ��Ҫת��������
	virtual BOOL OnPostData(DWORD dwUserID,int nChair,DWORD dwDestUserID,LPCSTR buf, int nLen) = NULL;
	// ��һ���˼�������
	virtual BOOL OnGameUserEnter(TABLE_USER_INFO *pUser) = NULL;
	// ��һ�����뿪����
	virtual BOOL OnGameUserExit(DWORD dwUserID) = NULL;
	// �û�׼�����
	virtual BOOL OnGameUserReady(DWORD dwUserID) = NULL;
	// ������Ϸ��ʼ
	virtual BOOL OnGameBegin() = NULL;
	// �û�����
	virtual BOOL OnGameUserOffLine(DWORD dwUserID) = NULL;
	// �û���������
	virtual BOOL OnGameUserReplay(DWORD dwUserID) = NULL;
	// ������ǿ�ƽ᱾����Ϸ��������������ڴ���
	virtual BOOL OnGameSetGameEnd() = NULL;
	// ��ʱ������
	virtual BOOL OnTimePass() = NULL;
	// �������Ѿ��߿գ���˿��Գ�ʼ�����ѵ�һЩ����
	virtual BOOL OnTableEmpty() = NULL;

	enum {
		IDD_MATCHEXTEND =1	,			// the dwParam is the point to struct MATCHEXTENDPARAM
		IDD_GOBETWEENEXTEND	,			// ��ϵ���չ
		IDD_CHECKVERSION	,			// ���汾
		IDD_HAMSTERBALLSPEC	,			// ������汾��չ
		IDD_IPRIZEFUND		=6,			// * /LeonLv:�ʳؽӿ�
		IDD_IGAMEQUERY		=8,			// * /LeonLv:֧��AI����
	};
	struct MATCHEXTENDPARAM {
		IMatchServerExtend * pServerExtend;		//in
		IMatchTableExtend  * pTableExtend;		// out
	};
	struct HAMSTERBALLSPEC{
		IHamsterBallServer * pHamServer;
	};

	/*
	 *	˵��: ȡICheckFileVersion�ķ���:
			ICheckFileVersion * pCheckFileVersion = NULL;
			OnDispatchInterface(IDD_CHECKVERSION,(DWORD)&pCheckFileVersion);
	 */
	virtual BOOL OnDispatchInterface(DWORD dwID, DWORD dwParam) = NULL;
	virtual void Delete() = NULL;

	/*
	 *	����: ����Ϸ�߼�ָ���ͻ�����ʾ�ķ�����������ݰ���ʲô
		����: �μ�IScoreDefs.h��enum Score_Type_Client����
	 */
	virtual int __cdecl OnGetClientScoreType() = 0;
};

// ��Ϸ����������dll��CTableServer�ӿڵİ汾��
#define VERSION_ITABLESERVER_GAME_SERVER_QUERY	0x0101
// CTableServer��С�汾��
#define VERSION_ITABLESERVER_MIN				0x0101

// ������������Ϸ������������
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


// * /LeonLv add:���AI�Ƿ������ָ����Ҵ���������Ϣ���ӿ�IID_CheckAICanReplace, 08Apr.2013
struct tagReplacedUserInfo 
{
	DWORD	dwUserID;
	int		nChairID;
};
enum{
	enReplaceTrust = 0,		// * /��Ҫ�йܽ���
	enReplaceNone,
};
// * /LeonLv add end


// * /LeonLv add:��ӵ���GameServer��ѯ�ӿ�, 04Jun.2013
// {2C99B3EF-B403-4f27-B262-5EBC97A9E75B}
static const GUID IID_ITableChannel = 
{ 0x2c99b3ef, 0xb403, 0x4f27, { 0xb2, 0x62, 0x5e, 0xbc, 0x97, 0xa9, 0xe7, 0x5b } };
struct ITableChannel	//����TableServer֧�ֵĽӿ�
{
	enum
	{
		IID_TableLogicSetChanged		= 1,	//�����߼����øı��ˣ�GameLogic����

		//����ʯͷ���Ĳ�ѯ�Ƿ����ʱ�����
		//lpDataΪintָ�룬���β��������󣬽��޸ĸñ���Ϊ0��1��0--�Ǳ�����1--������
		IID_SJB_IsMatchTime				= 2,	

		//ˢ�·�����������ҪĿ����Ϊ�˲��˳��������ȡ�³�ֵ���õı�����
		//lpDataΪָ��dwUserID��DWORD*���ͣ�ˢ�º�Ľ��������GameServerͨ��IGameQuery֪ͨ��Ϸ�߼�
		IID_SJB_RefreshCoin				= 3,	//
		//��ѯָ�����Ӻŵ�����ǲ��ǻ����ˣ�����tagQueryIsRobot�ṹ   add by huangyiguo 2010-9-28
		IID_Query_IsRobot				=4,
		//��Ϸ�߼�����������ĳ�������Ҫ��������ֹͣ ��ĿǰֻΪ����㣩add by pengfei 2011-04-14
		IID_StopServer					=5,
		//�ڱ�����㲥��Ϣ, add for �������Ϸ��׽���ƽ������ı������ڹ㲥��18Apr.2011
		IID_ShowMSGInOneRoom			=6,
		//����ĳ�����ĳ��Ϸ���ر���Ϸ���� BYTE buffer[512]
		IID_UpdateGameData				=7,
		//��ȡ�����������һЩ��Ϣ	// * /Ŀǰ�з����������Ӻţ�26Apr.2011
		IID_Get_Room_Info				=8,
		//��ȡ������
		IID_Get_Room_Name				=9,
		//��ѯĳ������ڵ�table id
		IID_Get_TableID					=10,
		//����ѯĳ��ҵ����н������		// * /������Ϸʹ�ýӿڣ�18Aug.2011
		IID_GET_USER_BANK_AMOUNT		=11,
		// * /LeonLv add:��չд�ֽӿڣ�������������⣬ͬʱд�빱�׶ȡ����ͻ�������չ��10Nov.2011
		IID_EXTEND_GAME_SCORE			=12,
		// * /LeonLv add:��ȡ�����๲�����ݽӿڣ�26Oct.2012
		IID_Get_Fishing_Data			=13,
		// * /LeonLv add:д�����๲�����ݽӿڣ�26Oct.2012
		IID_Set_Fishing_Data			=14,
		// * /LeonLv add:д������߸�������ӿڣ�26Oct.2012
		IID_Set_Fishing_Prop			=15,
		//��ѯ�ּ�ȴ�ʱ�䣬
		IID_GET_GAME_WAIT_TIME			=16,
		// * /LeonLv add:������Ϸ��ʱд���Ԥ��������ȹؼ����ݵ����ݿ⣬21Dec.2012
		IID_Set_Fish_PreScore_Info		=17,
		// * /LeonLv add:��ѯ���������Ϣ, 05Mar.2013
		IID_Get_Fish_Match_Info			=18,
		// add by shijian at 2016.9.1, ������Ϸ˽�����ݲ�ѯ�ӿ�
		IID_Get_GameToolInfo			=19,
		// add by shijian at 2016.9.1, ������Ϸ˽������д��ӿ�
		IID_Set_GameToolInfo			=20,
	};
	virtual HRESULT TableDispatch(DWORD dwMark, LPVOID lpData, int iDataLen) = 0;
};

// * /LeonLv End 04Jun.2013

//�û���Ϣ�ṹ
struct tagUserData
{
	//�û�����
	char								szName[64];					//�û�����

	//�û�����
	LONG								lInsureScore;						//���ѽ��
	LONG								lGameGold;							//��Ϸ���
	LONG								lScore;								//�û�����
	LONG								lWinCount;							//ʤ������
	LONG								lLostCount;							//ʧ������
	LONG								lDrawCount;							//�;�����
	LONG								lFleeCount;							//������Ŀ
	LONG								lExperience;						//�û�����

	//�û�״̬
	WORD								wTableID;							//���Ӻ���
	WORD								wChairID;							//����λ��
	BYTE								cbUserStatus;						//�û�״̬

};

//������õ�TIMER����
struct STimerManage
{
	DWORD dwTimerID;
	DWORD dwElapse;
	DWORD dwRepeat;
	DWORD dwSetTick;
	WPARAM wBindParam;
	BOOL  bEnable;
};

//��Ϸ״̬
enum enGamePhase
{
	GS_FREE,			//����״̬
	GS_PLAYING,			//��Ϸ״̬
	GS_END              //��Ϸ����״̬
};

// * /���ڼ��㸣����ȼ������ԭʼ��������
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
	LONGLONG llExpAdd;	// ��������
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

// * /��д�����ṹ��Ϊ��չÿ�ּ�¼���ף�04Jun.2013
struct Game_Ext_SCORE 
{
	int nChairID;
	LONGLONG llWinLose;				// * /��Ӯ�Ļ���
	long lWins;						// * /Ӯ
	long lLose;						// * /��
	long lDrawn;					// * /ƽ
	long lRunOut;					// * /����
	LONGLONG llTax;					// * /���׶�
	LONGLONG llFreqPile;			// * /���ͻ���
	LONGLONG llExt1st;				// * /��չδʹ��1
	LONGLONG llExt2nd;
};
// * /��չд�����ӿڣ�04Jun.2013
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
	BYTE buffer[1024];	// * /������һλ(buffer[511])ֵΪ1����ʾ��Ϸ�����������������
};

#pragma warning (pop)
#pragma pack(pop)

#endif // !defined(AFX_TABLESERVER_H__198C012A_AE41_11D4_A045_0080AD315721__INCLUDED_)

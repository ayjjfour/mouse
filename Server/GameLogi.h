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

// * /������
struct tagServerName
{
	char szServerName[MAX_NAME_LEN];//32 == MAX_NAME_LEN
};

//ϵͳ���롢ϵͳ֧���������ӵ���������������ܶ�����ƽ�����������ʹ�����ȡ�������

// ͳ�Ƶ�ǰ�����������ӵ���֧
struct SRoomTotal
{
	stUserMoney  llSysIncome;						//ϵͳ����
	stUserMoney  llSysExpense;						//ϵͳ֧��
	LONGLONG     llSumAvoidHitGetScore;			    //�����ʹ��ܽ����
	LONGLONG     llBuySkillCost;					//�����ܡ��һ��ȹ������ģ�������Ϸ���ѣ�
	LONGLONG     llBuySkillReturn;					//�����ܷ������һ���������

	SRoomTotal()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}
};

// �û�ͳ������
struct PlayerStatisticsData
{
	stUserMoney	llUserCost;		        //��ұ������ѣ�������Ϸ���ѣ��������������ģ�
	stUserMoney	llUserIncome;           //��ұ�������
	stUserMoney	llIntervalIncome;		//ϵͳʱ������루PreWriteScore�����
	stUserMoney	llIntervalExpense;		//ϵͳʱ���֧��
	LONGLONG	llAvoidHitSystemGetScore;   //���˹���ϵͳ��ȡ����� 

	LONGLONG    llBuySkillCost;         //�����ܡ��һ��ȹ������ģ�������Ϸ���ѣ�
	LONGLONG    llBuySkillReturn;       //�����ܷ������һ���������

	stUserMoney	llUserCarryScore;		//��ҽ���ʱЯ���Ľ��

	DWORD		nUserEnterRoomTick;		//��ҽ��뷿���tick,Ŀ�����ڼ�¼����˳�����ʱ�Ƿ񳬹�3����
	DWORD		nUserExitRoomTick;		//����˳�����tick���ô�ͬ��

	DWORD       dwLastHitTime;            // �ϴδ��ʱ��


	//�������ɿͻ��˷��ͣ�����ͳ���ڽ�ɫ��
	int			m_nStatictisHitCounts;                          //��������
	int			m_nStatictisSkillTriggerCounts[SKILL_COUNT + 1];	//�����ܴ�������

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
	enSBS_None,     // δ��ǰ����
	enSBS_Buyed,    // ����ǰ����
	enSBS_Payed,    // ��ǰ�������ѷ���
};

// SY::SkillStatus
//enum enSkillActiveStatus
//{
//    enSAS_Unactived,     // δ����
//    enSAS_Actived,       // �Ѽ���
//    enSAS_Disabled,      // �Ѽ������
//};

// ��ҽ������ݣ��Զ������ݣ�
struct UserInteractData
{
	ULONGLONG llMoleNormalKillNum;        // uint ��ͨ����
	ULONGLONG llMoleKuangGongKillNum;     // uint �󹤵���
	ULONGLONG llFlyGoblinKillNum;         // uint ��������
	ULONGLONG llSpiderKillNum;            // uint ֩��
	ULONGLONG llFlyKillNum;               // uint ��Ӭ
	ULONGLONG llMoleKingKillNum;          // uint ����

	// ��������ÿһλ��ʾһ�����ܵ�ֵ
	unsigned int nSkillBuyStatus;      // enSkillBuyStatus    0δ����1�ѹ��� 2�ѷ���
	unsigned int nSkillActiveStatus;   // enSkillActiveStatus 0δ���1�Ѽ�� 2����ѽ��ã���״̬��ʱ�������ڷ�������
	unsigned int nSkillLevel;          // byte 0-4�����֧��0-15

	unsigned char nAutoAttackLevel;    // unsigned byte 0-10 �Զ��һ����ŵȼ�

	unsigned char auto_hammer_lv;      //unsigned byte  0-10 �Զ������ŵȼ�


//	unsigned char auto_hammer_active_status; // 0δ���� 1�Ѽ���  2�ѽ���

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
//��ʼĬ�ϵ�ǹ������
static int G_iMustShotProbability[5][3] = {{0,0,100},{0,30,70},{0,40,60},{60,40,0},{80,20,0}};
//��״̬��ʶ 2016.10.20 hl 
enum WILD_SKILL_STATUS
{
	WILD_SKILL_STATUS_BLANK = 1, //��״̬׼���ڼ� 1���Ӽ��
	WILD_SKILL_STATUS_WILL_START,  //���30������״̬
	WILD_SKILL_STATUS_START,       //�����״̬
	WILD_SKILL_STATUS_WILL_END,    //�񱩼������������3�룩
	WILD_SKILL_STATUS_END,         //�񱩽���
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
				, public IGameQuery			// * /�����ѯ�ӿڣ�04May.2012
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
	STDMETHOD(SetTableID)(int nTableID);	// * /LeonLv add:��ȡ����ID�Ľӿ�, 22Aug.2012

	// ��ȡ���������ID��add by Ԭ���� 2016-10-12
	STDMETHOD_(int, GetRoomID)();
	STDMETHOD_(int, GetTableID)();


	// * /protocol buffer using interface
	STDMETHOD(GetPersonalSceneProtBuf)(int nIndex, UINT uflags, SCENE * pScene);	// * /��ȡ�ض���ҵ�Protocol buffer��Ϸ�ֳ�

public:		//IPlayerIOEvent // * /
	STDMETHOD(OnUserEnter)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserExit)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserOffline)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserReplay)(int nIdx, IGamePlayer * pGamePlayer);
	STDMETHOD(OnUserReady)(int nIdx, IGamePlayer * pGamePlayer);	// * /Add Ready interface


public:			// * /INewTimer
	int			GetComingTimerTick(DWORD &dwComingID);						//�����´δ����ļ�ʱ��
	DWORD		CheckActiveTimer();							//��ѯӦ�ü���ļ�ʱ��
	bool		SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam);	//���ö�ʱ��
	bool		ReSetGameTimer(DWORD dwTimerID, DWORD dwElapse);							// * /�������ü�ʱ��ʱ��
	bool		KillGameTimer(DWORD dwTimerID);				//ɾ����ʱ��
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

public:	// * /IGameQuery�ӿڣ�֧�ִ����ѯ��04May.2012
	//����ֵ��S_OK����ʾ��֧�ָò�ѯ
	HRESULT QueryGameInfo(DWORD dwMark, LPVOID lpData);

public:	// * /�ʳز�ѯ�ӿ�
	STDMETHOD(SetPrizeInterface)(IPrizeFund * pPrize);
	virtual void SetAdditionalData( DWORD dwUserID,void* apData, int size );

private:
	IServerSite * m_pSite;
	CServerBase * m_pServer;
	ITableChannel* m_TableChannel;
	IUserList * m_pList;
	ITableChannel			*m_pTableChannel;						// * /��ͣ�����ʳء���ѯ�����������ӺŵĽӿڣ�27Apr.2011

	bool	m_started;

	DWORD					m_dwCurTick;					// * /��ǰʱ���������
	int				m_nPrivTimerFlag;				// * /�������еļ�ʱ�����
	STimerManage	m_arTimerManage[TIMER_COUNT + 1];     //�����TIMER����ṹ��

	clock_t  Curclock, Preclock;    

	IPrizeFund	* m_pPrize;		// * /�ʳر���
	int           player_index;

public:
	CGameServer();
	virtual ~CGameServer();

private:		//��Ϸ���ݣ���ν�ֳ�����������Щ�������
	int                     m_nTableUserCount;          //��¼���������������ǿյģ���Щ�߼��Ϳ��Բ���ִ���ˣ����ڿ��ٱȽϣ�
	//int	m_nGameState;			//��Ϸ״̬GAME_STARTED, GAME_STOPPED, GAME_CONSTRUCT
	//int m_nActivePlayer;		//��û���һ��ֻ��һ������û����ǿ��Գ��Ƶ��ˡ�
								//��¼���Ӻ�(1-4)��Ч

	TCHAR					m_szErrorInfo[512];			//�����õĴ�����Ϣ
	TCHAR					m_szDatalogInfo[512];       //���ݶ�д��Ϣ
	int						m_nTableID;					// ����ID
	int				        m_nRoomID;					//��Ϸ����ID
	LogicalHelper m_quickStartelper;
	enGamePhase		        m_enGamePhase;
	private:												//ϵͳ����
		char					m_userName[PLAY_COUNT][64];								//�������
		DWORD					m_dwUserID[PLAY_COUNT];										//��ҵ�IDֵ
		enPlayerState			m_enUserState[PLAY_COUNT];									// ��ҵ�״̬

		UserInteractData        m_UserPreInteractData[PLAY_COUNT];          // �ϴα���ʱ�Ľ�������
		UserInteractData        m_UserInteractData[PLAY_COUNT];             // ʵʱ�Ľ�������
		tagFishingData			m_FishingData[PLAY_COUNT];					// ���㹲������

		stUserMoney				m_UserPreGameScore[PLAY_COUNT];			    // ��ҵ��ϴ�ͳ��ʱ�Ľ����
		stUserMoney				m_UserGameScore[PLAY_COUNT];			    // ��ҵ�ʵʱ�����  

		double					m_dbUserPreWelFarePoint[PLAY_COUNT];        // ��ҵ��ϴθ�����  
		double					m_dbUserWelFarePoint[PLAY_COUNT];           // ��ҵ�ʵʱ������  

		int                     m_nUserLevel[PLAY_COUNT];                // ��ҵȼ�
		LONGLONG                m_llPreGradeScore[PLAY_COUNT];           // ��Ҵ�����ĵ���Ϸ��ͳ�ƣ���Ӧת��Ϊ���飬1��1��
		LONGLONG                m_llGradeScore[PLAY_COUNT];              // ��Ҵ�����ĵ���Ϸ��ͳ�ƣ���Ӧת��Ϊ���飬1��1��


		tagServerName		    m_zsServerName;														//������
		long					m_lDrawnCount[PLAY_COUNT];								//���پ�

private:							//��־��¼������
	CLog					m_LogFile;																	//��־�ļ���¼

	// �����������ͳ�� 
	PlayerStatisticsData        m_UserStatisticsData[PLAY_COUNT];                 // ͳ��������

	// ����ͳ���ܼ�
	LONGLONG	m_llSumIntervalIncome;			//ϵͳʱ�����루ͳ�Ƶ�ǰ����
	stUserMoney	m_llSumIntervalExpense;   			//ϵͳʱ��֧����ͳ�Ƶ�ǰ����
	LONGLONG	m_llSumIncome;                    //ϵͳ���루ͳ�Ƶ�ǰ����
	stUserMoney	m_llSumExpense;					//ϵͳ֧����ͳ�Ƶ�ǰ����
	LONGLONG	m_llSumAvoidHitSystemGetScore;  //ϵͳ���˻�����棨ͳ�Ƶ�ǰ����


	//����ͳ��      
	LONGLONG	m_nStatictisAvoidCounts;		    //�����жϴ���
	LONGLONG	m_nStatictisAvoidValidCounts;		//������Ч���������������ʣ�
	LONGLONG	m_nStatictisMonsterKillCounts[MonsterTypeCount];		//�����ɱ�ܴ����������Ǳ����˵Ĺ֣�
	LONGLONG	m_nStatictisMonsterIncomingCounts[MonsterTypeCount];	//��������������������ɱ�ʣ�

	// ����ع�����ʱ�������Թۺ�Ч
	//ÿһ�ֹ����Ͷ��ɱ�
	LONGLONG					m_nMonsterPlayerCost[MonsterTypeCount];
	//ÿһ�ֹ��������				
	LONGLONG					m_nMonsterPlayerIncome[MonsterTypeCount];


private:                    // �������
	int                     wild_skill_blank;
	int						m_nSceneIndex;			//��������	
	bool					m_bIsPlaying;		    //��Ϸ�Ƿ�ʼ

	bool                    m_bAlarmUser[PLAY_COUNT];    //Σ���û� 
	bool					m_bOnlineChair[PLAY_COUNT];			// �������     
	int                     m_nBroadcastTime[PLAY_COUNT];          // �ϴβ���ʱ��
	bool                    wild_skill_status;                   //�Ƿ��ǿ�״̬
	//
	time_t m_LastTime; 
	char m_GameNo[28]; 
	int m_nGameIndx;              
	char m_RecordLogPath[MAX_PATH];  

	bool m_bEnableInfoLog;
	long m_iLogID;

private:					// ͳ����־
	CStatistic*				m_pStatistic;				
	
public:
	void WriteInfoLog(CString szMsg);

	void WriteKillLog(CString szmsg);

public:
	/// ��¼�����������ʱ��	
	void	RecordStartTime();

	///ˢ�¼�¼��־·��
	void UpdateRecordLogPath();
	//������־��
	bool GenerateRecordLogName(std::string& aName);

	///��ȡ��־·��
	//void GetRecordLogPath( char* apFile, char* apPath, int aSize );

	/// ɱ���м�ʱ��	
	void	KillAllTimer();
	/// ���ݳ�ʼ��	
	void	InitData();

private:
	void CreateMonster();           // �������һ�������Ĺ���
	void CreateGroupMonsters();     // ����һ�����                       

	// ��ʱ���ٹ���
	void OnTimerDestroyMonsters();
	// �����л�������Ϣ	
	void OnTimerChangeScene();
	// ��齱��
	void OnTimerCheckPool();

	//��״̬��ؼ�ʱ���ӿں������ӿ� start 2016.10.20 hl
	//��״̬����
	void wild_skill_state_end();
	//��״̬��������
	void wild_skill_will_end();
	//��״̬����ʱ�׶�
	void wild_skill_wait_step();

	//�񱩽׶ζ�ʱ����ʱ����

	void wild_skill_timer_deley();

	//���ڼ�������ڼ�ˢ��ʱ�����

	void change_monster_time(float rate = 1);
	//��ʾ��Ϣchar2utf8
	void make_tip_info(const int wild_skill_status,const int player_index_id,const int remain_time = 0);
	//���ڼ��������Ѳ���
	void send_weapon_cost_ex(const int modify_cost,const int player_index_id);

	//��ȡ�Ƿ��״̬��
	bool wild_monster(const int monster_id);

	//��״̬��ؼ�ʱ���ӿں������ӿ� end 2016.10.20 hl
	//�����	
	bool OnBuySkill(IGamePlayer *pPlayer, void *pData, int nSize);
	//�����Զ������ȼ�	
	bool OnBuyAutoAttack(IGamePlayer *pPlayer, void *pData, int nSize);

	//��ȡ������  ����
	float get_wild_config_val(GET_CONFIG_VAL type);
	//�����ӵȼ� 2016.10.18 hl

	bool OnBuyAutoHammer(IGamePlayer * pPlayer,void * pData,int nSize);


	//�޸ļ���״̬
	bool OnChangeSkillStatus(IGamePlayer *pPlayer, void *pData, int nSize);
	//�л�����	
	bool OnChangeWeapon(IGamePlayer *pPlayer, void *pData, int nSize);
	//��������	
	bool OnUpgradeSkill(IGamePlayer *pPlayer, void *pData, int nSize);
	// ��Ҵ������
	void OnPlayerHit(IGamePlayer *pPlayer,void *pBuffer ,int nSize);
	// ���Ӿ��飬���ܻ�����
	void AddUserExp(IGamePlayer* pPlayer, int nExpAdd);
	// �ж��Ƿ�����Զ������
	void CheckCanAutoActiveSkill(IGamePlayer* pPlayer, bool bFromeInit = false);
	//�ж��Ƿ���Ҫ�������
	void CheckCanReturnUserCoin(IGamePlayer* pPlayer);

	// ���������㣬һ����Ϊͬһ���͹���������������
	// WeaponCost�� ������������
	// nCount�� ����������Ĵ���
	// isIncoming�� true����ʾ��Ҫ�������棬 false����ʾֱ�ӿ۵������Ѽ���
	void CalcPlayerIncoming(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, MonsterType monsterType, WeaponID weaponID, vector<stMonsterIncoming>& lstMonsterIncoming, 
							bool isIncoming, stIncoming& UserIncoming, Change_t& Change,
							vector<SY::MonsterIncoming>& lstMonsterReback);

	void BroadcastMessage(IGamePlayer* pPlayer, const char* txt, const char* userName, WeaponID weaponID, int nKillRate, const char* monsterName, int rewardMoney, int rewardTicket);

	//������Ϸ���
	void buildGameNo();


	//���㹲��������Ϣ
	bool GetFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData);
	bool SaveFishingCommonData(DWORD dwChairID, DWORD dwUserID, tagFishingData* pData);
	bool ValidateFishingCommonData(tagFishingData* pData);

	//��Ϸ˽������
	bool GetCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData);
	bool SaveCustomData(DWORD dwChairID, DWORD dwUserID, UserInteractData* pData);
	
	//��һ��������߼�
	BOOL _is_CriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit);
	void _OnPlayerHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit, BOOL isCriticalHit);
	void _PlayerHitKing(IGamePlayer *pPlayer, SY::PlayerHit& playerhit);
	void _PlayerNotCriticalHit(IGamePlayer *pPlayer, SY::PlayerHit& PlayerHit);

	//------------------------------------------------------��Ϸ�߼����-----------------------------------------------------
public:			
	//��ȡ������Ϣ
	void InitRoomInfo();
	//������Ϸ״̬
	enGamePhase	SetGameStatus(enGamePhase emGamePase);	
	//��Ч�����Ӻ�
	bool CheckValidChairID(int nChairID);
	//�����ҵķ���
	LONGLONG GetPlayerScore(int nChair, IGamePlayer *pPlayer);
	// ��ȡ��Ҿ��顢�ȼ�
	LONGLONG GetPlayerExp(int nChair, IGamePlayer *pPlayer, int* pLevel);
	//�������
	bool CalcScore(IGamePlayer *pPlayer, int nChairID);
	//ʹ����չд�ֽӿڣ���¼˰�ʹ��׺ͳ��ͷ�
	bool WriteExitGameScore(IGamePlayer *pPlayer, int nChairID, long lDrawn, const stUserMoney& llWinScore, LONGLONG llTax, LONGLONG llFreqScore);
	//Ԥ������ҵ÷�
	bool CalcPreScore(int nChairID);
	void RecordServerLog(DWORD dwChairID, int UserDBID);
	//��GameServer��DBд����Ԥ����ֵȹؼ���Ϣ
	bool WritePreGameScore(DWORD dwChairID, const stUserMoney& llPreWinScore, LONGLONG llPreTotalScore, LONGLONG llGradeScore);

	//------------------------------------------------------��Ϸ��Ϣ�������-----------------------------------------------------
public:																				
	// ���ͽ�Ҹ�ָ�����
	// ��ʾʵʱ����
	void SendUserMoneyCountTo(BYTE byChairID, IGamePlayer *pPlayer, bool bRealTimeMoney = false);
	// �������������¼������
	void SendCriticalStrikeTo(IGamePlayer *pPlayer, const stUserMoney& UserMoney, SY::PlayerHit& PlayerHit, int nWeaponKingCost);
	// ������켣��Ϣ�����
	void SendAddFishCmd( int nCmd , char *pData , int nLen);
	//������Ļ�ֱ��ʸ����
	void SendScreenResolution(int iWidth,int iHeight);
	//�����û�������Ϣ
	void SendBasicInfo(DWORD byChairID, IGamePlayer *pPlayer);
	// * /����ͨ�ò���ȼ�, 27Oct.2015
	int SendGeneralLevel(int nChairID, IGamePlayer *pPlayer);

	//------------------------------------------------------protobuf��Ϣ���ͽ������-----------------------------------------------------

public:
	// * /protocol buffer framework send
	bool sendGameCmdProtBufAll(int aCmdId, void *apData, int aLen);                        //������Ϣ��ȫ�����
	bool sendGameCmdProtBufTo(int nInx,int cmdId, void* apData,int nLen);					 //������Ϣ��ָ�����
	bool sendGameCmdProtBufExcept(int nInx, int cmdId, void* apData,int nLen);				//������Ϣ��������������

	bool SendAdditionalGameData(int nInx);
	bool SendAdditionalGameData(int nInx, byte type);

	//�ֻ��洦���Ϸ�
	void MoblieDealUserAddScore(int ChairID);
	//�ֻ��洦������·�
	void MoblieDealUserRemoveSocer(EnableLeaveQuery *pEnableLeaveQuery,IGamePlayer *pPlayer);

	//------------------------------------------------------ͳ����־������-----------------------------------------------------
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

#pragma once

#include "Protocol/GameMessage.pb.h"
#include <list>

const static int PLAY_COUNT = 1;		// * /������Ϸ�����
const static int SKILL_COUNT = 4;       // ������          SkillTypeCount
const static int WEAPON_COUNT = 5;      // ����������      WeaponTypeCount

#define ROOM_COUNT 5        //������
#define PI          3.1415926    //
//#define FISH_TYPE_COUNT 25    //�������� 
#define AlarmlScore   5000    //�������������ÿ��д�ֵ�ʱ�����ֵ���ڴ�ֵ ��ô����Ϊ�������ˢ�ֽ����߳�

//���������
//#define GAME_PATH							30000600						/** ���� ID */
static char szTempStr[MAX_PATH] =		{0};			/** �ݴ��ַ�����������ȡ dll_name �ȵ�ʱ��ʹ�� */
#define GAME_PATH							"ServerWhacAMole\\WhacAMole"						/** ���� ID */
#define GAME_SERVER_FOLDER(A)				(sprintf(A,"%s",GAME_PATH),strcat(A,""))


/////////////////�����TIMER////////////////////////
#define STANDARD_TIME_ELAPSE		1000					//��׼ʱ����
#define MAX_TICK_COUNT				1000000000		//����ʱ��������

#define DEFAULT_WILD_SKILL_ELSPSE   60*1000  //Ĭ��30��һ�Σ���Ҫ����Ϊ�����30��Ҫ�ر���ʾ
#define DEFAULT_WILD_SKILL_PRETIP_TIME  3  //Ĭ����ǰ֪ͨ��״̬����ʱ��4��
enum TimerID
{
	TIMER_NONE = 0,
	TIME_ADD_MONSTER,					    //���һֻ��
	TIME_ADD_GROUP_MONSTER,					//����һ���
	TIME_CLEAR_MONSTER,						//��ʱ�����Ҫ���ٵĹ�
	TIME_CHANGE_SCENE,						//�л�����
	TIME_LOAD_FILE,							//���������ļ�
	TIME_CHECK_POOL,						//��齱��
	TIME_INTERVAL_PLAYER_ONE,		    	//���1ʱ�δ��
	TIME_INTERVAL_PLAYER_TWO,		    	//���2ʱ�δ��
	TIME_INTERVAL_PLAYER_THREE,		    	//���3ʱ�δ��
	TIME_INTERVAL_PLAYER_FOUR,		    	//���4ʱ�δ��

	TIME_WILD_SKILL_DELEY,                    //��״̬�ͻ��˵�һ�������ղ�����Ϣ ��ʱ5�뿪ʼ�����
	TIME_WILD_SKILL_WAIT_STEP,                  //�񱩼��ܴ�����ʱ Ĭ�ϼ��1���� hl 2016.10.20
	TIME_WILD_SKILL_WILL_END,               //��״̬����ʱ�䵹��ʱ����ǰ?��֪ͨ�ͻ��ˣ�hl 2016.10.20  
	TIME_WILD_SKILL_END,                    //��״̬����ʱ�����
	TIME_WILD_END_CREATE_MONSTER,           //�񱩽�����ǿ����һ��ˢ��

	TIME_COUNT                              //��ʱ������  10 
};

const static int TIMER_COUNT = TIME_COUNT;  //��ʱ������

////////////////////////////////////////////////////////////////////////

static const int CLIENT_WIDTH = 1920; // 1280;	//��Ļ��
static const int CLIENT_HEIGHT = 1080; // 720;	//��Ļ��

const int DIFF_WIDTH =20;		//��Ĳ�ֵ
const int DIFF_HEIGHT = 10;   //�ߵĲ�ֵ

// * /LeonLv add for pack , 19Feb.2011
#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)


//------------------------------------------------------------------------------------
//��ʼ�켣��ͽǶ�
//struct FishTrace
//{
//	float x;														//���X����
//	float y;														//���Y����
//	float fRotation;											//��ǰ�Ƕ�
//	double fMoveTime;										//�ƶ�ʱ�䱣������켣�˶���ʱ��  
//	float fChangeTime;									//�ı�ʱ�䣬��֮ǰ�ĽǶȸı䵽���ڽǶȹ涨�����ʱ�������
//	bool  bExist ;										//�����Ƿ���ڣ�Ĭ��Ϊ���ھ����������Ĭ�ϲ�����
//	unsigned int   nFishID;												//��������Ӧ��FISH��ID��
//	int   nFishType;											//��������Ӧ��FISH������
//	int   m_nPtIndex;										//���ζ����������
//	int   nBuildTime;										//������ʱ��
//	int   nHitCount;											//���д���
//	bool  bFirstHit[PLAY_COUNT];									//����״λ���
//	int	  nHitCountID[PLAY_COUNT];								// ��һ��д���
//	int	  nLastMultiple[PLAY_COUNT];						//���һ�λ�����ʱ����Ϸ����
//	float fAccamulateShootCountRatio[PLAY_COUNT];				//���ۼ���������
//	int   nHitBaseMaxCount;									//������е�������
//	bool  bCountedFull[PLAY_COUNT];										//�ۻ������������ 
//	int   nTakeFishType;									//����Ϊ21��22������Ը���С��
//	int   nSpeed;												//�ٶ�
//	bool  bMustdieZero;											//0�ֱ���������
//	int   debugCodeRow;                                         //���ڵ���
//	unsigned int   iGroupIndex;                                          //ͬ�������������һ���㣬������ֵ��ͬ
//	bool  bOneNetAllDie;										//һ����
//	bool  bInScreen;                                             //����Ļ��
//
//};

struct SkillInfo
{
	BYTE                nSkillID;
	SY::SkillStatus     SkilllState;
};

//------------------------------------------------------------------------------------
// ��Ϸ�������ݰ����������е����ݼ̳иýṹ�����ݰ�	
struct GameStationBase
{
	bool				bPlaying;								//��Ϸ�Ƿ��Ѿ���ʼ
	BYTE     			bySceneIndex;							//��ǰ����
	bool				bOnLineChair[PLAY_COUNT];				//����״̬
	int					nUserMoneyNum[PLAY_COUNT];				//�����Ϸ��
	int					nUserTicketNum[PLAY_COUNT];			    //�����Ʊ
	int					nWeaponCost[WEAPON_COUNT];				//��������
	SkillInfo			SkillInf[SKILL_COUNT];					//����״̬
};
////----------------------------------------------------------------------------------------------
////�Ϸֺ��·�
//struct CMD_UserScore
//{
//	bool							bBuy;										//�Ϸ��Ƿ�ɹ�
//	BYTE							byChairID;									//�Ϸֲ��������
//	int  						    nBuyScore;									//�Ϸ���
//	int							nAllScore;										//��������ӵ���
//	LONGLONG				llTotalBaseScore;							        //���ɹ�����ٷ֣�����ҵ�ʣ�������
//};
//----------------------------------------------------------------------------------------------

//struct SkillHit
//{
//    BYTE            bySkillID;              // �����ļ���
//    BYTE            nMonsterCount;          // ���еĹ������
//    int             MonsterList[0];         // ���еĹ���ID
//};
//
////�����ڵ�	
//struct CMD_UserShoot
//{
//	BYTE			byChairID;					//�����λ��
//	BYTE    		byWeaponID;					//��������������
//	int 			nMonsterID;					//����Ŀ��
//	bool			bIsRobot;					//�ж��Ƿ��ǻ�����
//    BYTE            nSkillCount;                //�����ļ��ܸ���
//    SkillHit*       SkillHitList;               //�����б�
//};

////----------------------------------------------------------------------------------------------
////�����ڻ���	
//struct CMD_ChangeWeapon
//{
//	BYTE			byChairID;					//������Ӻ�
//	BYTE			WeaponID;					//�л���Ŀ������
//};
//
////--------------------------------------------------------------------------------------
////�л�����	
//struct CMD_ChangeScene
//{
//    BYTE     		wSceneIndex;				//��ǰ����
//    int             wRmoveID;                   //Ԥ������
//};
////------------------------------------------------------------------------------------------------------
////����켣	
//struct CMD_FishTrace
//{
//	bool									bRegular;								//�����ʶ
//	FishTrace                          m_fishtrace[5];
//};
////----------------------------------------------------------------------------------------
////�����Ŀ
//struct CMD_UserMoneyCount
//{
//	BYTE			byChairID;								//�������
//	LONGLONG		llMoneyCount;							//�����Ŀ
//    LONGLONG		llTicketCount;							//�����Ŀ
//};

//////////////////////////////////////////////////////////////////////////
// �������ڲ�ʹ�ýṹ

// ��������
struct stSkillHit
{
	int nSkillID;
	list<int> lstMonster;

	stSkillHit()
	{
		nSkillID = 0;
	}
};

// ��Ҵ��
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
	int	 wild_king_client_not_die;//���ڼ������ͻ���û������������ˣ����һ��������Ǯ
	bool king_prob;
	stDeadMonster(int id, int type,bool is_wild_monster,int extern_money, bool isKingProb):monster_id(id), monster_type(type)
							,be_wild_monster(is_wild_monster),
							wild_king_client_not_die(extern_money),
							king_prob(isKingProb)
	{

	}
};

// ��ҵ�Ǯ������
struct stUserMoney
{
	LONGLONG    llMoneyNum;             //��ң����֣�
	LONGLONG    llTicketNum;           //��Ʊ��

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


// �����Ϸ�еĽ׶ζ���
enum enPlayerState 
{
	enPlayerNull = 0,					// û�����
	enPlayerEnter,						//��ҽ���
	enPlayerBuy,							//�ѹ����ӵ�
	enPlayerSettled,					//�ѽ���
};

// ������ҵ��Զ�������
enum enAdditionData_Type
{
	enADT_None = 0,

	// Ϊ��֤����ǰ����Ĳ���ͻ�����Դ�101��ʼ

	enADT_MoleNormalKillNum = 101,        // uint ��ͨ����
	enADT_MoleKuangGongKillNum,     // uint �󹤵���
	enADT_FlyGoblinKillNum,         // uint ��������
	enADT_SpiderKillNum,            // uint ֩��
	enADT_FlyKillNum,               // uint ��Ӭ
	enADT_MoleKingKillNum,          // uint ����

	enADT_SkillBuyStatus = 111,      // byte 0δ����1�ѹ��� 2�ѷ�����bit��
	enADT_SkillActiveStatus,         // byte 0δ���1�Ѽ���         ��bit��
	enADT_SkillLevel,                // byte 0-4                    ��bit��

	enADT_AutoAttackLevel = 121,     // byte 0-10 �Զ��һ����ŵȼ�

	enADT_AutoHammerLevel = 131,    //byte   0-10 �Զ������ŵȼ�   
};


/************************************************************************/

#pragma warning (pop)
#pragma pack(pop)
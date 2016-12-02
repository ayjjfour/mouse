#ifndef __TRANS_STRUCT_H__
#define __TRANS_STRUCT_H__

#pragma pack(push)
#pragma pack(1)

#pragma warning (push)
#pragma warning (disable : 4200)

#define GAME_ID 83	// * /��ϷID��
#define GAME_NAME "���Ķ�"

#define PLAYER_COUNT	4 // * /������Ϸ�����

//��ϢID����С��128��128���ϵ�ID��ϵͳ����
enum MSG_ID
{
	MSG_GAME_READY = 10,
};

// * /frame size
#define GAME_ZONE_WIDTH		1026	//1024	// * /frame�߿��ռ��2������
#define GAME_ZONE_HEIGHT	597		//595	// * /���У��������߶�44��ԭ43��

// * /������Ч����ߴ�Ϊ��1024��576, 10Sep.2012
#define SEA_SCENE_WITDH		1024
#define SEA_SCENE_HEIGHT	564	//����563����̨���µ�ƫ��Ҫ���

#define IsLegalChair(x) (0 <= (x) && (x) < PLAY_COUNT)

#define MAX_SHOW_FISH_COUNT		100// *V2012 /80				// * /�ͻ��������ʾ�������
#define MAX_BULLET_COUNT				20										//����ӵ���   
#define MAX_FISH_TRACE_COUNT			500										//�������켣��
#define MAX_FISH_KIND					25							//�����������
#define UserCellMultipleRate            100                      //ת����Ԫ�ֵ���Ϸ����
#define EXERCIZE_ROOM                    1                       //��ϰ��
#define HUNDRED_ROOM                     2						//���ڷ�
#define THOUSAND_ROOM					 3						//ǧ�ڷ�
#define TEN_THOUSAND_ROOM                4						//���ڷ�
#define MIN_SHOOT_TIME                   200                   //���ʱ��̼��ʱ��(��λ:200ms)
#define Max_Broadcast_TIME               3*1000                //��󲥱����ʱ��

#define DEBUG_SCORE                      1                      //
#define DEBUG_SHOOT                      1                     //

#define EXTEND_DATA_COUNT			20			//������չ�����ݸ���

// * /LeonLv add Start:�����๲��������Ϸ�ӿڣ�26Oct.2012
#define NUM_FISHING_DATA_LEN	1024		// * /���ݿ�洢�����๲�����ݵĴ�С

// * /��Ϸ֧�ֵĲ�ѯ���֣�����֧��GameServer��ѯ
enum{
	emSupptNoCheck	= -1,	// * /δ���
	emSupptZero		=  0,	// * /��֧��
	emSupptFishData =  1,	// * /֧�ֲ�����2.5.0֮��Ĺ�������
};

///////////////////////////////////////////////////////////
////��Ϸ����Ŷ���
////          1         0         1         1
//          δʹ��    δʹ��    �׷�ϵ��  �׷�����
//   
//    ���磺��������1011���׷�=�׷�ϵ��(1)��10^�׷�����(1) = 1��10 = 10
///////////////////////////////////////////////////////////


// ��Ҫɾ��������壬���ڽ������Ϸ��
const unsigned char GAMENOTIFY_CLIENT_SHOULD_CLOSE=0x73;
const unsigned char GAMENOTIFY_CLIENT_GAME_INFO = 0x74;	// * /��Ϸ��ҵ���ʾ��Ϣ

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
	int cbSize;				//�����ṹ�Ĵ�С������������scene�ĵ�һ��Ԫ��

public:			// methods
	void init() {
		// TODO: ������Լ��ĳ�ʼ������
	}
};

//��ϢID����С��128��128���ϵ�ID��ϵͳ����

//��Ϸʹ�õķ����ṹ
struct Game_SCORE {
	long lScore;					//����, �ڽ����Ϸ�о��ǽ����
	long lWins;						//Ӯ
	long lLose;						//��
	long lDrawn;					//ƽ
	long lRunOut;					//����

	void Clear() {
		ZeroMemory(this, sizeof(Game_SCORE));
	}
};

//��Ϸʹ�õľ���
struct Game_EXP 
{
	//int nLevel;						//�ȼ�
	LONGLONG llExp;					//����ֵ

	Game_EXP()
	{
		Clear();
	}

	void Clear() {
		ZeroMemory(this, sizeof(Game_EXP));
	}
};

//������Ϸ��ʱд�����ȹؼ�Ԥ�����¼
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
	LONGLONG	llWinScore;				//ԤӮ�����
	LONGLONG	llRemainScore;			//Ԥ�������
	LONGLONG	llGradeScore;			//��ҵȼ���
};















enum emPropNameID
{
    enPropNameNone		= 0,
    enPropNameFullCan	= 1,	// * /ȫ����ID
    enPropNameGoldSkCan	= 2,	// * /������ID
    enPropNamePointGoldCan	= 3,	// * /�����ID
	enPropNameSkillBuy	= 4,	// ���Ķ����ܹ���״̬
	enPropNameSkillActive	= 5,	// ���Ķ����ܼ���״̬
	enPropNameSkillLevel	= 6,	// ���Ķ����ܵȼ�
	enPropNameAutoAttack	= 7,	// �Զ��һ��ȼ�
    enPropNameEnd,				// * /20Feb.2013
};

// * /LeonLv add:��������¼��17May.2011
struct SRecEnergyBar 
{
    WORD wCellScore;		// ����
    WORD wEnergyBar1st;
    WORD wEnergyBar2nd;
    WORD wEnergyBar3rd;
    DWORD dwLastGameTime;	// * /��¼�����α�����ʱ���
};

// * /�������䱣�����ݣ�26Jul.2011
struct SWelfareData 
{
    DWORD dwLastSaveTime;	// * /
    WORD wGetWellCount;		// * /��ȡ�����Ĵ���
};

// * /�洢�����ݿ����Ҽ�¼��GameServer�ӿ�Ϊÿ������ṩ512BYTE�Ļ��湩��Ϸ�Զ���ʹ�ã�17May.2011
struct SGameData 
{
    WORD			wRecEnergyBarCount;		// * /�����ӵ�е���������¼��������Ч�ı�������
    SRecEnergyBar	arRecEnergyBar[20];		// * /��¼�Ǹ��䳤����
    SWelfareData	sWelData;				// * /�����������ݼ�¼
};

// * /���߼�¼�������ڵȣ�
struct tagPropRec 
{
	tagPropRec()
	{
		enPropID = (emPropNameID)0;
		byStatus = 0;
	}
	emPropNameID	enPropID;		// * /����ID
	BYTE			byStatus;		// * /����״̬
};

//��ͬ�����Ŀ���ֵ����
struct tagLossPool
{
	tagLossPool()
	{
		bUse = false;
		dwLossPool = 0;
		lCellScore = 0;
	}

	bool				bUse;						//�Ƿ�ʹ��
	DWORD			dwLossPool;			//��¼��ҵĿ���ֵ
	LONG				lCellScore;				//����
};

//��������Ϸ֧�ֹ�������
struct  tagFishingData
{
	LONGLONG	llVersion;				//���ݶ���汾�ţ����巽ʽ���£�����������ϷIDռnλ������1000000000�����汾��ռ3λ������1000000���ΰ汾ռ3λ������1000����Ͱ汾3λ�������������2.5.0Ϊ����Ϊ31002005000
	WORD			wDataLen;					//�������ݵĳ��ȣ�ÿ���汾����Ķ�Ӧ���ǹ̶�ֵ������Ϊ��������
	SGameData	sEnyGameData;		//V2.5.0֮ǰʹ�õ���������¼
	LONGLONG	llGradeScore;		//�ȼ��ּ�¼����Ҫת���ɵȼ�ʹ��
	tagPropRec	arPropRec[EXTEND_DATA_COUNT];		// * /���߼�¼����ʱ����20������
	tagLossPool  arLossPool[EXTEND_DATA_COUNT];		//��ҵĿ���ֵ�����
};

// * /��Ϸʹ�ò��㹲�����ݽӿ�
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

// add by shijian at 2016.9.1, ������Ϸ˽�����ݽӿ�
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
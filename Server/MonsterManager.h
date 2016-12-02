#pragma once    
#include "Monster.h"
#include <queue>
#include <map>

class CMonsterPool;
class CGameServer;

//////////////////////////////////////////////////////////////////////////

class CMonsterManager
{
public:
    CMonsterManager();
    ~CMonsterManager();

    static CMonsterManager& Instance()
    {
        static CMonsterManager _instance;
        return _instance;
    }

	void Attach(int nTableID, CGameServer* pServer);

	void Detach(int nTableID);

	void WriteInfoLog(int nTableID, CString szMsg);

    void Initialize(int nRoomID, int nTableID);     
    // ˢ�¹�� ��ָ�����ͣ�mtNone��ʱ��ˢ��ˢ�£� ��ˢ��ָ���Ĺ�������
    //CMonster* CreateMonster(int nTableID);
    void CreateMonster(int nTableID, list<CMonster*>& lstMonster,bool is_wild_monster, float monster_destroy_time_rate = 1.0,MonsterType monsterType = mtNone);
    // һ��ˢ��һ��֣�����ָ�����������ͣ���ָ������(monsterType)ʱ�����������
    void CreateGroupMonster(int nTableID, int nCount, list<CMonster*>& lstMonster,bool is_wild_monster,float destory_monster_speed  = 1.0, MonsterType monsterType = mtNone);

    // �����´γ��ֵĹ���
    // ˢ�»��ƣ�Ŀǰ��ÿ������һ��ˢ1-Nֻ�� ˢ�·�ʽ���÷�ʽ2�� Ч�ʸ���

    // 1.���й��ﹲ��ˢ�¼��ʣ���ͳ���ܳ��ֵ����й֣�Ȼ��һ�������һ�β���һֻ 
    //MonsterType GetNextRandomMonsterType(int nTableID);
    
    // 2.��ͬ���͵Ĺ֣�����ˢ�£�һ�ο��ܻ������ֻ
    //      ������һ��ˢ�£� 
    //      ֩�롢��Ӭ����һ��ˢ�£� 
    //      С���������ʣ�
    //      �����BOSS������ûˢ����ˢ��BOSS������ˢ��
    bool GetNextRandomMonsterGroupType(int nTableID, list<MonsterType>& lstMonsterType);

    // �ж���Щ�ֿ���������
    void CheckMonsterToDestroy(int nTableID);

    // ע���ڴ˺���֮��Monster�Ѿ����٣������Ҫ���ٴ˹��������Ϣ��ֻ���ڴ�ʱ��ȡ
    // ���ش����������ΪmtNone��ʾδ��������None���ʾ���Լ�������
    MonsterType PlayerHit(int nTableID, DWORD dwMonsterID,bool is_wild_status);

	//���ع��Ƿ�����������trueΪ�������� falseΪ��������Ǯ
	bool monster_hit_dead(int nTableID, DWORD dwMonsterID);
    //////////////////////////////////////////////////////////////////////////
    // ��ȡ����
    const CommonMonsterProperty* GetMonsterConfig(MonsterType monsterType);


    //////////////////////////////////////////////////////////////////////////
    // Pool���
    CMonsterPool* GetMonsterPool(int nTableID);
    CMonster* GetMonster(int nTableID, DWORD id /*GameObject obj*/);
	MonsterType GetMonsterType(int nTableID, DWORD id);
    void DestroyMonster(int nTableID, DWORD id);
    void DestroyAllMonster(int nTableID);
    std::string GetPoolState(int nTableID);

    // ��monsterTypeΪCountʱ�����������������򷵻ض�Ӧ���͹�������
    WORD GetMonsterNum(int nTableID, MonsterType monsterType);

	//��ȡ��ǰ������id
	std::vector<DWORD> get_all_monster_id(DWORD table_id); 
    //////////////////////////////////////////////////////////////////////////
    // static����

    // ��ʱ�����ض�Monster
    //static void CALLBACK OnTimerDestroyMonster(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

    //static bool CanBeHitted(MonsterStatus status)
    //{
    //    return (status == msComeOut || status == msEscape /*|| status == msIdle*/);
    //}

    // ��ռ���Ĺ���
    static bool IsMole(MonsterType monsterType)
    {
        return (mtMonsterTypeFirst <= monsterType && monsterType <= mtMoleKing);
    }

    // [nMin, nMax)
    static int Random(int nMin, int nMax);
    // [nMin, nMax) ����С�����4λ
    static float Random(float fMin, float fMax);
    // [0, nMax) �����Ƿ����У��������� < nCheck��ʾ����
    static bool Prob(int nCheck, int nMax);

    // д���ĳ������壬���޸�
    // ������Ϸ���ڴ�С�ᶯ̬�仯�������е����򶼰��ٷֱ�������[0,1]
    // ��Ϸ���ڴ�С  ��  0, 0     1920*1080
    // ��Ϸ��������Ϊ :  ��310,��160,��310,��160  1300*760         ���� 0.1614583f  ���� 0.148148f
    // ��Ϸ�߽�����Ϊ��  100                                       ���� 0.052083f   ���� 0.092593f

    // �����������仯�����¼���һ�Σ��������ظ�����
    //static void GetPlayZone(RECT& rect);
    //static void GetOutOfScreenZone(RECT& rect);

private:
	bool		   m_bInitialized;
    int            m_nRoomID;

    vector<CMonsterPool*>  m_lstPool;   // ÿ�����Ӷ�Ӧһ�������
	std::map<int, CGameServer*> m_LogInfo;
};


//////////////////////////////////////////////////////////////////////////



class CMonsterPool
{
    // ��������صĴ�С��ֻ���ǵ���(������֩��Ͳ�Ӭ)���9�����ɡ�  
    // ����Ϊ�ش�С���һ����ʼֵ������ռ��������Ҫ���䣬��������ӣ����ﵽ���ֵ�ǲ���������չ���������ʧ��

public:
    CMonsterPool(const MonsterPoolConfig* config);
    ~CMonsterPool();

    bool destroying() const;    // �Ƿ���������ȫ������

    CMonster* CreateMonster(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed = 1);
    // ��monsterTypeΪCountʱ�����������������򷵻ض�Ӧ���͹�������
    WORD GetMonsterNum(MonsterType monsterType);

    void DestroyMonster(DWORD id);
    void DestroyAllMonsters();

    CMonster* GetMonster(DWORD id /*GameObject objMonster*/);
    void CheckMonsterToDestroy(vector<DWORD>& lstMonster);

	map<DWORD, CMonster*> get_table_monster_list();
	
    // ��������״̬��Ϣ
    std::string GetPoolState();

    //////////////////////////////////////////////////////////////////////////
    // ԭMapManager����
    // ���ڲ���ҪHole�Ŀ��Դ���
    bool Respawn(CMonster* monster, CHole* hole);
    void RemoveFromHole(int idx);
    bool IsHoleFull() const;
    CHole* GetIdleHole();

	//�ж�˳�����
	void GetDetermineSeq(bool IsMole, vector<int>& out);

private:
    void Initialize();
    void UnInitialize(); // �رն����

    bool IsFull(MonsterType monsterType);

    bool AppendSize(MonsterType monsterType);

    CMonster* Attach(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed = 1);



private:
    const MonsterPoolConfig* m_config;

    WORD m_defaultSize;    // �����Ĭ��ֵ          
    WORD m_maxSize;        // �������������Ϊ0����ʾ���޴�С
    WORD m_offsetSize;     // ÿ������Ĵ�С   
    bool m_destroying;     // �Ƿ�����ȫ������ 

    // ������������
    vector<CHole*>   m_lstHole;      // ��¼9��������Ϣ
    vector<CHole*>   m_lstIdleHole;  // 9�����󶴣���ǿ��Գ��������λ�ã�����ʱ���б����Ƴ�������ʱ����ӽ�ȥ��

	vector<int> m_vecMonsterSeq;
	vector<int> m_vecFlySeq;

    queue<CMole*> m_lstIdleMole; // ���е������
    queue<CSpider*> m_lstIdleSpider; // ����֩�����
    queue<CFly*> m_lstIdleFly; // ���в�Ӭ����    
    queue<CFlyGoblin*> m_lstIdleFlyGoblin; // ���е���С������

    map<DWORD, CMonster*> m_mapMonster; // Ŀǰ��ʹ�õĶ���    ,����idΪ����

    WORD m_curFlySize;       // ��Ӭ����ص�Ŀǰ����
    WORD m_curSpiderSize;    // ֩�����ص�Ŀǰ����
    WORD m_curFlyGoblinSize; // ����С������ص�Ŀǰ����
    WORD m_MonsterCount[MonsterTypeCount];   // ͳ�Ƹ��ֹ��ﵱǰ����   


    DWORD CreateMonsterID();
    DWORD CreateMonsterGroupID();
    DWORD m_dwMonsterID;			 //�ֵı�ʶID
    DWORD m_dwGroupID;               //��ֱ�ʶ

public:
	CGameServer* pServer;
	
	void WriteInfoLog(CString szMsg);
};

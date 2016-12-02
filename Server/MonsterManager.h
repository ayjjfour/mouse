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
    // 刷新怪物， 不指定类型（mtNone）时会刷机刷新， 或刷新指定的怪物类型
    //CMonster* CreateMonster(int nTableID);
    void CreateMonster(int nTableID, list<CMonster*>& lstMonster,bool is_wild_monster, float monster_destroy_time_rate = 1.0,MonsterType monsterType = mtNone);
    // 一次刷新一组怪，可以指定数量和类型，不指定类型(monsterType)时会随机产生怪
    void CreateGroupMonster(int nTableID, int nCount, list<CMonster*>& lstMonster,bool is_wild_monster,float destory_monster_speed  = 1.0, MonsterType monsterType = mtNone);

    // 计算下次出现的怪物
    // 刷新机制：目前是每多少秒一次刷1-N只， 刷新方式采用方式2， 效率更高

    // 1.所有怪物共用刷新几率，会统计能出现的所有怪，然后一起随机，一次产生一只 
    //MonsterType GetNextRandomMonsterType(int nTableID);
    
    // 2.不同类型的怪，按组刷新，一次可能会产生多只
    //      地鼠共用一个刷新， 
    //      蜘蛛、苍蝇共用一个刷新， 
    //      小妖独立概率，
    //      如果是BOSS场景且没刷新则刷新BOSS，否则不刷新
    bool GetNextRandomMonsterGroupType(int nTableID, list<MonsterType>& lstMonsterType);

    // 判断哪些怪可以销毁了
    void CheckMonsterToDestroy(int nTableID);

    // 注：在此函数之后，Monster已经销毁，如果需要销毁此怪物相关信息，只能在此时获取
    // 返回打击结果，如果为mtNone表示未打死，非None则表示可以计算收益
    MonsterType PlayerHit(int nTableID, DWORD dwMonsterID,bool is_wild_status);

	//返回怪是否是正常死亡true为正常死亡 false为非正常扣钱
	bool monster_hit_dead(int nTableID, DWORD dwMonsterID);
    //////////////////////////////////////////////////////////////////////////
    // 读取配置
    const CommonMonsterProperty* GetMonsterConfig(MonsterType monsterType);


    //////////////////////////////////////////////////////////////////////////
    // Pool相关
    CMonsterPool* GetMonsterPool(int nTableID);
    CMonster* GetMonster(int nTableID, DWORD id /*GameObject obj*/);
	MonsterType GetMonsterType(int nTableID, DWORD id);
    void DestroyMonster(int nTableID, DWORD id);
    void DestroyAllMonster(int nTableID);
    std::string GetPoolState(int nTableID);

    // 当monsterType为Count时返回所有数量，否则返回对应类型怪物数量
    WORD GetMonsterNum(int nTableID, MonsterType monsterType);

	//获取当前桌存活怪id
	std::vector<DWORD> get_all_monster_id(DWORD table_id); 
    //////////////////////////////////////////////////////////////////////////
    // static方法

    // 延时清理特定Monster
    //static void CALLBACK OnTimerDestroyMonster(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

    //static bool CanBeHitted(MonsterStatus status)
    //{
    //    return (status == msComeOut || status == msEscape /*|| status == msIdle*/);
    //}

    // 会占洞的怪物
    static bool IsMole(MonsterType monsterType)
    {
        return (mtMonsterTypeFirst <= monsterType && monsterType <= mtMoleKing);
    }

    // [nMin, nMax)
    static int Random(int nMin, int nMax);
    // [nMin, nMax) 保留小数点后4位
    static float Random(float fMin, float fMax);
    // [0, nMax) 计算是否命中，即随机结果 < nCheck表示命中
    static bool Prob(int nCheck, int nMax);

    // 写死的常量定义，待修改
    // 由于游戏窗口大小会动态变化，故所有的区域都按百分比来计算[0,1]
    // 游戏窗口大小  ：  0, 0     1920*1080
    // 游戏区域设置为 :  左310,上160,右310,下160  1300*760         左右 0.1614583f  上下 0.148148f
    // 游戏边界设置为：  100                                       左右 0.052083f   上下 0.092593f

    // 最好是在区域变化后，重新计算一次，避免多次重复计算
    //static void GetPlayZone(RECT& rect);
    //static void GetOutOfScreenZone(RECT& rect);

private:
	bool		   m_bInitialized;
    int            m_nRoomID;

    vector<CMonsterPool*>  m_lstPool;   // 每个桌子对应一个对象池
	std::map<int, CGameServer*> m_LogInfo;
};


//////////////////////////////////////////////////////////////////////////



class CMonsterPool
{
    // 创建对象池的大小，只考虑地鼠(不考虑蜘蛛和苍蝇)最大9个即可。  
    // 考虑为池大小设计一个初始值，当被占满后仍需要分配，则扩大池子，当达到最大值是不再允许扩展，即会分配失败

public:
    CMonsterPool(const MonsterPoolConfig* config);
    ~CMonsterPool();

    bool destroying() const;    // 是否正在销毁全部怪物

    CMonster* CreateMonster(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed = 1);
    // 当monsterType为Count时返回所有数量，否则返回对应类型怪物数量
    WORD GetMonsterNum(MonsterType monsterType);

    void DestroyMonster(DWORD id);
    void DestroyAllMonsters();

    CMonster* GetMonster(DWORD id /*GameObject objMonster*/);
    void CheckMonsterToDestroy(vector<DWORD>& lstMonster);

	map<DWORD, CMonster*> get_table_monster_list();
	
    // 输出对象池状态信息
    std::string GetPoolState();

    //////////////////////////////////////////////////////////////////////////
    // 原MapManager内容
    // 对于不需要Hole的可以传空
    bool Respawn(CMonster* monster, CHole* hole);
    void RemoveFromHole(int idx);
    bool IsHoleFull() const;
    CHole* GetIdleHole();

	//判定顺序相关
	void GetDetermineSeq(bool IsMole, vector<int>& out);

private:
    void Initialize();
    void UnInitialize(); // 关闭对象池

    bool IsFull(MonsterType monsterType);

    bool AppendSize(MonsterType monsterType);

    CMonster* Attach(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed = 1);



private:
    const MonsterPoolConfig* m_config;

    WORD m_defaultSize;    // 对象池默认值          
    WORD m_maxSize;        // 对象池最大，如果设为0，表示不限大小
    WORD m_offsetSize;     // 每次扩充的大小   
    bool m_destroying;     // 是否正在全部销毁 

    // 按桌子来管理
    vector<CHole*>   m_lstHole;      // 记录9个洞的信息
    vector<CHole*>   m_lstIdleHole;  // 9个地鼠洞，标记可以出生地鼠的位置（出生时多列表中移除，死亡时再添加进去）

	vector<int> m_vecMonsterSeq;
	vector<int> m_vecFlySeq;

    queue<CMole*> m_lstIdleMole; // 空闲地鼠对象
    queue<CSpider*> m_lstIdleSpider; // 空闲蜘蛛对象
    queue<CFly*> m_lstIdleFly; // 空闲苍蝇对象    
    queue<CFlyGoblin*> m_lstIdleFlyGoblin; // 空闲盗宝小妖对象

    map<DWORD, CMonster*> m_mapMonster; // 目前已使用的对象    ,怪物id为索引

    WORD m_curFlySize;       // 苍蝇对象池的目前上限
    WORD m_curSpiderSize;    // 蜘蛛对象池的目前上限
    WORD m_curFlyGoblinSize; // 盗宝小妖对象池的目前上限
    WORD m_MonsterCount[MonsterTypeCount];   // 统计各种怪物当前数量   


    DWORD CreateMonsterID();
    DWORD CreateMonsterGroupID();
    DWORD m_dwMonsterID;			 //怪的标识ID
    DWORD m_dwGroupID;               //组怪标识

public:
	CGameServer* pServer;
	
	void WriteInfoLog(CString szMsg);
};

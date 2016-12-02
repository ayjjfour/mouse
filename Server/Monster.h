#pragma once
#include "GameConfig.h"

using namespace std;

const static int MAX_HOLE = 9;  //地鼠的洞的数量
const static int defaultWidth = 1920;
const static int defaultHeight = 1080;
const static float fBorderDist[] = { 0.1614583f, 0.148148f, 0.1614583f, 0.148148f };
const static float fOuterDist = 0.092593f * 2;    

const static RECT rtPlayZone = { 
    LONG(defaultWidth * (-0.5f + fBorderDist[0])), 
    LONG(defaultHeight * (-0.5f + fBorderDist[1])), 
    LONG(defaultWidth * (0.5f - fBorderDist[2])),
    LONG(defaultHeight * (1.0f - fBorderDist[3])) };

const static RECT rtOutOfScreenZone = {
    LONG(defaultWidth * (-0.5f - fOuterDist)),
    LONG(defaultHeight * (-0.5f - fOuterDist)),
    LONG(defaultWidth * (0.5f + fOuterDist)),
    LONG(defaultHeight * (0.5f + fOuterDist)) };

const static int PrecisionNumber = 10000;     // 配制中的精度 小数点后4位

class CHole;  

enum MonsterStatus
{
    msInit,       // 已创建
    msRespawn,    // 准备出生
    msComeOut,    // 出现（可打击）
    //Idle,       // 出现一段时间待机（可打击） // 不再需要此状态
    //Hit,        // 被打击  // 不再需要此状态
    msEscape,     // 落回
    msDestroy,    // 销毁（可以重新初始化）
    MonsterStatusCount
};

class CMonster
{
public:
    CMonster();
    virtual ~CMonster() {}
    virtual void Born(CHole* _hole);
    virtual void Escape();
    
    // 如果死亡返回true， 否则返回false
    bool PlayerHit(bool is_wild_status);
    void Remove();
    void Destroy();

    // Monster对象池使用： 重新附加到新的GameObject
    // 即为Monster设置对应类型的值
    void Attach(/*GameObject obj, */ DWORD dwMonsterID, MonsterType _monsterType,bool is_wild_monster,float destory_monster_speed = 1);

    // Monster与GameObject分离
    void Detach();

public:
    DWORD id() const;
    short curHP() const;
    short totalHP() const;
    float moveSpeed() const;
    MonsterStatus status() const;
    MonsterType monsterType() const;
    CHole* hole() const;
    int holeidx() const;  
    float waittoescapetime() const;
    float escapetime() const;

    DWORD destroytime() const;
	bool innormal_hit() const;

protected:
    DWORD m_id;
    short m_curHP;
    short m_totalHP;
    MonsterStatus m_status;
    MonsterType m_monsterType;
    CHole* m_hole;
    float m_moveSpeed;   
    float m_fWaitToEscapeTime;
    float m_fEscapeTime;
	int m_nMaxLifeTime;
	bool innormal_dead;
    //float m_fDestroyTime;
    DWORD m_dwTickDestroy;  // 销毁间隔，超出此时间会销毁（距出生时间算起）


	friend class CMonsterPool;
};



//////////////////////////////////////////////////////////////////////////

// 地鼠类
class CMole : CMonster
{
public:
    // 仅用于对象池调用
    CMole() : CMonster() {}

    // 对于地鼠应提供出生的洞的信息
    //virtual void Born(CHole* _hole);  
    //virtual void Escape();
};

// 苍蝇
class CFly : CMonster
{
public:
    CFly() : CMonster() {}

    // 对于地鼠应提供出生的洞的信息，其它飞行类创建出生、飞行、逃跑坐标
    virtual void Born(CHole* _hole);  
    //virtual void Escape();


    const POINT& pt_respawn() const;
    //void set_respawn(int x, int y);
    const POINT& pt_move() const;
    //void set_move(int x, int y);
    const POINT& pt_escape() const;
    //void set_escape(int x, int y);

protected:
    POINT   ptRespawn;  // 出生坐标
    POINT   ptMove;     // 出现移动目标坐标
    POINT   ptEscape;   // 逃跑目标坐标
};

// 蜘蛛
class CSpider : CFly
{
public:
    CSpider() : CFly() { }

    // 对于地鼠应提供出生的洞的信息，其它飞行类创建出生、飞行、逃跑坐标
    virtual void Born(CHole* _hole);
    //virtual void Escape();
};


// 盗宝小妖
class CFlyGoblin : CFly
{
public:
    CFlyGoblin() : CFly() {}

    // 对于地鼠应提供出生的洞的信息，其它飞行类创建出生、飞行、逃跑坐标
    virtual void Born(CHole* _hole);
    //virtual void Escape();
};



//////////////////////////////////////////////////////////////////////////


enum HoleStatus
{
    hsEmpty,      // 空的
    hsFull,       // 有怪物出现
    HoleStatusCount
};


class CHole
{
public:
    CHole(BYTE _idx);
    CHole(BYTE _row, BYTE _col);
    ~CHole() {}

    void Respawn(CMonster* monster);
    void Remove();

    BYTE idx() const;
    HoleStatus status() const;
    CMonster* monster() const;

private:
    BYTE m_idx;
    HoleStatus m_status;
    CMonster* m_monster;
};

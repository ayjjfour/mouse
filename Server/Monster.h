#pragma once
#include "GameConfig.h"

using namespace std;

const static int MAX_HOLE = 9;  //����Ķ�������
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

const static int PrecisionNumber = 10000;     // �����еľ��� С�����4λ

class CHole;  

enum MonsterStatus
{
    msInit,       // �Ѵ���
    msRespawn,    // ׼������
    msComeOut,    // ���֣��ɴ����
    //Idle,       // ����һ��ʱ��������ɴ���� // ������Ҫ��״̬
    //Hit,        // �����  // ������Ҫ��״̬
    msEscape,     // ���
    msDestroy,    // ���٣��������³�ʼ����
    MonsterStatusCount
};

class CMonster
{
public:
    CMonster();
    virtual ~CMonster() {}
    virtual void Born(CHole* _hole);
    virtual void Escape();
    
    // �����������true�� ���򷵻�false
    bool PlayerHit(bool is_wild_status);
    void Remove();
    void Destroy();

    // Monster�����ʹ�ã� ���¸��ӵ��µ�GameObject
    // ��ΪMonster���ö�Ӧ���͵�ֵ
    void Attach(/*GameObject obj, */ DWORD dwMonsterID, MonsterType _monsterType,bool is_wild_monster,float destory_monster_speed = 1);

    // Monster��GameObject����
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
    DWORD m_dwTickDestroy;  // ���ټ����������ʱ������٣������ʱ������


	friend class CMonsterPool;
};



//////////////////////////////////////////////////////////////////////////

// ������
class CMole : CMonster
{
public:
    // �����ڶ���ص���
    CMole() : CMonster() {}

    // ���ڵ���Ӧ�ṩ�����Ķ�����Ϣ
    //virtual void Born(CHole* _hole);  
    //virtual void Escape();
};

// ��Ӭ
class CFly : CMonster
{
public:
    CFly() : CMonster() {}

    // ���ڵ���Ӧ�ṩ�����Ķ�����Ϣ�����������ഴ�����������С���������
    virtual void Born(CHole* _hole);  
    //virtual void Escape();


    const POINT& pt_respawn() const;
    //void set_respawn(int x, int y);
    const POINT& pt_move() const;
    //void set_move(int x, int y);
    const POINT& pt_escape() const;
    //void set_escape(int x, int y);

protected:
    POINT   ptRespawn;  // ��������
    POINT   ptMove;     // �����ƶ�Ŀ������
    POINT   ptEscape;   // ����Ŀ������
};

// ֩��
class CSpider : CFly
{
public:
    CSpider() : CFly() { }

    // ���ڵ���Ӧ�ṩ�����Ķ�����Ϣ�����������ഴ�����������С���������
    virtual void Born(CHole* _hole);
    //virtual void Escape();
};


// ����С��
class CFlyGoblin : CFly
{
public:
    CFlyGoblin() : CFly() {}

    // ���ڵ���Ӧ�ṩ�����Ķ�����Ϣ�����������ഴ�����������С���������
    virtual void Born(CHole* _hole);
    //virtual void Escape();
};



//////////////////////////////////////////////////////////////////////////


enum HoleStatus
{
    hsEmpty,      // �յ�
    hsFull,       // �й������
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

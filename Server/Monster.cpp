#include "stdafx.h"
#include "MonsterManager.h"
#include "Monster.h"
#include "TraceOut.h"

CMonster::CMonster()
{
    m_id = 0;
    m_curHP = 0;
    m_totalHP = 0;
    m_status = msInit;
    m_monsterType = mtNone;
    m_moveSpeed = 0.0f;
    m_fWaitToEscapeTime = 0.0f;
    m_fEscapeTime = 0.0f;
    m_dwTickDestroy = 0;
	innormal_dead = false;
    m_hole = nullptr;
}

void CMonster::Born(CHole* _hole)
{
    // 如果为地鼠，则_hole不能为空
    if (CMonsterManager::IsMole(m_monsterType) && _hole == nullptr)
    {
        TRACEFALTAL("hole cant be null, if monster is a mole");
        return;
    }

    m_status = msRespawn;
    m_hole = _hole;

    // 出生动画

    m_status = msComeOut;
}

void CMonster::Escape()
{
    m_status = msEscape;

    // 逃跑动画
}

bool CMonster::PlayerHit(bool is_wild_status)
{
	
    m_curHP--;
	if(!innormal_dead)
		innormal_dead = is_wild_status;
    if (m_curHP <= 0)
    {
        /*
        // 死亡后，处理方式不同
        if (CMonsterManager::IsMole(m_monsterType))
        {
            //PlayerScore.MoleKilled++;
            Escape();
        }
        else
        {
            //if (m_monsterType == mtFly) { PlayerScore.FlyKilled++; }
            //else { PlayerScore.SpiderKilled++; }
            
            Kill();
        }
        */

        return true;
    }

    return false;
}

void CMonster::Remove()
{
    m_hole = nullptr;
}

void CMonster::Destroy()
{
    m_status = msDestroy; // 即将销毁
}

void CMonster::Attach(/*GameObject obj, */ DWORD dwMonsterID, MonsterType _monsterType,bool is_wild_monster,float destory_monster_speed /*= 1*/)
{
    //objMonster = obj;
    m_id = dwMonsterID;
    m_monsterType = _monsterType;

    const CommonMonsterProperty* infCommonProperty = CMonsterManager::Instance().GetMonsterConfig(m_monsterType);
    if (infCommonProperty == nullptr)
    {
        TRACEFALTAL("Get Monster Config Failed, type=%d", _monsterType);
        return;
    }
	if(is_wild_monster)
		m_totalHP = 1;
	else
		m_totalHP = infCommonProperty->MaxHP;
    m_curHP = m_totalHP; 
    m_moveSpeed = infCommonProperty->fMoveSpeed;
    m_status = msInit;
    m_hole = nullptr;

    // 出生到移动时间
    m_fWaitToEscapeTime = CMonsterManager::Random(infCommonProperty->fWaitToEscapeTimeMin, infCommonProperty->fWaitToEscapeTimeMax);
    // 移动到逃跑时间（等待挨打时间）
    m_fEscapeTime = CMonsterManager::Random(infCommonProperty->fEscapeTimeMin, infCommonProperty->fEscapeTimeMax);

    // 销毁时间
    DWORD curTick = GetTickCount();
	m_nMaxLifeTime = curTick + (int((m_fWaitToEscapeTime + m_fEscapeTime + 10.0f) * 1000))/destory_monster_speed;
    m_dwTickDestroy =curTick + (int((m_fWaitToEscapeTime + m_fEscapeTime + 10.0f) * 1000))/destory_monster_speed;


    //MoveSpeed = Config.MoveSpeedConfig[(int)monsterType];

    //lstUIAnimation = new List<GameObject>();
    //monsterScripts = objMonster.GetComponent<MonsterEvent>();
    //monsterScripts.RegisterButtonClickEvent();
    //Debug.Log(monsterType + " Status : " + status);
}

void CMonster::Detach()
{
    //objMonster = null;
    m_id = 0;
    m_monsterType = mtNone;
    m_hole = nullptr;   
    m_dwTickDestroy = 0;

    //curHP = TotalHP;
    //status = MonsterStatus.Init;

    //foreach(GameObject objAnim in lstUIAnimation)
    //    GameObject.Destroy(objAnim);
    //lstUIAnimation.Clear();
    //monsterScripts = null;

    //Debug.Log(monsterType + " Status : " + status);
}

DWORD CMonster::id() const
{
    return m_id;
}

short CMonster::curHP() const
{
    return m_curHP;
}

short CMonster::totalHP() const
{
    return m_totalHP;
}

float CMonster::moveSpeed() const
{
    return m_moveSpeed;
}

MonsterStatus CMonster::status() const
{
    return m_status;
}

MonsterType CMonster::monsterType() const
{
    return m_monsterType;
}

CHole* CMonster::hole() const
{
    return m_hole;
}

int CMonster::holeidx() const
{
    return (m_hole) ? m_hole->idx() : -1;
}

float CMonster::waittoescapetime() const
{
    return m_fWaitToEscapeTime;
}

float CMonster::escapetime() const
{
    return m_fEscapeTime;
}

DWORD CMonster::destroytime() const
{
    return m_dwTickDestroy;
}

bool CMonster::innormal_hit() const
{
	return innormal_dead;
}

//////////////////////////////////////////////////////////////////////////

CHole::CHole(BYTE _idx)
{
    ASSERT(_idx < MAX_HOLE);
    m_idx = _idx;
    m_status = hsEmpty;
    m_monster = nullptr;
}

CHole::CHole(BYTE _row, BYTE _col)
{
    ASSERT(_row < 3 && _col < 3);

    m_idx = _col * 3 + _col;
    m_status = hsEmpty;
    m_monster = nullptr;
}

void CHole::Respawn(CMonster * _monster)
{
    m_status = hsFull;
    m_monster = _monster;
    m_monster->Born(this);
}

void CHole::Remove()
{
    m_status = hsEmpty;
    m_monster->Remove();
    m_monster = nullptr;
}

BYTE CHole::idx() const
{
    return m_idx;
}

HoleStatus CHole::status() const
{
    return m_status;
}

CMonster* CHole::monster() const
{
    return m_monster;
}

//CMole::CMole() : CMonster()
//{
//
//}

//void CMole::Born(CHole* _hole)
//{
//    CMonster::Born(_hole);
//
//    //MoveBy(new Vector3(0, Config.MoleOffset, 0));
//}
//
//void CMole::Escape()
//{
//    //if (m_status == msComeOut)
//    //{
//    //    MoveBy(new Vector3(0, -Config.MoleOffset, 0));
//    //}
//
//    CMonster::Escape();
//}

//CFly::CFly() : CMonster()
//{
//
//}

void CFly::Born(CHole* _hole)
{
    CMonster::Born(_hole);

    //////////////////////////////////////////////////////////////////////////
    // 出生坐标
    //int x = 0, y = 0, z = -60;
    int dir = CMonsterManager::Random(0, 4);
    if (dir == 0) //左
    {
        ptRespawn.x = rtOutOfScreenZone.left;
        ptRespawn.y = CMonsterManager::Random(-defaultHeight / 2, defaultHeight / 2);
    }
    else if (dir == 1) //上
    {
        ptRespawn.x = CMonsterManager::Random(-defaultWidth / 2, defaultWidth / 2);
        ptRespawn.y = rtOutOfScreenZone.top;   //TODO: 如果Y坐标有问题再修改
    }
    else if (dir == 2) //右
    {
        ptRespawn.x = rtOutOfScreenZone.right;
        ptRespawn.y = CMonsterManager::Random(-defaultHeight / 2, defaultHeight / 2);
    }
    else if (dir == 3) //下
    {
        ptRespawn.x = CMonsterManager::Random(-defaultWidth / 2, defaultWidth / 2);
        ptRespawn.y = rtOutOfScreenZone.bottom;
    }


    //////////////////////////////////////////////////////////////////////////
    // 飞行坐标
    ptMove.x = CMonsterManager::Random(rtPlayZone.left, rtPlayZone.right);
    ptMove.y = CMonsterManager::Random(rtPlayZone.top, rtPlayZone.bottom);


    //////////////////////////////////////////////////////////////////////////
    // 逃跑坐标
    dir = CMonsterManager::Random(0, 4);
    if (dir == 0) //左
    {
        ptEscape.x = rtOutOfScreenZone.left;
        ptEscape.y = CMonsterManager::Random(-defaultHeight / 2, defaultHeight / 2);
    }
    else if (dir == 1) //上
    {
        ptEscape.x = CMonsterManager::Random(-defaultWidth / 2, defaultWidth / 2);
        ptEscape.y = rtOutOfScreenZone.top;
    }
    else if (dir == 2) //右
    {
        ptEscape.x = rtOutOfScreenZone.right;
        ptEscape.y = CMonsterManager::Random(-defaultHeight / 2, defaultHeight / 2);
    }
    else if (dir == 3) //下
    {
        ptEscape.x = CMonsterManager::Random(-defaultWidth / 2, defaultWidth / 2);
        ptEscape.y = rtOutOfScreenZone.bottom;
    }

    //
    //////////////////////////////////////////////////////////////////////////



    //MoveTo(new Vector3(x, y, 0));

    //Debug.Log(monsterType + " Status : " + status);
}

//void CFly::Escape()
//{
//    /*
//    if (m_status == msComeOut)
//    {
//        float x = 0, y = 0;
//        Rect rtOutOfScreenZone = Config.GetOutOfScreenZone();
//        // 随机选择一个目标飞离
//        int dir = Random.Range(0, 4);
//        if (dir == 0) //左
//        {
//            x = rtOutOfScreenZone.xMin;
//            y = Random.Range(-Config.defaultHeight / 2, Config.defaultHeight / 2);
//        }
//        else if (dir == 1) //上
//        {
//            x = Random.Range(-Config.defaultWidth / 2, Config.defaultWidth / 2);
//            y = rtOutOfScreenZone.yMax;
//        }
//        else if (dir == 2) //右
//        {
//            x = rtOutOfScreenZone.xMax;
//            y = Random.Range(-Config.defaultHeight / 2, Config.defaultHeight / 2);
//        }
//        else if (dir == 3) //下
//        {
//            x = Random.Range(-Config.defaultWidth / 2, Config.defaultWidth / 2);
//            y = rtOutOfScreenZone.yMin;
//        }
//
//        MoveTo(new Vector3(x, y, 0));
//    }
//    */
//    CMonster::Escape();
//}

const POINT& CFly::pt_respawn() const
{
    return ptRespawn;
}

//void CFly::set_respawn(int x, int y)
//{
//    ptRespawn.x = x;
//    ptRespawn.y = y;
//}

const POINT& CFly::pt_move() const
{
    return ptMove;
}

//void CFly::set_move(int x, int y)
//{
//    ptMove.x = x;
//    ptMove.y = y;
//}

const POINT& CFly::pt_escape() const
{
    return ptEscape;
}

//void CFly::set_escape(int x, int y)
//{
//    ptEscape.x = x;
//    ptEscape.y = y;
//}

// 对于地鼠应提供出生的洞的信息
void CSpider::Born(CHole* _hole)
{
    CFly::Born(_hole);

    //////////////////////////////////////////////////////////////////////////
    // 出生坐标
    // 对蜘蛛来说，只能从上面出现
    int rtPlayZoneWidth = rtPlayZone.right - rtPlayZone.left;
    ptRespawn.x = CMonsterManager::Random(-rtPlayZoneWidth / 2, rtPlayZoneWidth / 2);
    ptRespawn.y = defaultHeight / 2; // Screen.height; //rtOutOfScreenZone.yMax;


    //////////////////////////////////////////////////////////////////////////
    // 飞行坐标
    int dist = CMonsterManager::Random((int)(defaultHeight*0.4f), defaultHeight /*(int)rtPlayZone.height*/) + defaultHeight * (int)(fOuterDist + fBorderDist[1]);
    ptMove.x = ptRespawn.x;
    ptMove.y = ptRespawn.y - dist;


    //////////////////////////////////////////////////////////////////////////
    // 逃跑坐标
    dist = defaultHeight;
    ptEscape.x = ptMove.x;
    ptEscape.y = ptMove.y + defaultHeight;

    //
    //////////////////////////////////////////////////////////////////////////






    //// 从屏幕上方爬下（随机值，不超出屏幕区域即可）
    ////Rect rtPlayZone = Config.GetPlayZone();
    //int dist = Random.Range((int)(Config.defaultHeight*0.4f), Config.defaultHeight /*(int)rtPlayZone.height*/) + Config.defaultHeight * (int)(Config.fOuterDist + Config.fBorderDist[1]);
    //MoveBy(new Vector3(0, -dist, 0));

    //Debug.Log(monsterType + " Status : " + status);
}

//void CSpider::Escape()
//{
//    //if (m_status == msComeOut)
//    //{
//    //    // 从屏幕内爬出去
//    //    MoveBy(new Vector3(0, Config.defaultHeight, 0));
//    //}
//
//    CMonster::Escape();
//}

void CFlyGoblin::Born(CHole* _hole)
{
    //////////////////////////////////////////////////////////////////////////
    // 出生坐标
    //int x = 0, y = 0, z = -60;
    int dir = CMonsterManager::Random(0, 4);
    if (dir == 0) //左上
    {
        ptRespawn.x = rtOutOfScreenZone.left;
        ptRespawn.y = 170;
    }
    else if (dir == 1) //左下
    {
        ptRespawn.x = rtOutOfScreenZone.left;
        ptRespawn.y = -70;
    }
    else if (dir == 2) //右上
    {
        ptRespawn.x = rtOutOfScreenZone.right;
        ptRespawn.y = 100;
    }
    else if (dir == 3) //右下
    {
        ptRespawn.x = rtOutOfScreenZone.right;
        ptRespawn.y = -100;
    }


    //////////////////////////////////////////////////////////////////////////
    // 飞行坐标
    if (dir == 0 || dir == 1) // 左->右
        ptMove.x = CMonsterManager::Random(rtPlayZone.left, 0);
    else
        ptMove.x = CMonsterManager::Random(0, rtPlayZone.right);
    ptMove.y = ptRespawn.y;


    //////////////////////////////////////////////////////////////////////////
    // 逃跑坐标
    if (dir == 0 || dir == 1) // 左->右
        ptEscape.x = rtOutOfScreenZone.right;
    else
        ptEscape.x = rtOutOfScreenZone.left;
    ptEscape.y = ptRespawn.y;

    //
    //////////////////////////////////////////////////////////////////////////
}

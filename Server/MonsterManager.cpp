#include "stdafx.h"
#include "MonsterManager.h"
#include <list>
#include "TraceOut.h"
#include "GameLogi.h"
#include "StatisticMgr.h"

const static int MAX_INT = 2147483648;

CMonsterManager::CMonsterManager()
{
    init_genrand(GetTickCount());// * /MT19937 rand seed

	m_bInitialized = false;
	m_nRoomID = -1;
}
 
CMonsterManager::~CMonsterManager()
{
    for (auto it = m_lstPool.begin(); it != m_lstPool.end(); ++it)
    {
        delete (*it);
    }
    m_lstPool.clear();
}

const CommonMonsterProperty* CMonsterManager::GetMonsterConfig(MonsterType monsterType)
{
    return GameConfigMgr::getInstance().Current().GetMonsterCommonProperty(monsterType);
}

CMonsterPool* CMonsterManager::GetMonsterPool(int nTableID)
{
    if (nTableID < 0 || nTableID >= m_lstPool.size())
    {
        TRACEFALTAL("GetPool Failed, nTableID=%d, Max=%d", nTableID, m_lstPool.size());
		WriteInfoLog(nTableID, Format("GetPool Failed, nTableID=%d, Max=%d", nTableID, m_lstPool.size()));
        return nullptr;
    }
    return m_lstPool[nTableID];
}

std::vector<DWORD> CMonsterManager::get_all_monster_id(DWORD table_id)
{
	std::vector<DWORD> alive_monster_id;
	CMonsterPool* pool = GetMonsterPool(table_id);
	if (NULL  == pool)
	{
		//GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "房间[%d] 桌子[%d]未找到， dwMonsterID=%d", m_nRoomID, nTableID, dwMonsterID);
		return alive_monster_id;
	}
	auto map_monster = pool->get_table_monster_list();
	for(auto it = map_monster.begin(); it != map_monster.end(); ++it)
	{
		alive_monster_id.push_back(it->first);
	}
	return alive_monster_id;
}

//////////////////////////////////////////////////////////////////////////
// 对象池pool接口
CMonster* CMonsterManager::GetMonster(int nTableID, DWORD id /*GameObject obj*/)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);
    return pool ? pool->GetMonster(id /*obj*/) : nullptr;
}

MonsterType CMonsterManager::GetMonsterType(int nTableID, DWORD id)
{
	CMonsterPool* pool = GetMonsterPool(nTableID);
	if (pool == nullptr)
		return mtNone;

	CMonster* monster = pool->GetMonster(id /*obj*/);
	if (monster == nullptr)
		return mtNone;

	return monster->monsterType();
}

void CMonsterManager::DestroyMonster(int nTableID, DWORD id)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);

	WriteInfoLog(nTableID, Format("CMonsterManager::DestroyMonster ID: %d", id));

    if (pool)
    {
        pool->DestroyMonster(id);
    }
}

void CMonsterManager::DestroyAllMonster(int nTableID)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);

	WriteInfoLog(nTableID, "CMonsterManager::DestroyAllMonster");

    if (pool)
    {
        pool->DestroyAllMonsters();
    }
}

std::string CMonsterManager::GetPoolState(int nTableID)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);
    if (!pool)
    {
        TRACEFALTAL("cant find table pool. TableID : [%d]", nTableID);
        return "";
    }
    return pool->GetPoolState();
}

WORD CMonsterManager::GetMonsterNum(int nTableID, MonsterType monsterType)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);
    return pool ? pool->GetMonsterNum(monsterType) : -1;
}


//void CALLBACK CMonsterManager::OnTimerDestroyMonster(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
//{
//    KillTimer(hwnd, idEvent);
//    CMonster* pMonster = CMonsterManager::Instance().GetMonster(0, idEvent);
//    if (pMonster)
//    {
//        CMonsterManager::Instance().DestroyMonster(0, pMonster);
//    }   
//}

//////////////////////////////////////////////////////////////////////////
// CMonsterManager主要逻辑
void CMonsterManager::Initialize(int nRoomID, int nTableID)
{
    // 每个房间，配置文件只加载一次
    if (!m_bInitialized)
    {
        m_nRoomID = nRoomID;

		if (!GameConfigMgr::Initialize(nRoomID))
		{
			throw logic_error("配置加载失败，请查看日志！");
		}

        m_lstPool.reserve(180);

		StatisticMgr::getInstance().SetRoomID(nRoomID);
		StatisticMgr::getInstance().Initialize();

		StatisticMgr::getInstance().DumpConfig(true);

		m_bInitialized = true;
    }

    const MonsterPoolConfig* infMonsterPool = GameConfigMgr::getInstance().Current().GetMonsterPoolConfig(nRoomID);

    // 插入的TableID的序号应该正好等于POOL的大小 
    ASSERT(m_lstPool.size() == nTableID);
    m_lstPool.push_back(new CMonsterPool(infMonsterPool));
}

//CMonster* CMonsterManager::CreateMonster(int nTableID)
//{
//    CMonsterPool* pool = GetMonsterPool(nTableID);
//    if (!pool || pool && pool->destroying())
//    {
//        LOGE("Is Destroying Monster, cant create now!");
//        return nullptr;
//    }
//
//    MonsterType monsterType = GetNextRandomMonsterType(nTableID);
//    if (monsterType == mtNone)
//    {
//        LOGI("此次刷机结果不刷新怪，可能原因：怪物数量已达上限");
//        return nullptr;
//    }
//
//    CMonster* monster = CreateMonster(nTableID, monsterType);
//    //Assert.IsNotNull(monster, "可能创建Monster失败");
//
//    return monster;
//}

void CMonsterManager::CreateMonster(int nTableID, list<CMonster*>& lstMonster,bool is_wild_monster, float monster_destroy_time_rate/* = 1.0*/,MonsterType _monsterType /*= mtNone*/)
{
    lstMonster.clear();

    if (_monsterType >= MonsterTypeCount)
    {
        TRACEFALTAL("创建Monster失败， _monsterType=%d", _monsterType);
		WriteInfoLog(nTableID, Format("创建Monster失败， _monsterType=%d", _monsterType));
        return ;
    }

    CMonsterPool* pool = GetMonsterPool(nTableID);
    if (pool == nullptr)
    {
        TRACEFALTAL("创建Monster失败， GetMonsterPool Failed, nTableID=%d", nTableID);
		WriteInfoLog(nTableID, Format("创建Monster失败， GetMonsterPool Failed, nTableID=%d", nTableID));
        return ;
    }

    list<MonsterType> lstMonsterType;
    if (_monsterType == mtNone)
    {
        //_monsterType = GetNextRandomMonsterType(nTableID);
        //if (_monsterType == mtNone)
        //{
        //    LOGI("此次刷机结果不刷新怪，可能原因：怪物数量已达上限");
        //    return nullptr;
        //}
        
        GetNextRandomMonsterGroupType(nTableID, lstMonsterType);
    }
    else
    {
        lstMonsterType.push_back(_monsterType);
    }


    for (auto it = lstMonsterType.begin(); it != lstMonsterType.end(); ++it)
    {
        MonsterType monsterType = *it;

        CMonster* monster = nullptr;

        if (IsMole(monsterType))
        {
            CHole* hole = pool->GetIdleHole();
            if (hole == nullptr)
            {
                TRACEDEBUG("创建Monster失败， 没有多余的空洞了, monsterType = %d", monsterType);
                continue;
            }

            //Debug.Log(string.Format("{0}, {1}, {2}", monsterType, hole.idx, hole.Holder.transform.position));

            monster = pool->CreateMonster(/*go,*/ monsterType,is_wild_monster,monster_destroy_time_rate);

            // 创建失败的处理，即monster为nullptr，直接返回，不生成此对象
            if (monster == nullptr)
            {
                TRACEFALTAL("Monster创建失败 MOLE！ monsterType=%d", monsterType);
                continue;
            }

            pool->Respawn(monster, hole);
        }
        else if (monsterType == mtFly || monsterType == mtSpider || monsterType == mtFlyGoblin)
        {
            monster = pool->CreateMonster(monsterType,is_wild_monster,monster_destroy_time_rate);

            // 创建失败的处理，即monster为nullptr，直接返回，不生成此对象
            if (monster == nullptr)
            {
                TRACEFALTAL("Monster创建失败！ NOT MOLE!!   monsterType=%d", monsterType);
                continue;
            }

            pool->Respawn(monster, nullptr);
            //go.transform.SetParent(GameObject.Find(Global.GetFlyHolderName()).transform, false);
        }

        lstMonster.push_back(monster);
    }
}

void CMonsterManager::CreateGroupMonster(int nTableID, int nCount, list<CMonster*>& lstMonster,bool is_wild_monster,float destory_monster_speed/* = 1.0*/, MonsterType monsterType /*= mtNone*/)
{
    for (int i = 0; i < nCount; ++i)
    {
        list<CMonster*> lstMonsterTmp;
        CreateMonster(nTableID, lstMonsterTmp,is_wild_monster,destory_monster_speed, monsterType);
        if (lstMonsterTmp.empty())
            continue;

        lstMonster.insert(lstMonster.end(), lstMonsterTmp.begin(), lstMonsterTmp.end());
    }
}

struct RandInfo
{
    MonsterType monsterType;
    int randMonster;

    RandInfo(MonsterType _monsterType, int _randMonster)
    {
        monsterType = _monsterType;
        randMonster = _randMonster;
    }
};
// 刷新机制：目前是每多少秒一次刷1-N只
// 目前写的是所有怪物一起随机，一次产生一只，刷机的过程有点麻烦，如果能确定几率合为100，可以简化随机
//MonsterType CMonsterManager::GetNextRandomMonsterType(int nTableID)
//{
//    int randTotal = 0; 
//    list<RandInfo> lstMonsterType;
//
//    // 先判断洞是否满了，如果满了就直接刷不占洞的
//    CMonsterPool* pool = GetMonsterPool(nTableID);
//    if (!pool)
//        return mtNone;
//
//    bool isHoleFull = pool->IsHoleFull();
//    //StageType stageType = MapManager.Instance.Stage;
//    for (int monsterType = mtMonsterTypeFirst; monsterType < MonsterTypeCount; ++monsterType)
//    {
//        if (isHoleFull && IsMole((MonsterType)monsterType))
//            continue;
//
//        const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, (MonsterType)monsterType);
//        if (!pMonsterProperty)
//            continue;
//
//        int rand = pMonsterProperty->RespawnProb;   // Config.MonsterRespawnConfig[(int)stageType, (int)monsterType];
//        if (rand == 0)
//            continue;
//
//        //TODO: 限定每种怪的同屏数量
//        WORD curNum = GetMonsterNum(nTableID, (MonsterType)monsterType);
//        if (curNum >= pMonsterProperty->MaxOnScreen) // Config.MaxOnScreenConfig[(int)monsterType])
//            continue;
//
//        randTotal += rand;
//
//        RandInfo randInfo = RandInfo((MonsterType)monsterType, randTotal);
//        lstMonsterType.push_back(randInfo);
//    }
//
//    // 如果满了就不刷新
//    //Assert.IsTrue(randTotal != 0);
//    if (randTotal != 0)
//    {
//        int randResult = Random(0, randTotal) + 1;
//        for (auto it = lstMonsterType.begin(); it != lstMonsterType.end(); ++it)
//        {
//            if (randResult <= it->randMonster)
//            {
//                return it->monsterType;
//            }
//        }
//    }
//
//    return mtNone;
//}

bool CMonsterManager::GetNextRandomMonsterGroupType(int nTableID, list<MonsterType>& lstMonsterType)
{
    lstMonsterType.clear();

    // 先判断洞是否满了，如果满了就直接刷不占洞的
    CMonsterPool* pool = GetMonsterPool(nTableID);
    if (!pool)
        return false;

	vector<int> vecDetSeq;

    // 分组计算出现概率，默认每组的概率和为1000， 如果刷机到的怪数量已达上限，则不刷新 ！！！ 
    // 1.地鼠            
    bool isHoleFull = pool->IsHoleFull();
	WriteInfoLog(nTableID, Format("地鼠洞已满？ %s", isHoleFull ? "是" : "否"));
    if (!isHoleFull)
    {
#if 1
        int randTotal = 0;
        int randResult = Random(0, 1000) + 1;
		WriteInfoLog(nTableID, Format("CMonsterManager::GetNextRandomMonsterGroupType  地鼠->randResult: %d", randResult));

		pool->GetDetermineSeq(true, vecDetSeq);

        for (vector<int>::iterator i = vecDetSeq.begin(); i != vecDetSeq.end(); i++)
        {
            MonsterType monsterType = (MonsterType)*i;
            const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, monsterType);
            if (!pMonsterProperty)
            {
                TRACEFALTAL("房间[%d] MonsterPropertyInfo配置错误，未找到相应数据, monsterType=%d", m_nRoomID, monsterType);
				WriteInfoLog(nTableID, Format("房间[%d] MonsterPropertyInfo配置错误，未找到相应数据, monsterType=%d", m_nRoomID, monsterType));
                continue;
            }

			if (pMonsterProperty->RespawnProb <= 0)
			{
				continue;
			}

            randTotal += pMonsterProperty->RespawnProb;
            if (randResult <= randTotal)
            {
                // 判断同屏数量
                WORD curNum = GetMonsterNum(nTableID, monsterType);

				WriteInfoLog(nTableID, Format("CMonsterManager::GetNextRandomMonsterGroupType 地鼠->monsterType: %d, curNum: %d, Max: %d", monsterType, curNum, pMonsterProperty->MaxOnScreen));

                if (curNum >= pMonsterProperty->MaxOnScreen) // Config.MaxOnScreenConfig[(int)monsterType])
                    continue;

                lstMonsterType.push_back(monsterType);
                break;
            }

			if (randTotal >= 1000)
			{
				break;
			}
        }
#endif
    }

#if 1
    // 2.蜘蛛，苍蝇
    int randTotal = 0;
    int randResult = Random(0, 1000) + 1;
	WriteInfoLog(nTableID, Format("CMonsterManager::GetNextRandomMonsterGroupType  苍蝇->randResult: %d", randResult));

	pool->GetDetermineSeq(false, vecDetSeq);

    for (vector<int>::iterator i = vecDetSeq.begin(); i != vecDetSeq.end(); i++)
    {
        MonsterType monsterType = (MonsterType)*i;
        const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, monsterType);
        if (!pMonsterProperty)
        {
            TRACEFALTAL("房间[%d] MonsterPropertyInfo配置错误，未找到相应数据, monsterType=%d", m_nRoomID, monsterType);
			WriteInfoLog(nTableID, Format("房间[%d] MonsterPropertyInfo配置错误，未找到相应数据, monsterType=%d", m_nRoomID, monsterType));
            continue;
        }

		if (pMonsterProperty->RespawnProb <= 0)
		{
			continue;
		}

        randTotal += pMonsterProperty->RespawnProb;
        if (randResult <= randTotal)
        {
            // 判断同屏数量
            WORD curNum = GetMonsterNum(nTableID, monsterType);

			WriteInfoLog(nTableID, Format("CMonsterManager::GetNextRandomMonsterGroupType 苍蝇->monsterType: %d, curNum: %d, Max: %d", monsterType, curNum, pMonsterProperty->MaxOnScreen));

            if (curNum >= pMonsterProperty->MaxOnScreen) // Config.MaxOnScreenConfig[(int)monsterType])
                continue;

            lstMonsterType.push_back(monsterType);
            break;
        }

		if (randTotal >= 1000)
		{
			break;
		}
    }
#endif

    // 鼠王
    {
#if 0
        MonsterType monsterType = mtMoleKing;
    //    StageType stageType = MapManager.Instance.Stage;
    //    WORD curNum = GetMonsterNum(nTableID, monsterType);
    //   if (curNum == 0) // Config.MaxOnScreenConfig[(int)monsterType])
        {
            lstMonsterType.push_back(monsterType);
        }
#endif
    }

    // 小妖（树懒）
    {
        MonsterType monsterType = mtFlyGoblin;
        const MonsterPropertyInfo* pMonsterProperty = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_nRoomID, monsterType);
        if (pMonsterProperty)
        {
            WORD curNum = GetMonsterNum(nTableID, monsterType);
            if (curNum < pMonsterProperty->MaxOnScreen) // Config.MaxOnScreenConfig[(int)monsterType])
            {
                int randResult = Random(0, 1000) + 1;
                if (randResult <= pMonsterProperty->RespawnProb)
                {
                    lstMonsterType.push_back(monsterType);
                }
            }                  
        }
        else
        {
            TRACEFALTAL("房间[%d] MonsterPropertyInfo配置错误，未找到相应数据, monsterType=%d", m_nRoomID, monsterType);
        }
    }

    return true;
}

void CMonsterManager::CheckMonsterToDestroy(int nTableID)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);
    if (!pool)
	{
		WriteInfoLog(nTableID, _T("CheckMonsterToDestroy pool == null"));
        return;
	}

    vector<DWORD> lstMonster;
    pool->CheckMonsterToDestroy(lstMonster);

    if (lstMonster.empty())
	{
		WriteInfoLog(nTableID, _T("CheckMonsterToDestroy, Monster Empty"));
        return;
	}

    for (auto it = lstMonster.begin(); it != lstMonster.end(); ++it)
    {
        //LOGI("id=%d", *it);
        DestroyMonster(nTableID, *it);
    }
}

bool CMonsterManager::monster_hit_dead(int nTableID, DWORD dwMonsterID)
{
	 CMonsterPool* pool = GetMonsterPool(nTableID);
	 if (!pool)
	 {
		 //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "房间[%d] 桌子[%d]未找到， dwMonsterID=%d", m_nRoomID, nTableID, dwMonsterID);
		 return false;
	 }
	  CMonster* pMonster = pool->GetMonster(dwMonsterID);
	   if(pMonster)
	   {
		  return pMonster->innormal_hit();
	   }
	   return false;
		
}
MonsterType CMonsterManager::PlayerHit(int nTableID, DWORD dwMonsterID,bool is_wild_status)
{
    CMonsterPool* pool = GetMonsterPool(nTableID);
    if (!pool)
    {
        //GLOGFE(LOGID_ScoreBugTrace(m_nRoomID), "房间[%d] 桌子[%d]未找到， dwMonsterID=%d", m_nRoomID, nTableID, dwMonsterID);
        return mtNone;
    }

    CMonster* pMonster = pool->GetMonster(dwMonsterID);
    if (pMonster)
    {
        bool isHitDeath = pMonster->PlayerHit(is_wild_status);
        //GLOGFI(LOGID_Trace(m_nRoomID), "击中怪物ID=%d, isDeath=%d, MonsterType=%d, curHP=%d", dwMonsterID, isHitDeath, pMonster->monsterType(), pMonster->curHP());

        if (isHitDeath)
        {
            //GLOGFI(LOGID_Trace(m_nRoomID), "怪物收益情况： 击杀消耗 怪物ID=%d", dwMonsterID);

            MonsterType monsterType = pMonster->monsterType();
            DestroyMonster(nTableID, dwMonsterID);
            return monsterType;
        }
    }
    else
    {
		return MonsterTypeCount;
        //GLOGFI(LOGID_ScoreBugTrace(m_nRoomID), "房间[%d] 客户端打的怪已经不存在了，可能服务器已经销毁了, dwMonsterID=%d", m_nRoomID, dwMonsterID);
    }

    return mtNone;
}

int CMonsterManager::Random(int nMin, int nMax)
{
    ASSERT(nMax >= nMin);
    return (nMin == nMax) ? nMin : (int)(genrand_real2() * (nMax - nMin)) + nMin;
}

float CMonsterManager::Random(float fMin, float fMax)
{
    ASSERT(fMin >= fMin);
    if (abs(fMax - fMin) < 0.0001)
        return fMin;

	return (float)((genrand_real2() * (int)((fMax - fMin) * PrecisionNumber)) / PrecisionNumber) + fMin;

    //return (float)(rand() % (int)((fMax - fMin) * PrecisionNumber)) / PrecisionNumber + fMin;
}

bool CMonsterManager::Prob(int nCheck, int nMax)
{
    if (nCheck == 0 || nMax == 0) return false;
    return (Random(0, nMax) < nCheck);
}

//void CMonsterManager::GetPlayZone(RECT& rect)
//{
//    rect.left = LONG(defaultWidth * (-0.5f + fBorderDist[0]));
//    rect.top = LONG(defaultHeight * (-0.5f + fBorderDist[1]));
//    rect.right = LONG(defaultWidth * (0.5f - fBorderDist[2]));
//    rect.bottom = LONG(defaultHeight * (1.0f - fBorderDist[3]));
//}
//
//void CMonsterManager::GetOutOfScreenZone(RECT& rect)
//{
//    rect.left = LONG(defaultWidth * (-0.5f - fOuterDist));
//    rect.top = LONG(defaultHeight * (-0.5f - fOuterDist));
//    rect.right = LONG(defaultWidth * (0.5f + fOuterDist));
//    rect.bottom = LONG(defaultHeight * (0.5f + fOuterDist));
//}

//////////////////////////////////////////////////////////////////////////


CMonsterPool::CMonsterPool(const MonsterPoolConfig* config)
    : m_config(config)
{
    m_defaultSize = m_config->defaultSize;
    m_maxSize = m_config->maxSize;
    m_offsetSize = m_config->offsetSize;

    memset(m_MonsterCount, 0, sizeof(m_MonsterCount));

    m_destroying = false;
                                
    m_dwMonsterID = 0;
    m_dwGroupID = 0;

    m_lstHole.reserve(MAX_HOLE);
    m_lstIdleHole.reserve(MAX_HOLE);

    for (int i = 0; i < MAX_HOLE; ++i)
    {
        CHole* hole = new CHole(i);
        m_lstHole.push_back(hole);
        m_lstIdleHole.push_back(hole);
    }

    Initialize();
}

CMonsterPool::~CMonsterPool()
{
    UnInitialize();
}

bool CMonsterPool::destroying() const
{
    return m_destroying;
}

void CMonsterPool::Initialize()
{
    m_curFlySize = m_curSpiderSize = m_curFlyGoblinSize = m_defaultSize;

    // 初始化各种类型的生物池
    for (int i = 0; i < 9; i++)
    {
        CMole* mole = new CMole();
        m_lstIdleMole.push(mole);
    }

    for (int i = 0; i < m_defaultSize; i++)
    {
        CFly* fly = new CFly();
        m_lstIdleFly.push(fly);
    }

    for (int i = 0; i < m_defaultSize; i++)
    {
        CSpider* spider = new CSpider();
        m_lstIdleSpider.push(spider);
    }

    for (int i = 0; i < m_defaultSize; i++)
    {
        CFlyGoblin* goblin = new CFlyGoblin();
        m_lstIdleFlyGoblin.push(goblin);
    }

	//设定判定顺序
	for (int i = mtMonsterTypeFirst; i <= mtMoleKing; ++i)
	{
		m_vecMonsterSeq.push_back(i);
	}

	for (int i = mtSpider; i <= mtFly; ++i)
	{
		m_vecFlySeq.push_back(i);
	}
}

void CMonsterPool::UnInitialize()
{
    // 先销毁对象池中的数据
    DestroyAllMonsters();

    // 释放对象池中的内存
    while (!m_lstIdleMole.empty())
    {
        CMole* pMonster = m_lstIdleMole.front();
        delete pMonster;
        m_lstIdleMole.pop();
    }

    while (!m_lstIdleFly.empty())
    {
        CFly* pMonster = m_lstIdleFly.front();
        delete pMonster;
        m_lstIdleFly.pop();
    }

    while (!m_lstIdleSpider.empty())
    {
        CSpider* pMonster = m_lstIdleSpider.front();
        delete pMonster;
        m_lstIdleSpider.pop();
    }

    while (!m_lstIdleFly.empty())
    {
        CFlyGoblin* pMonster = m_lstIdleFlyGoblin.front();
        delete pMonster;
        m_lstIdleFlyGoblin.pop();
    }

    for (auto it = m_lstHole.begin(); it != m_lstHole.end(); ++it)
    {
        delete (*it);
    }
    m_lstHole.clear();
}

void CMonsterPool::GetDetermineSeq(bool IsMole, vector<int>& out)
{
	vector<int>* vecTarget = NULL;

	if (IsMole)
	{
		vecTarget = &m_vecMonsterSeq;
	}
	else
	{
		vecTarget = &m_vecFlySeq;
	}

	random_shuffle(vecTarget->begin(), vecTarget->end());

	//拷贝数据
	out.clear();
	out = *vecTarget;
}

bool CMonsterPool::IsFull(MonsterType monsterType)
{
    if (CMonsterManager::IsMole(monsterType))
    {
        return m_lstIdleMole.empty();
    }
    else if (monsterType == mtFly)
    {
        return m_lstIdleFly.empty();
    }
    else if (monsterType == mtSpider)
    {
        return m_lstIdleSpider.empty();
    }
    else if (monsterType == mtFlyGoblin)
    {
        return m_lstIdleFlyGoblin.empty();
    }

    TRACEFALTAL("类型错误: monsterType = %d", monsterType);
    return false;
}

bool CMonsterPool::AppendSize(MonsterType monsterType)
{
    TRACEDEBUG("monsterType[%d] is Full. Append + [%d]!", monsterType, m_offsetSize);

    if (CMonsterManager::IsMole(monsterType))
    {
        return false;
    }
    else if (monsterType == mtFly)
    {
        if (m_maxSize > 0 && m_curFlySize >= m_maxSize)
        {
            TRACEFALTAL("内存池空间不足，分配失败 FLY！ m_maxSize=%d", m_maxSize);
            return false;
        }
        else
        {
            m_curFlySize += m_offsetSize;
            for (int i = 0; i < m_offsetSize; i++)
            {
                CFly* _monster = new CFly();
                m_lstIdleFly.push(_monster);
            }
            return true;
        }
    }
    else if (monsterType == mtSpider)
    {
        if (m_maxSize > 0 && m_curSpiderSize >= m_maxSize)
        {
            TRACEFALTAL("内存池空间不足，分配失败 SPIDER！ m_maxSize=%d", m_maxSize);
            return false;
        }
        else
        {
            m_curSpiderSize += m_offsetSize;
            for (int i = 0; i < m_offsetSize; i++)
            {
                CSpider* _monster = new CSpider();
                m_lstIdleSpider.push(_monster);
            }
            return true;
        }
    }
    else if (monsterType == mtFlyGoblin)
    {
        if (m_maxSize > 0 && m_curFlyGoblinSize >= m_maxSize)
        {
            TRACEFALTAL("内存池空间不足，分配失败 FLYGOBLIN！ m_maxSize=%d", m_maxSize);
            return false;
        }
        else
        {
            m_curFlyGoblinSize += m_offsetSize;
            for (int i = 0; i < m_offsetSize; i++)
            {
                CFlyGoblin* _monster = new CFlyGoblin();
                m_lstIdleFlyGoblin.push(_monster);
            }
            return true;
        }
    }

    TRACEFALTAL("类型错误: monsterType=%d", monsterType);
    return false;
}

CMonster* CMonsterPool::Attach(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed/* = 1*/)
{
    CMonster* monster = nullptr;

    DWORD dwMonsterID = CreateMonsterID();

    if (CMonsterManager::IsMole(monsterType))
    {
        monster = (CMonster*)m_lstIdleMole.front();
        m_lstIdleMole.pop();
        monster->Attach(/*obj,*/ dwMonsterID, monsterType,is_wild_monster,destory_monster_speed);
    }
    else if (monsterType == mtFly)
    {
        monster = (CMonster*)m_lstIdleFly.front();
        m_lstIdleFly.pop();
        monster->Attach(/*obj,*/ dwMonsterID, monsterType,is_wild_monster,destory_monster_speed);
    }
    else if (monsterType == mtSpider)
    {
        monster = (CMonster*)m_lstIdleSpider.front();
        m_lstIdleSpider.pop();
        monster->Attach(/*obj,*/ dwMonsterID, monsterType,is_wild_monster,destory_monster_speed);
    }
    else if (monsterType == mtFlyGoblin)
    {
        monster = (CMonster*)m_lstIdleFlyGoblin.front();
        m_lstIdleFlyGoblin.pop();
        monster->Attach(/*obj,*/ dwMonsterID, monsterType,is_wild_monster,destory_monster_speed);
    }

    m_MonsterCount[(int)monsterType]++;
    m_mapMonster[dwMonsterID] = monster;
    //lstMonster.Add(obj, monster);
    return monster;
}

CMonster* CMonsterPool::CreateMonster(/*GameObject obj,*/ MonsterType monsterType,bool is_wild_monster,float destory_monster_speed/* = 1*/)
{
    if (IsFull(monsterType))
    {
        if (!AppendSize(monsterType))
        {
            TRACEFALTAL("对象池已满，创建失败:\n%s", GetPoolState());
            return nullptr;
        }
    }

    return Attach(/*obj,*/ monsterType,is_wild_monster,destory_monster_speed);
}

void CMonsterPool::DestroyMonster(DWORD id)
{
    auto it = m_mapMonster.find(id);
    if (it == m_mapMonster.end())
    {
        TRACEDEBUG("Cant find monsterid[%d]", id);
		WriteInfoLog(Format("CMonsterPool::DestroyMonster: %ld", id));
        return;
    }

    CMonster* monster = it->second;
    if (!m_destroying)
    {
        // 正在销毁时，交由统一移除
        m_mapMonster.erase(id);
    }

    MonsterType monsterType = monster->monsterType();

    //GameObject.Destroy(monster->objMonster);

    bool isMole = CMonsterManager::IsMole(monsterType);
    if (isMole)
    {
        RemoveFromHole(monster->holeidx());
    }

    monster->Detach();

    if (isMole)
    {
        m_lstIdleMole.push((CMole*)monster);
    }
    else if (monsterType == mtFly)
    {
        m_lstIdleFly.push((CFly*)monster);
    }
    else if (monsterType == mtSpider)
    {
        m_lstIdleSpider.push((CSpider*)monster);
    }
    else if (monsterType == mtFlyGoblin)
    {
        m_lstIdleFlyGoblin.push((CFlyGoblin*)monster);
    }
    m_MonsterCount[monsterType]--;
}

void CMonsterPool::DestroyAllMonsters()
{
    m_destroying = true;

    for (auto it = m_mapMonster.begin(); it != m_mapMonster.end(); it++)
    {
        it->second->Destroy(); // OnKilled();
        DestroyMonster(it->second->id()); // 回收对象池资源
    }

    m_mapMonster.clear();

    m_destroying = false;
}

CMonster* CMonsterPool::GetMonster(DWORD id /*GameObject objMonster*/)
{
    auto it = m_mapMonster.find(id);
    if (it == m_mapMonster.end())
        return nullptr;

    return it->second;
}

map<DWORD, CMonster*> CMonsterPool::get_table_monster_list()
{
	return m_mapMonster;
}
void CMonsterPool::CheckMonsterToDestroy(vector<DWORD>& lstMonster)
{
    DWORD curTick = GetTickCount();
    //LOGI("=====CheckMonsterToDestroy  CurTick=%d", curTick);
    for (auto it = m_mapMonster.begin(); it != m_mapMonster.end(); ++it)
    {      
        CMonster* pMonster = it->second;
        if (pMonster)
        {
            DWORD dwTickDestroy = pMonster->destroytime();
			WriteInfoLog(Format(_T("CheckMonsterToDestroy, MonsterType: %d, Hole:%d, dwTickDestroy: %u, curTick: %u"), (int)pMonster->monsterType(), pMonster->holeidx(), dwTickDestroy, curTick));

            //LOGI("Destroy=%d, id=%d", dwTickDestroy, pMonster->id());
            if (dwTickDestroy > 0 && curTick >= dwTickDestroy)
            {
                //LOGI("ADD Destroy=%d, id=%d", dwTickDestroy, pMonster->id());
                lstMonster.push_back(pMonster->id());
            }
			else
			{
				//校验一下数据
				const CommonMonsterProperty* infCommonProperty = CMonsterManager::Instance().GetMonsterConfig(pMonster->monsterType());

				if (infCommonProperty == nullptr)
				{
					continue;
				}

				DWORD dwEspectTime = curTick + (int((infCommonProperty->fWaitToEscapeTimeMax + infCommonProperty->fEscapeTimeMax + 10.0f) * 1000));

				if (dwTickDestroy > dwEspectTime)
				{
					WriteInfoLog(Format(_T("CheckMonsterToDestroy TimeFixed, dwTickDestroy: %u, dwEspectTime: %u"), dwTickDestroy, dwEspectTime));
					pMonster->m_dwTickDestroy = dwEspectTime;
				}
			}
        }
    }
}

std::string CMonsterPool::GetPoolState()
{
    char lBuffer[MAX_MSG_LEN] = {0};
    wsprintf(lBuffer, "占用情况：地鼠{%d}/9+苍蝇{%d}/{%d}+蜘蛛{%d}/{%d}+树懒{%d}/{%d}=总数{%d}  初始值：{%d}  最大值：{%d}", 
        MAX_HOLE - m_lstIdleMole.size(), m_curFlySize - m_lstIdleFly.size(), m_curFlySize, m_curSpiderSize - m_lstIdleSpider.size(), m_curSpiderSize,
        m_mapMonster.size(), m_curFlyGoblinSize - m_lstIdleFlyGoblin.size(), m_curFlyGoblinSize, m_defaultSize, m_maxSize);

	WriteInfoLog(CString(lBuffer));

    return lBuffer;
}

WORD CMonsterPool::GetMonsterNum(MonsterType monsterType)
{
    ASSERT(monsterType != mtNone && monsterType < MonsterTypeCount);
    if (monsterType == mtNone || monsterType >= MonsterTypeCount)
    {
        TRACEWARNING("monsterType[%d]错误", monsterType);
		WriteInfoLog(Format("CMonsterPool::GetMonsterNum   monsterType[%d]错误", monsterType));
        return 0;
    }

    if (monsterType == MonsterTypeCount)
        return m_mapMonster.size();
    else
        return m_MonsterCount[monsterType];
}

//-------------------------------------------------------------------------------------------------
DWORD CMonsterPool::CreateMonsterID()
{
    //	递增标识
    m_dwMonsterID++;   //

    // 由于MonsterID定义为int，所以实际还是按有符型计算，so
    if (MAX_INT == m_dwMonsterID)
    {
        m_dwMonsterID = 1;
    }
    return m_dwMonsterID;
}

DWORD CMonsterPool::CreateMonsterGroupID()
{
    m_dwGroupID++;
    if (MAX_INT == m_dwGroupID)
    {
        m_dwGroupID = 1;
    }
    return m_dwGroupID;
}


//-------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// MapManager

bool CMonsterPool::Respawn(CMonster* monster, CHole* hole)
{
    if (monster == nullptr)
    {
        TRACEWARNING("Monster cant be NULL");
        return false;
    }

    if (CMonsterManager::IsMole(monster->monsterType()))
    {
        if (hole == nullptr)
        {
            TRACEWARNING("Create Mole, Hole cant be NULL");
            return false;
        }

        if (hole->monster() != nullptr || hole->status() != hsEmpty)
        {
            TRACEWARNING("申请的洞被别人占了 %d？ ", hole->idx());
            return false;
        }

        hole->Respawn(monster);

        m_lstHole[hole->idx()] = hole;
        m_lstIdleHole.erase(std::find(m_lstIdleHole.begin(), m_lstIdleHole.end(), hole));
    }
    else
    {
        monster->Born(nullptr);
    }

    return true;
}

void CMonsterPool::RemoveFromHole(int idx)
{
    ASSERT(0 <= idx && idx <= 8);
    if (idx >= MAX_HOLE)
    {
        TRACEWARNING("idx[%d]错误，超过了最大值[%d]", idx, MAX_HOLE);
        return;
    }

    CHole* hole = m_lstHole[idx];
    hole->Remove();
    m_lstIdleHole.push_back(hole);
}

bool CMonsterPool::IsHoleFull() const
{
    return m_lstIdleHole.empty();
}

CHole* CMonsterPool::GetIdleHole()
{
    if (IsHoleFull())
        return nullptr;

    // 随机从空闲洞中取出一个
    int idx = CMonsterManager::Random(0, m_lstIdleHole.size());
    return m_lstIdleHole[idx];
}

void CMonsterManager::WriteInfoLog(int nTableID, CString szMsg)
{
	std::map<int, CGameServer*>::iterator itFind = m_LogInfo.find(nTableID);

	if (itFind != m_LogInfo.end())
	{
		CGameServer* pServer = itFind->second;

		if (pServer == NULL)
		{
			return;
		}

		pServer->WriteInfoLog(szMsg);
	}
}

void CMonsterManager::Attach(int nTableID, CGameServer* pServer)
{
	if (m_LogInfo.find(nTableID) == m_LogInfo.end())
	{
		m_LogInfo.insert(make_pair(nTableID, pServer));
	}

	CMonsterPool* pool = GetMonsterPool(nTableID);
	if (pool)
	{
		pool->pServer = pServer;
	}
}

void CMonsterManager::Detach(int nTableID)
{
	m_LogInfo.erase(nTableID);

	CMonsterPool* pool = GetMonsterPool(nTableID);
	if (pool)
	{
		pool->pServer = NULL;
	}
}

void CMonsterPool::WriteInfoLog(CString szMsg)
{
	if (pServer == NULL)
	{
		return;
	}

	pServer->WriteInfoLog(szMsg);
}
#include "stdafx.h"
#include "GameLogi.h"
#include "StatisticMgr.h"
#include "CharSetHelper.h"
#include "MonsterManager.h"

StatisticMgr& StatisticMgr::getInstance()
{
	static StatisticMgr _instance;

	return _instance;
}

StatisticMgr::StatisticMgr(void)
{
	InitializeCriticalSection(&m_Lock);

	m_hThread = INVALID_HANDLE_VALUE;
	m_dwFreq = 0;
	m_dtLastWrite = COleDateTime::GetTickCount();
	m_bStop = true;
}

StatisticMgr::~StatisticMgr(void)
{
	StopThread();

	DeleteCriticalSection(&m_Lock);
}

void StatisticMgr::EnsurePath(CString strPath)
{
	if (::PathIsRoot(strPath))
	{
		return;
	}//endif

	if (strPath.Right(1) == "\\")
	{
		strPath = strPath.Left(strPath.GetLength() - 1);
	}//endif

	if(::PathFileExists(strPath))
	{
		return;
	}//endif

	if(!PathFileExists(strPath))//文件夹不存在则创建
	{
		TCHAR* lpTemp = new TCHAR[strPath.GetLength() + 1];
		_tcscpy(lpTemp, strPath);

		::PathRemoveFileSpec(lpTemp);

		CString lRoot = lpTemp;

		EnsurePath(lRoot);

		::CreateDirectory(strPath, NULL);

		delete[] lpTemp;
	}//endif
}//EnsurePath(string strPath)

void StatisticMgr::Initialize()
{
	const StatisticConfig* info = GameConfigMgr::getInstance().Current().GetStatisticConfig();

	char szBuffer[1024] = {0};
	CharSetHelper::UTF8ToANSI(info->Target.c_str(), szBuffer, sizeof(szBuffer));

	m_szTarget = szBuffer;
	m_dwFreq = info->Frequency;

	if (m_szRoomName.IsEmpty() || m_szTarget.IsEmpty() || m_dwFreq <= 0)
	{
		return;
	}

	m_szTarget.AppendFormat(_T("\\打地鼠\\%s"), m_szRoomName);

	EnsurePath(m_szTarget);

	m_dwStartTime = COleDateTime::GetTickCount();

	//开启写日志线程
	StartThread();
}

void StatisticMgr::NormalHit(MonsterType monster, long lBet, long lReturn, long lCount, long lMiss, long lProb)
{
	AutoLock l(m_Lock);

	if (monster >= MonsterTypeCount)
	{
		return;
	}

	m_NormalHit[monster].lBet += lBet;
	m_NormalHit[monster].lReturn += lReturn;
	m_NormalHit[monster].lCount += lCount;
	m_NormalHit[monster].lMiss += lMiss;
	m_NormalHit[monster].lProb += lProb;
}

void StatisticMgr::KingHit(MonsterType monster, long lReturn, long lCount, long lMiss, long lProb)
{
	AutoLock l(m_Lock);

	if (monster >= MonsterTypeCount)
	{
		return;
	}

	m_KingHit[monster].lReturn += lReturn;
	m_KingHit[monster].lCount += lCount;
	m_KingHit[monster].lMiss += lMiss;
	m_KingHit[monster].lProb += lProb;
}

void StatisticMgr::DumpConfig(bool bStartup)
{
	COleDateTime dtCur = COleDateTime::GetTickCount();

	AutoLock l(m_Lock);

	FILE* pFile = NULL;

	try
	{
		CString szFileName = Format(_T("%s\\统计信息_%04d-%02d-%02d.log"), m_szTarget, dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay());

		pFile = fopen((LPSTR)(LPCTSTR)szFileName, "a");

		if (pFile == NULL)
		{
			return;
		}

		//写入信息
		fprintf(pFile, 
			Format(_T("******************** 写入时间：%04d年%02d月%02d日 %02d时%02d分%02d秒 ********************\r\n"),
			dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay(),
			dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond()));

		if (bStartup)
		{
			fprintf(pFile, _T("=== 游戏开始 ===\r\n"));
		}
		else
		{
			fprintf(pFile, _T("=== 配置更新 ===\r\n"));
		}

		for (int i = mtNone + 1; i < MonsterTypeCount; i++)
		{
			const MonsterPropertyInfo* pInfo = GameConfigMgr::getInstance().Current().GetMonsterPropertyInfo(m_iRoomID, (MonsterType)i);
			const CommonMonsterProperty* pProperty = GameConfigMgr::getInstance().Current().GetMonsterCommonProperty((MonsterType)i);

			if (pInfo == nullptr || pProperty == nullptr)
			{
				continue;
			}

			char szBuffer[1024] = {0};
			CharSetHelper::UTF8ToANSI(pProperty->MonsterName.c_str(), szBuffer, sizeof(szBuffer));

			m_MonsterNames[i] = szBuffer;

			fprintf(pFile, Format(_T("【%s】  ID:%d  出现率:%d(%0.2f%%%%)  同屏数:%d  倍率范围:[%d, %d]  免伤:%d(%0.2f%%%%)\r\n"), 
				szBuffer, 
				pInfo->monsterType,
				pInfo->RespawnProb, pInfo->RespawnProb / 10.0,
				pInfo->MaxOnScreen,
				pInfo->KillRateMin, pInfo->KillRateMax,
				pInfo->MissRate, pInfo->MissRate / 100.0));
		}

		fprintf(pFile, 
			Format(_T("\r\n******************** 写入时间：%04d年%02d月%02d日 %02d时%02d分%02d秒 ********************\r\n\r\n\r\n"),
			dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay(),
			dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond()));

		fflush(pFile);

		fclose(pFile);
		pFile = NULL;
	}
	catch (...)
	{
		if (!pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}
}
















CString _GetRate(int value, int base)
{
	if (base == 0)
	{
		return _T("0.0%%");
	}
	else
	{
		return Format(_T("%0.1f"), 100.0 * value / base) + _T("%%");
	}
}

void StatisticMgr::_WriteLogProc(void* apParam)
{
	StatisticMgr* pThis = (StatisticMgr*)apParam;

	if (!pThis)
	{
		return;
	}

	pThis->m_dtLastWrite  = COleDateTime::GetTickCount();

	while (!pThis->m_bStop)
	{
		//一秒执行一次
		Sleep(1000);

		COleDateTime dtCur = COleDateTime::GetTickCount();

		if ((dtCur - pThis->m_dtLastWrite).GetTotalSeconds() < pThis->m_dwFreq)
		{
			continue;
		}

		pThis->m_dtLastWrite = dtCur;

		//写日志
		AutoLock (pThis->m_Lock);

		FILE* pFile = NULL;

		try
		{
			CString szFileName = Format(_T("%s\\统计信息_%04d-%02d-%02d.log"), pThis->m_szTarget, dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay());

			pFile = fopen((LPSTR)(LPCTSTR)szFileName, "a");

			if (pFile == NULL)
			{
				continue;;
			}

			//写入基本信息
			fprintf(pFile, 
				Format(_T("==================== 写入时间：%04d年%02d月%02d日 %02d时%02d分%02d秒 ====================\r\n"),
				dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay(),
				dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond()));

			LONGLONG llTotalBet = 0, llTotalReturn = 0;
			LONGLONG llTotalKingBet = 0, llTotalKingReturn = 0;

			fprintf(pFile, _T("=== 基础统计 ===\r\n"));

			for (int i = mtNone + 1; i < MonsterTypeCount; i++)
			{
				HitInfo& info = pThis->m_NormalHit[(MonsterType)i];

				fprintf(pFile, Format(_T("【%s】  击杀:%lld(%s)  免伤:%lld(%s)  未命中:%lld(%s)  ==>  收入:%lld  返还:%lld  利润贡献:%lld\r\n"), pThis->m_MonsterNames[i], 
					info.lCount - info.lMiss - info.lProb, _GetRate(info.lCount - info.lMiss - info.lProb, info.lCount),
					info.lProb, _GetRate(info.lProb, info.lCount), 
					info.lMiss, _GetRate(info.lMiss, info.lCount), 
					info.lBet, info.lReturn, info.lBet - info.lReturn));

				llTotalBet += info.lBet;
				llTotalReturn += info.lReturn;

				if (i == mtMoleKing)
				{
					llTotalKingBet += info.lBet;
				}
			}

			fprintf(pFile, _T("\r\n=== 鼠王统计 ===\r\n"));

			for (int i = mtNone + 1; i < MonsterTypeCount; i++)
			{
				HitInfo& info = pThis->m_KingHit[(MonsterType)i];

				fprintf(pFile, Format(_T("【%s】  击杀:%lld(%s)  免伤:%lld(%s)  未命中:%lld(%s)  ==>  返还:%lld\r\n"), pThis->m_MonsterNames[i], 
					info.lCount - info.lMiss - info.lProb, _GetRate(info.lCount - info.lMiss - info.lProb, info.lCount),
					info.lProb, _GetRate(info.lProb, info.lCount), 
					info.lMiss, _GetRate(info.lMiss, info.lCount), 
					info.lReturn));

				llTotalKingReturn += info.lReturn;
				llTotalReturn += info.lReturn;
			}

			fprintf(pFile, _T("\r\n=== 总结 ===\r\n"));
			fprintf(pFile, Format(_T("【全局】  总收入:%lld  共返还:%lld  利润:%lld\r\n"), llTotalBet, llTotalReturn, llTotalBet - llTotalReturn));
			fprintf(pFile, Format(_T("【鼠王】  总收入:%lld  共返还:%lld  利润:%lld\r\n"), llTotalKingBet, llTotalKingReturn, llTotalKingBet - llTotalKingReturn));
			fprintf(pFile, 
				Format(_T("\r\n==================== 写入时间：%04d年%02d月%02d日 %02d时%02d分%02d秒 ====================\r\n\r\n\r\n"),
				dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay(),
				dtCur.GetHour(), dtCur.GetMinute(), dtCur.GetSecond()));

			fflush(pFile);

			fclose(pFile);
			pFile = NULL;
		}
		catch (...)
		{
			if (!pFile)
			{
				fclose(pFile);
				pFile = NULL;
			}
		}
	}
}

void StatisticMgr::StartThread()
{
	m_bStop = false;
	m_hThread = (HANDLE)_beginthread(_WriteLogProc, NULL, this);
}

void StatisticMgr::StopThread()
{
	m_bStop = true;

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = WaitForSingleObject(m_hThread, 3000);

		if (dwRet == WAIT_TIMEOUT)
		{
			::TerminateThread(m_hThread, -1);
		}

		m_hThread = INVALID_HANDLE_VALUE;
	}
}
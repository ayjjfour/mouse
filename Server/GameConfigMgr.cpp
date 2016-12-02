#include "stdafx.h"
#include "GameConfig.h"
#include "Trace.h"
#include "StatisticMgr.h"
#include "CMD5.h"

////////////////////////////////////////////////////////////////////////// static
GameConfigMgr& GameConfigMgr::getInstance()
{
	static GameConfigMgr _instance;

	return _instance;
}

bool GameConfigMgr::Initialize(int iRoomID)
{
	getInstance().m_iRoomID = iRoomID;
	if (getInstance().ReloadConfig())
	{
		getInstance().StartThread();

		return true;
	}
	else
	{
		AfxMessageBox(_T("配置文件有错，游戏无法启动，请检查配置"));
		return false;
	}
}

const CString GameConfigMgr::ConfigFile()
{
	return AfxGetApplicationDir() + _T("WhacAMole.xml");
}

////////////////////////////////////////////////////////////////////////// constructor
GameConfigMgr::GameConfigMgr()
{
	InitializeCriticalSection(&m_Lock);

	m_pCurConfig = NULL;
	m_hThread = INVALID_HANDLE_VALUE;
	m_bStop = true;
}

GameConfigMgr::~GameConfigMgr()
{
	DeleteCriticalSection(&m_Lock);
}


////////////////////////////////////////////////////////////////////////// funcs
CGameConfig& GameConfigMgr::Current()
{
	AutoLock l(m_Lock);

	if (m_pCurConfig != NULL)
	{
		return *m_pCurConfig;
	}
	else
	{
		return CGameConfig();
	}
}

bool GameConfigMgr::ReloadConfig()
{
	CGameConfig* pCache;

	if (m_pCurConfig != NULL)
	{
		pCache = &m_Config1;
	}
	else
	{
		if (m_pCurConfig != &m_Config1)
		{
			pCache = &m_Config1;
		}
		else
		{
			pCache = &m_Config2;
		}
	}

	TCHAR pMD5[100] = {0};

	{
		CMD5 md5;

		if (!md5.GetFileMD5(GameConfigMgr::ConfigFile(), pMD5, 100))
		{
			return false;
		}
	}

	//通过缓存加载
	if (!pCache->LoadConfig())
	{
		return false;
	}
	else
	{
		AutoLock l(m_Lock);
		m_pCurConfig = pCache;

		szMD5 = pMD5;

		return true;
	}
}


////////////////////////////////////////////////////////////////////////// multi thread
void GameConfigMgr::StartThread()
{
	m_bStop = false;
	m_hThread = (HANDLE)_beginthread(_LoadConfigProc, NULL, this);
}

void GameConfigMgr::StopThread()
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

void GameConfigMgr::_LoadConfigProc(void* apParam)
{
	GameConfigMgr* pThis = (GameConfigMgr*)apParam;

	if (pThis == NULL)
	{
		return;
	}

	int iRetryTime = 1000;
	TCHAR pMD5[100] = {0};

	//这个函数是负责重新加载配置文件的，所以这个函数对对象的直接操作可以保证绝对的线程同步，因此不用加锁
	while (!pThis->m_bStop)
	{
		if (pThis->m_pCurConfig == NULL)
		{
			::Sleep(10000);
			continue;
		}

		const GamePropertyInfo* infGameProperty = pThis->m_pCurConfig->GetGamePropertyInfo(pThis->m_iRoomID);
		::Sleep(infGameProperty->LoadFileTime);

		iRetryTime = 1000;

		//判断是不是需要重新加载
		{
			CMD5 md5;

			while (!md5.GetFileMD5(GameConfigMgr::ConfigFile(), pMD5, 100) && !pThis->m_bStop)
			{
				iRetryTime *= 2;

				if (iRetryTime > infGameProperty->LoadFileTime)
				{
					iRetryTime = infGameProperty->LoadFileTime;
				}

				TRACEFALTAL(_T("加载配置文件失败，%d毫秒后重试"), iRetryTime);

				::Sleep(iRetryTime);
			}

			if (pThis->szMD5.CompareNoCase(pMD5) == 0)
			{
				continue;
			}
		}

		iRetryTime = 1000;

		while (!pThis->ReloadConfig() && !pThis->m_bStop)
		{
			iRetryTime *= 2;

			if (iRetryTime > infGameProperty->LoadFileTime)
			{
				iRetryTime = infGameProperty->LoadFileTime;
			}

			TRACEFALTAL(_T("加载配置文件失败，%d毫秒后重试"), iRetryTime);

			::Sleep(iRetryTime);
		}

		StatisticMgr::getInstance().DumpConfig(false);
	}
}
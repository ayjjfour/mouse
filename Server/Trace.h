//////////////////////////
// 日志输出
/////////////////////////

#pragma once

#include <map>
#include <exception>
#include <process.h>
#include <Shlwapi.h>
using namespace std;

#define MAX_TRACE_MSG_LEN			2048			// 每次Trace的最大长度
#define MAX_LEN 256

#define TRACE_TYPE_DEBUG	0
#define TRACE_TYPE_WARNING	1
#define TRACE_TYPE_FALTAL	2

//返回的指针需要外部进行释放，采用 delete[]
static TCHAR* sprintf_c(TCHAR* apFormat, va_list aArgList)
{
	TCHAR* lpRet = NULL;

	{
		int iLength = _vsctprintf(apFormat, aArgList) + 1;

		lpRet = new TCHAR[iLength];

		_vstprintf_s(lpRet, iLength, apFormat, aArgList);
	}

	return lpRet;
}

//返回的指针需要外部进行释放，采用 delete[]
static TCHAR* sprintf_c(TCHAR* apFormat, ...)
{
	TCHAR* lpRet = NULL;

	va_list argList;
	va_start( argList, apFormat );
	{
		lpRet = sprintf_c(apFormat, argList);
	}
	va_end( argList );

	return lpRet;
}

class FileTrace
{
private:
	typedef struct _TraceBuffer
	{
		CStringA Msg;
		struct _TraceBuffer* pNext;
		struct _TraceBuffer* pTail;
	}  _TraceBuffer;

public:
	static FileTrace* getInstance()
	{
		static FileTrace instance;
		return &instance;
	}//getInstance()

private:
	FileTrace()
	{
		InitializeCriticalSection(&m_CriSec);

		m_bBlock = false;
		m_bTerminate = false;
		m_Lock = 0;
		m_ThreadID = INVALID_HANDLE_VALUE;

		m_ThreadID = (HANDLE)_beginthread(&FileTrace::OutputTimerProc, 0, (void*)this);
	}//FileTrace()

	~FileTrace()
	{
		Dispose();

		DeleteCriticalSection(&m_CriSec);
	}//~FileTrace()
private:
	static void OutputTimerProc(void* apParam)
	{
		FileTrace* pThis = (FileTrace*)apParam;

		time_t lPrevTime = time(NULL);

		while (!pThis->m_bTerminate)
		{
			Sleep(500);

			if (pThis->m_bBlock)
			{
				continue;
			}//endif

			Sleep(500);

			time_t lCurTime = time(NULL);

			if (lCurTime - lPrevTime < 1)
			{
				continue;;
			}//endif

			pThis->Lock(true);
			{
				map<CString, _TraceBuffer*>::iterator i = pThis->m_Output.begin();

				while (i != pThis->m_Output.end())
				{
					if (pThis->m_bBlock || pThis->m_bTerminate)
					{
						break;
					}

					if (WriteToFile((*i).first, (*i).second))
					{
						pThis->m_Output.erase(i++);
					}
					else
					{
						i++;
					}
				}
			}
			pThis->Lock(false);

			lPrevTime = time(NULL);
		}//while (!pThis->m_bTerminate)
	}//OutputTimerProc(void* apParam)
	//自动生成目录树
	static void EnsurePath(CString strPath)
	{
		if (PathIsRoot(strPath))
		{
			return;
		}//endif

		if (strPath.Right(1) == "\\")
		{
			strPath = strPath.Left(strPath.GetLength() - 1);
		}//endif

		if(PathFileExists(strPath))
		{
			return;
		}//endif

		if(!PathFileExists(strPath))//文件夹不存在则创建
		{
			TCHAR* lpTemp = new TCHAR[strPath.GetLength() + 1];
			_tcscpy(lpTemp, strPath);

			PathRemoveFileSpec(lpTemp);

			CString lRoot = lpTemp;

			EnsurePath(lRoot);

			CreateDirectory(strPath, NULL);

			delete[] lpTemp;
		}//endif
	}//EnsurePath(string strPath)
	//相对路径映射成绝对路径
	static CString MapPath(CString aRelativePath, CString aAbsolutePath)
	{
		TCHAR lpBuffer[MAX_PATH + 1];

		if (aAbsolutePath.IsEmpty())
		{
			TCHAR szFilePath[MAX_PATH];

			GetModuleFileName(NULL, szFilePath, MAX_PATH);

			PathRemoveFileSpec(szFilePath);

			aAbsolutePath = szFilePath;

			aAbsolutePath = PathCombine(lpBuffer, aAbsolutePath, _T("App_Log"));
		}//endif

		return PathCombine(lpBuffer, aAbsolutePath, aRelativePath);
	}//MapPath(string aRelativePath, string aAbsolutePath)

	static bool WriteToFile(CString aFilePath, _TraceBuffer* apMsg)
	{
		CString lFilePath;

		if (PathIsRelative(aFilePath))
		{
			lFilePath = MapPath(aFilePath, _T(""));
		}//else
		else
		{
			lFilePath = aFilePath;
		}//endif

		TCHAR* lpTemp = new TCHAR[lFilePath.GetLength() + 1];
		_tcscpy(lpTemp, lFilePath);
		PathRemoveFileSpec(lpTemp);

		EnsurePath(lpTemp);

		delete [] lpTemp;

		HANDLE hFile = CreateFile(lFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}//endif

		SetFilePointer(hFile,0, NULL, FILE_END);
		DWORD dwWritten;

		while (apMsg)
		{
			WriteFile(hFile, apMsg->Msg, apMsg->Msg.GetLength(), &dwWritten, NULL);
			

			_TraceBuffer* lpCur = apMsg;
			apMsg = apMsg->pNext;

			delete lpCur;
		}//while (apMsg)

		CloseHandle(hFile);

		return true;
	}//WriteToFile(string aFilePath, _TraceBuffer* apMsg)

	virtual void Lock(bool aLock)
	{
		if (aLock)
		{
			EnterCriticalSection(&m_CriSec);
		}
		else
		{
			LeaveCriticalSection(&m_CriSec);
		}
	}//while (apMsg)

public:
	void WriteLogWithFileName(TCHAR* aFileName, TCHAR* aFormat, ...)
	{
		if (m_bBlock)
		{
			return;
		}//endif

		TCHAR* szBuffer = NULL;

		time_t ltmCurTime = time(NULL);
		tm* lpCurTime = localtime(&ltmCurTime);
	
		va_list args;
		va_start(args, aFormat);
		{
			TCHAR* lFormat = sprintf_c(_T("[%04d-%02d-%02d %02d:%02d:%02d] %s\r\n") 
				, lpCurTime->tm_year + 1900 
				, lpCurTime->tm_mon + 1
				, lpCurTime->tm_mday
				, lpCurTime->tm_hour
				, lpCurTime->tm_min
				, lpCurTime->tm_sec
				, aFormat);

			szBuffer = sprintf_c(lFormat, args);

			delete[] lFormat;
		}
		va_end(args);

		_TraceBuffer* lpBuffer = new _TraceBuffer;
		lpBuffer->Msg = szBuffer;
		lpBuffer->pNext = NULL;
		lpBuffer->pTail = NULL;

		CString sFileName = aFileName;
		sFileName += ".log";

		delete[] szBuffer;

		Lock(true);
		{
			OUTPUTCACHE::iterator itFind = m_Output.find(sFileName);

			if (itFind != m_Output.end())
			{
				itFind->second->pTail->pNext = lpBuffer;
				itFind->second->pTail = lpBuffer;
			}
			else
			{
				lpBuffer->pTail = lpBuffer;
				m_Output.insert(make_pair(sFileName, lpBuffer));
			}
		}
		Lock(false);
	}//WriteLogWithFileName(TCHAR* aFileName, TCHAR* aFormat, ...)

	void WriteLog(TCHAR* aFormat, ...)
	{
		if (m_bBlock)
		{
			return;
		}//endif

		TCHAR szModule[MAX_PATH];

		GetModuleFileName(NULL, szModule, MAX_PATH);

		va_list args;
		va_start(args, aFormat);  
		{
			WriteLogWithFileName(PathFindFileName(szModule), aFormat, args);
		}
		va_end(args);  
	}//WriteLog(TCHAR* aFormat, ...)

	void BlockWriteLog(bool aBlock)
	{
		m_bBlock = aBlock;
	}//BlockWriteLog(bool aBlock)

	void Dispose()
	{
		m_bBlock = true;
		m_bTerminate = true;

		if (m_ThreadID != INVALID_HANDLE_VALUE)
		{
			DWORD dwRet = WaitForSingleObject(m_ThreadID, 2000);

			if (WAIT_TIMEOUT == dwRet)
			{
				TerminateThread(m_ThreadID, 0);
			}

			m_ThreadID = INVALID_HANDLE_VALUE;
		}//endif
	}//Dispose()

private:
	typedef map<CString, _TraceBuffer*> OUTPUTCACHE;
	OUTPUTCACHE m_Output;

	bool m_bBlock;
	bool m_bTerminate;
	long m_Lock;
	HANDLE m_ThreadID;

	CRITICAL_SECTION m_CriSec;
};

static TCHAR* GetTraceType(int aType)
{
	switch (aType)
	{
	case TRACE_TYPE_DEBUG:
		return _T("DEBUG");
	case TRACE_TYPE_WARNING:
		return _T("WARNING");
	case TRACE_TYPE_FALTAL:
		return _T("FALTAL");
	default:
		return _T("UNKNOWN");
	}//switch
}//GetTraceType(int aType)

static void WriteTraceInfoWithFileName(TCHAR* apFileName, int aType, TCHAR* apMsg, ...)
{
	TCHAR* str = sprintf_c(_T("%s->%s"), GetTraceType(aType), apMsg);

	va_list args;
	va_start(args, apMsg);  
	{
		TCHAR* lpTemp = sprintf_c(str, args);

		FileTrace::getInstance()->WriteLogWithFileName(apFileName, lpTemp);

		delete[] lpTemp;
	}
	va_end(args);  

	delete[] str;
}//WriteTraceInfoWithFileName(TCHAR* apFileName, int aType, TCHAR* apMsg, ...)

static void WriteTraceInfo(int aType, TCHAR* apMsg, ...)
{
	TCHAR* str = sprintf_c(_T("%s->%s"), GetTraceType(aType), apMsg);

	va_list args;
	va_start(args, apMsg);  
	{
		TCHAR* lpTemp = sprintf_c(str, args);

		FileTrace::getInstance()->WriteLog(lpTemp, args);

		delete[] lpTemp;
	}
	va_end(args);  

	delete[] str;
}//WriteTraceInfo(int aType, TCHAR* apMsg, ...)

#if defined(_TRACE_LEVEL_2_)

#define TRACEDEBUGFILE(file, msg, ...) WriteTraceInfoWithFileName(file, TRACE_TYPE_DEBUG, msg, __VA_ARGS__)
#define TRACEDEBUG(msg, ...) WriteTraceInfo(TRACE_TYPE_DEBUG, msg, __VA_ARGS__)

#else

#define TRACEDEBUG(msg, ...)
#define TRACEDEBUGFILE(file, msg, ...)

#endif

#if defined(_TRACE_LEVEL_1_) || defined(_TRACE_LEVEL_2_)

#define TRACEWARNINGFILE(file, msg, ...) WriteTraceInfoWithFileName(file, TRACE_TYPE_WARNING, msg, __VA_ARGS__)
#define TRACEWARNING(msg, ...) WriteTraceInfo(TRACE_TYPE_WARNING, msg, __VA_ARGS__)

#else

#define TRACEWARNING(msg, ...)
#define TRACEWARNINGFILE(file, msg, ...)

#endif


#define TRACEFALTALFILE(file, msg, ...) WriteTraceInfoWithFileName(file, TRACE_TYPE_FALTAL, msg, __VA_ARGS__)
#define TRACEFALTAL(msg, ...) WriteTraceInfo(TRACE_TYPE_FALTAL, msg, __VA_ARGS__)
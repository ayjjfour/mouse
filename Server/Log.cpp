#include "stdafx.h"
#include "log.h"

CLog::CLog(void)
{
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);

	PathRemoveFileSpec(szFilePath);

	m_szConfigPath.Format("%s\\GameServerConfig.ini", szFilePath);
}

CLog::~CLog(void)
{
}

//设置规则号进来，标识它是哪个房间用
void CLog::SetGameRoomID( int nID )
{
	m_nGameRoomID = nID;
	itoa( nID , cRoomID ,10);
}

//初始化日志
void	CLog::InitLog()
{
/*	TCHAR szBuffer[MAX_PATH] = {'\0'};
	TCHAR szFileName[MAX_PATH] = {'\0'};
	TCHAR szFilePath[MAX_PATH] = {'\0'};
	GetCurrentDirectory(sizeof(szBuffer), szBuffer);	
	_sntprintf(szFileName, sizeof(szFileName), TEXT("%s\\gameserverconfig.ini"), szBuffer);
	GetPrivateProfileString("GameLogConfig","Path","d:\\",szFilePath, sizeof(szFilePath), szFileName);

	CString strPath;
	strPath = szFilePath;
	strPath += "\\金蟾捕鱼\\";

	SuperMkDir(strPath);
	TCHAR szTempPath[MAX_PATH] = {'\0'};
	string tempPath;
	GetModuleFileName(NULL,szTempPath,sizeof(szTempPath));
	tempPath = szTempPath;
	tempPath = tempPath.substr(0,tempPath.rfind("\\"));
	tempPath += "\\";
	string dllpath;
	dllpath =  tempPath;
	dllpath += "ReadSqliteLogDll.dll";
	string DBpath;

	DBpath = strPath;
	DBpath += "NiuNiu.db";
	string configPath;
	configPath = tempPath;
	configPath +="GameInI\\";
	configPath += "NiuNiuLogIni.ini";
	TCHAR szConfigPath[MAX_PATH] = {'\0'};
	memset(m_szDBpath,0,MAX_PATH);
	memcpy(szConfigPath,configPath.c_str(),strlen(configPath.c_str())+1);
	memcpy(m_szDBpath,DBpath.c_str(),strlen(DBpath.c_str())+1);

	hs = LoadLibrary(dllpath.c_str());
	if (hs != NULL)
	{
		pf = (pt_fun)GetProcAddress(hs ,"IniDBLog");
		if ( pf != NULL)
		{
			pf(m_szDBpath,cRoomID,szConfigPath,"金蟾捕鱼");
			pf_insert = (pt_Insertfun)GetProcAddress(hs ,"insertData");
		}
	}
	*/
}

//当前的本地时间年月日数转换数字，如20110825
int CLog::GetCurYearMonthDayNo()
{
	int nYMDay = 0;
	time_t sTime = time(NULL);
	struct tm *tm_local;

	tm_local = localtime(&sTime);
	nYMDay = (tm_local->tm_year + 1900) * 10000 + (tm_local->tm_mon + 1) * 100 + tm_local->tm_mday;

	return nYMDay;
}


// 写错误日志文件，nOp有3种类型：0：打开文件；1：写文件；2：关闭文件
void CLog::WriteErrorLogFile(LPCTSTR pszMessage, enLogOp nOp)
{
	CFile fileRecord;
	static bool bOpenned = false;
	static TCHAR szPath[MAX_PATH]=TEXT("");
	static TCHAR szFileName[MAX_PATH]=TEXT("");

	try
	{
		switch (nOp)
		{
		case LOG_OPEN:
			{
				m_nErrorLogFileSerialNo = GetCurYearMonthDayNo();

				GetPrivateProfileString("PATH", TEXT("DBpath"), TEXT("C:\\"), szPath, sizeof(szPath), m_szConfigPath);

				_sntprintf(szFileName, _countof(szFileName), TEXT("%s\\%s\\WhacAMole_Error_%d_%d.txt"), szPath, FISHING_LOG_DIR, m_nGameRoomID, m_nErrorLogFileSerialNo);

				EnsureDir(szFileName);

				//打开文件
				if (fileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
				{
					if ( lstrlen(pszMessage) > 0/* && (DWORD)(time(NULL)-60) > GetPrivateProfileInt(TEXT("LogRecord"), TEXT("LogInitTime"), 0, m_szStatisticFileName)*/ )
					{
						//WritePrivateProfileString(TEXT("LogRecord"), TEXT("LogInitTime"), szBuf, m_szStatisticFileName);
						fileRecord.SeekToEnd();
						fileRecord.Write(pszMessage,lstrlen(pszMessage));
						{	
							//带时间打印
							struct tm *ptr;
							time_t lt;
							lt = time_t(time(NULL));
							ptr=localtime(&lt);
							fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
						}
						fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
					}
					bOpenned = true;
					fileRecord.Close();
				}
			}
			break;
		case LOG_WRITE:
			if (true == bOpenned && lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
			{
				fileRecord.SeekToEnd();
				fileRecord.Write(pszMessage,lstrlen(pszMessage));
				{	
					// 带时间打印
					struct tm *ptr;
					time_t lt;
					lt = time_t(time(NULL));
					ptr=localtime(&lt);
					fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
				}
				fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
 
				fileRecord.Close();
			}
			break;
		case LOG_CLOSE:
			if (true == bOpenned)
			{
				if (lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
				{
					fileRecord.SeekToEnd();
					fileRecord.Write(pszMessage,lstrlen(pszMessage));
					{	// * /带时间打印
						struct tm *ptr;
						time_t lt;
						lt = time_t(time(NULL));
						ptr=localtime(&lt);
						fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
					}
					fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

					fileRecord.Close();
				}
				bOpenned = false;
			}
			break;
		}
	}
	catch(...)
	{

	}	;
}
//写问题跟踪日志文件
void CLog::WriteTraceDataFishLogFile(LPCTSTR pszMessage,enLogOp nOp)
{
	CFile fileRecord;
	static bool bOpenned = false;
	static TCHAR szPath[MAX_PATH]=TEXT("");
	static TCHAR szFileName[MAX_PATH]=TEXT("");

	int l = 0;
	l =  sizeof(pszMessage);


	try
	{
		switch (nOp)
		{
		case LOG_OPEN:
			{
				m_nErrorLogFileSerialNo = GetCurYearMonthDayNo();

				//GetCurrentDirectory(_countof(szPath),szPath);
				GetPrivateProfileString("PATH", TEXT("DBpath"), TEXT("C:\\"), szPath, sizeof(szPath), m_szConfigPath);
				_sntprintf(szFileName, _countof(szFileName), TEXT("%s\\%s\\WhacAMole_TraceDataFish_%d_%d.txt"), szPath, FISHING_LOG_DIR, m_nGameRoomID, m_nErrorLogFileSerialNo);

				EnsureDir(szFileName);

				//打开文件
				if (fileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
				{
					if ( lstrlen(pszMessage) > 0/* && (DWORD)(time(NULL)-60) > GetPrivateProfileInt(TEXT("LogRecord"), TEXT("LogInitTime"), 0, m_szStatisticFileName)*/ )
					{
						//WritePrivateProfileString(TEXT("LogRecord"), TEXT("LogInitTime"), szBuf, m_szStatisticFileName);
						fileRecord.SeekToEnd();
						fileRecord.Write(pszMessage,lstrlen(pszMessage));
						{	
							//带时间打印
							struct tm *ptr;
							time_t lt;
							lt = time_t(time(NULL));
							ptr=localtime(&lt);
							fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
						}
						fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
					}
					bOpenned = true;
					fileRecord.Close();
				}
			}
			break;
		case LOG_WRITE:
			if (true == bOpenned && lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
			{
				fileRecord.SeekToEnd();
				fileRecord.Write(pszMessage,lstrlen(pszMessage));
				{	
					// 带时间打印
					struct tm *ptr;
					time_t lt;
					lt = time_t(time(NULL));
					ptr=localtime(&lt);
					fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
				}
				fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

				fileRecord.Close();
			}
			break;
		case LOG_CLOSE:
			if (true == bOpenned)
			{
				if (lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
				{
					fileRecord.SeekToEnd();
					fileRecord.Write(pszMessage,lstrlen(pszMessage));
					{	// * /带时间打印
						struct tm *ptr;
						time_t lt;
						lt = time_t(time(NULL));
						ptr=localtime(&lt);
						fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
					}
					fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

					fileRecord.Close();
				}
				bOpenned = false;
			}
			break;
		}
	}
	catch(...)
	{

	}	;
}


// 写数据日志文件，nOp有3种类型：0：打开文件；1：写文件；2：关闭文件
void CLog::WriteDataLogFile(LPCTSTR pszMessage, enLogOp nOp)
{
	CFile fileRecord;
	static bool bOpenned = false;
	static TCHAR szPath[MAX_PATH]=TEXT("");
	static TCHAR szFileName[MAX_PATH]=TEXT("");
	
	int l = 0;
	l =  sizeof(pszMessage);


	try
	{
		switch (nOp)
		{
		case LOG_OPEN:
			{
				m_nErrorLogFileSerialNo = GetCurYearMonthDayNo();

				//GetCurrentDirectory(_countof(szPath),szPath);
				GetPrivateProfileString("PATH", TEXT("DBpath"), TEXT("C:\\"), szPath, sizeof(szPath), m_szConfigPath);
				_sntprintf(szFileName, _countof(szFileName), TEXT("%s\\%s\\WhacAMole_Data_%d_%d.txt"), szPath, FISHING_LOG_DIR, m_nGameRoomID, m_nErrorLogFileSerialNo);

				EnsureDir(szFileName);

				//打开文件
				if (fileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
				{
					if ( lstrlen(pszMessage) > 0/* && (DWORD)(time(NULL)-60) > GetPrivateProfileInt(TEXT("LogRecord"), TEXT("LogInitTime"), 0, m_szStatisticFileName)*/ )
					{
						//WritePrivateProfileString(TEXT("LogRecord"), TEXT("LogInitTime"), szBuf, m_szStatisticFileName);
						fileRecord.SeekToEnd();
						fileRecord.Write(pszMessage,lstrlen(pszMessage));
						{	
							//带时间打印
							struct tm *ptr;
							time_t lt;
							lt = time_t(time(NULL));
							ptr=localtime(&lt);
							fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
						}
						fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
					}
					bOpenned = true;
					fileRecord.Close();
				}
			}
			break;
		case LOG_WRITE:
			if (true == bOpenned && lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
			{
				fileRecord.SeekToEnd();
				fileRecord.Write(pszMessage,lstrlen(pszMessage));
				{	
					// 带时间打印
					struct tm *ptr;
					time_t lt;
					lt = time_t(time(NULL));
					ptr=localtime(&lt);
					fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
				}
				fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

				fileRecord.Close();
			}
			break;
		case LOG_CLOSE:
			if (true == bOpenned)
			{
				if (lstrlen(pszMessage) > 0 && fileRecord.Open(szFileName,CFile::modeNoTruncate|CFile::modeWrite))
				{
					fileRecord.SeekToEnd();
					fileRecord.Write(pszMessage,lstrlen(pszMessage));
					{	// * /带时间打印
						struct tm *ptr;
						time_t lt;
						lt = time_t(time(NULL));
						ptr=localtime(&lt);
						fileRecord.Write(asctime(ptr), lstrlen(asctime(ptr)));
					}
					fileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

					fileRecord.Close();
				}
				bOpenned = false;
			}
			break;
		}
	}
	catch(...)
	{

	}	;
}

//是否存在该文件
bool CLog::FolderExists(CString sPath)
{
	DWORD attr;
	attr = GetFileAttributes(sPath);
	return (attr != (DWORD)(-1) ) &&
		( attr & FILE_ATTRIBUTE_DIRECTORY);
}

// 递归创建目录， 如果目录已经存在或者创建成功返回TRUE
bool CLog::SuperMkDir(CString sPath)
{
	int len=sPath.GetLength();
	if ( len <2 ) return false;

	if('\\'==sPath[len-1])
	{
		sPath=sPath.Left(len-1);
		len=sPath.GetLength();
	}
	if ( len <=0 ) return false;

	if (len <=3)
	{
		if (FolderExists(sPath))return true;
		else return false;
	}

	if (FolderExists(sPath))return true;    

	CString Parent;
	Parent=sPath.Left(sPath.ReverseFind('\\') );

	if(Parent.GetLength()<=0)return false;

	bool Ret=SuperMkDir(Parent);

	if(Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor=NULL;
		sa.bInheritHandle=0;
		Ret=(CreateDirectory(sPath,&sa)==TRUE);
		return Ret;
	}
	else
		return false;
}

void CLog::EnsureDir(CString sPath)
{
	TCHAR szDir[MAX_PATH] = {0};
	CString::CopyChars(szDir, sPath, sPath.GetLength());

	PathRemoveFileSpec(szDir);

	SuperMkDir(szDir);
}
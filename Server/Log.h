#pragma once
//用于游戏日志记录类

#define FISHING_LOG_DIR			"打地鼠"					//日志路径，26Oct.2012
enum enLogOp { LOG_OPEN , LOG_WRITE , LOG_CLOSE };					//打开，写入，关闭
//新日志系统
typedef bool (*pt_fun)(char *,char *  ,char *,char *);
typedef bool (*pt_Insertfun)(char *,char *,int,...);

class CLog
{
public:
	CLog(void);
	~CLog(void);
private:
	int							m_nErrorLogFileSerialNo;										//错误日志文件序列号
	TCHAR						m_tcSaveDataFileName[MAX_PATH];					//日志文件名
	int							m_nGameRoomID;												//规则号指定的统计文件的名称编号
	TCHAR						cRoomID[32];
	DWORD					m_dwWriteHistoryElapse;										//写统计历史数据的时间间隔，单位：秒
	TCHAR						m_szStatisticFileName[MAX_PATH];
	TCHAR						m_szHistoryFileName[MAX_PATH];						//从统计中分离历史记录文件
private:
	HINSTANCE hs;
	pt_fun  pf;
	pt_Insertfun  pf_insert;

public:
	//设置规则号进来，标识它是哪个房间用
	void SetGameRoomID( int nID );
	//初始化日志
	void	InitLog();
private:
	//是否存在该文件
	bool FolderExists(CString sPath);
		// 递归创建目录， 如果目录已经存在或者创建成功返回TRUE
	bool SuperMkDir(CString sPath);
	//当前的本地时间年月日数转换数字，如20110825
	int GetCurYearMonthDayNo();

	void EnsureDir(CString sPath);
public:

	// 写错误日志文件，nOp有3种类型：0：打开文件；1：写文件；2：关闭文件
	void WriteErrorLogFile(LPCTSTR pszMessage, enLogOp nOp = LOG_WRITE );
	// 写数据日志文件，nOp有3种类型：0：打开文件；1：写文件；2：关闭文件
	void WriteDataLogFile(LPCTSTR pszMessage, enLogOp nOp = LOG_WRITE );
	// 写鱼问题跟踪数据
	void  WriteTraceDataFishLogFile(LPCTSTR pszMessage,enLogOp nOp = LOG_WRITE);      

private:
	CString m_szConfigPath;
};

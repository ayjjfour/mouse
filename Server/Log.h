#pragma once
//������Ϸ��־��¼��

#define FISHING_LOG_DIR			"�����"					//��־·����26Oct.2012
enum enLogOp { LOG_OPEN , LOG_WRITE , LOG_CLOSE };					//�򿪣�д�룬�ر�
//����־ϵͳ
typedef bool (*pt_fun)(char *,char *  ,char *,char *);
typedef bool (*pt_Insertfun)(char *,char *,int,...);

class CLog
{
public:
	CLog(void);
	~CLog(void);
private:
	int							m_nErrorLogFileSerialNo;										//������־�ļ����к�
	TCHAR						m_tcSaveDataFileName[MAX_PATH];					//��־�ļ���
	int							m_nGameRoomID;												//�����ָ����ͳ���ļ������Ʊ��
	TCHAR						cRoomID[32];
	DWORD					m_dwWriteHistoryElapse;										//дͳ����ʷ���ݵ�ʱ��������λ����
	TCHAR						m_szStatisticFileName[MAX_PATH];
	TCHAR						m_szHistoryFileName[MAX_PATH];						//��ͳ���з�����ʷ��¼�ļ�
private:
	HINSTANCE hs;
	pt_fun  pf;
	pt_Insertfun  pf_insert;

public:
	//���ù���Ž�������ʶ�����ĸ�������
	void SetGameRoomID( int nID );
	//��ʼ����־
	void	InitLog();
private:
	//�Ƿ���ڸ��ļ�
	bool FolderExists(CString sPath);
		// �ݹ鴴��Ŀ¼�� ���Ŀ¼�Ѿ����ڻ��ߴ����ɹ�����TRUE
	bool SuperMkDir(CString sPath);
	//��ǰ�ı���ʱ����������ת�����֣���20110825
	int GetCurYearMonthDayNo();

	void EnsureDir(CString sPath);
public:

	// д������־�ļ���nOp��3�����ͣ�0�����ļ���1��д�ļ���2���ر��ļ�
	void WriteErrorLogFile(LPCTSTR pszMessage, enLogOp nOp = LOG_WRITE );
	// д������־�ļ���nOp��3�����ͣ�0�����ļ���1��д�ļ���2���ر��ļ�
	void WriteDataLogFile(LPCTSTR pszMessage, enLogOp nOp = LOG_WRITE );
	// д�������������
	void  WriteTraceDataFishLogFile(LPCTSTR pszMessage,enLogOp nOp = LOG_WRITE);      

private:
	CString m_szConfigPath;
};

#pragma once 
#include "ZMD5/ZMD5.h"


//------------
//class CMD5
//------------
class CMD5
{
public:
    CMD5();
    ~CMD5();

public:
    //����ָ���ļ���MD5�����ֽ�ģʽ��ʹ�ã�
    BOOL GetFileMD5A(const char* sFileName, char* sOut, const int nMaxLen);

    //����ָ���ļ���MD5��UNICODE�ֽ�ģʽ��ʹ�ã�
    BOOL GetFileMD5W(const WCHAR* sFileName, WCHAR* sOut, const int nMaxLen);

	BOOL GetFileMD5(const TCHAR* szFileName, TCHAR* sOut, const int nMaxLen);

private:
    ZMD5 m_ZMD5;
};
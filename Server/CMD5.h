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
    //返回指定文件的MD5（多字节模式下使用）
    BOOL GetFileMD5A(const char* sFileName, char* sOut, const int nMaxLen);

    //返回指定文件的MD5（UNICODE字节模式下使用）
    BOOL GetFileMD5W(const WCHAR* sFileName, WCHAR* sOut, const int nMaxLen);

	BOOL GetFileMD5(const TCHAR* szFileName, TCHAR* sOut, const int nMaxLen);

private:
    ZMD5 m_ZMD5;
};
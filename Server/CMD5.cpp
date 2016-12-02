#include "stdafx.h"
#include "CMD5.h"


CMD5::CMD5()
{
    //
}//CMD5

CMD5::~CMD5()
{
    //
}//~CMD5

BOOL CMD5::GetFileMD5A(const char* sFileName, char* sOut, const int nMaxLen)
{
    //清零
    ::ZeroMemory(sOut, nMaxLen);

    //获取MD5
    std::string sMD5 = m_ZMD5.GetMD5OfFile((char*)sFileName, false);

    //判断
    if(sMD5.length() <= 0 || nMaxLen < sMD5.length()) return FALSE;

    //拷贝
    if( 0 != ::strcpy_s(sOut, nMaxLen, sMD5.c_str())) return FALSE;

    return TRUE;
}//GetFileMD5A

//返回指定文件的MD5（UNICODE字节模式下使用）
BOOL CMD5::GetFileMD5W(const WCHAR* sFileName, WCHAR* sOut, const int nMaxLen)
{
    //把UNICODE的文件名--->多字节的文件名
    char sMultiByteFilePath[512] = "";      //多字节的文件名
    ::WideCharToMultiByte(CP_ACP, 0,sFileName, wcslen(sFileName), sMultiByteFilePath, sizeof(sMultiByteFilePath), NULL, NULL);

    //把多字节的MD5--->UNICODE的MD5
    char sMultiByteMD5[512] = "";   //多字节的MD5码
    if( !this->GetFileMD5A(sMultiByteFilePath, sMultiByteMD5, sizeof(sMultiByteMD5)) ) return FALSE;

    ::MultiByteToWideChar(CP_ACP, 0, sMultiByteMD5, -1, sOut, nMaxLen);

    return TRUE;
}//GetFileMD5W

BOOL CMD5::GetFileMD5(const TCHAR* szFileName, TCHAR* sOut, const int nMaxLen)
{
#ifdef  UNICODE
	return GetFileMD5W(szFileName, sOut, nMaxLen);
#else
	return GetFileMD5A(szFileName, sOut, nMaxLen);
#endif
}
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
    //����
    ::ZeroMemory(sOut, nMaxLen);

    //��ȡMD5
    std::string sMD5 = m_ZMD5.GetMD5OfFile((char*)sFileName, false);

    //�ж�
    if(sMD5.length() <= 0 || nMaxLen < sMD5.length()) return FALSE;

    //����
    if( 0 != ::strcpy_s(sOut, nMaxLen, sMD5.c_str())) return FALSE;

    return TRUE;
}//GetFileMD5A

//����ָ���ļ���MD5��UNICODE�ֽ�ģʽ��ʹ�ã�
BOOL CMD5::GetFileMD5W(const WCHAR* sFileName, WCHAR* sOut, const int nMaxLen)
{
    //��UNICODE���ļ���--->���ֽڵ��ļ���
    char sMultiByteFilePath[512] = "";      //���ֽڵ��ļ���
    ::WideCharToMultiByte(CP_ACP, 0,sFileName, wcslen(sFileName), sMultiByteFilePath, sizeof(sMultiByteFilePath), NULL, NULL);

    //�Ѷ��ֽڵ�MD5--->UNICODE��MD5
    char sMultiByteMD5[512] = "";   //���ֽڵ�MD5��
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
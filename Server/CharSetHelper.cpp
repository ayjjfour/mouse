#include "StdAfx.h"
#include "CharSetHelper.h"


bool CharSetHelper::IsUTF8( char const* apStr, int aLen )
{
	//字符编码
	UINT CodePage=0;
	//字符串长度
	int nLen=strlen(apStr);

	//判断是否为UTF-8
	//至少要3字节
	bool lRet = true;
	if(nLen>=3)
	{
		unsigned char U1,U2,U3;
		int nNow=0;
		while(nNow<nLen)
		{
			U1=(unsigned)apStr[nNow];
			if((U1&0x80)==0x80)
			{
				//中文字符，则要三个字符
				if(nLen>nNow+2)
				{
					U2=(unsigned)apStr[nNow+1];
					U3=(unsigned)apStr[nNow+2];
					//中文三字节为0xE0 0xC0 0xC0
					if(((U1&0xE0)==0XE0) && ((U2&0xC0)==0x80) && ((U3&0xC0)==0x80))
					{
						//有可能是UTF-8
						CodePage=65001;
						nNow=nNow+3;
					}
					else
					{
						//不是UTF-8
						CodePage=0;
						lRet = false;
						break;
					}
				}
				else
				{
					//不是UTF-8
					CodePage=0;
					lRet = false;
					break;
				}
			}
			else
			{
				//非中文字符
				nNow++;
			}
		}
	}
	else
	{
		lRet = false;
	}
	return lRet;
}

char* CharSetHelper::ANSIToUTF8( char const* apAnsi, char* apUTF8, int aSize )
{
	if(apAnsi == NULL || apUTF8 == NULL)
		return apUTF8;

	//计算转换的字符长度
	int iWLen = MultiByteToWideChar(CP_ACP, 0, apAnsi, -1, NULL, 0);
	WCHAR *pWChar = new WCHAR[iWLen + sizeof(WCHAR)];
	MultiByteToWideChar(CP_ACP, 0, apAnsi, -1, pWChar, iWLen);

	int iSLen = WideCharToMultiByte(CP_UTF8, 0, pWChar, -1, NULL, 0, NULL, FALSE);
	if(iSLen < aSize - 1)
	{
		WideCharToMultiByte(CP_UTF8, 0, pWChar, -1, apUTF8, iSLen, NULL, FALSE);
	}
	else
	{
		WideCharToMultiByte(CP_UTF8, 0, pWChar, -1, apUTF8, aSize - 1, NULL, FALSE);
	}

	delete [] pWChar;

	return apUTF8;
}

char* CharSetHelper::UTF8ToANSI( char const* apUFT8, char* apANSI, int aSize )
{
	int length = MultiByteToWideChar(CP_UTF8, 0, apUFT8, -1, NULL, 0);
	wchar_t* wide = new wchar_t[length];
	MultiByteToWideChar(CP_UTF8, 0, apUFT8, -1, wide, length);

	length = WideCharToMultiByte( CP_ACP, 0, wide, -1, NULL, 0, NULL, NULL ); 
	if(length < aSize)
	{
		WideCharToMultiByte(CP_ACP, 0, wide, length, apANSI, length, NULL, FALSE);
	}
	delete[] wide;

	return apANSI;
}

bool CharSetHelper::HaveChinese( char const* apStr)
{
	for (int i = 0; i < strlen(apStr); i ++)
	{
		if (*(apStr + i) & 0x80)
		{
			return true;
		}
	}
	return false;
}

bool CharSetHelper::HaveChinese( char const* apStr, int alen)
{
	for (int i = 0; i < alen; i ++)
	{
		if (*(apStr + i) & 0x80)
		{
			return true;
		}
	}
	return false;
}

static char* g_Chinese_Numbers[] = {
	"一",
	"二",
	"三",
	"四",
	"五",
	"六",
	"七",
	"八",
	"九"
};

char* CharSetHelper::ToChineseNumber( int aID, char* apOut, int aLength )
{
	char lBuffer[512] = {0};

	aID = aID % 100; // 超过百的不转换

	int lTen = aID / 10;
	if(lTen > 1 ){
		_snprintf(lBuffer, sizeof(lBuffer) - 1, "%s十", g_Chinese_Numbers[lTen - 1]);
	}
	else if( lTen == 1) {
		_snprintf(lBuffer, sizeof(lBuffer) - 1, "十");
	}

	int lLingTou = aID % 10;
	if(lLingTou > 0)
		_snprintf(lBuffer, sizeof(lBuffer) - 1, "%s%s号", lBuffer, g_Chinese_Numbers[lLingTou - 1]);
	else
		_snprintf(lBuffer, sizeof(lBuffer) - 1, "%s号", lBuffer);

	ANSIToUTF8(lBuffer, apOut, aLength);

	return apOut;
}

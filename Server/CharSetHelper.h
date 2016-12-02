#pragma once
class CharSetHelper
{
public:
	static bool IsUTF8(char const* apStr, int aLen);

	static char* ANSIToUTF8(char const* apAnsi, char* apUTF8, int aSize);

	static char* UTF8ToANSI(char const* apUFT8, char* apANSI, int aSize);

	static bool HaveChinese(char const* apStr);

	static bool HaveChinese(char const* apStr, int alen);

	static char* ToChineseNumber(int aID, char* apOut, int aLength);
};


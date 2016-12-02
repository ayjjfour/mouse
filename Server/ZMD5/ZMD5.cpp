
/*-======================- Copyright (c) 2006 -======================-

程序作者:
    张鲁夺(zhangluduo) : 为所有爱我的人和我爱的人努力!

联系方式:
    oopfans@msn.com

修改时间:
    2007-02-19

功能描述:
    将原信息加密成MD5摘要这个类的头文件下面附上了ronald l. rivest在
	1992年8月向ieft提交的rfc1321.txt,这是一份最权威的文档.

使用说明:
	这个类是一个虚基类,要使用这个类必须继承自这个类,并实现这个类里的
	virtual void OnProcessing(int PrcessNum)=0;纯虚函数,这个纯虚函数
	用于在对一个较大文件进行MD5计算时,将处理进度返回给用户

版权声明:
    许可任何单位,个人随意使用,拷贝,修改,散布及出售这份代码,但是必须保
    留此版权信息,以慰藉作者辛勤的劳动,及表明此代码的来源,如若此份代码
    有任何BUG,请通知作者,以便弥补作者由于水平所限而导致的一些错误和不
    足,谢谢!

-======================- Copyright (c) 2006 -======================-*/

#include "../stdafx.h"
#include "ZMD5.h"

ZMD5::ZMD5()
{
	m_FileOpen=false;
}

ZMD5::~ZMD5()
{
	if(m_FileOpen)
		fclose(m_pFile);
}

unsigned int ZMD5::ROTATE_LEFT(unsigned int x,unsigned int n)
{
	return (((x) << (n)) | ((x) >> (32-(n))));
}

unsigned int ZMD5::F(unsigned int x,unsigned int y,unsigned int z)
{
	return ((x & y) | ((~x) & z));
}
unsigned int ZMD5::G(unsigned int x,unsigned int y,unsigned int z)
{
	return ((x & z) | (y & (~z)));
}

unsigned int ZMD5::H(unsigned int x,unsigned int y,unsigned int z)
{
	return x ^ y ^ z;
}

unsigned int ZMD5::I(unsigned int x,unsigned int y,unsigned int z)
{
	return (y ^ (x | (~z)));
}	

void ZMD5::FF(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
	(a) += F ((b), (c), (d)) + (x) + (ac);
	(a) = ROTATE_LEFT ((a), (s));
	(a) += (b);
}

void ZMD5::GG(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
	(a) += G ((b), (c), (d)) + (x) + (ac);
	(a) = ROTATE_LEFT ((a), (s));
	(a) += (b);
}

void ZMD5::HH(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
	(a) += H ((b), (c), (d)) + (x) + (ac);
	(a) = ROTATE_LEFT ((a), (s));
	(a) += (b);
}

void ZMD5::II(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
	(a) += I ((b), (c), (d)) + (x) + (ac);
	(a) = ROTATE_LEFT ((a), (s));
	(a) += (b);
}

void ZMD5::Init()
{
	S11 =  7;	S21 =  5;	S31 =  4;	S41 =  6;
	S12 = 12;	S22 =  9;	S32 = 11;	S42 = 10;
	S13 = 17;	S23 = 14;	S33 = 16;	S43 = 15;
	S14 = 22;	S24 = 20;	S34 = 23;	S44 = 21;

	A = 0x67452301;  // in memory, this is 0x01234567
	B = 0xEFCDAB89;  // in memory, this is 0x89ABCDEF
	C = 0x98BADCFE;  // in memory, this is 0xFEDCBA98
	D = 0x10325476;  // in memory, this is 0x76543210
}

void ZMD5::Append(unsigned int MsgLen)
{
	//计算要补位的字节数
	int m = MsgLen % 64;
	if(m==0)
		m_AppendByte=56;
	else if(m<56)
		m_AppendByte=56-m;
	else
		m_AppendByte=64-m+56;

	/*
	记录原始信息长度(单位:bit),以下过程将一个32位的字节数转
	换为64位的bit数,这个计算过程和rfc1321.txt中的略有不同.
	*/
	
	//截取传入长度的高十六位和低十六位
	int hWord=(MsgLen & 0xFFFF0000) >> 16;
	int lWord=MsgLen & 0x0000FFFF;

	//将低十六位和高十六位分别乘以八(1byte=8bit)
	int hDiv=hWord*8;
	int lDiv=lWord*8;

	//
	m_MsgLen[0] = lDiv & 0xFF ;
	m_MsgLen[1] = (lDiv >> 8) & 0xFF ;
	m_MsgLen[2] = ((lDiv >> 16) & 0xFF) | (hDiv & 0xFF);
	m_MsgLen[3] = (hDiv >> 8) & 0xFF ;
	m_MsgLen[4] = (hDiv >> 16) & 0xFF ;
	m_MsgLen[5] = (hDiv >> 24) & 0xFF ;
	m_MsgLen[6] = 0;
	m_MsgLen[7] = 0;
}

void ZMD5::Transform(unsigned char Block[64])
{
	//将64字节位转换为16个字节
	unsigned long x[16];
	for (int i=0,j=0;j<64;i++,j+=4)
		x[i]=Block[j] | Block[j+1] <<8 | Block[j+2] <<16 | Block[j+3] <<24 ;

	//初始化临时寄存器变量
	unsigned int a,b,c,d;
	a=A; b=B; c=C; d=D;

	//第一轮计算
	FF (a, b, c, d, x[ 0], S11, 0xD76AA478); //  1
	FF (d, a, b, c, x[ 1], S12, 0xE8C7B756); //  2
	FF (c, d, a, b, x[ 2], S13, 0x242070DB); //  3
	FF (b, c, d, a, x[ 3], S14, 0xC1BDCEEE); //  4
	FF (a, b, c, d, x[ 4], S11, 0xF57C0FAF); //  5
	FF (d, a, b, c, x[ 5], S12, 0x4787C62A); //  6
	FF (c, d, a, b, x[ 6], S13, 0xA8304613); //  7
	FF (b, c, d, a, x[ 7], S14, 0xFD469501); //  8
	FF (a, b, c, d, x[ 8], S11, 0x698098D8); //  9
	FF (d, a, b, c, x[ 9], S12, 0x8B44F7AF); // 10 
	FF (c, d, a, b, x[10], S13, 0xFFFF5BB1); // 11 
	FF (b, c, d, a, x[11], S14, 0x895CD7BE); // 12 
	FF (a, b, c, d, x[12], S11, 0x6B901122); // 13 
	FF (d, a, b, c, x[13], S12, 0xFD987193); // 14 
	FF (c, d, a, b, x[14], S13, 0xA679438E); // 15 
	FF (b, c, d, a, x[15], S14, 0x49B40821); // 16 

	//第二轮计算
	GG (a, b, c, d, x[ 1], S21, 0xF61E2562); // 17 
	GG (d, a, b, c, x[ 6], S22, 0xC040B340); // 18 
	GG (c, d, a, b, x[11], S23, 0x265E5A51); // 19 
	GG (b, c, d, a, x[ 0], S24, 0xE9B6C7AA); // 20 
	GG (a, b, c, d, x[ 5], S21, 0xD62F105D); // 21 
	GG (d, a, b, c, x[10], S22,  0x2441453); // 22 
	GG (c, d, a, b, x[15], S23, 0xD8A1E681); // 23 
	GG (b, c, d, a, x[ 4], S24, 0xE7D3FBC8); // 24 
	GG (a, b, c, d, x[ 9], S21, 0x21E1CDE6); // 25 
	GG (d, a, b, c, x[14], S22, 0xC33707D6); // 26 
	GG (c, d, a, b, x[ 3], S23, 0xF4D50D87); // 27 
	GG (b, c, d, a, x[ 8], S24, 0x455A14ED); // 28 
	GG (a, b, c, d, x[13], S21, 0xA9E3E905); // 29 
	GG (d, a, b, c, x[ 2], S22, 0xFCEFA3F8); // 30 
	GG (c, d, a, b, x[ 7], S23, 0x676F02D9); // 31 
	GG (b, c, d, a, x[12], S24, 0x8D2A4C8A); // 32 

	//第三轮计算
	HH (a, b, c, d, x[ 5], S31, 0xFFFA3942); // 33 
	HH (d, a, b, c, x[ 8], S32, 0x8771F681); // 34 
	HH (c, d, a, b, x[11], S33, 0x6D9D6122); // 35 
	HH (b, c, d, a, x[14], S34, 0xFDE5380C); // 36 
	HH (a, b, c, d, x[ 1], S31, 0xA4BEEA44); // 37 
	HH (d, a, b, c, x[ 4], S32, 0x4BDECFA9); // 38 
	HH (c, d, a, b, x[ 7], S33, 0xF6BB4B60); // 39 
	HH (b, c, d, a, x[10], S34, 0xBEBFBC70); // 40 
	HH (a, b, c, d, x[13], S31, 0x289B7EC6); // 41 
	HH (d, a, b, c, x[ 0], S32, 0xEAA127FA); // 42 
	HH (c, d, a, b, x[ 3], S33, 0xD4EF3085); // 43 
	HH (b, c, d, a, x[ 6], S34,  0x4881D05); // 44 
	HH (a, b, c, d, x[ 9], S31, 0xD9D4D039); // 45 
	HH (d, a, b, c, x[12], S32, 0xE6DB99E5); // 46 
	HH (c, d, a, b, x[15], S33, 0x1FA27CF8); // 47 
	HH (b, c, d, a, x[ 2], S34, 0xC4AC5665); // 48 

	//第四轮计算
	II (a, b, c, d, x[ 0], S41, 0xF4292244); // 49 
	II (d, a, b, c, x[ 7], S42, 0x432AFF97); // 50 
	II (c, d, a, b, x[14], S43, 0xAB9423A7); // 51 
	II (b, c, d, a, x[ 5], S44, 0xFC93A039); // 52 
	II (a, b, c, d, x[12], S41, 0x655B59C3); // 53 
	II (d, a, b, c, x[ 3], S42, 0x8F0CCC92); // 54 
	II (c, d, a, b, x[10], S43, 0xFFEFF47D); // 55 
	II (b, c, d, a, x[ 1], S44, 0x85845DD1); // 56 
	II (a, b, c, d, x[ 8], S41, 0x6FA87E4F); // 57 
	II (d, a, b, c, x[15], S42, 0xFE2CE6E0); // 58 
	II (c, d, a, b, x[ 6], S43, 0xA3014314); // 59 
	II (b, c, d, a, x[13], S44, 0x4E0811A1); // 60 
	II (a, b, c, d, x[ 4], S41, 0xF7537E82); // 61 
	II (d, a, b, c, x[11], S42, 0xBD3AF235); // 62 
	II (c, d, a, b, x[ 2], S43, 0x2AD7D2BB); // 63 
	II (b, c, d, a, x[ 9], S44, 0xEB86D391); // 64 

	//保存当前寄存器结果
	A+=a; B+=b; C+=c; D+=d;
}

/*********************************************
将寄存器ABCD的最终值转换为十六进制返回给用户
注意:低位在前高位在后
*********************************************/
string ZMD5::ToHex(bool UpperCase)
{
	string strResult;
	int ResultArray[4]={A,B,C,D};
	char Buf[33]={0};
	for(int i=0;i<4;i++)
	{
		memset(Buf,0,3);
		sprintf_s(Buf,"%02x",ResultArray[i] & 0x00FF);
		strResult+=Buf;

		memset(Buf,0,3);
		sprintf_s(Buf,"%02x",(ResultArray[i] >>  8) & 0x00FF);
		strResult+=Buf;

		memset(Buf,0,3);
		sprintf_s(Buf,"%02x",(ResultArray[i] >> 16) & 0x00FF);
		strResult+=Buf;

		memset(Buf,0,3);
		sprintf_s(Buf,"%02x",(ResultArray[i] >> 24) & 0x00FF);
		strResult+=Buf;
	}
	if(UpperCase) CharUpperA((char *)strResult.c_str());
	return strResult;
}

/**************************************
MD5的主要计算过程可以参照这个函数
**************************************/
string ZMD5::GetMD5OfString(string InputMessage,bool UpperCase)
{
	//初始化MD5所需常量
	Init();

	//计算追加长度
	Append(InputMessage.length());

	//对原始信息进行补位
	for(int i=0;i<m_AppendByte;i++)
	{
		if(i==0) InputMessage+=(char)0x80;
		else InputMessage+=(char)0x0;
	}

	//将原始信息长度附加在补位后的数据后面
	for(int i=0;i<8;i++) InputMessage+=m_MsgLen[i];

	//位块数组
	unsigned char x[64]={0};

	//循环,将原始信息以64字节为一组拆分进行处理
	for(int i=0,Index=-1;i<(int)InputMessage.length();i++)
	{
		x[++Index]=InputMessage[i];
		if(Index==63)
		{
			Index=-1;
			Transform(x);
		}
	}
	return ToHex(UpperCase);
}

/************************************************************************
对文件做MD5运算的道理和对字符串的道理一样,只不过数据来源于磁盘文件而已
记事:以为自己对文件操作很熟悉,不小心把"rb"写成"wb",唉,后果可想而知!
************************************************************************/
string ZMD5::GetMD5OfFile(const string FileName,bool UpperCase)
{
	//定义读取文件的缓冲区
	char* ReadBuf =new char[FILE_BUFFER_READ+1];
	memset(ReadBuf,0,FILE_BUFFER_READ);

	try
	{
		//检查文件是否存在
		if((_access(FileName.c_str(),0 )) == -1) return "";

		//二进制方式读取文件
        fopen_s(&m_pFile, FileName.c_str(),"rb");
		if(m_pFile==NULL) return "";
		m_FileOpen=true;

		//获取文件大小
		unsigned long FileSize=0xFFFF;
		WIN32_FIND_DATAA  win32_find_data;
		HANDLE hFile;
		if((hFile=FindFirstFileA(FileName.c_str(),&win32_find_data))!=INVALID_HANDLE_VALUE)
		if(hFile==NULL) return "";
		if(FileSize=win32_find_data.nFileSizeLow,FileSize==0xFFFF || FileSize==0) return "";
		FindClose(hFile);

		//初始化MD5所需常量
		Init();

		//通过文件长度计算追加长度
		Append(FileSize);

		//位块数组
		unsigned char x[64]={0};

		//本次读取字节数
		int ReadSize=fread(ReadBuf,1,FILE_BUFFER_READ,m_pFile);

		//读取次数
		int ReadCount=0;

		while(ReadSize==FILE_BUFFER_READ)
		{
			/*
			如果用户开启了另一个线程调用此函数,则允许用户从外部结
			束此函数,为安全起见,没有在这个类的内部开启线程,可以最
			大限度的保正了文件安全关闭
			*/
			if(!m_FileOpen) 
			{
				fclose(m_pFile);
				return "";
			}

			//将处理进度返回给用户
			ReadCount++;
		//	OnProcessing((int)(FILE_BUFFER_READ * ReadCount / (FileSize / 100)));

			//将原始信息以64字节为一组拆分进行处理
			for(int i=0,Index=-1;i<FILE_BUFFER_READ;i++)
			{
				x[++Index]=ReadBuf[i];
				if(Index==63)
				{
					Index=-1;
					Transform(x);
				}
			}
			memset(ReadBuf,0,FILE_BUFFER_READ); // 重置缓冲区
			ReadSize=fread(ReadBuf,1,FILE_BUFFER_READ,m_pFile);
		} // end while

		/*
		处理不能被整除的剩余部分数据,此时要对剩
		余部分数据进行补位及长原始信息长度追加
		*/

		/*
		如果最后一次读取数据的长度为零,说明文件已被读完,
		则直接将补位数据及原信息长度送入Transform处理
		*/
		if(ReadSize==0)
		{
			string strData;
			for(int i=0;i<m_AppendByte;i++)
			{
				if(i==0) strData+=(char)0x80;
				else strData+=(char)0x0;
			}

			for(int i=0;i<8;i++) strData+=m_MsgLen[i];

			for(int i=0,Index=-1;i<(int)strData.length();i++)
			{
				x[++Index]=strData[i];
				if(Index==63)
				{
					Index=-1; 
					Transform(x);
				}
			}
		}
		else // 将剩余数据处理完再补位
		{
			for(int i=0,Index=-1;i<ReadSize+m_AppendByte+8;i++)
			{
				//将原始信息以64字节为一组,进行拆分处理
				if(i<ReadSize)
					x[++Index]=ReadBuf[i];
				else if(i==ReadSize)
					x[++Index]=(unsigned char)0x80; 
				else if(i<ReadSize+m_AppendByte)
					x[++Index]=(unsigned char)0x0;
				else if(i==ReadSize+m_AppendByte)
					x[++Index]=m_MsgLen[0];
				else if(i==ReadSize+m_AppendByte+1)
					x[++Index]=m_MsgLen[1];
				else if(i==ReadSize+m_AppendByte+2)
					x[++Index]=m_MsgLen[2];
				else if(i==ReadSize+m_AppendByte+3)
					x[++Index]=m_MsgLen[3];
				else if(i==ReadSize+m_AppendByte+4)
					x[++Index]=m_MsgLen[4];
				else if(i==ReadSize+m_AppendByte+5)
					x[++Index]=m_MsgLen[5];
				else if(i==ReadSize+m_AppendByte+6)
					x[++Index]=m_MsgLen[6];
				else if(i==ReadSize+m_AppendByte+7)
					x[++Index]=m_MsgLen[7];

				if(Index==63)
				{
					Index=-1; 
					Transform(x);
				}
			}
		}
	//	OnProcessing(100);	//处理进度百分之百
		fclose(m_pFile);	//关闭文件
		m_FileOpen=false;	//文件打开状态为false
		delete[] ReadBuf;	//释放动态申请的内存
	}
	catch(...)
	{
		if(m_FileOpen) 
		fclose(m_pFile);	//关闭文件
		m_FileOpen=false;	//文件打开状态为false
		delete[] ReadBuf;	//释放动态申请的内存
		return "";
	}
	return ToHex(UpperCase);
}

void ZMD5::GetMD5OfFile_Terminate()
{
	if(m_FileOpen) m_FileOpen=false;
}

//	VS60.iso
//	715,898,880 = 2AAB,C000
//	715,898,880 x 8 = 5727191040 = 1555E0000
//	740ec84eab66bb81b4c8175f34991df5
//
//	00000000,00000000,00000000,00000001_,_01010101,01011110,00000000,00000000 = 1555E 0000
//	00000000,00000000,00000000,00000001_,_01010101,01011000,00000000,00000000
//	00000000,00000000,00000000,00000000_,_00000000,00000110,0000000,000000000
//	-------------------------------------------------------------------------
//												  |0000005E              
//
//	2AAB = 0010101010101011
//	C000 = 1100000000000000
//	2AAB x 8 = 00000000,00000001,01010101,01011000 = 00,00,00,00,00,01,55,58 (hex)
//	C000 x 8 = 00000000,00000110,00000000,00000000 = 00,00,00,00,00,06,00,00 (hex)
//
//	01010101=55
//	01011110=5E
//	0x58 | 0x06 = 5E
//
//	00
//	00
//	5e
//	55
//
//	01
//	00
//	00
//	00
//
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//	MSDN_OCT_2001.iso
//	1,561,942,016 == 5D19,5800
//	1,561,942,016 x 8 = 2,E8CA,C000
//	578af06e1670e020a9e4694f36136f85
//
//	00000000,00000000,00000000,00000010_,_11101000,11001010,11000000,00000000 = 2,E8CA,C000
//									 10_,_11101000,11001000
//														 10,11000000,00000000
//
//	5D19 = 101110100011001
//	5800 = 101100000000000
//	5D19 x 8 = 101110100011001000 = 0002E8C8 (hex)
//	5800 x 8 = 101100000000000000 = 0002C000 (hex)
//
//	11001010=CA
//	11000000=C0
//	11101000=E8
//
//	0xC8 | 0x2 = 0xCA
//
//	00
//	c0
//	ca
//	e8
//
//	02
//	00
//	00
//	00
//
//	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
//	// 512 M 的文件
//	m_MsgLen[0]= 0x0; 
//	m_MsgLen[1]= 0x0; 
//	m_MsgLen[2]= 0x0; 
//	m_MsgLen[3]= 0x0; 
//	m_MsgLen[4]= 0x01;
//	m_MsgLen[5]= 0x0; 
//	m_MsgLen[6]= 0x0; 
//	m_MsgLen[7]= 0x0; 
//
//	// VS60.iso
//	m_MsgLen[0]= 0x00;
//	m_MsgLen[1]= 0x00;
//	m_MsgLen[2]= 0x5e;
//	m_MsgLen[3]= 0x55;
//	m_MsgLen[4]= 0x01;
//	m_MsgLen[5]= 0x00;
//	m_MsgLen[6]= 0x00;
//	m_MsgLen[7]= 0x00;
//
//	// MSDN_OCT_2001.iso
//	m_MsgLen[0]= 0x00;
//	m_MsgLen[1]= 0xc0;
//	m_MsgLen[2]= 0xca;
//	m_MsgLen[3]= 0xe8;
//	m_MsgLen[4]= 0x02;
//	m_MsgLen[5]= 0x00;
//	m_MsgLen[6]= 0x00;
//	m_MsgLen[7]= 0x00;











#ifndef _CONFIG_GAME_SITE_H
#define _CONFIG_GAME_SITE_H

/*
 * ˵����ѡ������Ԥ�������Ըı�վ��֧��
 *       ������վ�㻹Ӧ�޸�Basegame\tab.rc��Դ�ļ��������IDR_MAINFRAME����
 *
 */

//enum ONLINE_GAME_SITE
//{ // ������Ϸվ���б�
//	siteNone,siteChinagames = 1,siteShOnline = 2,siteWHOL = 3,siteShVIP = 4, siteEmin =5, siteZBcsc =6, siteNJ =7, siteHarbin =8, 
//	siteQPGame=9, siteZJGame=10
//};
//
//#if defined(SITE_CHINAGAMES_NET)
//	#define GAME_SITE_INDEX	siteChinagames					// վ����Ż��ʶ
//	#define REG_MAIN_KEY	"szDomain"						// ��ע�����
//#elif defined(SITE_SHONLINE)
//	//#define ONLINE_GAME		"�Ϻ�������ϷƵ��"				// ��װ����
//	//#define EMAIL_SUPPORT	"mailto:shdomain@21cn.com"		// ����֧������
//	#define GAME_SITE_INDEX	siteShOnline					// վ����Ż��ʶ
//	#define REG_MAIN_KEY	"shDomain"						// ��ע�����
//#elif defined(SITE_SHVIP)
//	#define GAME_SITE_INDEX	siteShVIP						// վ����Ż��ʶ,�Ϻ������������
//	#define REG_MAIN_KEY	"ShVIP"							// ��ע�����
//#elif defined(SITE_SHVIP2)
//	#define GAME_SITE_INDEX	siteShVIP						// վ����Ż��ʶ,�Ϻ������������
//	#define REG_MAIN_KEY	"ShVIP.NG"							// ��ע�����
//#elif defined(SITE_EMIN)
//	#define GAME_SITE_INDEX	siteEmin						// վ����Ż��ʶ,�����������
//	#define REG_MAIN_KEY	"Emin"							// ��ע�����
//#elif defined(SITE_ZBCSC)
//	#define GAME_SITE_INDEX	siteZBcsc						// վ����Ż��ʶ,�Ͳ���ͨ�������
//	#define REG_MAIN_KEY	"ZBCSC"							// ��ע�����
//#elif defined(SITE_NJONLINE)
//	#define GAME_SITE_INDEX	siteNJ						// վ����Ż��ʶ,���������������
//	#define REG_MAIN_KEY	"njVIP.NG"							// ��ע�����
//#elif defined(SITE_HARBIN)
//	#define GAME_SITE_INDEX	siteHarbin						// վ����Ż��ʶ,�������������
//	#define REG_MAIN_KEY	"HARBIN"							// ��ע�����
//#elif defined(SITE_QPGAME)
//	#define GAME_SITE_INDEX siteQPGame					
//	#define REG_MAIN_KEY	"QPGame"
//#elif defined(SITE_ZJGAME)
//	#define GAME_SITE_INDEX siteZJGame					
//	#define REG_MAIN_KEY	"ZJGame"
////{{
//	//... other site defines
////}}
//
//#else
//	#error "No site options defined, define SITE_CHINAGAMES_NET or SITE_SHONLINE or other site in C/C++ and Resource Preprocessor definition of Project Settings"
//	// ˵��: ��CChatClient����Ķ�վ��֧������,�����ô˱���ѡ��
//#endif
//
//#endif // _CONFIG_GAME_SITE_H
//

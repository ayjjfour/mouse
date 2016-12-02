#ifndef _CONFIG_GAME_SITE_H
#define _CONFIG_GAME_SITE_H

/*
 * 说明：选择下列预定义宏可以改变站点支持
 *       增加新站点还应修改Basegame\tab.rc资源文件，比如对IDR_MAINFRAME定义
 *
 */

//enum ONLINE_GAME_SITE
//{ // 在线游戏站点列表
//	siteNone,siteChinagames = 1,siteShOnline = 2,siteWHOL = 3,siteShVIP = 4, siteEmin =5, siteZBcsc =6, siteNJ =7, siteHarbin =8, 
//	siteQPGame=9, siteZJGame=10
//};
//
//#if defined(SITE_CHINAGAMES_NET)
//	#define GAME_SITE_INDEX	siteChinagames					// 站点序号或标识
//	#define REG_MAIN_KEY	"szDomain"						// 主注册表项
//#elif defined(SITE_SHONLINE)
//	//#define ONLINE_GAME		"上海热线游戏频道"				// 安装标题
//	//#define EMAIL_SUPPORT	"mailto:shdomain@21cn.com"		// 技术支持热线
//	#define GAME_SITE_INDEX	siteShOnline					// 站点序号或标识
//	#define REG_MAIN_KEY	"shDomain"						// 主注册表项
//#elif defined(SITE_SHVIP)
//	#define GAME_SITE_INDEX	siteShVIP						// 站点序号或标识,上海热线棋牌天地
//	#define REG_MAIN_KEY	"ShVIP"							// 主注册表项
//#elif defined(SITE_SHVIP2)
//	#define GAME_SITE_INDEX	siteShVIP						// 站点序号或标识,上海热线棋牌天地
//	#define REG_MAIN_KEY	"ShVIP.NG"							// 主注册表项
//#elif defined(SITE_EMIN)
//	#define GAME_SITE_INDEX	siteEmin						// 站点序号或标识,亿民棋牌天地
//	#define REG_MAIN_KEY	"Emin"							// 主注册表项
//#elif defined(SITE_ZBCSC)
//	#define GAME_SITE_INDEX	siteZBcsc						// 站点序号或标识,淄博网通棋牌天地
//	#define REG_MAIN_KEY	"ZBCSC"							// 主注册表项
//#elif defined(SITE_NJONLINE)
//	#define GAME_SITE_INDEX	siteNJ						// 站点序号或标识,金陵热线棋牌天地
//	#define REG_MAIN_KEY	"njVIP.NG"							// 主注册表项
//#elif defined(SITE_HARBIN)
//	#define GAME_SITE_INDEX	siteHarbin						// 站点序号或标识,哈尔滨棋牌天地
//	#define REG_MAIN_KEY	"HARBIN"							// 主注册表项
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
//	// 说明: 因CChatClient等类的多站点支持特性,需设置此编译选项
//#endif
//
//#endif // _CONFIG_GAME_SITE_H
//

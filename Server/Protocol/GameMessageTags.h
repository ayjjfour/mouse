#pragma once

namespace GameMessages
{
    enum MessageTags
    {
        // * /game framework analyzing command
        GameDataReq = 1,
        GameDataRsp,
        ClientConfirmStartReq,      //CLIENTSITE_CONFIRM_START                  ClientConfirmStartReq
        ClientConfirmStartRsp,
        TimerProbeTimerErrReq,      //TIMERPROBE_TIMERERR
        ServerPersonalSceneRsp,     //SERVERSITE_PERSONAL_SCENE                 ServerPersonalSceneRsp
        ServerMatchProgressRsp,     //SERVERSITE_MATCH_PROGRESSING, no data
        HelperReadyConfirmRsp,      //LOGICALHELPER_READY_CONFIRMED
        ServerFirstSceneRsp,        // * /add SERVERSITE_FIRST_SCENE
        //SERVERSITE_SAVE_STORAGE\SERVERSITE_SAVE_STORAGE_CONTINUE\SERVERSITE_SAVE_STORAGE_END do NOT use

		//-------------------客户端消息------------------------------------
		BuySkillReq 			=		20,				//提前激活技能      BuySkillReq
		ChangeSkillStatusReq    =		21,				//修改技能状态（启用、禁用）     ChangeSkillStatus
		ChangeWeaponReq         =		22,				//切换武器         ChangeWeaponReq
        PlayerHit               =		23,				//玩家打击地鼠      PlayerHit
        UpgradeSkillReq         =       24,				//升级技能等级      BuySkillReq
		BuyAutoAttackReq 		=		25,				//激活自动挂机等级      BuyAutoAttackReq

		//---------服务端消息命令------------------------------------------
		ArenaInfo				=		63,				//房间信息
		GameStationBase     	=		64,				//游戏基础数据包，其他共有的数据继承该结构体数据包
        UserMoneySync           =		65,				//发送游戏币和礼票到客户端     UserMoneySync
        PlayerHitSync           =		66,				//打击反馈                   PlayerHitSync
		BuySkillRsp				=		67,				//客户端收到购买技能           BuySkillRsp
		ChangeWeaponRsp			=		68,				//客户端收到切换武器（如果失败才需要返回）   ChangeWeaponRsp
		ChangeSkillStatus		=		69,				//切换技能状态（启用，禁用）    ChangeSkillStatus

        CreateMonsterTrace		=		70,				//刷新一只怪物消息             MonsterTrace
		CreateMonsterTraces		=		71,				//刷新一队怪物消息             MonsterTraces
		ChangeScene				=		72,				//改变场景                    ChangeScene
		ScreenResolution		=		73,				//客户端屏幕分辨率	         ScreenResolution
        UpgradeSkillRsp			=		74,				//客户端收到升级技能（如果失败才需要返回）   BuySkillRsp
        BuyAutoAttackRsp        =       75,				//客户端收到购买挂机等级结果           BuyAutoAttackRsp

        PlayerHitIncomingSync   =       78,             //击中鱼后的收益消息           PlayerHitIncoming
		MonsterDieSync    		=   	79,             //强制鱼死亡消息               MonsterDieSync
		BroadcastS2C            =		83,             //高分播报                    BroadcastS2C
        ReturnUserMoney         =       84,             //返还玩家金币提示             ReturnUserMoney
		
		HallGsAdditionalGameData1Rsp = 	90,
		AdditionalGameDataRsp	=		91,
		RefreshUserFishExp		=		92,				//玩家的Exp更新
		BuyHammerLevelReq       =       93,             //玩家购买自动锤等级扣费 2016.10.17 hl
		BuyHammerLevelRsp       =       94,             //玩家购买自动锤等级返回  2016.10.17 hl
		KingCriticalHit			=		95,				//鼠王击毙，通知客户端是否触发召唤"雷神之锤"
		WildSkillStatus           =       96,             //狂暴状态通知（包含间隔阶段，准备阶段，持续阶段，结束阶段）
		WildSkillWeaponCostModify  =     97,             //狂暴状态武器消费服务端验证不足，导致回退金币消息
        GameCommandEnd			=		99,				//game command end tag
		
    };
}
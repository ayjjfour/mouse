/********************************************************************
	created:	2002/08/30
	created:	30:8:2002   13:54
	filename: 	D:\iGame.VSS\include\pui.h
	file path:	D:\iGame.VSS\include
	file base:	pui
	file ext:	h
	author:		hhh
	
	purpose:	
*********************************************************************/

#ifndef __PUI_H__
#define __PUI_H__

#include "chatClient.h"

// ���ܣ����������������״̬���Լ����˶���(���������£��Թ۵�)
//		 ��IRoomView����
interface ITableList
{
	//##ModelId=3D47547A0018
	virtual BOOL GetFlashInfo(int& nTable,int& nChair,int& nFaceIndex) = NULL;
	//##ModelId=3D47547A0022
	virtual BOOL IsTablePlaying(int nTable) = NULL;
	//##ModelId=3D47547A0025
	virtual BOOL IsTableLocked(int nTable) = NULL;
	//##ModelId=3D47547A002E
	virtual BOOL getChairInfo(int nTable,int nChair,CHAIR_INFO& info) = NULL;
	//##ModelId=3D47547A0038
	virtual int	 GetMaxTablePlayer() = NULL;

	//	added by hhh
	virtual BOOL IsTableNobody(int nTable) PURE;
};

// ���� : ��ʾ��ͼ
//		  ��ITableList����
interface IRoomView2
{
	//##ModelId=3D47547A0063
	virtual BOOL RepaintTable(int nTable) PURE;

	//##ModelId=3D47547A006B
	virtual BOOL BeginFlashUser() PURE;
	//##ModelId=3D47547A0073
	virtual BOOL EndFlashUser() PURE;

	//##ModelId=3D47547A0075
	virtual void OnUserListEnd() PURE;

	// ʹĳ���ɼ�
	//##ModelId=3D47547A007E
	virtual void EnsureTableVisable(int nTable,int nChair) PURE;
	//##ModelId=3D47547A0087
	virtual void Close() PURE;
	//##ModelId=3D47547A0089
	virtual void Release() PURE;
};

typedef BOOL (* protoCreateRoomView)(HWND hwndParent, int nGameID, int nTotalTable, ITableList * pTableList, CChatClient * pChat);

#endif	//__PUI_H__

// PlayerList.h: interface for the PlayerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERLIST_H__C6FD9488_39D3_41E1_88BF_66B0483549CB__INCLUDED_)
#define AFX_PLAYERLIST_H__C6FD9488_39D3_41E1_88BF_66B0483549CB__INCLUDED_

#include "GamePlayer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserList.h"

class PlayerList  : public UserList
{
/*protected:
#pragma warning (disable: 4200)
	typedef struct GameSide{
		GamePlayer player;					// the player
		int nMaxLookOns;					// define the LookOnPeople Array's upbound, this value can be adjust when nLookOns grows up
		int nLookOns;						// defined how much the look on people
		GamePlayer * *LookOnPeopleArray;			// it will be treated as an array
	};
#pragma warning (default: 4200)

*/

public:
	HRESULT Init(int nMaxPlayers);
	HRESULT AddPeople(TABLE_USER_INFO & PlayerInfo, IGamePlayer ** ppGamePlayer=NULL);
	HRESULT RemovePeople(UINT dwUserID);
	HRESULT FindPeople(UINT dwUserID, IGamePlayer ** ppPlayer=NULL);
	HRESULT FindPlayer(UINT dwUserID, IGamePlayer ** ppPlayer=NULL);
	void Clear();

	PlayerList();
	virtual ~PlayerList();

public:				// helper function
	HRESULT ChangePlayerState(int nState);
	HRESULT PrepareRemovePeople(DWORD dwUserID);

	BOOL IsValidChair(int nChair)
	{
		return (nChair >= 0 && nChair < m_nSides);
	};

/*	BOOL IsValidUserID(DWORD dwUserID)
	{
		return (WORD)dwUserID != INVALID_USER_ID && (WORD)dwUserID < MAX_TABLE_USER_ID;
	};
*/
	BOOL IsValidUser(TABLE_USER_INFO & ui)
	{
		return IsValidUserID(ui.dwUserID) && IsValidChair(ui.nChair);
	};

protected:
	int m_nSides;

private:
	BOOL	m_bInited;

	void	ensureClearSamePlayer(TABLE_USER_INFO & PlayerInfo){
		DWORD dwUserID =PlayerInfo.dwUserID;
		long lDBID =PlayerInfo.lDBID;
		ensureClearPlayer(dwUserID, lDBID);

		IUserList * ul;
		for (int i=0; i<GetUserNumber(); i++) {
			m_pArray[i]->GetLookOnPeopleList(&ul);
			ul->ensureClearPlayer(dwUserID, lDBID);
			ul->Release();
		}
	}

friend class CServerSite;
};

#endif // !defined(AFX_PLAYERLIST_H__C6FD9488_39D3_41E1_88BF_66B0483549CB__INCLUDED_)

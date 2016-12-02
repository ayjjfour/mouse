// LogicalHelper.h: interface for the LogicalHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICALHELPER_H__2CFF10C8_FB3D_4D9F_8A8B_12CDC1064898__INCLUDED_)
#define AFX_LOGICALHELPER_H__2CFF10C8_FB3D_4D9F_8A8B_12CDC1064898__INCLUDED_

#include "site_i.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogicalHelper  
{
public:
	int m_nRequiredPlayer;
	HRESULT Reset();
	HRESULT OnGameOperation(int nIndex, DWORD dwUserID, int opID, void * oData);
	HRESULT OnUserExit(int nIdx, IGamePlayer * pGamePlayer);
	HRESULT SetServerSite(IServerSite *pSite);

	LogicalHelper();
	virtual ~LogicalHelper();

	BOOL	isReady(int idx) {
		ASSERT(m_ready);
		ASSERT(idx>=0 && idx<m_nPlayers);

		if (!m_ready) return FALSE;
		if (idx>=0 && idx<m_nPlayers) return m_ready[idx];
		else return FALSE;
	}

private:
	HRESULT tryStartGame();
	IServerSite * m_pSite;
	int m_nPlayers;
	BOOL * m_ready;
};

#endif // !defined(AFX_LOGICALHELPER_H__2CFF10C8_FB3D_4D9F_8A8B_12CDC1064898__INCLUDED_)

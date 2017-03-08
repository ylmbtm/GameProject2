#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "SceneManager.h"

class CGameService
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnection(NetPacket *pPacket);

	BOOL		OnCloseConnection(NetPacket *pPacket);

	BOOL		SetWorldServerID(UINT32 dwSvrID);

public:
	UINT32				m_dwWorldServerID;
protected:
	CSceneManager		m_SceneManager;
};

#endif

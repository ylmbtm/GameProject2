#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "DBCmdHandler.h"
class CConnection;

#define DB_THREAD_NUM 10

class CGameService :
	public ServiceBase
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

public:
	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnection(NetPacket *pPacket);

	BOOL		OnCloseConnection(NetPacket *pPacket);

public:
	CDBCmdHandler		m_DBCmdHandler;
};

#endif

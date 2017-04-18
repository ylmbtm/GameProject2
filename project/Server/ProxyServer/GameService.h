#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "CharWillEnterList.h"
#include "ProxyCmdHandler.h"
class  CConnection;

class CGameService  : public IPacketDispatcher
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnect(CConnection *pConn);

	BOOL		OnCloseConnect(CConnection *pConn);

	BOOL		DispatchPacket( NetPacket *pNetPacket);

public:
	BOOL		OnCmdGMCommand(UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper);

	BOOL		OnCmdHeartBeatReq(UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper);


public:
	//处理普通的网络连接

	CProxyCmdHandler	m_ProxyCmdHandler;

	CWillEnterNodeMgr   m_WillEnterNodeMgr;

	//本服务器的信息
	UINT32              m_dwServerID;
	UINT32				m_dwServerType;
	std::string         m_strIpAddr;
	UINT16              m_sPort;

public:
	void			SetStatConnID(UINT64 ConnID){m_u64StatConnID = ConnID;}
	BOOL			SendCmdToStatConnection(IDataBuffer *pDataBuf);

	void			SetDBConnID(UINT64 ConnID){m_u64DBConnID = ConnID;}
	BOOL			SendCmdToDBConnection(IDataBuffer *pDataBuf);

	void			SetWorldConnID( UINT32 ConnID ){m_dwWorldConnID = ConnID;}
	UINT32			GetWorldConnID() {return m_dwWorldConnID;}

	UINT32			GetServerID(){return m_dwServerID;}
	UINT32			GetServerType(){	return m_dwServerType;}
protected:
	UINT64			m_u64DBConnID;
	UINT64			m_u64StatConnID;
	UINT32			m_dwWorldConnID;
	
};

#endif

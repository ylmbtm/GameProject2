#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "IBufferHandler.h"
#include "Utility/CritSec.h"

#define  NET_CMD_RECV				1
#define  NET_CMD_SEND				2
#define  NET_CMD_CONNECT			3

#define Hash_Map                    std::map

struct NetIoOperatorData
{
#ifdef WIN32
	OVERLAPPED		Overlap;
#endif

	UINT32			dwCmdType;

	IDataBuffer		*pDataBuffer;

	void			Clear();
};

class CConnection
{
public:
	CConnection(void);
	virtual ~CConnection(void);

public:
	BOOL	HandleRecvEvent(UINT32 dwBytes); 

    UINT32  GetConnectionID();

	UINT32  GetConnectionType();

    VOID    SetConnectionID(UINT32 dwConnID);

	VOID    SetConnectionType(UINT32 dwType);

	BOOL	Close();

	BOOL	SetSocket(SOCKET hSocket);

	SOCKET  GetSocket();

	BOOL	SetDataHandler(IDataHandler *pHandler);

	BOOL	ExtractBuffer();
	
	BOOL	DoReceive();

	BOOL	IsConnectionOK();

	BOOL	SetConnectionOK(BOOL bOk);

    BOOL    ReInit();

	BOOL    CheckPacketHeader();

	BOOL    SendBuffer(IDataBuffer	*pBuff);

	BOOL    DoSend();

public:
	SOCKET						m_hSocket;

	BOOL						m_bConnected;

	NetIoOperatorData			m_IoOverlapRecv;

    UINT32                      m_dwConnID;
	UINT32                      m_dwConnType;

	IDataHandler				*m_pDataHandler;

	UINT32						m_dwIpAddr;
	UINT32						m_dwDataLen;

	CHAR						m_pRecvBuf[CONST_BUFF_SIZE];

	std::vector<IDataBuffer*>   m_SendBuffList;
	BOOL						m_IsSending;

    CConnection                *m_pNext;
};


class CConnectionMgr
{
private:
	CConnectionMgr();

	~CConnectionMgr();

public:
	static CConnectionMgr* GetInstancePtr();

public:
    BOOL            InitConnectionList(UINT32 nMaxCons);

    CConnection*    CreateConnection();

	VOID		    DeleteConnection(CConnection *pConnection);

    CConnection*    GetConnectionByConnID(UINT64 dwConnID);

	///////////////////////////////////////////
	BOOL		    CloseAllConnection();

	BOOL		    DestroyAllConnection();

public:
    CCritSec        m_CritSec;
    CConnection     *m_pFreeConnRoot;
    std::vector<CConnection> m_vtConnList;            //连接列表
};

#endif
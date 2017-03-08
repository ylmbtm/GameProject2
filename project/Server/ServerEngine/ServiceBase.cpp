#include "stdafx.h"
#include "ServiceBase.h"
#include "NetManager.h"
#include "DataBuffer/BufferHelper.h"
#include "Utility/CommonSocket.h"
#include "CommandDef.h"
#include "PacketDef/ClientPacket.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonEvent.h"
#include "Utility/CommonConvert.h"
#include "DataBuffer/DataBuffer.h"
#include "Connection.h"
#include "Utility/CommonFunc.h"

ServiceBase::ServiceBase(void)
{
	m_pPacketDispatcher = NULL;
}

ServiceBase::~ServiceBase(void)
{
}

ServiceBase* ServiceBase::GetInstancePtr()
{
	static ServiceBase _ServiceBase;

	return &_ServiceBase;
}


BOOL ServiceBase::OnDataHandle(IDataBuffer *pDataBuffer , CConnection *pConnection)
{
	PacketHeader *pHeader = (PacketHeader *)pDataBuffer->GetBuffer();
	m_DataQueue.push(NetPacket(pConnection, pDataBuffer,pHeader->wCommandID));
	return TRUE;
}

BOOL ServiceBase::StartNetwork(UINT16 nPortNum, UINT32 nMaxConn, IPacketDispatcher *pDispather)
{
	if (pDispather == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	m_pPacketDispatcher = pDispather;

	if (!CNetManager::GetInstancePtr()->Start(nPortNum, nMaxConn, this))
	{
		CLog::GetInstancePtr()->AddLog("启动网络层失败!");
		return FALSE;
	}

	CLog::GetInstancePtr()->AddLog("服务器启动成功!");

	return TRUE;
}

BOOL ServiceBase::StopNetwork()
{
	CLog::GetInstancePtr()->AddLog("==========服务器开始关闭=======================");

	CNetManager::GetInstancePtr()->Close();

	CLog::GetInstancePtr()->CloseLog();

	return TRUE;
}

template<typename T>
BOOL ServiceBase::SendCmdToConnection(UINT16 uCmdID, T &Data, UINT64 u64ConnID, UINT64 uCharID,UINT32 dwSceneID)
{
	if(u64ConnID == 0)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(uCmdID, dwSceneID, uCharID);
	WriteHelper.Write(Data);
	WriteHelper.EndWrite();

	return CNetManager::GetInstancePtr()->SendBufferByConnID(u64ConnID, WriteHelper.GetDataBuffer());
}


BOOL ServiceBase::SendCmdToConnection(UINT64 u64ConnID, IDataBuffer *pSrcBuffer )
{
	if(u64ConnID == 0)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	IDataBuffer *pSendBuffer = CBufferManagerAll::GetInstancePtr()->AllocDataBuff(pSrcBuffer->GetDataLenth());
	if(pSendBuffer == NULL)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	pSendBuffer->CopyFrom(pSrcBuffer);

	return CNetManager::GetInstancePtr()->SendBufferByConnID(u64ConnID, pSendBuffer);
}

BOOL ServiceBase::SendCmdToConnection( UINT64 u64ConnID, UINT64 u64CharID, UINT32 dwSceneID, IDataBuffer *pSrcBuffer )
{
	if(u64ConnID == 0)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	IDataBuffer *pSendBuffer = CBufferManagerAll::GetInstancePtr()->AllocDataBuff(pSrcBuffer->GetDataLenth());
	if(pSendBuffer == NULL)
	{
		ASSERT_FAIELD;
		
		return FALSE;
	}

	pSendBuffer->CopyFrom(pSrcBuffer);

	PacketHeader *pPacketHeader = (PacketHeader *)(pSendBuffer->GetBuffer());

	pPacketHeader->u64CharID	= u64CharID;
	pPacketHeader->dwSceneID	= dwSceneID;

	ASSERT(pPacketHeader->u64CharID  != 0);
	ASSERT(pPacketHeader->wCommandID != 0);

	return CNetManager::GetInstancePtr()->SendBufferByConnID(u64ConnID, pSendBuffer);
}


BOOL ServiceBase::ConnectToOtherSvr( std::string strIpAddr, UINT16 sPort )
{
	return CNetManager::GetInstancePtr()->ConnectToOtherSvrEx(strIpAddr, sPort);
}

BOOL ServiceBase::OnCloseConnect( CConnection *pConnection )
{
	if(pConnection->GetConnectionID() == 0)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	m_CloseConList.push(pConnection);
	return TRUE;
}

BOOL ServiceBase::OnNewConnect( CConnection *pConnection )
{
	if(pConnection->GetConnectionID() == 0)
	{
		ASSERT_FAIELD;
		return FALSE;
	}
	m_NewConList.push(pConnection);
	return TRUE;
}


CConnection* ServiceBase::GetConnectionByID( UINT64 u64ConnID )
{
	return CConnectionMgr::GetInstancePtr()->GetConnectionByConnID(u64ConnID);
}

// BOOL ServiceBase::Update()
// {
// 	NetPacket item;
// 	//处理新连接的通知
// 	CConnection *pConnection = NULL;
// 	while(m_NewConList.pop(pConnection))
// 	{
// 		item.m_pDataBuffer = NULL;
// 		item.m_pConnect = pConnection;
// 		FireMessage(1, &item);
// 	}
// 
// 	while(m_DataQueue.pop(item))
// 	{
// 		PacketHeader *pPacketHeader = (PacketHeader *)item.m_pDataBuffer->GetBuffer();
// 
// 		FireMessage(pPacketHeader->wCommandID, &item);
// 	}
// 
// 	//处理断开的连接
// 	while(m_CloseConList.pop(pConnection))
// 	{
// 		//发送通知
// 		item.m_pDataBuffer = NULL;
// 		item.m_pConnect = pConnection;
// 		FireMessage(2, &item);
// 		//发送通知
// 		CConnectionMgr::GetInstancePtr()->DeleteConnection(pConnection);
// 	}
// 
// 	return TRUE;
// }

BOOL ServiceBase::Update()
{
	NetPacket item;
	//处理新连接的通知
	CConnection *pConnection = NULL;
	while(m_NewConList.pop(pConnection))
	{
		item.m_pDataBuffer = NULL;
		item.m_pConnect = pConnection;
		m_pPacketDispatcher->OnNewConnect(pConnection);
	}

	while(m_DataQueue.pop(item))
	{
		m_pPacketDispatcher->DispatchPacket(&item);
	}

	//处理断开的连接
	while(m_CloseConList.pop(pConnection))
	{
		//发送通知
		item.m_pDataBuffer = NULL;
		item.m_pConnect = pConnection;
		m_pPacketDispatcher->OnCloseConnect(pConnection);
		//发送通知
		CConnectionMgr::GetInstancePtr()->DeleteConnection(pConnection);
	}

	return TRUE;
}






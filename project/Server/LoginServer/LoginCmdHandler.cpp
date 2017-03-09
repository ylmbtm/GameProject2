#include "stdafx.h"
#include "CommandDef.h"
#include "GameDefine.h"
#include "LoginCmdHandler.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/ClientPacket.h"
#include "GameService.h"
#include "Utility/CommonSocket.h"
#include "PacketDef/PacketHeader.h"
#include "DataBuffer/BufferHelper.h"
#include "DataBuffer/DataBuffer.h"
#include "PacketDef/ClientPacket.h"
#include "PacketDef/DBPacket.h"
#include "Error.h"
#include "PacketDef/SvrReportPacket.h"



CLoginCmdHandler::CLoginCmdHandler()
{

}

CLoginCmdHandler::~CLoginCmdHandler()
{

}

BOOL CLoginCmdHandler::Init()
{
	

	return TRUE;
}

BOOL CLoginCmdHandler::Uninit()
{
	

	return TRUE;
}



BOOL CLoginCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
		PROCESS_COMMAND_ITEM(CMD_CHAR_NEW_ACCOUNT_REQ,	OnCmdNewAccountReq);
		PROCESS_COMMAND_ITEM(CMD_CHAR_NEW_CHAR_REQ,		OnCmdNewCharReq);
		PROCESS_COMMAND_ITEM(CMD_CHAR_DEL_CHAR_REQ,		OnCmdDelCharReq);
		PROCESS_COMMAND_ITEM(CMD_CHAR_PICK_CHAR_REQ,	OnCmdPickCharReq);
		PROCESS_COMMAND_ITEM(CMD_CHAR_LOGIN_REQ,		OnCmdLoginReq);
		PROCESS_COMMAND_ITEM(CMD_DB_NEW_ACCOUNT_ACK,	OnCmdDBNewAccountAck);
		PROCESS_COMMAND_ITEM(CMD_DB_NEW_CHAR_ACK,		OnCmdDBNewCharAck);
		PROCESS_COMMAND_ITEM(CMD_DB_DEL_CHAR_ACK,		OnCmdDBDelCharAck);
		PROCESS_COMMAND_ITEM(CMD_DB_PICK_CHAR_ACK,		OnCmdDBPickCharAck);
		PROCESS_COMMAND_ITEM(CMD_DB_LOGIN_ACK,			OnCmdDBLoginAck);
	default:
		{

		}
		break;
	}

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdLoginReq(NetPacket *pPacket)
{
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	StCharLoginReq MsgLoginReq;
	bh.Read(MsgLoginReq);

	StDBCharLoginReq DBCharLoginReq;
	DBCharLoginReq.u64ConnID = pPacket->m_pConnect->GetConnectionID();
	DBCharLoginReq.CharLoginReq = MsgLoginReq;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_LOGIN_REQ, 0, 0);
	WriteHelper.Write(DBCharLoginReq);
	WriteHelper.EndWrite();
	CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer());
	
	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdNewAccountReq(NetPacket *pPacket )
{
	StCharNewAccountReq CharNewAccountReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(CharNewAccountReq);

	StDBNewAccountReq DBNewAccountReq;
	DBNewAccountReq.u64ConnID = pPacket->m_pConnect->GetConnectionID();
	DBNewAccountReq.CharNewAccountReq = CharNewAccountReq;

	ASSERT(DBNewAccountReq.u64ConnID != 0);
	
	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_NEW_ACCOUNT_REQ, 0, 0);
	WriteHelper.Write(DBNewAccountReq);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBNewAccountReq)+22));
	CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdNewCharReq(NetPacket *pPacket)
{
	StCharNewCharReq CharNewCharReq;

	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(CharNewCharReq);

	StDBNewCharReq DBNewCharReq;
	DBNewCharReq.u64ConnID = pPacket->m_pConnect->GetConnectionID();
	DBNewCharReq.CharNewCharReq = CharNewCharReq;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_NEW_CHAR_REQ , 0, 0);
	WriteHelper.Write(DBNewCharReq);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBNewCharReq)+22));
	CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdPickCharReq(NetPacket *pPacket)
{
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);

	StCharPickCharReq CharPickCharReq;
	bh.Read(CharPickCharReq);

	StDBCharPickCharReq DBPickCharReq;
	DBPickCharReq.u64ConnID = pPacket->m_pConnect->GetConnectionID();
	DBPickCharReq.CharPickCharReq = CharPickCharReq;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_PICK_CHAR_REQ , 0, 0);
	WriteHelper.Write(DBPickCharReq);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBPickCharReq)+22));
	CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDBNewAccountAck( NetPacket *pPacket )
{
	StDBNewAccountAck DBNewAccountAck;

	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBNewAccountAck);

	ASSERT(DBNewAccountAck.u64ConnID != 0);

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_CHAR_NEW_ACCOUNT_ACK , 0, 0);
	WriteHelper.Write(DBNewAccountAck.CharNewAccountAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBNewAccountAck.CharNewAccountAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(DBNewAccountAck.u64ConnID, WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDBNewCharAck( NetPacket *pPacket )
{	
	StDBCharNewCharAck DBNewCharAck;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBNewCharAck);

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_CHAR_NEW_CHAR_ACK , 0, 0);
	WriteHelper.Write(DBNewCharAck.CharNewCharAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBNewCharAck.CharNewCharAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(DBNewCharAck.u64ConnID, WriteHelper.GetDataBuffer());
	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDBPickCharAck( NetPacket *pPacket )
{
	StDBCharPickCharAck DBCharPickCharAck;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBCharPickCharAck);

	//选人这里需要做以下几件事。
	//1。将可用的proxyver地址和端口，ID发给客户端。
	//2。将登录的识别码和ID发给代理服
	//3。将玩家应该进入的地图告诉玩家。

	DBCharPickCharAck.CharPickCharAck.dwIdentifyCode	= rand()%10000;
	DBCharPickCharAck.CharPickCharAck.nRetCode			= E_SUCCESSED;
	DBCharPickCharAck.CharPickCharAck.nProxyPort		= 7200;
	
	strncpy(DBCharPickCharAck.CharPickCharAck.szProxyIpAddr, "127.0.0.1", 32);

	StCharWillEnterGame CharWillEnterGame;
	CharWillEnterGame.dwIdentifyCode	= DBCharPickCharAck.CharPickCharAck.dwIdentifyCode;
	CharWillEnterGame.u64CharID			= DBCharPickCharAck.CharPickCharAck.u64CharID;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_SVR_CHAR_WILL_ENTER , 0, 0);
	WriteHelper.Write(CharWillEnterGame);
	WriteHelper.EndWrite();
	ServiceBase::GetInstancePtr()->SendCmdToConnection(201, WriteHelper.GetDataBuffer());

	WriteHelper.BeginWrite(CMD_CHAR_PICK_CHAR_ACK , 0, 0);
	WriteHelper.Write(DBCharPickCharAck.CharPickCharAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBCharPickCharAck.CharPickCharAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(DBCharPickCharAck.u64ConnID, WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDBLoginAck( NetPacket *pPacket )
{
	StDBCharLoginAck DBCharLoginAck;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBCharLoginAck);

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_CHAR_LOGIN_ACK , 0, 0);
	WriteHelper.Write(DBCharLoginAck.CharLoginAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBCharLoginAck.CharLoginAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(DBCharLoginAck.u64ConnID, WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDelCharReq( NetPacket *pPacket )
{
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);

	StCharDelCharReq CharDelCharReq;
	bh.Read(CharDelCharReq);

	StDBDelCharReq DBDelCharReq;
	DBDelCharReq.u64ConnID = pPacket->m_pConnect->GetConnectionID();
	DBDelCharReq.CharDelCharReq = CharDelCharReq;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_DEL_CHAR_REQ , 0, 0);
	WriteHelper.Write(DBDelCharReq);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBDelCharReq)+22));
	CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CLoginCmdHandler::OnCmdDBDelCharAck( NetPacket *pPacket )
{
	StDBCharDelCharAck DBDelCharAck;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBDelCharAck);

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_CHAR_DEL_CHAR_ACK, 0, 0);
	WriteHelper.Write(DBDelCharAck.CharDelCharAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBDelCharAck.CharDelCharAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(DBDelCharAck.u64ConnID, WriteHelper.GetDataBuffer());

	return TRUE;
}

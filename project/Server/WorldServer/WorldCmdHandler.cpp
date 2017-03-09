#include "stdafx.h"
#include "CommandDef.h"
#include "GameDefine.h"
#include "Error.h"
#include "WorldCmdHandler.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/ClientPacket.h"
#include "GameService.h"
#include "PacketDef/PacketHeader.h"
#include "PacketDef/DBPacket.h"
#include "Utility/CommonSocket.h"
#include "DataBuffer/BufferHelper.h"
#include "DataBuffer/DataBuffer.h"
#include "PacketDef/SvrReportPacket.h"
#include "PlayerObject.h"




CWorldCmdHandler::CWorldCmdHandler()
{

}

CWorldCmdHandler::~CWorldCmdHandler()
{

}

BOOL CWorldCmdHandler::Init(UINT32 dwReserved)
{
	

	return TRUE;
}

BOOL CWorldCmdHandler::Uninit()
{
	return TRUE;
}

BOOL CWorldCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
		PROCESS_COMMAND_ITEM(CMD_CHAR_ENTER_GAME_REQ,	OnCmdEnterGameReq);
		PROCESS_COMMAND_ITEM(CMD_DB_LOAD_CHAR_ACK,		OnCmdDBLoadCharAck);
		PROCESS_COMMAND_ITEM(CMD_SVR_CREATE_SCENE_ACK,	OnCmdCreateSceneAck);
		PROCESS_COMMAND_ITEM(CMD_CHAR_LEAVE_GAME_REQ,	OnCmdLeaveGameReq);
	default:
		{

		}
		break;
	}

	return TRUE;
}

BOOL CWorldCmdHandler::OnUpdate( UINT32 dwTick )
{


	return TRUE;
}

BOOL CWorldCmdHandler::OnCmdEnterGameReq(NetPacket *pNetPacket)
{
	StCharEnterGameReq CharEnterGameReq;
	CBufferHelper bh(FALSE, pNetPacket->m_pDataBuffer);
	bh.Read(CharEnterGameReq);

	StDBLoadCharInfoReq DBLoadCharInfoReq;
	DBLoadCharInfoReq.u64CharID		= CharEnterGameReq.u64CharID;
	DBLoadCharInfoReq.dwProxySvrID	= (UINT32)pNetPacket->m_pConnect->GetConnectionID();

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_LOAD_CHAR_REQ, 0, 0);
	WriteHelper.Write(DBLoadCharInfoReq);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBLoadCharInfoReq)+22));
	if (!CGameService::GetInstancePtr()->SendCmdToDBConnection(WriteHelper.GetDataBuffer()))
	{
		ASSERT_FAIELD;
	}

	return TRUE;
}

BOOL CWorldCmdHandler::OnCmdLeaveGameReq(NetPacket *pNetPacket)
{
	StCharLeaveGameReq CharLeaveGameReq;
	CBufferHelper bh(FALSE, pNetPacket->m_pDataBuffer);
	bh.Read(CharLeaveGameReq);

	if(CharLeaveGameReq.dwLeaveReason == LGR_Disconnect)
	{

	}
	else if(CharLeaveGameReq.dwLeaveReason == LGR_Quit)
	{

	}

	CPlayerObject *pPlayerObject = m_PlayerObjectMgr.GetPlayer(bh.GetPacketHeader()->u64CharID);
	if(pPlayerObject == NULL)
	{
		ASSERT_FAIELD;
		return TRUE;
	}

	m_PlayerObjectMgr.ReleasePlayer(pPlayerObject->GetObjectID());

	return TRUE;
}


BOOL CWorldCmdHandler::OnCmdDBLoadCharAck(NetPacket *pNetPacket)
{
	StDBLoadCharInfoAck DBLoadCharInfoAck;
	CBufferHelper bh(FALSE, pNetPacket->m_pDataBuffer);
	bh.Read(DBLoadCharInfoAck);

	CPlayerObject *pPlayerObject = m_PlayerObjectMgr.CreatePlayerByID(DBLoadCharInfoAck.u64CharID);
	CHECK_AND_RETURN_ASSERT(pPlayerObject, TRUE);

	if(!pPlayerObject->LoadFromDBPacket(&bh))
	{
		ASSERT_FAIELD;
		return TRUE;
	}

	UINT32 dwGameSvrID = m_GameSvrMgr.GetServerIDBySceneID(pPlayerObject->m_dwSceneID);
	if(dwGameSvrID == 0)
	{
		ASSERT_FAIELD;
		return TRUE;
	}

	//继续往游戏服转移
	StSvrEnterSceneReq SvrEnterSceneReq;
	SvrEnterSceneReq.dwGameSvrID  = dwGameSvrID;
	SvrEnterSceneReq.dwProxySvrID = DBLoadCharInfoAck.dwProxySvrID;
	SvrEnterSceneReq.dwSceneID    = pPlayerObject->m_dwSceneID;
	SvrEnterSceneReq.u64CharID    = DBLoadCharInfoAck.u64CharID;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_SVR_ENTER_SCENE_REQ, SvrEnterSceneReq.dwSceneID, DBLoadCharInfoAck.u64CharID);
	WriteHelper.Write(SvrEnterSceneReq);

	pPlayerObject->SaveDataToPacket(&WriteHelper);

	WriteHelper.EndWrite();
	ServiceBase::GetInstancePtr()->SendCmdToConnection(SvrEnterSceneReq.dwGameSvrID, WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CWorldCmdHandler::OnCmdCreateSceneAck(NetPacket *pNetPacket)
{
	StSvrCreateSceneAck CreateSceneAck;
	CBufferHelper bh(FALSE, pNetPacket->m_pDataBuffer);
	bh.Read(CreateSceneAck);

	if(CreateSceneAck.dwAckCode == E_SUCCESSED)
	{
		//创建场景成功 
	}
	else
	{
		//创建场景失败
	}


	return TRUE;
}


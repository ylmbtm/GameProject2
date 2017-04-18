#include "stdafx.h"
#include "ProxyCmdHandler.h"
#include "CommandDef.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/PacketHeader.h"
#include "PacketDef/ClientPacket.h"
#include "GameService.h"
#include "Utility/CommonSocket.h"
#include "DataBuffer/BufferHelper.h"
#include "DataBuffer/DataBuffer.h"
#include "ObjectID.h"
#include "StaticPlayerMgr.h"


CProxyCmdHandler::CProxyCmdHandler()
{
	CStaticPlayerMgr::GetInstancePtr();
}

CProxyCmdHandler::~CProxyCmdHandler()
{

}

BOOL CProxyCmdHandler::Init(UINT32 dwReserved)
{
	return TRUE;
}

BOOL CProxyCmdHandler::Uninit()
{
	return TRUE;
}

BOOL CProxyCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	PacketHeader *pPacketHeader = (PacketHeader *)pNetPacket->m_pDataBuffer->GetBuffer();
	if(pPacketHeader == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	CBufferHelper bh(FALSE, pNetPacket->m_pDataBuffer);
	
	switch(pNetPacket->m_dwCmdID)
	{
	case CMD_CHAR_ENTER_GAME_REQ:
		{
			CLog::GetInstancePtr()->AddLog("---Receive Message:[%s]", "CMD_CHAR_ENTER_GAME_REQ");
			CHECK_PAYER_ID(pPacketHeader->u64CharID);
			/*CWillEnterNode *pWillEnterNode = m_WillEnterNodeMgr.GetByCharID(pBufferHelper->GetPacketHeader()->u64CharID);
			if(pWillEnterNode == NULL)
			{
				//非法的进入
				ASSERT_FAIELD;
				break;
			}

			StCharEnterGameReq CharEnterGameReq;
			pBufferHelper->Read(CharEnterGameReq);

			CHECK_PAYER_ID(CharEnterGameReq.u64CharID);
			
			if(pWillEnterNode->m_dwIdentifyCode != CharEnterGameReq.dwIdentifyCode)
			{
				//非法的进入
				//m_WillEnterNodeMgr.RemoveByCharID(CharEnterGameReq.u64CharID);
				ASSERT_FAIELD;
				break;
			}

			//m_WillEnterNodeMgr.RemoveByCharID(CharEnterGameReq.u64CharID);
			
			*/
			StCharEnterGameReq CharEnterGameReq;
			bh.Read(CharEnterGameReq);
			CHECK_PAYER_ID(CharEnterGameReq.u64CharID);
			CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(CharEnterGameReq.u64CharID);
			if(pStaticPlayer == NULL)
			{
				pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->CreateStaicPlayer(pPacketHeader->u64CharID);
				if(pStaticPlayer == NULL)
				{
					ASSERT_FAIELD;
					break;
				}

				pStaticPlayer->m_CharState = LS_Online;
			}
			else
			{
				if(pStaticPlayer->m_CharState == LS_Online)
				{
					ASSERT_FAIELD;
					break;
				}

				pStaticPlayer->m_CharState = LS_Online;
			}

			ASSERT(pStaticPlayer->m_CharState==LS_Online);

			//发向世界服
			RelayToWorldServer(pStaticPlayer, pNetPacket->m_pDataBuffer);
		}
		break;
	case CMD_SVR_CHAR_WILL_ENTER:
		{
			/*CLog::GetInstancePtr()->AddLog("---Receive Message:[%s]", "CMD_SVR_CHAR_WILL_ENTER");
			StCharWillEnterGame CharWillEnterGame;
			pBufferHelper->Read(CharWillEnterGame);

			CWillEnterNode *pWillEnterNode = m_WillEnterNodeMgr.CreateWillEnterNode(CharWillEnterGame.u64CharID);
			if(pWillEnterNode == NULL)
			{
				ASSERT_FAIELD;
				break;
			}

			pWillEnterNode->m_dwIdentifyCode  = CharWillEnterGame.dwIdentifyCode;*/
		}
		break;
	case CMD_CHAR_GAME_MANAGER:
		{
			CLog::GetInstancePtr()->AddLog("---Receive Message:[%s]----", "CMD_CHAR_GAME_MANAGER");
			CConnection *pConn = pNetPacket->m_pConnect;
			if(pConn == NULL)
			{
				break;
			}

			//判断IP是否合法
			if(pConn->m_dwIpAddr == 0)
			{

			}

			StCharGmCmdReq CharGmCmdReq;
			bh.Read(CharGmCmdReq);
	
			/*if(!OnCmdGMCommand(CMD_CHAR_GAME_MANAGER, u64ConnID, pBufferHelper))
			{
				CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(pBufferHelper->GetPacketHeader()->u64CharID);
				if(pStaticPlayer == NULL)
				{
					ASSERT_FAIELD;
					break;
				}

				ASSERT(pStaticPlayer->m_CharState==LS_Online);

				RelayToGameServer(pStaticPlayer, pBufferHelper->GetDataBuffer());
			}*/
		}
		break;

	case CMD_CHAR_ENTER_GAME_ACK:
		{
			CLog::GetInstancePtr()->AddLog("---Receive Message:[%s]----", "CMD_CHAR_ENTER_GAME_ACK");
			CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(pPacketHeader->u64CharID);
			if(pStaticPlayer == NULL)
			{
				ASSERT_FAIELD;
				break;
			}

			ASSERT(pStaticPlayer->m_CharState==LS_Online);

			StCharEnterGameAck CharEnterGameAck;
			bh.Read(CharEnterGameAck);

			pStaticPlayer->SetSceneID(CharEnterGameAck.dwSceneID);

			pStaticPlayer->SetGameSvrConnID(pNetPacket->m_pConnect->GetConnectionID());

			RelayToClient(pStaticPlayer, pNetPacket->m_pDataBuffer);
		}
		break;
	case CMD_CHAR_UPDATE_MYSELF:
		{
			CLog::GetInstancePtr()->AddLog("---Receive Message:[%s]----", "CMD_CHAR_UPDATE_MYSELF");
			CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(pPacketHeader->u64CharID);
			if(pStaticPlayer == NULL)
			{
				ASSERT_FAIELD;
				break;
			}

			if(pStaticPlayer->m_CharState == LS_OffLine)
			{
				break;
			}

			RelayToClient(pStaticPlayer, pNetPacket->m_pDataBuffer);
		}
		break;
	default:
		{
			CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(pPacketHeader->u64CharID);
			if(pStaticPlayer == NULL)
			{
				ASSERT_FAIELD;
				break;
			}

			if(pStaticPlayer->m_CharState == LS_OffLine)
			{
				break;
			}

			ASSERT(pStaticPlayer->m_CharState==LS_Online);

			if(u64ConnID < 1000)
			{
				RelayToClient(pStaticPlayer, pBufferHelper->GetDataBuffer());
			}
			else
			{
				pBufferHelper->GetPacketHeader()->dwSceneID = pStaticPlayer->GetSceneID();

				RelayToGameServer(pStaticPlayer, pNetPacket->m_pDataBuffer);
			}
		}
	}

	return TRUE;
}


BOOL CProxyCmdHandler::RelayToGameServer( CStaticPlayer *pClientObj, IDataBuffer *pBuffer )
{
	if(pClientObj == NULL)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(pClientObj->m_CharState == LS_OffLine)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(!ServiceBase::GetInstancePtr()->SendCmdToConnection(pClientObj->GetGameSvrConnID(), pBuffer))
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	return TRUE;
}

BOOL CProxyCmdHandler::RelayToWorldServer( CStaticPlayer *pClientObj, IDataBuffer *pBuffer )
{
	if(pClientObj == NULL)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(pClientObj->m_CharState == LS_OffLine)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(CGameService::GetInstancePtr()->GetWorldConnID() == 0)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(!ServiceBase::GetInstancePtr()->SendCmdToConnection(CGameService::GetInstancePtr()->GetWorldConnID(), pBuffer))
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	return TRUE;
}

BOOL CProxyCmdHandler::RelayToClient( CStaticPlayer *pStaticPlayer, IDataBuffer *pBuffer )
{
	if(pStaticPlayer == NULL)
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	if(!ServiceBase::GetInstancePtr()->SendCmdToConnection(pStaticPlayer->GetCharID(), pBuffer))
	{
		ASSERT_FAIELD;

		return FALSE;
	}

	return TRUE;
}

BOOL CProxyCmdHandler::OnCmdDisConnectNotify( UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper )
{
	if(u64ConnID < 1024)
	{
		return TRUE;
	}

	CStaticPlayer *pStaticPlayer = CStaticPlayerMgr::GetInstancePtr()->GetByCharID(u64ConnID);
	if(pStaticPlayer == NULL)
	{
		CLog::GetInstancePtr()->AddLog("收到连接断开的事件， 但没有玩家连接数据!!!!!!");
		return TRUE;
	}

	if(pStaticPlayer->m_CharState == LS_OffLine)
	{
		ASSERT_FAIELD;
		return TRUE;
	}

	StCharLeaveGameReq CharLeaveGameReq;
	CharLeaveGameReq.dwLeaveReason = LGR_Disconnect;
	IDataBuffer *pSendBuffer = CBufferManagerAll::GetInstancePtr()->AllocDataBuff(1024);
	if(pSendBuffer == NULL)
	{
		return FALSE;
	}

	CBufferHelper WriteHelper(TRUE, pSendBuffer);
	WriteHelper.BeginWrite(CMD_CHAR_LEAVE_GAME_REQ, pStaticPlayer->GetSceneID(), pStaticPlayer->GetCharID());
	WriteHelper.Write(CharLeaveGameReq);
	WriteHelper.EndWrite();

	ServiceBase::GetInstancePtr()->SendCmdToConnection(pStaticPlayer->GetGameSvrConnID(), pSendBuffer);
	ServiceBase::GetInstancePtr()->SendCmdToConnection(CGameService::GetInstancePtr()->GetWorldConnID(), pSendBuffer);
	pStaticPlayer->SetGameSvrConnID(0);
	pStaticPlayer->m_CharState = LS_OffLine;
	pSendBuffer->Release();

	//CStaticPlayerMgr::GetInstancePtr()->RemoveByCharID(pStaticPlayer->GetCharID());

	return TRUE;
}

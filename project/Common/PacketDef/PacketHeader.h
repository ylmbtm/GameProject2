#ifndef __PACKET__HEADER_H__
#define __PACKET__HEADER_H__

#pragma  pack(push)
#pragma  pack(1)

#define CLIENT_CODE 0x88
#define SERVER_CODE 0x99

struct PacketHeader
{
	UINT8	 CheckCode;
	UINT16   wCommandID;
	UINT32   dwSize;
	UINT32   dwPacketNo;	//包序号 = wCommandID^dwSize+1;
	UINT32   dwSceneID;
	UINT64   u64CharID;
};

#pragma  pack(pop)



#endif /* __PACKET__HEADER_H__*/

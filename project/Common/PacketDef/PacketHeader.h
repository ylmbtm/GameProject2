#ifndef __PACKET__HEADER_H__
#define __PACKET__HEADER_H__

#pragma  pack(push)
#pragma  pack(1)

struct PacketHeader
{
	UINT8	 CheckCode;
	UINT16   dwSize;
	UINT16   wCommandID;
	UINT32   dwPacketNo;	//包序号
	UINT32   dwSceneID;
	UINT64   u64CharID;
};

#pragma  pack(pop)



#endif /* __PACKET__HEADER_H__*/

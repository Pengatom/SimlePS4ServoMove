/*
* GroupSyncRead.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_C_H_

#include "RobotisDef.h"
#include "PortHandler.h"
#include "PacketHandler.h"

WINDECLSPEC int         GroupSyncRead               (int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length);

WINDECLSPEC void        GroupSyncRead_MakeParam     (int group_num);
WINDECLSPEC bool        GroupSyncRead_AddParam      (int group_num, UINT8_T id);
WINDECLSPEC void        GroupSyncRead_RemoveParam   (int group_num, UINT8_T id);
WINDECLSPEC void        GroupSyncRead_ClearParam    (int group_num);

WINDECLSPEC void        GroupSyncRead_TxPacket      (int group_num);
WINDECLSPEC void        GroupSyncRead_RxPacket      (int group_num);
WINDECLSPEC void        GroupSyncRead_TxRxPacket    (int group_num);

WINDECLSPEC bool        GroupSyncRead_IsAvailable   (int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length);
WINDECLSPEC UINT32_T    GroupSyncRead_GetData       (int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length);


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_C_H_ */

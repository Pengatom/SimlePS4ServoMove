/*
* GroupSyncWrite.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCWRITE_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCWRITE_C_H_

#include "RobotisDef.h"
#include "PortHandler.h"
#include "PacketHandler.h"

WINDECLSPEC int     GroupSyncWrite              (int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length);

WINDECLSPEC void    GroupSyncWrite_MakeParam    (int group_num);
WINDECLSPEC bool    GroupSyncWrite_AddParam     (int group_num, UINT8_T id, UINT32_T data, UINT16_T data_length);
WINDECLSPEC void    GroupSyncWrite_RemoveParam  (int group_num, UINT8_T id);
WINDECLSPEC bool    GroupSyncWrite_ChangeParam  (int group_num, UINT8_T id, UINT32_T data, UINT16_T data_length, UINT16_T data_pos);
WINDECLSPEC void    GroupSyncWrite_ClearParam   (int group_num);

WINDECLSPEC void    GroupSyncWrite_TxPacket     (int group_num);


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCWRITE_C_H_ */

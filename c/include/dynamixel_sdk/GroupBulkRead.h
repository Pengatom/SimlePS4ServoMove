/*
* GroupBulkRead.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_C_H_


#include "RobotisDef.h"
#include "PortHandler.h"
#include "PacketHandler.h"

WINDECLSPEC int         GroupBulkRead               (int port_num, int protocol_version);

WINDECLSPEC void        GroupBulkRead_MakeParam     (int group_num);
WINDECLSPEC bool        GroupBulkRead_AddParam      (int group_num, UINT8_T id, UINT16_T start_address, UINT16_T data_length);
WINDECLSPEC void        GroupBulkRead_RemoveParam   (int group_num, UINT8_T id);
WINDECLSPEC void        GroupBulkRead_ClearParam    (int group_num);

WINDECLSPEC void        GroupBulkRead_TxPacket      (int group_num);
WINDECLSPEC void        GroupBulkRead_RxPacket      (int group_num);
WINDECLSPEC void        GroupBulkRead_TxRxPacket    (int group_num);

WINDECLSPEC bool        GroupBulkRead_IsAvailable   (int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length);
WINDECLSPEC UINT32_T    GroupBulkRead_GetData       (int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_C_H_ */

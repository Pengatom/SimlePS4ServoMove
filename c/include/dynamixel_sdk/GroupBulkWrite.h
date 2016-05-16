/*
* GroupBulkWrite.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_C_H_

#include "RobotisDef.h"
#include "PortHandler.h"
#include "PacketHandler.h"

WINDECLSPEC int     GroupBulkWrite              (int port_num, int protocol_version);

WINDECLSPEC void    GroupBulkWrite_MakeParam    (int group_num);
WINDECLSPEC bool    GroupBulkWrite_AddParam     (int group_num, UINT8_T id, UINT16_T start_address, UINT16_T data_length, UINT32_T data, UINT16_T input_length);
WINDECLSPEC void    GroupBulkWrite_RemoveParam  (int group_num, UINT8_T id);
WINDECLSPEC bool    GroupBulkWrite_ChangeParam  (int group_num, UINT8_T id, UINT16_T start_address, UINT16_T data_length, UINT32_T data, UINT16_T input_length, UINT16_T data_pos);
WINDECLSPEC void    GroupBulkWrite_ClearParam   (int group_num);

WINDECLSPEC void    GroupBulkWrite_TxPacket     (int group_num);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_C_H_ */

/*
* Protocol2PacketHandler.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL2PACKETHANDLER_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL2PACKETHANDLER_C_H_


#include "PacketHandler.h"

WINDECLSPEC UINT16_T    UpdateCRC           (UINT16_T crc_accum, UINT8_T *data_blk_ptr, UINT16_T data_blk_size);
WINDECLSPEC void        AddStuffing         (UINT8_T *packet);
WINDECLSPEC void        RemoveStuffing      (UINT8_T *packet);

WINDECLSPEC void        PrintTxRxResult2    (int result);
WINDECLSPEC void        PrintRxPacketError2     (UINT8_T error);

WINDECLSPEC int         GetLastTxRxResult2  (int port_num);
WINDECLSPEC UINT8_T     GetLastRxPacketError2   (int port_num);

WINDECLSPEC void        SetDataWrite2       (int port_num, UINT16_T data_length, UINT16_T data_pos, UINT32_T data);
WINDECLSPEC UINT32_T    GetDataRead2        (int port_num, UINT16_T data_length, UINT16_T data_pos);

WINDECLSPEC void        TxPacket2           (int port_num);
WINDECLSPEC void        RxPacket2           (int port_num);
WINDECLSPEC void        TxRxPacket2         (int port_num);

WINDECLSPEC void        Ping2               (int port_num, UINT8_T id);
WINDECLSPEC UINT16_T    PingGetModelNum2    (int port_num, UINT8_T id);

// BroadcastPing
WINDECLSPEC void        BroadcastPing2      (int port_num);
WINDECLSPEC bool        GetBroadcastPingResult2 (int port_num, int id);

WINDECLSPEC void        Action2             (int port_num, UINT8_T id);
WINDECLSPEC void        Reboot2             (int port_num, UINT8_T id);
WINDECLSPEC void        FactoryReset2       (int port_num, UINT8_T id, UINT8_T option);

WINDECLSPEC void        ReadTx2             (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        ReadRx2             (int port_num, UINT16_T length);
WINDECLSPEC void        ReadTxRx2           (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        Read1ByteTx2        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT8_T     Read1ByteRx2        (int port_num);
WINDECLSPEC UINT8_T     Read1ByteTxRx2      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read2ByteTx2        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT16_T    Read2ByteRx2        (int port_num);
WINDECLSPEC UINT16_T    Read2ByteTxRx2      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read4ByteTx2        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT32_T    Read4ByteRx2        (int port_num);
WINDECLSPEC UINT32_T    Read4ByteTxRx2      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        WriteTxOnly2        (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        WriteTxRx2          (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        Write1ByteTxOnly2   (int port_num, UINT8_T id, UINT16_T address, UINT8_T data);
WINDECLSPEC void        Write1ByteTxRx2     (int port_num, UINT8_T id, UINT16_T address, UINT8_T data);

WINDECLSPEC void        Write2ByteTxOnly2   (int port_num, UINT8_T id, UINT16_T address, UINT16_T data);
WINDECLSPEC void        Write2ByteTxRx2     (int port_num, UINT8_T id, UINT16_T address, UINT16_T data);

WINDECLSPEC void        Write4ByteTxOnly2   (int port_num, UINT8_T id, UINT16_T address, UINT32_T data);
WINDECLSPEC void        Write4ByteTxRx2     (int port_num, UINT8_T id, UINT16_T address, UINT32_T data);

WINDECLSPEC void        RegWriteTxOnly2     (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        RegWriteTxRx2       (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        SyncReadTx2         (int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);
// SyncReadRx   -> GroupSyncRead class
// SyncReadTxRx -> GroupSyncRead class

// param : ID1 DATA0 DATA1 ... DATAn ID2 DATA0 DATA1 ... DATAn ID3 DATA0 DATA1 ... DATAn
WINDECLSPEC void        SyncWriteTxOnly2   (int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);

// param : ID1 ADDR_L1 ADDR_H1 LEN_L1 LEN_H1 ID2 ADDR_L2 ADDR_H2 LEN_L2 LEN_H2 ...
WINDECLSPEC void        BulkReadTx2        (int port_num, UINT16_T param_length);
// BulkReadRx   -> GroupBulkRead class
// BulkReadTxRx -> GroupBulkRead class

// param : ID1 START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H DATA0 DATA1 ... DATAn ID2 START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H DATA0 DATA1 ... DATAn
WINDECLSPEC void        BulkWriteTxOnly2   (int port_num, UINT16_T param_length);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL2PACKETHANDLER_C_H_ */

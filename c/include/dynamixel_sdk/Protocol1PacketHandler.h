/*
* Protocol1PacketHandler.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL1PACKETHANDLER_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL1PACKETHANDLER_C_H_


#include "PacketHandler.h"

WINDECLSPEC void        PrintTxRxResult1    (int result);
WINDECLSPEC void        PrintRxPacketError1 (UINT8_T error);

WINDECLSPEC int         GetLastTxRxResult1  (int port_num);
WINDECLSPEC UINT8_T     GetLastRxPacketError1   (int port_num);

WINDECLSPEC void        SetDataWrite1       (int port_num, UINT16_T data_length, UINT16_T data_pos, UINT32_T data);
WINDECLSPEC UINT32_T    GetDataRead1        (int port_num, UINT16_T data_length, UINT16_T data_pos);

WINDECLSPEC void        TxPacket1           (int port_num);
WINDECLSPEC void        RxPacket1           (int port_num);
WINDECLSPEC void        TxRxPacket1         (int port_num);

WINDECLSPEC void        Ping1               (int port_num, UINT8_T id);
WINDECLSPEC UINT16_T    PingGetModelNum1    (int port_num, UINT8_T id);

// BroadcastPing
WINDECLSPEC void        BroadcastPing1      (int port_num);
WINDECLSPEC bool        GetBroadcastPingResult1 (int port_num, int id);

WINDECLSPEC void        Action1             (int port_num, UINT8_T id);
WINDECLSPEC void        Reboot1             (int port_num, UINT8_T id);
WINDECLSPEC void        FactoryReset1       (int port_num, UINT8_T id, UINT8_T option);

WINDECLSPEC void        ReadTx1             (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        ReadRx1             (int port_num, UINT16_T length);
WINDECLSPEC void        ReadTxRx1           (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        Read1ByteTx1        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT8_T     Read1ByteRx1        (int port_num);
WINDECLSPEC UINT8_T     Read1ByteTxRx1      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read2ByteTx1        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT16_T    Read2ByteRx1        (int port_num);
WINDECLSPEC UINT16_T    Read2ByteTxRx1      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read4ByteTx1        (int port_num, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT32_T    Read4ByteRx1        (int port_num);
WINDECLSPEC UINT32_T    Read4ByteTxRx1      (int port_num, UINT8_T id, UINT16_T address);

WINDECLSPEC void        WriteTxOnly1        (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        WriteTxRx1          (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        Write1ByteTxOnly1   (int port_num, UINT8_T id, UINT16_T address, UINT8_T data);
WINDECLSPEC void        Write1ByteTxRx1     (int port_num, UINT8_T id, UINT16_T address, UINT8_T data);

WINDECLSPEC void        Write2ByteTxOnly1   (int port_num, UINT8_T id, UINT16_T address, UINT16_T data);
WINDECLSPEC void        Write2ByteTxRx1     (int port_num, UINT8_T id, UINT16_T address, UINT16_T data);

WINDECLSPEC void        Write4ByteTxOnly1   (int port_num, UINT8_T id, UINT16_T address, UINT32_T data);
WINDECLSPEC void        Write4ByteTxRx1     (int port_num, UINT8_T id, UINT16_T address, UINT32_T data);

WINDECLSPEC void        RegWriteTxOnly1     (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        RegWriteTxRx1       (int port_num, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        SyncReadTx1         (int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);
// SyncReadRx   -> GroupSyncRead class
// SyncReadTxRx -> GroupSyncRead class

// param : ID1 DATA0 DATA1 ... DATAn ID2 DATA0 DATA1 ... DATAn ID3 DATA0 DATA1 ... DATAn
WINDECLSPEC void        SyncWriteTxOnly1    (int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);

// param : LEN1 ID1 ADDR1 LEN2 ID2 ADDR2 ...
WINDECLSPEC void        BulkReadTx1         (int port_num, UINT16_T param_length);
// BulkReadRx   -> GroupBulkRead class
// BulkReadTxRx -> GroupBulkRead class

// param : ID1 DATA0 DATA1 ... DATAn ID2 DATA0 DATA1 ... DATAn ID3 DATA0 DATA1 ... DATAn
WINDECLSPEC void        BulkWriteTxOnly1    (int port_num, UINT16_T param_length);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PROTOCOL1PACKETHANDLER_C_H_ */

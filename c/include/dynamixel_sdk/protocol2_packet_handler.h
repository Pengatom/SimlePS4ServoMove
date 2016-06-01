/*******************************************************************************
* Copyright (c) 2016, ROBOTIS CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* * Neither the name of ROBOTIS nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* Author: Leon Ryu Woon Jung */

/*
* Protocol2PacketHandler.h
*
*  Created on: 2016. 5. 4.
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

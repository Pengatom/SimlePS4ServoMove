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
* PacketHandler.h
*
*  Created on: 2016. 5. 4.
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PACKETHANDLER_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PACKETHANDLER_C_H_


#include <stdio.h>
#include "RobotisDef.h"
#include "PortHandler.h"

#define BROADCAST_ID        0xFE    // 254
#define MAX_ID              0xFC    // 252

/* Macro for Control Table Value */
#define DXL_MAKEWORD(a, b)  ((unsigned short)(((unsigned char)(((unsigned long)(a)) & 0xff)) | ((unsigned short)((unsigned char)(((unsigned long)(b)) & 0xff))) << 8))
#define DXL_MAKEDWORD(a, b) ((unsigned int)(((unsigned short)(((unsigned long)(a)) & 0xffff)) | ((unsigned int)((unsigned short)(((unsigned long)(b)) & 0xffff))) << 16))
#define DXL_LOWORD(l)       ((unsigned short)(((unsigned long)(l)) & 0xffff))
#define DXL_HIWORD(l)       ((unsigned short)((((unsigned long)(l)) >> 16) & 0xffff))
#define DXL_LOBYTE(w)       ((unsigned char)(((unsigned long)(w)) & 0xff))
#define DXL_HIBYTE(w)       ((unsigned char)((((unsigned long)(w)) >> 8) & 0xff))

/* Instruction for DXL Protocol */
#define INST_PING           1
#define INST_READ           2
#define INST_WRITE          3
#define INST_REG_WRITE      4
#define INST_ACTION         5
#define INST_FACTORY_RESET  6
#define INST_SYNC_WRITE     131     // 0x83
#define INST_BULK_READ      146     // 0x92
// --- Only for 2.0 --- //
#define INST_REBOOT         8
#define INST_STATUS         85      // 0x55
#define INST_SYNC_READ      130     // 0x82
#define INST_BULK_WRITE     147     // 0x93

// Communication Result
#define COMM_SUCCESS        0       // tx or rx packet communication success
#define COMM_PORT_BUSY      -1000   // Port is busy (in use)
#define COMM_TX_FAIL        -1001   // Failed transmit instruction packet
#define COMM_RX_FAIL        -1002   // Failed get status packet
#define COMM_TX_ERROR       -2000   // Incorrect instruction packet
#define COMM_RX_WAITING     -3000   // Now recieving status packet
#define COMM_RX_TIMEOUT     -3001   // There is no status packet
#define COMM_RX_CORRUPT     -3002   // Incorrect status packet
#define COMM_NOT_AVAILABLE  -9000   //

typedef struct
{
    UINT8_T     *data_write_;
    UINT8_T     *data_read_;
    UINT8_T     *txpacket_;
    UINT8_T     *rxpacket_;
    UINT8_T     error_;
    int         communication_result_;
    UINT8_T     *broadcastping_id_list;
}PacketData;

PacketData *packetData;

WINDECLSPEC void        PacketHandler       ();

WINDECLSPEC void        PrintTxRxResult     (int protocol_version, int result);
WINDECLSPEC void        PrintRxPacketError  (int protocol_version, UINT8_T error);

WINDECLSPEC int         GetLastTxRxResult   (int port_num, int protocol_version);
WINDECLSPEC UINT8_T     GetLastRxPacketError    (int port_num, int protocol_version);

WINDECLSPEC void        SetDataWrite        (int port_num, int protocol_version, UINT16_T data_length, UINT16_T data_pos, UINT32_T data);
WINDECLSPEC UINT32_T    GetDataRead         (int port_num, int protocol_version, UINT16_T data_length, UINT16_T data_pos);

WINDECLSPEC void        TxPacket            (int port_num, int protocol_version);

WINDECLSPEC void        RxPacket            (int port_num, int protocol_version);

WINDECLSPEC void        TxRxPacket          (int port_num, int protocol_version);

WINDECLSPEC void        Ping                (int port_num, int protocol_version, UINT8_T id);

WINDECLSPEC UINT16_T    PingGetModelNum     (int port_num, int protocol_version, UINT8_T id);

// BroadcastPing
WINDECLSPEC void        BroadcastPing       (int port_num, int protocol_version);
WINDECLSPEC bool        GetBroadcastPingResult  (int port_num, int protocol_version, int id);

WINDECLSPEC void        Reboot              (int port_num, int protocol_version, UINT8_T id);

WINDECLSPEC void        FactoryReset        (int port_num, int protocol_version, UINT8_T id, UINT8_T option);

WINDECLSPEC void        ReadTx              (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void        ReadRx              (int port_num, int protocol_version, UINT16_T length);
WINDECLSPEC void        ReadTxRx            (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void        Read1ByteTx         (int port_num, int protocol_version, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT8_T     Read1ByteRx         (int port_num, int protocol_version);
WINDECLSPEC UINT8_T     Read1ByteTxRx       (int port_num, int protocol_version, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read2ByteTx         (int port_num, int protocol_version, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT16_T    Read2ByteRx         (int port_num, int protocol_version);
WINDECLSPEC UINT16_T    Read2ByteTxRx       (int port_num, int protocol_version, UINT8_T id, UINT16_T address);

WINDECLSPEC void        Read4ByteTx         (int port_num, int protocol_version, UINT8_T id, UINT16_T address);
WINDECLSPEC UINT32_T    Read4ByteRx         (int port_num, int protocol_version);
WINDECLSPEC UINT32_T    Read4ByteTxRx       (int port_num, int protocol_version, UINT8_T id, UINT16_T address);

WINDECLSPEC void    WriteTxOnly             (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void    WriteTxRx               (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void    Write1ByteTxOnly        (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT8_T data);
WINDECLSPEC void    Write1ByteTxRx          (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT8_T data);

WINDECLSPEC void    Write2ByteTxOnly        (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T data);
WINDECLSPEC void    Write2ByteTxRx          (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T data);

WINDECLSPEC void    Write4ByteTxOnly        (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT32_T data);
WINDECLSPEC void    Write4ByteTxRx          (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT32_T data);

WINDECLSPEC void    RegWriteTxOnly          (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);
WINDECLSPEC void    RegWriteTxRx            (int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length);

WINDECLSPEC void    SyncReadTx              (int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);
// SyncReadRx   -> GroupSyncRead class
// SyncReadTxRx -> GroupSyncRead class

WINDECLSPEC void    SyncWriteTxOnly         (int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length);

WINDECLSPEC void    BulkReadTx              (int port_num, int protocol_version, UINT16_T param_length);
// BulkReadRx   -> GroupBulkRead class
// BulkReadTxRx -> GroupBulkRead class

WINDECLSPEC void    BulkWriteTxOnly         (int port_num, int protocol_version, UINT16_T param_length);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PACKETHANDLER_C_H_ */

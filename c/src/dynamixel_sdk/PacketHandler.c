/*
* PacketHandler.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include "dynamixel_sdk/PacketHandler.h"
#include "dynamixel_sdk/Protocol1PacketHandler.h"
#include "dynamixel_sdk/Protocol2PacketHandler.h"

void PacketHandler()
{
    int _port_num;

    packetData = (PacketData*)realloc(packetData, used_port_num_ * sizeof(PacketData));

    for (_port_num = 0; _port_num < used_port_num_; _port_num++)
    {
        packetData[_port_num].data_write_ = (UINT8_T *)calloc(1, sizeof(UINT8_T));
        packetData[_port_num].data_read_ = (UINT8_T *)calloc(1, sizeof(UINT8_T));
        packetData[_port_num].txpacket_ = (UINT8_T *)calloc(1, sizeof(UINT8_T));
        packetData[_port_num].rxpacket_ = (UINT8_T *)calloc(1, sizeof(UINT8_T));
        packetData[_port_num].error_ = 0;
        packetData[_port_num].communication_result_ = 0;
    }
}

void PrintTxRxResult(int protocol_version, int result)
{
    if (protocol_version == 1)
        PrintTxRxResult1(result);
    else
        PrintTxRxResult2(result);
};
void PrintRxPacketError(int protocol_version, UINT8_T error)
{
    if (protocol_version == 1)
        PrintRxPacketError1(error);
    else
        PrintRxPacketError2(error);
};

int GetLastTxRxResult(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        return GetLastTxRxResult1(port_num);
    else
        return GetLastTxRxResult2(port_num);
}
UINT8_T GetLastRxPacketError(int port_num, int protocol_version) 
{
    if (protocol_version == 1)
        return GetLastRxPacketError1(port_num);
    else
        return GetLastRxPacketError2(port_num);
}


void SetDataWrite(int port_num, int protocol_version, UINT16_T data_length, UINT16_T data_pos, UINT32_T data)
{
    if (protocol_version == 1)
        SetDataWrite1(port_num, data_length, data_pos, data);
    else
        SetDataWrite2(port_num, data_length, data_pos, data);
}
UINT32_T GetDataRead(int port_num, int protocol_version, UINT16_T data_length, UINT16_T data_pos)
{
    if (protocol_version == 1)
        return GetDataRead1(port_num, data_length, data_pos);
    else
        return GetDataRead2(port_num, data_length, data_pos);
}

void TxPacket(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        TxPacket1(port_num);
    else
        TxPacket2(port_num);
};

void RxPacket(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        RxPacket1(port_num);
    else
        return RxPacket2(port_num);
};

void TxRxPacket(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        TxRxPacket1(port_num);
    else
        return TxRxPacket2(port_num);
};

void Ping(int port_num, int protocol_version, UINT8_T id)
{
    if (protocol_version == 1)
        Ping1(port_num, id);
    else
        Ping2(port_num, id);
};

UINT16_T PingGetModelNum(int port_num, int protocol_version, UINT8_T id)
{
    if (protocol_version == 1)
        return PingGetModelNum1(port_num, id);
    else
        return PingGetModelNum2(port_num, id);
};

// BroadcastPing
void BroadcastPing(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        BroadcastPing1(port_num);
    else
        BroadcastPing2(port_num);
};

bool GetBroadcastPingResult(int port_num, int protocol_version, int id)
{
    if (protocol_version == 1)
        return GetBroadcastPingResult1(port_num, id);
    else
        return GetBroadcastPingResult2(port_num, id);
}

void Reboot(int port_num, int protocol_version, UINT8_T id)
{
    if (protocol_version == 1)
        Reboot1(port_num, id);
    else
        Reboot2(port_num, id);
};

void FactoryReset(int port_num, int protocol_version, UINT8_T id, UINT8_T option)
{
    if (protocol_version == 1)
        FactoryReset1(port_num, id, option);
    else
        FactoryReset2(port_num, id, option);
};

void ReadTx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        ReadTx1(port_num, id, address, length);
    else
        ReadTx2(port_num, id, address, length);
};
void ReadRx(int port_num, int protocol_version, UINT16_T length)
{
    if (protocol_version == 1)
        ReadRx1(port_num, length);
    else
        ReadRx2(port_num, length);
};
void ReadTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        ReadTxRx1(port_num, id, address, length);
    else
        ReadTxRx2(port_num, id, address, length);
};

void Read1ByteTx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        Read1ByteTx1(port_num, id, address);
    else
        Read1ByteTx2(port_num, id, address);
};
UINT8_T Read1ByteRx(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        return Read1ByteRx1(port_num);
    else
        return Read1ByteRx2(port_num);
};
UINT8_T Read1ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        return Read1ByteTxRx1(port_num, id, address);
    else
        return Read1ByteTxRx2(port_num, id, address);
};

void Read2ByteTx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        Read2ByteTx1(port_num, id, address);
    else
        Read2ByteTx2(port_num, id, address);
};
UINT16_T Read2ByteRx(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        return Read2ByteRx1(port_num);
    else
        return Read2ByteRx2(port_num);
};
UINT16_T Read2ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        return Read2ByteTxRx1(port_num, id, address);
    else
        return Read2ByteTxRx2(port_num, id, address);
};

void Read4ByteTx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        Read4ByteTx1(port_num, id, address);
    else
        Read4ByteTx2(port_num, id, address);
};
UINT32_T Read4ByteRx(int port_num, int protocol_version)
{
    if (protocol_version == 1)
        return Read4ByteRx1(port_num);
    else
        return Read4ByteRx2(port_num);
};
UINT32_T Read4ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address)
{
    if (protocol_version == 1)
        return Read4ByteTxRx1(port_num, id, address);
    else
        return Read4ByteTxRx2(port_num, id, address);
};

void WriteTxOnly(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        WriteTxOnly1(port_num, id, address, length);
    else
        WriteTxOnly2(port_num, id, address, length);
};
void WriteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        WriteTxRx1(port_num, id, address, length);
    else
        WriteTxRx2(port_num, id, address, length);
};

void Write1ByteTxOnly(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT8_T data)
{
    if (protocol_version == 1)
        Write1ByteTxOnly1(port_num, id, address, data);
    else
        Write1ByteTxOnly2(port_num, id, address, data);
};
void Write1ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT8_T data)
{
    if (protocol_version == 1)
        Write1ByteTxRx1(port_num, id, address, data);
    else
        Write1ByteTxRx2(port_num, id, address, data);
};

void Write2ByteTxOnly(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T data)
{
    if (protocol_version == 1)
        Write2ByteTxOnly1(port_num, id, address, data);
    else
        Write2ByteTxOnly2(port_num, id, address, data);
};
void Write2ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T data)
{
    if (protocol_version == 1)
        Write2ByteTxRx1(port_num, id, address, data);
    else
        Write2ByteTxRx2(port_num, id, address, data);
};

void Write4ByteTxOnly(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT32_T data)
{
    if (protocol_version == 1)
        Write4ByteTxOnly1(port_num, id, address, data);
    else
        Write4ByteTxOnly2(port_num, id, address, data);
};
void Write4ByteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT32_T data)
{
    if (protocol_version == 1)
        Write4ByteTxRx1(port_num, id, address, data);
    else
        Write4ByteTxRx2(port_num, id, address, data);
};

void RegWriteTxOnly(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        RegWriteTxOnly1(port_num, id, address, length);
    else
        RegWriteTxOnly2(port_num, id, address, length);
};
void RegWriteTxRx(int port_num, int protocol_version, UINT8_T id, UINT16_T address, UINT16_T length)
{
    if (protocol_version == 1)
        RegWriteTxRx1(port_num, id, address, length);
    else
        RegWriteTxRx2(port_num, id, address, length);
};

void SyncReadTx(int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    if (protocol_version == 1)
        SyncReadTx1(port_num, start_address, data_length, param_length);
    else
        SyncReadTx2(port_num, start_address, data_length, param_length);
};
// SyncReadRx   -> GroupSyncRead class
// SyncReadTxRx -> GroupSyncRead class

void SyncWriteTxOnly(int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    if (protocol_version == 1)
        SyncWriteTxOnly1(port_num, start_address, data_length, param_length);
    else
        SyncWriteTxOnly2(port_num, start_address, data_length, param_length);
};

void BulkReadTx(int port_num, int protocol_version, UINT16_T param_length)
{
    if (protocol_version == 1)
        BulkReadTx1(port_num, param_length);
    else
        BulkReadTx2(port_num, param_length);
};
// BulkReadRx   -> GroupBulkRead class
// BulkReadTxRx -> GroupBulkRead class

void BulkWriteTxOnly(int port_num, int protocol_version, UINT16_T param_length)
{
    if (protocol_version == 1)
        BulkWriteTxOnly1(port_num, param_length);
    else
        BulkWriteTxOnly2(port_num, param_length);
};

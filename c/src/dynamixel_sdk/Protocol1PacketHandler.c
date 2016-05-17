/*
* Protocol1PacketHandler.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <string.h>
#include <stdlib.h>
#include "dynamixel_sdk/Protocol1PacketHandler.h"

#define TXPACKET_MAX_LEN    (250)
#define RXPACKET_MAX_LEN    (250)

///////////////// for Protocol 1.0 Packet /////////////////
#define PKT_HEADER0             0
#define PKT_HEADER1             1
#define PKT_ID                  2
#define PKT_LENGTH              3
#define PKT_INSTRUCTION         4
#define PKT_ERROR               4
#define PKT_PARAMETER0          5

///////////////// Protocol 1.0 Error bit /////////////////
#define ERRBIT_VOLTAGE          1       // Supplied voltage is out of the range (operating volatage set in the control table)
#define ERRBIT_ANGLE            2       // Goal position is written out of the range (from CW angle limit to CCW angle limit)
#define ERRBIT_OVERHEAT         4       // Temperature is out of the range (operating temperature set in the control table)
#define ERRBIT_RANGE            8       // Command(setting value) is out of the range for use.
#define ERRBIT_CHECKSUM         16      // Instruction packet checksum is incorrect.
#define ERRBIT_OVERLOAD         32      // The current load cannot be controlled by the set torque.
#define ERRBIT_INSTRUCTION      64      // Undefined instruction or delivering the action command without the reg_write command.


void PrintTxRxResult1(int result)
{
    switch (result)
    {
    case COMM_SUCCESS:
        printf("[TxRxResult] Communication success.\n");
        break;

    case COMM_PORT_BUSY:
        printf("[TxRxResult] Port is in use!\n");
        break;

    case COMM_TX_FAIL:
        printf("[TxRxResult] Failed transmit instruction packet!\n");
        break;

    case COMM_RX_FAIL:
        printf("[TxRxResult] Failed get status packet from device!\n");
        break;

    case COMM_TX_ERROR:
        printf("[TxRxResult] Incorrect instruction packet!\n");
        break;

    case COMM_RX_WAITING:
        printf("[TxRxResult] Now recieving status packet!\n");
        break;

    case COMM_RX_TIMEOUT:
        printf("[TxRxResult] There is no status packet!\n");
        break;

    case COMM_RX_CORRUPT:
        printf("[TxRxResult] Incorrect status packet!\n");
        break;

    case COMM_NOT_AVAILABLE:
        printf("[TxRxResult] Protocol does not support This function!\n");
        break;

    default:
        break;
    }
}

void PrintRxPacketError1(UINT8_T error)
{
    if (error & ERRBIT_VOLTAGE)
        printf("[RxPacketError] Input voltage error!\n");

    if (error & ERRBIT_ANGLE)
        printf("[RxPacketError] Angle limit error!\n");

    if (error & ERRBIT_OVERHEAT)
        printf("[RxPacketError] Overheat error!\n");

    if (error & ERRBIT_RANGE)
        printf("[RxPacketError] Out of range error!\n");

    if (error & ERRBIT_CHECKSUM)
        printf("[RxPacketError] Checksum error!\n");

    if (error & ERRBIT_OVERLOAD)
        printf("[RxPacketError] Overload error!\n");

    if (error & ERRBIT_INSTRUCTION)
        printf("[RxPacketError] Instruction code error!\n");
}

int GetLastTxRxResult1(int port_num)
{
    return packetData[port_num].communication_result_;
}
UINT8_T GetLastRxPacketError1(int port_num)
{
    return packetData[port_num].error_;
}

void SetDataWrite1(int port_num, UINT16_T data_length, UINT16_T data_pos, UINT32_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, (data_pos + data_length) * sizeof(UINT8_T));

    switch (data_length)
    {
    case 1:
        packetData[port_num].data_write_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        break;

    case 2:
        packetData[port_num].data_write_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        packetData[port_num].data_write_[data_pos + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        break;

    default:
        printf("[Set Data for Write] failed");
        break;
    }
}

UINT32_T GetDataRead1(int port_num, UINT16_T data_length, UINT16_T data_pos)
{
    switch (data_length)
    {
    case 1:
        return packetData[port_num].data_read_[data_pos + 0];

    case 2:
        return DXL_MAKEWORD(packetData[port_num].data_read_[data_pos + 0], packetData[port_num].data_read_[data_pos + 1]);

    default:
        printf("[Set Data Read] failed... ");
        return 0;
    }
}

void TxPacket1(int port_num)
{
    int _idx;

    UINT8_T _checksum = 0;
    UINT8_T _total_packet_length = packetData[port_num].txpacket_[PKT_LENGTH] + 4; // 4: HEADER0 HEADER1 ID LENGTH
    UINT8_T _written_packet_length = 0;

    if (is_using_[port_num])
    {
        packetData[port_num].communication_result_ = COMM_PORT_BUSY;
        return ;
    }
    is_using_[port_num] = true;

    // check max packet length
    if (_total_packet_length > TXPACKET_MAX_LEN)
    {
        is_using_[port_num] = false;
        packetData[port_num].communication_result_ = COMM_TX_ERROR;
        return;
    }

    // make packet header
    packetData[port_num].txpacket_[PKT_HEADER0] = 0xFF;
    packetData[port_num].txpacket_[PKT_HEADER1] = 0xFF;

    // add a checksum to the packet
    for (_idx = 2; _idx < _total_packet_length - 1; _idx++)   // except header, checksum
        _checksum += packetData[port_num].txpacket_[_idx];
    packetData[port_num].txpacket_[_total_packet_length - 1] = ~_checksum;

    // tx packet
    ClearPort(port_num);
    _written_packet_length = WritePort(port_num, packetData[port_num].txpacket_, _total_packet_length);
    if (_total_packet_length != _written_packet_length)
    {
        is_using_[port_num] = false;
        packetData[port_num].communication_result_ = COMM_TX_FAIL;
        return;
    }

    packetData[port_num].communication_result_ = COMM_SUCCESS;
}

void RxPacket1(int port_num)
{
    UINT8_T _idx, _s;
    int _i;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    UINT8_T _checksum = 0;
    UINT8_T _rx_length = 0;
    UINT8_T _wait_length = 6;    // minimum length ( HEADER0 HEADER1 ID LENGTH ERROR CHKSUM )

    while (true)
    {
        _rx_length += ReadPort(port_num, &packetData[port_num].rxpacket_[_rx_length], _wait_length - _rx_length);
        if (_rx_length >= _wait_length)
        {
            _idx = 0;

            // find packet header
            for (_idx = 0; _idx < (_rx_length - 1); _idx++)
            {
                if (packetData[port_num].rxpacket_[_idx] == 0xFF && packetData[port_num].rxpacket_[_idx + 1] == 0xFF)
                    break;
            }

            if (_idx == 0)   // found at the beginning of the packet
            {
                if (packetData[port_num].rxpacket_[PKT_ID] > 0xFD ||                   // unavailable ID
                    packetData[port_num].rxpacket_[PKT_LENGTH] > RXPACKET_MAX_LEN ||   // unavailable Length
                    packetData[port_num].rxpacket_[PKT_ERROR] >= 0x64)                 // unavailable Error
                {
                    // remove the first byte in the packet
                    for (_s = 0; _s < _rx_length - 1; _s++)
                        packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[1 + _s];

                    _rx_length -= 1;
                    continue;
                }

                // re-calculate the exact length of the rx packet
                _wait_length = packetData[port_num].rxpacket_[PKT_LENGTH] + PKT_LENGTH + 1;
                if (_rx_length < _wait_length)
                {
                    // check timeout
                    if (IsPacketTimeout(port_num) == true)
                    {
                        if (_rx_length == 0)
                            packetData[port_num].communication_result_ = COMM_RX_TIMEOUT;
                        else
                            packetData[port_num].communication_result_ = COMM_RX_CORRUPT;
                        break;
                    }
                    else
                        continue;
                }

                // calculate checksum
                for (_i = 2; _i < _wait_length - 1; _i++)   // except header, checksum
                    _checksum += packetData[port_num].rxpacket_[_i];
                _checksum = ~_checksum;

                // verify checksum
                if (packetData[port_num].rxpacket_[_wait_length - 1] == _checksum)
                    packetData[port_num].communication_result_ = COMM_SUCCESS;
                else
                    packetData[port_num].communication_result_ = COMM_RX_CORRUPT;
                break;
            }
            else
            {
                // remove unnecessary packets
                for (_s = 0; _s < _rx_length - _idx; _s++)
                    packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[_idx + _s];
                _rx_length -= _idx;
            }
        }
        else
        {
            // check timeout
            if (IsPacketTimeout(port_num) == true)
            {
                if (_rx_length == 0)
                    packetData[port_num].communication_result_ = COMM_RX_TIMEOUT;
                else
                    packetData[port_num].communication_result_ = COMM_RX_CORRUPT;
                break;
            }
        }
    }
    is_using_[port_num] = false;
}

// NOT for BulkRead instruction
void TxRxPacket1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    // tx packet
    TxPacket1(port_num);

    if (packetData[port_num].communication_result_ != COMM_SUCCESS)
        return;

    // (ID == Broadcast ID && NOT BulkRead) == no need to wait for status packet
    // (Instruction == Action) == no need to wait for status packet
    if ((packetData[port_num].txpacket_[PKT_ID] == BROADCAST_ID && packetData[port_num].txpacket_[PKT_INSTRUCTION] != INST_BULK_READ) ||
        (packetData[port_num].txpacket_[PKT_INSTRUCTION] == INST_ACTION))
    {
        is_using_[port_num] = false;
        return;
    }

    // set packet timeout
    if (packetData[port_num].txpacket_[PKT_INSTRUCTION] == INST_READ)
        SetPacketTimeout(port_num, (UINT16_T)(packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] + 6));
    else
        SetPacketTimeout(port_num, (UINT16_T)6);

    // rx packet
    RxPacket1(port_num);
    // check txpacket ID == rxpacket ID
    if (packetData[port_num].txpacket_[PKT_ID] != packetData[port_num].rxpacket_[PKT_ID])
        RxPacket1(port_num);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS && packetData[port_num].txpacket_[PKT_ID] != BROADCAST_ID)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
    }
}

void Ping1(int port_num, UINT8_T id)
{
    PingGetModelNum1(port_num, id);
}

UINT16_T PingGetModelNum1(int port_num, UINT8_T id)
{
	packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 2 * sizeof(UINT8_T));
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(6);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(6);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return 0;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_PING;

    TxRxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        ReadTxRx1(port_num, id, 0, 2);  // Address 0 : Model Number
        if (packetData[port_num].communication_result_ == COMM_SUCCESS)
            return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    }

    return 0;
}

void BroadcastPing1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

bool GetBroadcastPingResult1(int port_num, int id)
{
    return false;
}

void Action1(int port_num, UINT8_T id)
{
    packetData[port_num].txpacket_ = (UINT8_T *)malloc(6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_ACTION;

    TxRxPacket1(port_num);
}

void Reboot1(int port_num, UINT8_T id)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void FactoryReset1(int port_num, UINT8_T id, UINT8_T option)
{
    packetData[port_num].txpacket_ = (UINT8_T *)malloc(6);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_FACTORY_RESET;

    TxRxPacket1(port_num);
}

void ReadTx1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(8);

    if (id >= BROADCAST_ID) 
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 4;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)length;

    TxPacket1(port_num);

    // set packet timeout
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        SetPacketTimeout(port_num, (UINT16_T)(length + 6));
}

void ReadRx1(int port_num, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(RXPACKET_MAX_LEN);

    RxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + _s];
    }

    free(packetData[port_num].rxpacket_);
}

void ReadTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(8);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(RXPACKET_MAX_LEN);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 4;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)length;

    TxRxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + _s];
    }

    free(packetData[port_num].rxpacket_);
}

void Read1ByteTx1(int port_num, UINT8_T id, UINT16_T address)
{
    ReadTx1(port_num, id, address, 1);
}
UINT8_T Read1ByteRx1(int port_num)
{
	packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    ReadRx1(port_num, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}
UINT8_T Read1ByteTxRx1(int port_num, UINT8_T id, UINT16_T address)
{
	packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    ReadTxRx1(port_num, id, address, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}

void Read2ByteTx1(int port_num, UINT8_T id, UINT16_T address)
{
    ReadTx1(port_num, id, address, 2);
}
UINT16_T Read2ByteRx1(int port_num)
{
	packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    ReadRx1(port_num, 2);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    return 0;
}
UINT16_T Read2ByteTxRx1(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 2 * sizeof(UINT8_T));
	packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    ReadTxRx1(port_num, id, address, 2);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);

    return 0;
}

void Read4ByteTx1(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}
UINT32_T Read4ByteRx1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return 0;
}
UINT32_T Read4ByteTxRx1(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return 0;
}

void WriteTxOnly1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 7);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (UINT8_T)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    TxPacket1(port_num);
    is_using_[port_num] = false;

    free(packetData[port_num].txpacket_);
}

void WriteTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 6);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (UINT8_T)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    TxRxPacket1(port_num);

    free(packetData[port_num].txpacket_);
}

void Write1ByteTxOnly1(int port_num, UINT8_T id, UINT16_T address, UINT8_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = data;
    WriteTxOnly1(port_num, id, address, 1);
}
void Write1ByteTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT8_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = data;
    WriteTxRx1(port_num, id, address, 1);
}

void Write2ByteTxOnly1(int port_num, UINT8_T id, UINT16_T address, UINT16_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    WriteTxOnly1(port_num, id, address, 2);
}
void Write2ByteTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT16_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    WriteTxRx1(port_num, id, address, 2);
}

void Write4ByteTxOnly1(int port_num, UINT8_T id, UINT16_T address, UINT32_T data)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}
void Write4ByteTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT32_T data)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void RegWriteTxOnly1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (UINT8_T)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

     TxPacket1(port_num);
    is_using_[port_num] = false;

    free(packetData[port_num].txpacket_);
}

void RegWriteTxRx1(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 6);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (UINT8_T)address;

    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, length * sizeof(UINT8_T));

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    TxRxPacket1(port_num);

    free(packetData[port_num].txpacket_);
}

void SyncReadTx1(int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void SyncWriteTxOnly1(int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 8);    // 8: HEADER0 HEADER1 ID LEN INST START_ADDR DATA_LEN ... CHKSUM
                                                               
    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH] = param_length + 4; // 4: INST START_ADDR DATA_LEN ... CHKSUM
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_SYNC_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = start_address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = data_length;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];

    TxRxPacket1(port_num);

    free(packetData[port_num].txpacket_);
}

void BulkReadTx1(int port_num, UINT16_T param_length)
{
    UINT8_T _s;

    int _i;
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 7);    // 7: HEADER0 HEADER1 ID LEN INST 0x00 ... CHKSUM

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH] = param_length + 3; // 3: INST 0x00 ... CHKSUM
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_BULK_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = 0x00;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    TxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        int _wait_length = 0;
        for (_i = 0; _i < param_length; _i += 3)
            _wait_length += packetData[port_num].data_write_[_i] + 7;
        SetPacketTimeout(port_num, (UINT16_T)_wait_length);
    }

    free(packetData[port_num].txpacket_);
}

void BulkWriteTxOnly1(int port_num, UINT16_T param_length)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

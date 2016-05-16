/*
* Protocol2PacketHandler.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dynamixel_sdk/Protocol2PacketHandler.h"

#define TXPACKET_MAX_LEN    (4*1024)
#define RXPACKET_MAX_LEN    (4*1024)

///////////////// for Protocol 2.0 Packet /////////////////
#define PKT_HEADER0             0
#define PKT_HEADER1             1
#define PKT_HEADER2             2
#define PKT_RESERVED            3
#define PKT_ID                  4
#define PKT_LENGTH_L            5
#define PKT_LENGTH_H            6
#define PKT_INSTRUCTION         7
#define PKT_ERROR               8
#define PKT_PARAMETER0          8

///////////////// Protocol 2.0 Error bit /////////////////
#define ERRNUM_RESULT_FAIL      1       // Failed to process the instruction packet.
#define ERRNUM_INSTRUCTION      2       // Instruction error
#define ERRNUM_CRC              3       // CRC check error
#define ERRNUM_DATA_RANGE       4       // Data range error
#define ERRNUM_DATA_LENGTH      5       // Data length error
#define ERRNUM_DATA_LIMIT       6       // Data limit error
#define ERRNUM_ACCESS           7       // Access error

#define ERRBIT_ALERT            128     //When the device has a problem, this bit is set to 1. Check "Device Status Check" value.

void PrintTxRxResult2(int result)
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

void PrintRxPacketError2(UINT8_T error)
{
    if (error & ERRBIT_ALERT)
        printf("[RxPacketError] Hardware error occurred. Check the error at Control Table (Hardware Error Status)!\n");

    int _error = error & ~ERRBIT_ALERT;

    switch (_error)
    {
    case 0:
        break;

    case ERRNUM_RESULT_FAIL:
        printf("[RxPacketError] Failed to process the instruction packet!\n");
        break;

    case ERRNUM_INSTRUCTION:
        printf("[RxPacketError] Undefined instruction or incorrect instruction!\n");
        break;

    case ERRNUM_CRC:
        printf("[RxPacketError] CRC doesn't match!\n");
        break;

    case ERRNUM_DATA_RANGE:
        printf("[RxPacketError] The data value is out of range!\n");
        break;

    case ERRNUM_DATA_LENGTH:
        printf("[RxPacketError] The data length does not match as expected!\n");
        break;

    case ERRNUM_DATA_LIMIT:
        printf("[RxPacketError] The data value exceeds the limit value!\n");
        break;

    case ERRNUM_ACCESS:
        printf("[RxPacketError] Writing or Reading is not available to target address!\n");
        break;

    default:
        printf("[RxPacketError] Unknown error code!\n");
        break;
    }
}

int GetLastTxRxResult2(int port_num)
{
    return packetData[port_num].communication_result_;
}
UINT8_T GetLastRxPacketError2(int port_num)
{
    return packetData[port_num].error_;
}

void SetDataWrite2(int port_num, UINT16_T data_length, UINT16_T data_pos, UINT32_T data)
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

    case 4:
        packetData[port_num].data_write_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        packetData[port_num].data_write_[data_pos + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        packetData[port_num].data_write_[data_pos + 2] = DXL_LOBYTE(DXL_HIWORD(data));
        packetData[port_num].data_write_[data_pos + 3] = DXL_HIBYTE(DXL_HIWORD(data));
        break;

    default:
        printf("[Set Data Write] failed... ");
        break;
    }
}
UINT32_T GetDataRead2(int port_num, UINT16_T data_length, UINT16_T data_pos)
{
    switch (data_length)
    {
    case 1:
        return packetData[port_num].data_read_[data_pos + 0];

    case 2:
        return DXL_MAKEWORD(packetData[port_num].data_read_[data_pos + 0], packetData[port_num].data_read_[data_pos + 1]);

    case 4:
        return DXL_MAKEDWORD(DXL_MAKEWORD(packetData[port_num].data_read_[data_pos + 0], packetData[port_num].data_read_[data_pos + 1])
            , DXL_MAKEWORD(packetData[port_num].data_read_[data_pos + 2], packetData[port_num].data_read_[data_pos + 3]));

    default:
        printf("[Set Data Read] failed... ");
        return 0;
    }
}

unsigned short UpdateCRC(UINT16_T crc_accum, UINT8_T *data_blk_ptr, UINT16_T data_blk_size)
{
    UINT16_T i, j;
    UINT16_T crc_table[256] = { 0x0000,
        0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
        0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
        0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
        0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
        0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
        0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
        0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
        0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
        0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
        0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
        0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
        0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
        0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
        0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
        0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
        0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
        0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
        0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
        0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
        0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
        0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
        0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
        0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
        0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
        0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
        0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
        0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
        0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
        0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
        0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
        0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
        0x820D, 0x8207, 0x0202 };

    for (j = 0; j < data_blk_size; j++)
    {
        i = ((UINT16_T)(crc_accum >> 8) ^ *data_blk_ptr++) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

void AddStuffing(UINT8_T *packet)
{
    UINT8_T _s;

    int i = 0, index = 0;
    int packet_length_in = DXL_MAKEWORD(packet[PKT_LENGTH_L], packet[PKT_LENGTH_H]);
    int packet_length_out = packet_length_in;
    UINT8_T temp[TXPACKET_MAX_LEN] = { 0 };

    for (_s = PKT_HEADER0; _s <= PKT_LENGTH_H; _s++)
        temp[_s] = packet[_s]; // FF FF FD XX ID LEN_L LEN_H
                               //memcpy(temp, packet, PKT_LENGTH_H+1);
    index = PKT_INSTRUCTION;
    for (i = 0; i < packet_length_in - 2; i++)  // except CRC
    {
        temp[index++] = packet[i + PKT_INSTRUCTION];
        if (packet[i + PKT_INSTRUCTION] == 0xFD && packet[i + PKT_INSTRUCTION - 1] == 0xFF && packet[i + PKT_INSTRUCTION - 2] == 0xFF)
        {   // FF FF FD
            temp[index++] = 0xFD;
            packet_length_out++;
        }
    }
    temp[index++] = packet[PKT_INSTRUCTION + packet_length_in - 2];
    temp[index++] = packet[PKT_INSTRUCTION + packet_length_in - 1];

    if (packet_length_in != packet_length_out)
        packet = (UINT8_T *)realloc(packet, index * sizeof(UINT8_T));

    for (_s = 0; _s < index; _s++)
        packet[_s] = temp[_s];
    //memcpy(packet, temp, index);
    packet[PKT_LENGTH_L] = DXL_LOBYTE(packet_length_out);
    packet[PKT_LENGTH_H] = DXL_HIBYTE(packet_length_out);
}

void RemoveStuffing(UINT8_T *packet)
{
    int i = 0, index = 0;
    int packet_length_in = DXL_MAKEWORD(packet[PKT_LENGTH_L], packet[PKT_LENGTH_H]);
    int packet_length_out = packet_length_in;

    index = PKT_INSTRUCTION;
    for (i = 0; i < packet_length_in - 2; i++)  // except CRC
    {
        if (packet[i + PKT_INSTRUCTION] == 0xFD && packet[i + PKT_INSTRUCTION + 1] == 0xFD && packet[i + PKT_INSTRUCTION - 1] == 0xFF && packet[i + PKT_INSTRUCTION - 2] == 0xFF)
        {   // FF FF FD FD
            packet_length_out--;
            i++;
        }
        packet[index++] = packet[i + PKT_INSTRUCTION];
    }
    packet[index++] = packet[PKT_INSTRUCTION + packet_length_in - 2];
    packet[index++] = packet[PKT_INSTRUCTION + packet_length_in - 1];

    packet[PKT_LENGTH_L] = DXL_LOBYTE(packet_length_out);
    packet[PKT_LENGTH_H] = DXL_HIBYTE(packet_length_out);
}

void TxPacket2(int port_num)
{
    UINT16_T _total_packet_length = 0;
    UINT16_T _written_packet_length = 0;

    if (is_using_[port_num])
    {
        packetData[port_num].communication_result_ = COMM_PORT_BUSY;
        return;
    }
    is_using_[port_num] = true;

    // byte stuffing for header
    AddStuffing(packetData[port_num].txpacket_);

    // check max packet length
    _total_packet_length = DXL_MAKEWORD(packetData[port_num].txpacket_[PKT_LENGTH_L], packetData[port_num].txpacket_[PKT_LENGTH_H]) + 7;
    // 7: HEADER0 HEADER1 HEADER2 RESERVED ID LENGTH_L LENGTH_H
    if (_total_packet_length > TXPACKET_MAX_LEN)
    {
        is_using_[port_num] = false;
        packetData[port_num].communication_result_ = COMM_TX_ERROR;
        return;
    }

    // make packet header
    packetData[port_num].txpacket_[PKT_HEADER0] = 0xFF;
    packetData[port_num].txpacket_[PKT_HEADER1] = 0xFF;
    packetData[port_num].txpacket_[PKT_HEADER2] = 0xFD;
    packetData[port_num].txpacket_[PKT_RESERVED] = 0x00;

    // add CRC16
    UINT16_T crc = UpdateCRC(0, packetData[port_num].txpacket_, _total_packet_length - 2);    // 2: CRC16
    packetData[port_num].txpacket_[_total_packet_length - 2] = DXL_LOBYTE(crc);
    packetData[port_num].txpacket_[_total_packet_length - 1] = DXL_HIBYTE(crc);

    //int _l;
    //for(_l = 0; _l < _total_packet_length; _l++)
    //printf(" %d ", packetData[port_num].txpacket_[_l]);

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

void RxPacket2(int port_num)
{
    UINT8_T _s;
    UINT16_T _idx;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    UINT16_T _rx_length = 0;
    UINT16_T _wait_length = 11;
    // minimum length ( HEADER0 HEADER1 HEADER2 RESERVED ID LENGTH_L LENGTH_H INST ERROR CRC16_L CRC16_H )

    while (true)
    {
        _rx_length += ReadPort(port_num, &packetData[port_num].rxpacket_[_rx_length], _wait_length - _rx_length);
        if (_rx_length >= _wait_length)
        {
            _idx = 0;

            // find packet header
            for (_idx = 0; _idx < (_rx_length - 3); _idx++)
            {
                if ((packetData[port_num].rxpacket_[_idx] == 0xFF) && (packetData[port_num].rxpacket_[_idx + 1] == 0xFF) && (packetData[port_num].rxpacket_[_idx + 2] == 0xFD) && (packetData[port_num].rxpacket_[_idx + 3] != 0xFD))
                    break;
            }

            if (_idx == 0)   // found at the beginning of the packet
            {
                if (packetData[port_num].rxpacket_[PKT_RESERVED] != 0x00 ||
                    packetData[port_num].rxpacket_[PKT_ID] > 0xFC ||
                    DXL_MAKEWORD(packetData[port_num].rxpacket_[PKT_LENGTH_L], packetData[port_num].rxpacket_[PKT_LENGTH_H]) > RXPACKET_MAX_LEN ||
                    packetData[port_num].rxpacket_[PKT_INSTRUCTION] != 0x55)
                {
                    // remove the first byte in the packet
                    for (_s = 0; _s < _rx_length - 1; _s++)
                        packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[1 + _s];
                    //memcpy(&rxpacket[0], &rxpacket[_idx], _rx_length - _idx);
                    _rx_length -= 1;
                    continue;
                }

                // re-calculate the exact length of the rx packet
                if (_wait_length != DXL_MAKEWORD(packetData[port_num].rxpacket_[PKT_LENGTH_L], packetData[port_num].rxpacket_[PKT_LENGTH_H]) + PKT_LENGTH_H + 1)
                {
                    _wait_length = DXL_MAKEWORD(packetData[port_num].rxpacket_[PKT_LENGTH_L], packetData[port_num].rxpacket_[PKT_LENGTH_H]) + PKT_LENGTH_H + 1;
                    continue;
                }

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

                // verify CRC16
                UINT16_T crc = DXL_MAKEWORD(packetData[port_num].rxpacket_[_wait_length - 2], packetData[port_num].rxpacket_[_wait_length - 1]);
                if (UpdateCRC(0, packetData[port_num].rxpacket_, _wait_length - 2) == crc)
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
                //memcpy(&rxpacket[0], &rxpacket[_idx], _rx_length - _idx);
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

    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        RemoveStuffing(packetData[port_num].rxpacket_);
}

// NOT for BulkRead / SyncRead instruction
void TxRxPacket2(int port_num)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    // tx packet
    TxPacket2(port_num);
    if (packetData[port_num].communication_result_ != COMM_SUCCESS)
        return;

    // (ID == Broadcast ID && NOT BulkRead) == no need to wait for status packet
    // (Instruction == Action) == no need to wait for status packet
    if ((packetData[port_num].txpacket_[PKT_ID] == BROADCAST_ID && packetData[port_num].txpacket_[PKT_INSTRUCTION] != INST_BULK_READ) ||
        (packetData[port_num].txpacket_[PKT_ID] == BROADCAST_ID && packetData[port_num].txpacket_[PKT_INSTRUCTION] != INST_SYNC_READ) ||
        (packetData[port_num].txpacket_[PKT_INSTRUCTION] == INST_ACTION))
    {
        is_using_[port_num] = false;
        return;
    }

    // set packet timeout
    if (packetData[port_num].txpacket_[PKT_INSTRUCTION] == INST_READ)
        SetPacketTimeout(port_num, (UINT16_T)(DXL_MAKEWORD(packetData[port_num].txpacket_[PKT_PARAMETER0 + 2], packetData[port_num].txpacket_[PKT_PARAMETER0 + 3]) + 11));
    else
        SetPacketTimeout(port_num, (UINT16_T)11);   // HEADER0 HEADER1 HEADER2 RESERVED ID LENGTH_L LENGTH_H INST ERROR CRC16_L CRC16_H

                                                    // rx packet
    RxPacket2(port_num);
    // check txpacket ID == rxpacket ID
    if (packetData[port_num].txpacket_[PKT_ID] != packetData[port_num].rxpacket_[PKT_ID])
        RxPacket2(port_num);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS && packetData[port_num].txpacket_[PKT_ID] != BROADCAST_ID)
    {
        packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
    }
}

void Ping2(int port_num, UINT8_T id)
{
    PingGetModelNum2(port_num, id);
}

UINT16_T PingGetModelNum2(int port_num, UINT8_T id)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(10);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(14);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return 0;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 3;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_PING;

    TxRxPacket2(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].rxpacket_[PKT_PARAMETER0 + 1], packetData[port_num].rxpacket_[PKT_PARAMETER0 + 2]);
    return 0;
}

void BroadcastPing2(int port_num)
{
    int _id;
    UINT16_T _idx;

    packetData[port_num].broadcastping_id_list = (UINT8_T *)calloc(255, sizeof(UINT8_T));

    const int STATUS_LENGTH     = 14;
    int _result = COMM_TX_FAIL;

    for (_id = 0; _id < 255; _id++)
        packetData[port_num].broadcastping_id_list[_id] = 255;
    //id_list.clear();

    UINT16_T _rx_length = 0;
    UINT16_T _wait_length = STATUS_LENGTH * MAX_ID;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(10 * sizeof(UINT8_T));
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(STATUS_LENGTH * MAX_ID * sizeof(UINT8_T));

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 3;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_PING;

    TxPacket2(port_num);
    if (packetData[port_num].communication_result_ != COMM_SUCCESS)
    {
        is_using_[port_num] = false;
        return;
    }

    // set rx timeout
    SetPacketTimeout(port_num, (UINT16_T)(_wait_length * 30));

    while (1)
    {
        _rx_length += ReadPort(port_num, &packetData[port_num].rxpacket_[_rx_length], _wait_length - _rx_length);
        if (IsPacketTimeout(port_num) == true)// || _rx_length >= _wait_length)
            break;
    }

    is_using_[port_num] = false;

    if (_rx_length == 0)
    {
        packetData[port_num].communication_result_ = COMM_RX_TIMEOUT;
        return;
    }

    while (1)
    {
        if (_rx_length < STATUS_LENGTH)
        {
            packetData[port_num].communication_result_ = COMM_RX_CORRUPT;
        }

        _idx = 0;

        // find packet header
        for (_idx = 0; _idx < (_rx_length - 2); _idx++)
        {
            if (packetData[port_num].rxpacket_[_idx] == 0xFF && packetData[port_num].rxpacket_[_idx + 1] == 0xFF && packetData[port_num].rxpacket_[_idx + 2] == 0xFD)
                break;
        }

        if (_idx == 0)   // found at the beginning of the packet
        {
            // verify CRC16
            UINT16_T crc = DXL_MAKEWORD(packetData[port_num].rxpacket_[STATUS_LENGTH - 2], packetData[port_num].rxpacket_[STATUS_LENGTH - 1]);

            if (UpdateCRC(0, packetData[port_num].rxpacket_, STATUS_LENGTH - 2) == crc)
            {
                _result = COMM_SUCCESS;

                packetData[port_num].broadcastping_id_list[packetData[port_num].rxpacket_[PKT_ID]] = packetData[port_num].rxpacket_[PKT_ID];

                for (UINT8_T _s = 0; _s < _rx_length - STATUS_LENGTH; _s++)
                    packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[STATUS_LENGTH + _s];
                _rx_length -= STATUS_LENGTH;

                if (_rx_length == 0)
                    return _result;
            }
            else
            {
                _result = COMM_RX_CORRUPT;

                // remove header (0xFF 0xFF 0xFD)
                for (UINT8_T _s = 0; _s < _rx_length - 3; _s++)
                    packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[3 + _s];
                _rx_length -= 3;
            }
        }
        else
        {
            // remove unnecessary packets
            for (UINT8_T _s = 0; _s < _rx_length - _idx; _s++)
                packetData[port_num].rxpacket_[_s] = packetData[port_num].rxpacket_[_idx + _s];
            _rx_length -= _idx;
        }
    }

    packetData[port_num].communication_result_ = _result;
    return;
}

bool GetBroadcastPingResult2(int port_num, int id)
{
    if (packetData[port_num].broadcastping_id_list[id] == id)
        return true;
    else
        return false;
}

void Action2(int port_num, UINT8_T id)
{
    packetData[port_num].txpacket_ = (UINT8_T *)malloc(10);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 3;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_ACTION;

    TxRxPacket2(port_num);
}

void Reboot2(int port_num, UINT8_T id)
{
    packetData[port_num].txpacket_ = (UINT8_T *)malloc(10);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(11);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 3;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REBOOT;

    TxRxPacket2(port_num);
}

void FactoryReset2(int port_num, UINT8_T id, UINT8_T option)
{
    packetData[port_num].txpacket_ = (UINT8_T *)malloc(11);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(11);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 4;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_FACTORY_RESET;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = option;

    TxRxPacket2(port_num);
}

void ReadTx2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(14);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 7;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 2] = (UINT8_T)DXL_LOBYTE(length);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 3] = (UINT8_T)DXL_HIBYTE(length);

    TxPacket2(port_num);

    // set packet timeout
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        SetPacketTimeout(port_num, (UINT16_T)(length + 11));
}

void ReadRx2(int port_num, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(RXPACKET_MAX_LEN);//(length + 11 + (length/3));  // (length/3): consider stuffing
                                                          
    RxPacket2(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + 1 + _s];
        //memcpy(data, &rxpacket[PKT_PARAMETER0+1], length);
    }

    free(packetData[port_num].rxpacket_);
}

void ReadTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(14);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(RXPACKET_MAX_LEN);//(length + 11 + (length/3));  // (length/3): consider stuffing

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = 7;
    packetData[port_num].txpacket_[PKT_LENGTH_H] = 0;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 2] = (UINT8_T)DXL_LOBYTE(length);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 3] = (UINT8_T)DXL_HIBYTE(length);

    TxRxPacket2(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (UINT8_T)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + 1 + _s];
        //memcpy(data, &rxpacket[PKT_PARAMETER0+1], length);
    }

    free(packetData[port_num].rxpacket_);
}

void Read1ByteTx2(int port_num, UINT8_T id, UINT16_T address)
{
    ReadTx2(port_num, id, address, 1);
}
UINT8_T Read1ByteRx2(int port_num)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    ReadRx2(port_num, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}
UINT8_T Read1ByteTxRx2(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    ReadTxRx2(port_num, id, address, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}

void Read2ByteTx2(int port_num, UINT8_T id, UINT16_T address)
{
    ReadTx2(port_num, id, address, 2);
}
UINT16_T Read2ByteRx2(int port_num)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    ReadRx2(port_num, 2);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    return 0;
}
UINT16_T Read2ByteTxRx2(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    ReadTxRx2(port_num, id, address, 2);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    return 0;
}

void Read4ByteTx2(int port_num, UINT8_T id, UINT16_T address)
{
    ReadTx2(port_num, id, address, 4);
}
UINT32_T Read4ByteRx2(int port_num)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 4 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    packetData[port_num].data_read_[2] = 0;
    packetData[port_num].data_read_[3] = 0;
    ReadRx2(port_num, 4);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEDWORD(DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]), DXL_MAKEWORD(packetData[port_num].data_read_[2], packetData[port_num].data_read_[3]));
    return 0;
}
UINT32_T Read4ByteTxRx2(int port_num, UINT8_T id, UINT16_T address)
{
    packetData[port_num].data_read_ = (UINT8_T *)realloc(packetData[port_num].data_read_, 4 * sizeof(UINT8_T));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    packetData[port_num].data_read_[2] = 0;
    packetData[port_num].data_read_[3] = 0;
    ReadTxRx2(port_num, id, address, 4);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEDWORD(DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]), DXL_MAKEWORD(packetData[port_num].data_read_[2], packetData[port_num].data_read_[3]));
    return 0;
}


void WriteTxOnly2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 12);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+2], packetData[port_num].data_write_, length);

    TxPacket2(port_num);
    is_using_[port_num] = false;

    free(packetData[port_num].txpacket_);
}

void WriteTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 12);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(11);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+2], packetData[port_num].data_write_, length);

    TxRxPacket2(port_num);

    free(packetData[port_num].txpacket_);
}

void Write1ByteTxOnly2(int port_num, UINT8_T id, UINT16_T address, UINT8_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = data;
    WriteTxOnly2(port_num, id, address, 1);
}
void Write1ByteTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT8_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 1 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = data;
    WriteTxRx2(port_num, id, address, 1);
}

void Write2ByteTxOnly2(int port_num, UINT8_T id, UINT16_T address, UINT16_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    WriteTxOnly2(port_num, id, address, 2);
}
void Write2ByteTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT16_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 2 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    WriteTxRx2(port_num, id, address, 2);
}

void Write4ByteTxOnly2(int port_num, UINT8_T id, UINT16_T address, UINT32_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 4 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(DXL_LOWORD(data));
    packetData[port_num].data_write_[1] = DXL_HIBYTE(DXL_LOWORD(data));
    packetData[port_num].data_write_[2] = DXL_LOBYTE(DXL_HIWORD(data));
    packetData[port_num].data_write_[3] = DXL_HIBYTE(DXL_HIWORD(data));
    WriteTxOnly2(port_num, id, address, 4);
}
void Write4ByteTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT32_T data)
{
    packetData[port_num].data_write_ = (UINT8_T *)realloc(packetData[port_num].data_write_, 4 * sizeof(UINT8_T));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(DXL_LOWORD(data));
    packetData[port_num].data_write_[1] = DXL_HIBYTE(DXL_LOWORD(data));
    packetData[port_num].data_write_[2] = DXL_LOBYTE(DXL_HIWORD(data));
    packetData[port_num].data_write_[3] = DXL_HIBYTE(DXL_HIWORD(data));
    WriteTxRx2(port_num, id, address, 4);
}

void RegWriteTxOnly2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 12);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+2], packetData[port_num].data_write_, length);

    TxPacket2(port_num);
    is_using_[port_num] = false;

    free(packetData[port_num].txpacket_);
}

void RegWriteTxRx2(int port_num, UINT8_T id, UINT16_T address, UINT16_T length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(length + 12);
    packetData[port_num].rxpacket_ = (UINT8_T *)malloc(11);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(length + 5);
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (UINT8_T)DXL_LOBYTE(address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (UINT8_T)DXL_HIBYTE(address);

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+2], packetData[port_num].data_write_, length);

    TxRxPacket2(port_num);

    free(packetData[port_num].txpacket_);
}

void SyncReadTx2(int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 14);
    // 14: HEADER0 HEADER1 HEADER2 RESERVED ID LEN_L LEN_H INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(param_length + 7); // 7: INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(param_length + 7); // 7: INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_SYNC_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = DXL_LOBYTE(start_address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = DXL_HIBYTE(start_address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 2] = DXL_LOBYTE(data_length);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 3] = DXL_HIBYTE(data_length);

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 4 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+4], packetData[port_num].data_write_, param_length);

    TxPacket2(port_num);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        SetPacketTimeout(port_num, (UINT16_T)((11 + data_length) * param_length));

    free(packetData[port_num].txpacket_);
}

void SyncWriteTxOnly2(int port_num, UINT16_T start_address, UINT16_T data_length, UINT16_T param_length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 14);
    // 14: HEADER0 HEADER1 HEADER2 RESERVED ID LEN_L LEN_H INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(param_length + 7); // 7: INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(param_length + 7); // 7: INST START_ADDR_L START_ADDR_H DATA_LEN_L DATA_LEN_H CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_SYNC_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = DXL_LOBYTE(start_address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = DXL_HIBYTE(start_address);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 2] = DXL_LOBYTE(data_length);
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 3] = DXL_HIBYTE(data_length);

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 4 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0+4], packetData[port_num].data_write_, param_length);

    TxRxPacket2(port_num);

    free(packetData[port_num].txpacket_);
}

void BulkReadTx2(int port_num, UINT16_T param_length)
{
    UINT8_T _s;
    int _i;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 10);
    // 10: HEADER0 HEADER1 HEADER2 RESERVED ID LEN_L LEN_H INST CRC16_L CRC16_H

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(param_length + 3); // 3: INST CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(param_length + 3); // 3: INST CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_BULK_READ;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0], packetData[port_num].data_write_, param_length);

    TxPacket2(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        int _wait_length = 0;
        for (_i = 0; _i < param_length; _i += 5)
            _wait_length += DXL_MAKEWORD(packetData[port_num].data_write_[_i + 3], packetData[port_num].data_write_[_i + 4]) + 10;
        SetPacketTimeout(port_num, (UINT16_T)_wait_length);
    }

    free(packetData[port_num].txpacket_);
}

void BulkWriteTxOnly2(int port_num, UINT16_T param_length)
{
    UINT8_T _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (UINT8_T *)malloc(param_length + 10);
    // 10: HEADER0 HEADER1 HEADER2 RESERVED ID LEN_L LEN_H INST CRC16_L CRC16_H

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH_L] = DXL_LOBYTE(param_length + 3); // 3: INST CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_LENGTH_H] = DXL_HIBYTE(param_length + 3); // 3: INST CRC16_L CRC16_H
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_BULK_WRITE;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + _s] = packetData[port_num].data_write_[_s];
    //memcpy(&packetData[port_num].txpacket_[PKT_PARAMETER0], packetData[port_num].data_write_, param_length);

    TxRxPacket2(port_num);

    free(packetData[port_num].txpacket_);
}

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
* protocol1_packet_handler.c
*
*  Created on: 2016. 5. 4.
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <string.h>
#include <stdlib.h>
#include "dynamixel_sdk/protocol1_packet_handler.h"

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


void printTxRxResult1(int result)
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

void printRxPacketError1(uint8_t error)
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

int getLastTxRxResult1(int port_num)
{
    return packetData[port_num].communication_result_;
}
uint8_t getLastRxPacketError1(int port_num)
{
    return packetData[port_num].error_;
}

void setDataWrite1(int port_num, uint16_t data_length, uint16_t data_pos, uint32_t data)
{
    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, (data_pos + data_length) * sizeof(uint8_t));

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

uint32_t getDataRead1(int port_num, uint16_t data_length, uint16_t data_pos)
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

void txPacket1(int port_num)
{
    int _idx;

    uint8_t _checksum = 0;
    uint8_t _total_packet_length = packetData[port_num].txpacket_[PKT_LENGTH] + 4; // 4: HEADER0 HEADER1 ID LENGTH
    uint8_t _written_packet_length = 0;

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
    clearPort(port_num);
    _written_packet_length = writePort(port_num, packetData[port_num].txpacket_, _total_packet_length);
    if (_total_packet_length != _written_packet_length)
    {
        is_using_[port_num] = false;
        packetData[port_num].communication_result_ = COMM_TX_FAIL;
        return;
    }

    packetData[port_num].communication_result_ = COMM_SUCCESS;
}

void rxPacket1(int port_num)
{
    uint8_t _idx, _s;
    int _i;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    uint8_t _checksum = 0;
    uint8_t _rx_length = 0;
    uint8_t _wait_length = 6;    // minimum length ( HEADER0 HEADER1 ID LENGTH ERROR CHKSUM )

    while (true)
    {
        _rx_length += readPort(port_num, &packetData[port_num].rxpacket_[_rx_length], _wait_length - _rx_length);
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
                    if (isPacketTimeout(port_num) == true)
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
            if (isPacketTimeout(port_num) == true)
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
void txRxPacket1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    // tx packet
    txPacket1(port_num);

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
        setPacketTimeout(port_num, (uint16_t)(packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] + 6));
    else
        setPacketTimeout(port_num, (uint16_t)6);

    // rx packet
    rxPacket1(port_num);
    // check txpacket ID == rxpacket ID
    if (packetData[port_num].txpacket_[PKT_ID] != packetData[port_num].rxpacket_[PKT_ID])
        rxPacket1(port_num);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS && packetData[port_num].txpacket_[PKT_ID] != BROADCAST_ID)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (uint8_t)packetData[port_num].rxpacket_[PKT_ERROR];
    }
}

void ping1(int port_num, uint8_t id)
{
    pingGetModelNum1(port_num, id);
}

uint16_t pingGetModelNum1(int port_num, uint8_t id)
{
	packetData[port_num].data_read_ = (uint8_t *)realloc(packetData[port_num].data_read_, 2 * sizeof(uint8_t));
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, 6);
    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, 6);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return 0;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_PING;

    txRxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        readTxRx1(port_num, id, 0, 2);  // Address 0 : Model Number
        if (packetData[port_num].communication_result_ == COMM_SUCCESS)
            return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    }

    return 0;
}

void broadcastPing1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

bool getBroadcastPingResult1(int port_num, int id)
{
    return false;
}

void action1(int port_num, uint8_t id)
{
    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_ACTION;

    txRxPacket1(port_num);
}

void reboot1(int port_num, uint8_t id)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void factoryReset1(int port_num, uint8_t id, uint8_t option)
{
    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, 6);
    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 2;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_FACTORY_RESET;

    txRxPacket1(port_num);
}

void readTx1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, 8);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 4;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (uint8_t)address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (uint8_t)length;

    txPacket1(port_num);

    // set packet timeout
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        setPacketTimeout(port_num, (uint16_t)(length + 6));
}

void readRx1(int port_num, uint16_t length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, RXPACKET_MAX_LEN);

    rxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (uint8_t)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + _s];
    }
}

void readTxRx1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    uint8_t _s;
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, 8);
    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, RXPACKET_MAX_LEN);

    if (id >= BROADCAST_ID)
    {
        packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = 4;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = (uint8_t)address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = (uint8_t)length;

    txRxPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        if (packetData[port_num].error_ != 0)
            packetData[port_num].error_ = (uint8_t)packetData[port_num].rxpacket_[PKT_ERROR];
        for (_s = 0; _s < length; _s++)
            packetData[port_num].data_read_[_s] = packetData[port_num].rxpacket_[PKT_PARAMETER0 + _s];
    }
}

void read1ByteTx1(int port_num, uint8_t id, uint16_t address)
{
    readTx1(port_num, id, address, 1);
}
uint8_t read1ByteRx1(int port_num)
{
	packetData[port_num].data_read_ = (uint8_t *)realloc(packetData[port_num].data_read_, 1 * sizeof(uint8_t));
    packetData[port_num].data_read_[0] = 0;
    readRx1(port_num, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}
uint8_t read1ByteTxRx1(int port_num, uint8_t id, uint16_t address)
{
	packetData[port_num].data_read_ = (uint8_t *)realloc(packetData[port_num].data_read_, 1 * sizeof(uint8_t));
    packetData[port_num].data_read_[0] = 0;
    readTxRx1(port_num, id, address, 1);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return packetData[port_num].data_read_[0];
    return 0;
}

void read2ByteTx1(int port_num, uint8_t id, uint16_t address)
{
    readTx1(port_num, id, address, 2);
}
uint16_t read2ByteRx1(int port_num)
{
	packetData[port_num].data_read_ = (uint8_t *)realloc(packetData[port_num].data_read_, 2 * sizeof(uint8_t));
    packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    readRx1(port_num, 2);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);
    return 0;
}
uint16_t read2ByteTxRx1(int port_num, uint8_t id, uint16_t address)
{
    packetData[port_num].data_read_ = (uint8_t *)realloc(packetData[port_num].data_read_, 2 * sizeof(uint8_t));
	packetData[port_num].data_read_[0] = 0;
    packetData[port_num].data_read_[1] = 0;
    readTxRx1(port_num, id, address, 2);

    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
        return DXL_MAKEWORD(packetData[port_num].data_read_[0], packetData[port_num].data_read_[1]);

    return 0;
}

void read4ByteTx1(int port_num, uint8_t id, uint16_t address)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}
uint32_t read4ByteRx1(int port_num)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return 0;
}
uint32_t read4ByteTxRx1(int port_num, uint8_t id, uint16_t address)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return 0;
}

void writeTxOnly1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, length + 7);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (uint8_t)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    txPacket1(port_num);
    is_using_[port_num] = false;

}

void writeTxRx1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, length + 7);
    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (uint8_t)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    txRxPacket1(port_num);
}

void write1ByteTxOnly1(int port_num, uint8_t id, uint16_t address, uint8_t data)
{
    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, 1 * sizeof(uint8_t));
    packetData[port_num].data_write_[0] = data;
    writeTxOnly1(port_num, id, address, 1);
}
void write1ByteTxRx1(int port_num, uint8_t id, uint16_t address, uint8_t data)
{
    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, 1 * sizeof(uint8_t));
    packetData[port_num].data_write_[0] = data;
    writeTxRx1(port_num, id, address, 1);
}

void write2ByteTxOnly1(int port_num, uint8_t id, uint16_t address, uint16_t data)
{
    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, 2 * sizeof(uint8_t));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    writeTxOnly1(port_num, id, address, 2);
}
void write2ByteTxRx1(int port_num, uint8_t id, uint16_t address, uint16_t data)
{
    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, 2 * sizeof(uint8_t));
    packetData[port_num].data_write_[0] = DXL_LOBYTE(data);
    packetData[port_num].data_write_[1] = DXL_HIBYTE(data);
    writeTxRx1(port_num, id, address, 2);
}

void write4ByteTxOnly1(int port_num, uint8_t id, uint16_t address, uint32_t data)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}
void write4ByteTxRx1(int port_num, uint8_t id, uint16_t address, uint32_t data)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void regWriteTxOnly1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, length + 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (uint8_t)address;

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

     txPacket1(port_num);
    is_using_[port_num] = false;
}

void regWriteTxRx1(int port_num, uint8_t id, uint16_t address, uint16_t length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, length + 6);
    packetData[port_num].rxpacket_ = (uint8_t *)realloc(packetData[port_num].rxpacket_, 6);

    packetData[port_num].txpacket_[PKT_ID] = id;
    packetData[port_num].txpacket_[PKT_LENGTH] = length + 3;
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_REG_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0] = (uint8_t)address;

    packetData[port_num].data_write_ = (uint8_t *)realloc(packetData[port_num].data_write_, length * sizeof(uint8_t));

    for (_s = 0; _s < length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    txRxPacket1(port_num);
}

void syncReadTx1(int port_num, uint16_t start_address, uint16_t data_length, uint16_t param_length)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

void syncWriteTxOnly1(int port_num, uint16_t start_address, uint16_t data_length, uint16_t param_length)
{
    uint8_t _s;

    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, param_length + 8); // 8: HEADER0 HEADER1 ID LEN INST START_ADDR DATA_LEN ... CHKSUM

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH] = param_length + 4; // 4: INST START_ADDR DATA_LEN ... CHKSUM
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_SYNC_WRITE;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = start_address;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 1] = data_length;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 2 + _s] = packetData[port_num].data_write_[_s];

    txRxPacket1(port_num);
}

void bulkReadTx1(int port_num, uint16_t param_length)
{
    uint8_t _s;

    int _i;
    packetData[port_num].communication_result_ = COMM_TX_FAIL;

    packetData[port_num].txpacket_ = (uint8_t *)realloc(packetData[port_num].txpacket_, param_length + 7);  // 7: HEADER0 HEADER1 ID LEN INST 0x00 ... CHKSUM

    packetData[port_num].txpacket_[PKT_ID] = BROADCAST_ID;
    packetData[port_num].txpacket_[PKT_LENGTH] = param_length + 3; // 3: INST 0x00 ... CHKSUM
    packetData[port_num].txpacket_[PKT_INSTRUCTION] = INST_BULK_READ;
    packetData[port_num].txpacket_[PKT_PARAMETER0 + 0] = 0x00;

    for (_s = 0; _s < param_length; _s++)
        packetData[port_num].txpacket_[PKT_PARAMETER0 + 1 + _s] = packetData[port_num].data_write_[_s];

    txPacket1(port_num);
    if (packetData[port_num].communication_result_ == COMM_SUCCESS)
    {
        int _wait_length = 0;
        for (_i = 0; _i < param_length; _i += 3)
            _wait_length += packetData[port_num].data_write_[_i] + 7;
        setPacketTimeout(port_num, (uint16_t)_wait_length);
    }
}

void bulkWriteTxOnly1(int port_num, uint16_t param_length)
{
    packetData[port_num].communication_result_ = COMM_NOT_AVAILABLE;
}

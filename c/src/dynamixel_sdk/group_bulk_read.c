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
* group_bulk_read.c
*
*  Created on: 2016. 5. 4.
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdio.h>
#include <stdlib.h>
#include "dynamixel_sdk/group_bulk_read.h"

#define NOT_USED_ID         255

typedef struct
{
    uint8_t     id_;
    uint16_t    start_address_;
    uint16_t    data_length_;
    uint8_t     *data_;
}DataListBulkRead;

typedef struct
{
    int         port_num;
    int         protocol_version;

    int         data_list_length_;

    bool        last_result_;
    bool        is_param_changed_;

    DataListBulkRead   *data_list_;
}GroupDataBulkRead;

GroupDataBulkRead *groupDataBulkRead;

int used_group_num_bulkread_ = 0;

int groupBulkReadSize(int group_num)
{
    int _data_num;
    int size = 0;

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
            size++;
    return size;
}

int groupBulkReadFind(int group_num, int id)
{
    int _data_num;

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == id)
            break;

    return _data_num;
}

int groupBulkRead(int port_num, int protocol_version)
{
    int _group_num = 0;

    if (used_group_num_bulkread_ != 0)
        for (_group_num = 0; _group_num < used_group_num_bulkread_; _group_num++)
            if (groupDataBulkRead[_group_num].is_param_changed_ != true)
                break;

    if (_group_num == used_group_num_bulkread_)
    {
        used_group_num_bulkread_++;
        groupDataBulkRead = (GroupDataBulkRead *)realloc(groupDataBulkRead, used_group_num_bulkread_ * sizeof(GroupDataBulkRead));
    }

    groupDataBulkRead[_group_num].port_num = port_num;
    groupDataBulkRead[_group_num].protocol_version = protocol_version;
    groupDataBulkRead[_group_num].data_list_length_ = 0;
    groupDataBulkRead[_group_num].last_result_ = false;
    groupDataBulkRead[_group_num].is_param_changed_ = false;
    groupDataBulkRead[_group_num].data_list_ = 0;

    groupBulkReadClearParam(_group_num);

    return _group_num;
}

void groupBulkReadMakeParam(int group_num)
{
    int _data_num, _idx;
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (groupBulkReadSize(group_num) == 0)
        return;

    if (groupDataBulkRead[group_num].protocol_version == 1)
        packetData[_port_num].data_write_ = (uint8_t*)realloc(packetData[_port_num].data_write_, groupBulkReadSize(group_num) * sizeof(uint8_t) * 3); // ID(1) + ADDR(1) + LENGTH(1)
    else    // 2.0
        packetData[_port_num].data_write_ = (uint8_t*)realloc(packetData[_port_num].data_write_, groupBulkReadSize(group_num) * sizeof(uint8_t) * 5); // ID(1) + ADDR(2) + LENGTH(2)

    _idx = 0;
    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        if (groupDataBulkRead[group_num].protocol_version == 1)
        {
            packetData[_port_num].data_write_[_idx++] = (uint8_t)groupDataBulkRead[group_num].data_list_[_data_num].data_length_;       // LEN
            packetData[_port_num].data_write_[_idx++] = groupDataBulkRead[group_num].data_list_[_data_num].id_;                         // ID
            packetData[_port_num].data_write_[_idx++] = (uint8_t)groupDataBulkRead[group_num].data_list_[_data_num].start_address_;     // ADDR
        }
        else    // 2.0
        {
            packetData[_port_num].data_write_[_idx++] = groupDataBulkRead[group_num].data_list_[_data_num].id_;                         // ID
            packetData[_port_num].data_write_[_idx++] = DXL_LOBYTE(groupDataBulkRead[group_num].data_list_[_data_num].start_address_);  // ADDR_L
            packetData[_port_num].data_write_[_idx++] = DXL_HIBYTE(groupDataBulkRead[group_num].data_list_[_data_num].start_address_);  // ADDR_H
            packetData[_port_num].data_write_[_idx++] = DXL_LOBYTE(groupDataBulkRead[group_num].data_list_[_data_num].data_length_);    // LEN_L
            packetData[_port_num].data_write_[_idx++] = DXL_HIBYTE(groupDataBulkRead[group_num].data_list_[_data_num].data_length_);    // LEN_H
        }
    }
}

bool groupBulkReadAddParam(int group_num, uint8_t id, uint16_t start_address, uint16_t data_length)
{
    int _data_num = 0;

    if (id == NOT_USED_ID)
        return false;

    if (groupDataBulkRead[group_num].data_list_length_ != 0)
        _data_num = groupBulkReadFind(group_num, id);

    if (groupDataBulkRead[group_num].data_list_length_ == _data_num)
    {
        groupDataBulkRead[group_num].data_list_length_++;
        groupDataBulkRead[group_num].data_list_ = (DataListBulkRead *)realloc(groupDataBulkRead[group_num].data_list_, groupDataBulkRead[group_num].data_list_length_ * sizeof(DataListBulkRead));

        groupDataBulkRead[group_num].data_list_[_data_num].id_ = id;
        groupDataBulkRead[group_num].data_list_[_data_num].data_length_ = data_length;
        groupDataBulkRead[group_num].data_list_[_data_num].start_address_ = start_address;
        groupDataBulkRead[group_num].data_list_[_data_num].data_ = (uint8_t *)calloc(groupDataBulkRead[group_num].data_list_[_data_num].data_length_, sizeof(uint8_t));
    }

    groupDataBulkRead[group_num].is_param_changed_ = true;
    return true;
}

void groupBulkReadRemoveParam(int group_num, uint8_t id)
{
    int _data_num = groupBulkReadFind(group_num, id);

    if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
        return;

    groupDataBulkRead[group_num].data_list_[_data_num].data_ = 0;

    groupDataBulkRead[group_num].data_list_[_data_num].id_ = NOT_USED_ID;
    groupDataBulkRead[group_num].data_list_[_data_num].data_length_ = 0;
    groupDataBulkRead[group_num].data_list_[_data_num].start_address_ = 0;

    groupDataBulkRead[group_num].is_param_changed_ = true;
}

void groupBulkReadClearParam(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (groupBulkReadSize(group_num) == 0)
        return;

    groupDataBulkRead[group_num].data_list_ = 0;

    packetData[_port_num].data_write_ = 0;

    groupDataBulkRead[group_num].data_list_length_ = 0;
}

void groupBulkReadTxPacket(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (groupBulkReadSize(group_num) == 0)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    if (groupDataBulkRead[group_num].is_param_changed_ == true)
        groupBulkReadMakeParam(group_num);

    if (groupDataBulkRead[group_num].protocol_version == 1)
        bulkReadTx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, groupBulkReadSize(group_num) * 3);
    else
        bulkReadTx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, groupBulkReadSize(group_num) * 5);
}

void groupBulkReadRxPacket(int group_num)
{
    int _data_num, _c;
    int _port_num = groupDataBulkRead[group_num].port_num;

    packetData[_port_num].communication_result_ = COMM_RX_FAIL;

    groupDataBulkRead[group_num].last_result_ = false;

    if (groupBulkReadSize(group_num) == 0)
    {
        packetData[groupDataBulkRead[group_num].port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        packetData[_port_num].data_read_
            = (uint8_t *)realloc(packetData[_port_num].data_read_, groupDataBulkRead[group_num].data_list_[_data_num].data_length_ * sizeof(uint8_t));

        readRx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, groupDataBulkRead[group_num].data_list_[_data_num].data_length_);
        if (packetData[groupDataBulkRead[group_num].port_num].communication_result_ != COMM_SUCCESS)
            return;

        for (_c = 0; _c < groupDataBulkRead[group_num].data_list_[_data_num].data_length_; _c++)
            groupDataBulkRead[group_num].data_list_[_data_num].data_[_c] = packetData[_port_num].data_read_[_c];
    }

    if (packetData[_port_num].communication_result_ == COMM_SUCCESS)
        groupDataBulkRead[group_num].last_result_ = true;
}

void groupBulkReadTxRxPacket(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    packetData[_port_num].communication_result_ = COMM_TX_FAIL;

    groupBulkReadTxPacket(group_num);
    if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
        return;

    groupBulkReadRxPacket(group_num);
}

bool groupBulkReadIsAvailable(int group_num, uint8_t id, uint16_t address, uint16_t data_length)
{
    int _data_num = groupBulkReadFind(group_num, id);
    uint16_t _start_addr, _data_length;

    if (groupDataBulkRead[group_num].last_result_ == false || groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
        return false;

    _start_addr = groupDataBulkRead[group_num].data_list_[_data_num].start_address_;
    _data_length = groupDataBulkRead[group_num].data_list_[_data_num].data_length_;

    if (address < _start_addr || _start_addr + _data_length - data_length < address)
        return false;

    return true;
}

uint32_t groupBulkReadGetData(int group_num, uint8_t id, uint16_t address, uint16_t data_length)
{
    int _data_num = groupBulkReadFind(group_num, id);

    if (groupBulkReadIsAvailable(group_num, id, address, data_length) == false)
        return 0;

    uint16_t _start_addr = groupDataBulkRead[group_num].data_list_[_data_num].start_address_;

    switch (data_length)
    {
    case 1:
        return groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr];

    case 2:
        return DXL_MAKEWORD(groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr], groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr + 1]);

    case 4:
        return DXL_MAKEDWORD(DXL_MAKEWORD(groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr + 0], groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr + 1]),
            DXL_MAKEWORD(groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr + 2], groupDataBulkRead[group_num].data_list_[_data_num].data_[address - _start_addr + 3]));

    default:
        return 0;
    }
}

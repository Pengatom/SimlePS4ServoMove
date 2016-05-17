/*
* GroupBulkRead.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdio.h>
#include <stdlib.h>
#include "dynamixel_sdk/GroupBulkRead.h"

#define NOT_USED_ID         255

typedef struct
{
    UINT8_T     id_;
    UINT16_T    start_address_;
    UINT16_T    data_length_;
    UINT8_T     *data_;
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

int GroupBulkRead_Size(int group_num)
{
    int _data_num;
    int size = 0;

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
            size++;
    return size;
}

int GroupBulkRead_Find(int group_num, int id)
{
    int _data_num;

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == id)
            break;

    return _data_num;
}

int GroupBulkRead(int port_num, int protocol_version)
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

    GroupBulkRead_ClearParam(_group_num);

    return _group_num;
}

void GroupBulkRead_MakeParam(int group_num)
{
    int _data_num, _idx;
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (GroupBulkRead_Size(group_num) == 0)
        return;

    if (packetData[_port_num].data_write_ != 0)
        free(packetData[_port_num].data_write_);

    if (groupDataBulkRead[group_num].protocol_version == 1)
        packetData[_port_num].data_write_ = (UINT8_T*)malloc(GroupBulkRead_Size(group_num) * sizeof(UINT8_T) * 3); // ID(1) + ADDR(1) + LENGTH(1)
    else    // 2.0
        packetData[_port_num].data_write_ = (UINT8_T*)malloc(GroupBulkRead_Size(group_num) * sizeof(UINT8_T) * 5); // ID(1) + ADDR(2) + LENGTH(2)

    _idx = 0;
    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        if (groupDataBulkRead[group_num].protocol_version == 1)
        {
            packetData[_port_num].data_write_[_idx++] = (UINT8_T)groupDataBulkRead[group_num].data_list_[_data_num].data_length_;       // LEN
            packetData[_port_num].data_write_[_idx++] = groupDataBulkRead[group_num].data_list_[_data_num].id_;                         // ID
            packetData[_port_num].data_write_[_idx++] = (UINT8_T)groupDataBulkRead[group_num].data_list_[_data_num].start_address_;     // ADDR
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

bool GroupBulkRead_AddParam(int group_num, UINT8_T id, UINT16_T start_address, UINT16_T data_length)
{
    int _data_num = 0;

    if (id == NOT_USED_ID)
        return false;

    if (groupDataBulkRead[group_num].data_list_length_ != 0)
        _data_num = GroupBulkRead_Find(group_num, id);

    if (groupDataBulkRead[group_num].data_list_length_ == _data_num)
    {
        groupDataBulkRead[group_num].data_list_length_++;
        groupDataBulkRead[group_num].data_list_ = (DataListBulkRead *)realloc(groupDataBulkRead[group_num].data_list_, groupDataBulkRead[group_num].data_list_length_ * sizeof(DataListBulkRead));

        groupDataBulkRead[group_num].data_list_[_data_num].id_ = id;
        groupDataBulkRead[group_num].data_list_[_data_num].data_length_ = data_length;
        groupDataBulkRead[group_num].data_list_[_data_num].start_address_ = start_address;
        groupDataBulkRead[group_num].data_list_[_data_num].data_ = (UINT8_T *)calloc(groupDataBulkRead[group_num].data_list_[_data_num].data_length_, sizeof(UINT8_T));
    }

    groupDataBulkRead[group_num].is_param_changed_ = true;
    return true;
}

void GroupBulkRead_RemoveParam(int group_num, UINT8_T id)
{
    int _data_num = GroupBulkRead_Find(group_num, id);

    if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
        return;

    groupDataBulkRead[group_num].data_list_[_data_num].data_ = 0;
    free(groupDataBulkRead[group_num].data_list_[_data_num].data_);

    groupDataBulkRead[group_num].data_list_[_data_num].id_ = NOT_USED_ID;
    groupDataBulkRead[group_num].data_list_[_data_num].data_length_ = 0;
    groupDataBulkRead[group_num].data_list_[_data_num].start_address_ = 0;

    groupDataBulkRead[group_num].is_param_changed_ = true;
}

void GroupBulkRead_ClearParam(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (GroupBulkRead_Size(group_num) == 0)
        return;

    groupDataBulkRead[group_num].data_list_ = 0;
    free(groupDataBulkRead[group_num].data_list_);

    packetData[_port_num].data_write_ = 0;
    free(packetData[_port_num].data_write_);

    groupDataBulkRead[group_num].data_list_length_ = 0;
}

void GroupBulkRead_TxPacket(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    if (GroupBulkRead_Size(group_num) == 0)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    if (groupDataBulkRead[group_num].is_param_changed_ == true)
        GroupBulkRead_MakeParam(group_num);

    if (groupDataBulkRead[group_num].protocol_version == 1)
        BulkReadTx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, GroupBulkRead_Size(group_num) * 3);
    else
        BulkReadTx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, GroupBulkRead_Size(group_num) * 5);
}

void GroupBulkRead_RxPacket(int group_num)
{
    int _data_num, _c;
    int _port_num = groupDataBulkRead[group_num].port_num;
    
    packetData[_port_num].communication_result_ = COMM_RX_FAIL;

    groupDataBulkRead[group_num].last_result_ = false;

    if (GroupBulkRead_Size(group_num) == 0)
    {
        packetData[groupDataBulkRead[group_num].port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    for (_data_num = 0; _data_num < groupDataBulkRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        packetData[_port_num].data_read_
            = (UINT8_T *)realloc(packetData[_port_num].data_read_, groupDataBulkRead[group_num].data_list_[_data_num].data_length_ * sizeof(UINT8_T));

        ReadRx(groupDataBulkRead[group_num].port_num, groupDataBulkRead[group_num].protocol_version, groupDataBulkRead[group_num].data_list_[_data_num].data_length_);
        if (packetData[groupDataBulkRead[group_num].port_num].communication_result_ != COMM_SUCCESS)
            return;

        for (_c = 0; _c < groupDataBulkRead[group_num].data_list_[_data_num].data_length_; _c++)
            groupDataBulkRead[group_num].data_list_[_data_num].data_[_c] = packetData[_port_num].data_read_[_c];
    }

    if (packetData[_port_num].communication_result_ == COMM_SUCCESS)
        groupDataBulkRead[group_num].last_result_ = true;
}

void GroupBulkRead_TxRxPacket(int group_num)
{
    int _port_num = groupDataBulkRead[group_num].port_num;

    packetData[_port_num].communication_result_ = COMM_TX_FAIL;

    GroupBulkRead_TxPacket(group_num);
    if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
        return;

    GroupBulkRead_RxPacket(group_num);
}

bool GroupBulkRead_IsAvailable(int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length)
{
    int _data_num = GroupBulkRead_Find(group_num, id);
    UINT16_T _start_addr, _data_length;

    if (groupDataBulkRead[group_num].last_result_ == false || groupDataBulkRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
        return false;

    _start_addr = groupDataBulkRead[group_num].data_list_[_data_num].start_address_;
    _data_length = groupDataBulkRead[group_num].data_list_[_data_num].data_length_;

    if (address < _start_addr || _start_addr + _data_length - data_length < address)
        return false;

    return true;
}

UINT32_T GroupBulkRead_GetData(int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length)
{
    int _data_num = GroupBulkRead_Find(group_num, id);

    if (GroupBulkRead_IsAvailable(group_num, id, address, data_length) == false)
        return 0;

    UINT16_T _start_addr = groupDataBulkRead[group_num].data_list_[_data_num].start_address_;

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

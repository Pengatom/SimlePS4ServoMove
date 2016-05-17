/*
* GroupSyncRead.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdlib.h>
#include "dynamixel_sdk/GroupSyncRead.h"

#define NOT_USED_ID         255

typedef struct
{
    UINT8_T     id_;
    UINT8_T     *data_;
}DataListSyncRead;

typedef struct
{
    int         port_num;
    int         protocol_version;

    int         data_list_length_;

    bool        last_result_;
    bool        is_param_changed_;

    UINT16_T    start_address_;
    UINT16_T    data_length_;

    DataListSyncRead   *data_list_;
}GroupDataSyncRead;

GroupDataSyncRead *groupDataSyncRead;

int used_group_num_syncread = 0;

int GroupSyncRead_Find(int group_num, int id)
{
    int _data_num;

    for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
        if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == id)
            break;

    return _data_num;
}

int GroupSyncRead_Size(int group_num)
{
    int _data_num;
    int size = 0;

    for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
        if (groupDataSyncRead[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
            size++;
    return size;
};

int GroupSyncRead(int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length)
{
    int group_num = 0;

    if(used_group_num_syncread != 0)
        for (group_num = 0; group_num < used_group_num_syncread; group_num++)
            if (groupDataSyncRead[group_num].is_param_changed_ != true)
                break;

    if (group_num == used_group_num_syncread)
    {
        used_group_num_syncread++;
        groupDataSyncRead = (GroupDataSyncRead *)realloc(groupDataSyncRead, used_group_num_syncread * sizeof(GroupDataSyncRead));
    }

    groupDataSyncRead[group_num].port_num = port_num;
    groupDataSyncRead[group_num].protocol_version = protocol_version;
    groupDataSyncRead[group_num].data_list_length_ = 0;
    groupDataSyncRead[group_num].last_result_ = false;
    groupDataSyncRead[group_num].is_param_changed_ = false;
    groupDataSyncRead[group_num].start_address_ = start_address;
    groupDataSyncRead[group_num].data_length_ = data_length;
    groupDataSyncRead[group_num].data_list_ = 0;

    GroupSyncRead_ClearParam(group_num);

    return group_num;
}

void GroupSyncRead_MakeParam(int group_num)
{
    int _data_num, _idx;
    int _port_num = groupDataSyncRead[group_num].port_num;

    if (groupDataSyncRead[group_num].protocol_version == 1)
        return;

    if (GroupSyncRead_Size(group_num) == 0)
        return;

    if (packetData[_port_num].data_write_ != 0)
        free(packetData[_port_num].data_write_);
    packetData[_port_num].data_write_ = (UINT8_T*)malloc(GroupSyncRead_Size(group_num) * (1) * sizeof(UINT8_T)); // ID(1)

    _idx = 0;
    for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        packetData[_port_num].data_write_[_idx++] = groupDataSyncRead[group_num].data_list_[_data_num].id_;
    }
}

bool GroupSyncRead_AddParam(int group_num, UINT8_T id)
{
    int _data_num = 0;

    if (groupDataSyncRead[group_num].protocol_version == 1)
        return false;

    if (id == NOT_USED_ID)
        return false;

    if (groupDataSyncRead[group_num].data_list_length_ != 0)
        _data_num = GroupSyncRead_Find(group_num, id);

    if (groupDataSyncRead[group_num].data_list_length_ == _data_num)
    {
        groupDataSyncRead[group_num].data_list_length_++;
        groupDataSyncRead[group_num].data_list_ = (DataListSyncRead *)realloc(groupDataSyncRead[group_num].data_list_, groupDataSyncRead[group_num].data_list_length_ * sizeof(DataListSyncRead));

        groupDataSyncRead[group_num].data_list_[_data_num].id_ = id;
        groupDataSyncRead[group_num].data_list_[_data_num].data_ = (UINT8_T *)calloc(groupDataSyncRead[group_num].data_length_, sizeof(UINT8_T));
    }
    
    groupDataSyncRead[group_num].is_param_changed_ = true;
    return true;
}
void GroupSyncRead_RemoveParam(int group_num, UINT8_T id)
{
    int _data_num = GroupSyncRead_Find(group_num, id);

    if (groupDataSyncRead[group_num].protocol_version == 1)
        return;

    if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
        return;

    groupDataSyncRead[group_num].data_list_[_data_num].data_ = 0;
    free(groupDataSyncRead[group_num].data_list_[_data_num].data_);

    groupDataSyncRead[group_num].data_list_[_data_num].id_ = NOT_USED_ID;

    groupDataSyncRead[group_num].is_param_changed_ = true;
}
void GroupSyncRead_ClearParam(int group_num)
{
    int _port_num = groupDataSyncRead[group_num].port_num;

    if (groupDataSyncRead[group_num].protocol_version == 1)
        return;

    if (GroupSyncRead_Size(group_num) == 0)
        return;

    groupDataSyncRead[group_num].data_list_ = 0;
    free(groupDataSyncRead[group_num].data_list_);

    packetData[_port_num].data_write_ = 0;
    free(packetData[_port_num].data_write_);

    groupDataSyncRead[group_num].data_list_length_ = 0;
}

void GroupSyncRead_TxPacket(int group_num)
{
    int _port_num = groupDataSyncRead[group_num].port_num;

    if (groupDataSyncRead[group_num].protocol_version == 1)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    if (GroupSyncRead_Size(group_num) == 0)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    if (groupDataSyncRead[group_num].is_param_changed_ == true)
        GroupSyncRead_MakeParam(group_num);

    SyncReadTx(groupDataSyncRead[group_num].port_num
        , groupDataSyncRead[group_num].protocol_version
        , groupDataSyncRead[group_num].start_address_
        , groupDataSyncRead[group_num].data_length_
        , (GroupSyncRead_Size(group_num) * 1));
}

void GroupSyncRead_RxPacket(int group_num)
{
    int _data_num, _c;
    int _port_num = groupDataSyncRead[group_num].port_num;

    groupDataSyncRead[group_num].last_result_ = false;

    if (groupDataSyncRead[group_num].protocol_version == 1)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }
    
    packetData[groupDataSyncRead[group_num].port_num].communication_result_ = COMM_RX_FAIL;

    if (GroupSyncRead_Size(group_num) == 0)
    {
        packetData[groupDataSyncRead[group_num].port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
    {
        if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

            packetData[_port_num].data_read_
                = (UINT8_T *)realloc(packetData[_port_num].data_read_, groupDataSyncRead[group_num].data_length_ * sizeof(UINT8_T));

            ReadRx(groupDataSyncRead[group_num].port_num, groupDataSyncRead[group_num].protocol_version, groupDataSyncRead[group_num].data_length_);
            if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
                return;

            for (_c = 0; _c < groupDataSyncRead[group_num].data_length_; _c++)
                groupDataSyncRead[group_num].data_list_[_data_num].data_[_c] = packetData[_port_num].data_read_[_c];
    }

    if (packetData[_port_num].communication_result_ == COMM_SUCCESS)
        groupDataSyncRead[group_num].last_result_ = true;
}

void GroupSyncRead_TxRxPacket(int group_num)
{
    int _port_num = groupDataSyncRead[group_num].port_num;

    if (groupDataSyncRead[group_num].protocol_version == 1)
    {
        packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
        return;
    }

    packetData[_port_num].communication_result_ = COMM_TX_FAIL;

    GroupSyncRead_TxPacket(group_num);
    if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
        return;

    GroupSyncRead_RxPacket(group_num);
}

bool GroupSyncRead_IsAvailable(int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length)
{
    int _data_num = GroupSyncRead_Find(group_num, id);

    if (groupDataSyncRead[group_num].protocol_version == 1 || groupDataSyncRead[group_num].last_result_ == false || groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
        return false;

    if (address < groupDataSyncRead[group_num].start_address_ || groupDataSyncRead[group_num].start_address_ + groupDataSyncRead[group_num].data_length_ - data_length < address) {
        return false;
    }
    return true;
}

UINT32_T GroupSyncRead_GetData(int group_num, UINT8_T id, UINT16_T address, UINT16_T data_length)
{
    int _data_num = GroupSyncRead_Find(group_num, id);

    if (GroupSyncRead_IsAvailable(group_num, id, address, data_length) == false)
        return 0;

    switch (data_length)
    {
    case 1:
        return groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_];

    case 2:
        return DXL_MAKEWORD(groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_], groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_ + 1]);

    case 4:
        return DXL_MAKEDWORD(
            DXL_MAKEWORD(
            groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_ + 0]
            , groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_ + 1])
            , DXL_MAKEWORD(
                groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_ + 2]
                , groupDataSyncRead[group_num].data_list_[_data_num].data_[address - groupDataSyncRead[group_num].start_address_ + 3])
        );

    default:
        return 0;
    }
}

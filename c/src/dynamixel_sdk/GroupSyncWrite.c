/*
* GroupSyncWrite.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdlib.h>
#include "dynamixel_sdk/GroupSyncWrite.h"

#define NOT_USED_ID         255

typedef struct
{
    UINT8_T     id_;
    UINT16_T    data_end_;
    UINT8_T     *data_;
}DataListSyncWrite;

typedef struct
{
    int         port_num;
    int         protocol_version;
    
    int         data_list_length_;

    bool        is_param_changed_;

    UINT16_T    start_address_;
    UINT16_T    data_length_;

    DataListSyncWrite   *data_list_;
}GroupDataSyncWrite;

GroupDataSyncWrite *groupDataSyncWrite;

int used_group_num_syncwrite_ = 0;

int GroupSyncWrite_Size(int group_num)
{
    int _data_num;
    int size = 0;

    for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
        if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
            size++;
    return size;
}

int GroupSyncWrite_Find(int group_num, int id)
{
    int _data_num;

    for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
        if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == id)
            break;

    return _data_num;
}

int GroupSyncWrite(int port_num, int protocol_version, UINT16_T start_address, UINT16_T data_length)
{
    int _group_num = 0;

    if (used_group_num_syncwrite_ != 0)
        for (_group_num = 0; _group_num < used_group_num_syncwrite_; _group_num++)
            if (groupDataSyncWrite[_group_num].is_param_changed_ != true)
                break;

    if (_group_num == used_group_num_syncwrite_)
    {
        used_group_num_syncwrite_++;
        groupDataSyncWrite = (GroupDataSyncWrite *)realloc(groupDataSyncWrite, used_group_num_syncwrite_ * sizeof(GroupDataSyncWrite));
    }

    groupDataSyncWrite[_group_num].port_num = port_num;
    groupDataSyncWrite[_group_num].protocol_version = protocol_version;
    groupDataSyncWrite[_group_num].data_list_length_ = 0;
    groupDataSyncWrite[_group_num].is_param_changed_ = false;
    groupDataSyncWrite[_group_num].start_address_ = start_address;
    groupDataSyncWrite[_group_num].data_length_ = data_length;
    groupDataSyncWrite[_group_num].data_list_ = 0;

    GroupSyncWrite_ClearParam(_group_num);

    return _group_num;
}

void GroupSyncWrite_MakeParam(int group_num)
{
    int _data_num, _c, _idx;
    int _port_num = groupDataSyncWrite[group_num].port_num;

    if (GroupSyncWrite_Size(group_num) == 0)
        return;

    if (packetData[_port_num].data_write_ != 0)
        free(packetData[_port_num].data_write_);
    packetData[_port_num].data_write_ = (UINT8_T*)malloc(GroupSyncWrite_Size(group_num) * (1 + groupDataSyncWrite[group_num].data_length_) * sizeof(UINT8_T)); // ID(1) + DATA(data_length)

    _idx = 0;
    for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
    {
        if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
            continue;

        packetData[_port_num].data_write_[_idx++] = groupDataSyncWrite[group_num].data_list_[_data_num].id_;
        for (_c = 0; _c < groupDataSyncWrite[group_num].data_length_; _c++)
            packetData[_port_num].data_write_[_idx++] = groupDataSyncWrite[group_num].data_list_[_data_num].data_[_c];
    }
}

bool GroupSyncWrite_AddParam(int group_num, UINT8_T id, UINT32_T data, UINT16_T input_length)
{
    int _data_num = 0;

    if (id == NOT_USED_ID)
        return false;

    if (groupDataSyncWrite[group_num].data_list_length_ != 0)
        _data_num = GroupSyncWrite_Find(group_num, id);

    if (groupDataSyncWrite[group_num].data_list_length_ == _data_num)
    {
        groupDataSyncWrite[group_num].data_list_length_++;
        groupDataSyncWrite[group_num].data_list_ = (DataListSyncWrite *)realloc(groupDataSyncWrite[group_num].data_list_, groupDataSyncWrite[group_num].data_list_length_ * sizeof(DataListSyncWrite));

        groupDataSyncWrite[group_num].data_list_[_data_num].id_ = id;
        groupDataSyncWrite[group_num].data_list_[_data_num].data_ = (UINT8_T *)calloc(groupDataSyncWrite[group_num].data_length_, sizeof(UINT8_T));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ = 0;
    }
    else
        if (groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + input_length > groupDataSyncWrite[group_num].data_length_)
            return false;

    switch (input_length)
    {
    case 1:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        break;

    case 2:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        break;

    case 4:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 2] = DXL_LOBYTE(DXL_HIWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + 3] = DXL_HIBYTE(DXL_HIWORD(data));
        break;

    default:
        return false;
    }
    groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ = input_length;

    groupDataSyncWrite[group_num].is_param_changed_ = true;
    return true;
}

void GroupSyncWrite_RemoveParam(int group_num, UINT8_T id)
{
    int _data_num = GroupSyncWrite_Find(group_num, id);

    if (_data_num == groupDataSyncWrite[group_num].data_list_length_)
        return;

    if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
        return;

    groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ = 0;
    
    groupDataSyncWrite[group_num].data_list_[_data_num].data_ = 0;
    free(groupDataSyncWrite[group_num].data_list_[_data_num].data_);

    groupDataSyncWrite[group_num].data_list_[_data_num].id_ = NOT_USED_ID;

    groupDataSyncWrite[group_num].is_param_changed_ = true;
}

bool GroupSyncWrite_ChangeParam(int group_num, UINT8_T id, UINT32_T data, UINT16_T input_length, UINT16_T data_pos)
{
    if (id == NOT_USED_ID)  // NOT exist
        return false;

    int _data_num = GroupSyncWrite_Find(group_num, id);

    if (_data_num == groupDataSyncWrite[group_num].data_list_length_)
        return false;

    if (data_pos + input_length > groupDataSyncWrite[group_num].data_length_)
        return false;

    switch (input_length)
    {
    case 1:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        break;

    case 2:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        break;

    case 4:
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 0] = DXL_LOBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 1] = DXL_HIBYTE(DXL_LOWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 2] = DXL_LOBYTE(DXL_HIWORD(data));
        groupDataSyncWrite[group_num].data_list_[_data_num].data_[data_pos + 3] = DXL_HIBYTE(DXL_HIWORD(data));
        break;

    default:
        return false;
    }

    groupDataSyncWrite[group_num].is_param_changed_ = true;
    return true;
}

void GroupSyncWrite_ClearParam(int group_num)
{
    int _port_num = groupDataSyncWrite[group_num].port_num;

    if (GroupSyncWrite_Size(group_num) == 0)
        return;

    groupDataSyncWrite[group_num].data_list_ = 0;
    free(groupDataSyncWrite[group_num].data_list_);

    packetData[_port_num].data_write_ = 0;
    free(packetData[_port_num].data_write_);

    groupDataSyncWrite[group_num].data_list_length_ = 0;
}

void GroupSyncWrite_TxPacket(int group_num)
{
    int _port_num = groupDataSyncWrite[group_num].port_num;

    if (GroupSyncWrite_Size(group_num) == 0)
    {
    	packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
    	return;
    }

    if (groupDataSyncWrite[group_num].is_param_changed_ == true)
        GroupSyncWrite_MakeParam(group_num);

	SyncWriteTxOnly(
        groupDataSyncWrite[group_num].port_num
        , groupDataSyncWrite[group_num].protocol_version
        , groupDataSyncWrite[group_num].start_address_
        , groupDataSyncWrite[group_num].data_length_
        , GroupSyncWrite_Size(group_num) * (1 + groupDataSyncWrite[group_num].data_length_));
}

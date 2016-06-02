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
* group_sync_write.c
*
*  Created on: 2016. 5. 4.
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdlib.h>
#include "dynamixel_sdk/group_sync_write.h"

#define NOT_USED_ID         255

typedef struct
{
  uint8_t     id_;
  uint16_t    data_end_;
  uint8_t     *data_;
}DataListSyncWrite;

typedef struct
{
  int         port_num;
  int         protocol_version;

  int         data_list_length_;

  bool        is_param_changed_;

  uint16_t    start_address_;
  uint16_t    data_length_;

  DataListSyncWrite   *data_list_;
}GroupDataSyncWrite;

GroupDataSyncWrite *groupDataSyncWrite;

int used_group_num_syncwrite_ = 0;

int groupSyncWriteSize(int group_num)
{
  int _data_num;
  int size = 0;

  for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
    if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
      size++;
  return size;
}

int groupSyncWriteFind(int group_num, int id)
{
  int _data_num;

  for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == id)
      break;
  }

  return _data_num;
}

int groupSyncWrite(int port_num, int protocol_version, uint16_t start_address, uint16_t data_length)
{
  int _group_num = 0;

  if (used_group_num_syncwrite_ != 0)
  {
    for (_group_num = 0; _group_num < used_group_num_syncwrite_; _group_num++)
    {
      if (groupDataSyncWrite[_group_num].is_param_changed_ != true)
        break;
    }
  }

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

  groupSyncWriteClearParam(_group_num);

  return _group_num;
}

void groupSyncWriteMakeParam(int group_num)
{
  int _data_num, _c, _idx;
  int _port_num = groupDataSyncWrite[group_num].port_num;

  if (groupSyncWriteSize(group_num) == 0)
    return;

  packetData[_port_num].data_write_ = (uint8_t*)realloc(packetData[_port_num].data_write_, groupSyncWriteSize(group_num) * (1 + groupDataSyncWrite[group_num].data_length_) * sizeof(uint8_t)); // ID(1) + DATA(data_length)

  _idx = 0;
  for (_data_num = 0; _data_num < groupDataSyncWrite[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
      continue;

    packetData[_port_num].data_write_[_idx++] = groupDataSyncWrite[group_num].data_list_[_data_num].id_;
    for (_c = 0; _c < groupDataSyncWrite[group_num].data_length_; _c++)
    {
      packetData[_port_num].data_write_[_idx++] = groupDataSyncWrite[group_num].data_list_[_data_num].data_[_c];
    }
  }
}

bool groupSyncWriteAddParam(int group_num, uint8_t id, uint32_t data, uint16_t input_length)
{
  int _data_num = 0;

  if (id == NOT_USED_ID)
    return false;

  if (groupDataSyncWrite[group_num].data_list_length_ != 0)
    _data_num = groupSyncWriteFind(group_num, id);

  if (groupDataSyncWrite[group_num].data_list_length_ == _data_num)
  {
    groupDataSyncWrite[group_num].data_list_length_++;
    groupDataSyncWrite[group_num].data_list_ = (DataListSyncWrite *)realloc(groupDataSyncWrite[group_num].data_list_, groupDataSyncWrite[group_num].data_list_length_ * sizeof(DataListSyncWrite));

    groupDataSyncWrite[group_num].data_list_[_data_num].id_ = id;
    groupDataSyncWrite[group_num].data_list_[_data_num].data_ = (uint8_t *)calloc(groupDataSyncWrite[group_num].data_length_, sizeof(uint8_t));
    groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ = 0;
  }
  else
  {
    if (groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ + input_length > groupDataSyncWrite[group_num].data_length_)
      return false;
  }

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

void groupSyncWriteRemoveParam(int group_num, uint8_t id)
{
  int _data_num = groupSyncWriteFind(group_num, id);

  if (_data_num == groupDataSyncWrite[group_num].data_list_length_)
    return;

  if (groupDataSyncWrite[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
    return;

  groupDataSyncWrite[group_num].data_list_[_data_num].data_end_ = 0;

  groupDataSyncWrite[group_num].data_list_[_data_num].data_ = 0;

  groupDataSyncWrite[group_num].data_list_[_data_num].id_ = NOT_USED_ID;

  groupDataSyncWrite[group_num].is_param_changed_ = true;
}

bool groupSyncWriteChangeParam(int group_num, uint8_t id, uint32_t data, uint16_t input_length, uint16_t data_pos)
{
  if (id == NOT_USED_ID)  // NOT exist
    return false;

  int _data_num = groupSyncWriteFind(group_num, id);

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

void groupSyncWriteClearParam(int group_num)
{
  int _port_num = groupDataSyncWrite[group_num].port_num;

  if (groupSyncWriteSize(group_num) == 0)
    return;

  groupDataSyncWrite[group_num].data_list_ = 0;

  packetData[_port_num].data_write_ = 0;

  groupDataSyncWrite[group_num].data_list_length_ = 0;
}

void groupSyncWriteTxPacket(int group_num)
{
  int _port_num = groupDataSyncWrite[group_num].port_num;

  if (groupSyncWriteSize(group_num) == 0)
  {
  	packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
  	return;
  }

  if (groupDataSyncWrite[group_num].is_param_changed_ == true)
    groupSyncWriteMakeParam(group_num);

	syncWriteTxOnly(
    groupDataSyncWrite[group_num].port_num
    , groupDataSyncWrite[group_num].protocol_version
    , groupDataSyncWrite[group_num].start_address_
    , groupDataSyncWrite[group_num].data_length_
    , groupSyncWriteSize(group_num) * (1 + groupDataSyncWrite[group_num].data_length_));
}

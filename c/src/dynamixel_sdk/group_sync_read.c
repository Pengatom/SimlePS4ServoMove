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
* group_sync_read.c
*
*  Created on: 2016. 5. 4.
*/
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdlib.h>
#include "dynamixel_sdk/group_sync_read.h"

#define NOT_USED_ID         255

typedef struct
{
  uint8_t     id_;
  uint8_t     *data_;
}DataListSyncRead;

typedef struct
{
  int         port_num;
  int         protocol_version;

  int         data_list_length_;

  bool        last_result_;
  bool        is_param_changed_;

  uint16_t    start_address_;
  uint16_t    data_length_;

  DataListSyncRead   *data_list_;
}GroupDataSyncRead;

GroupDataSyncRead *groupDataSyncRead;

int used_group_num_syncread = 0;

int groupSyncReadFind(int group_num, int id)
{
  int _data_num;

  for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == id)
      break;
  }

  return _data_num;
}

int groupSyncReadSize(int group_num)
{
  int _data_num;
  int size = 0;

  for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncRead[group_num].data_list_[_data_num].id_ != NOT_USED_ID)
      size++;
  }
  return size;
};

int groupSyncRead(int port_num, int protocol_version, uint16_t start_address, uint16_t data_length)
{
  int group_num = 0;

  if (used_group_num_syncread != 0)
  {
    for (group_num = 0; group_num < used_group_num_syncread; group_num++)
    {
      if (groupDataSyncRead[group_num].is_param_changed_ != true)
        break;
    }
  }

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

  groupSyncReadClearParam(group_num);

  return group_num;
}

void groupSyncReadMakeParam(int group_num)
{
  int _data_num, _idx;
  int _port_num = groupDataSyncRead[group_num].port_num;

  if (groupDataSyncRead[group_num].protocol_version == 1)
    return;

  if (groupSyncReadSize(group_num) == 0)
    return;

  packetData[_port_num].data_write_ = (uint8_t*)realloc(packetData[_port_num].data_write_, groupSyncReadSize(group_num) * (1) * sizeof(uint8_t)); // ID(1)

  _idx = 0;
  for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
      continue;

    packetData[_port_num].data_write_[_idx++] = groupDataSyncRead[group_num].data_list_[_data_num].id_;
  }
}

bool groupSyncReadAddParam(int group_num, uint8_t id)
{
  int _data_num = 0;

  if (groupDataSyncRead[group_num].protocol_version == 1)
    return false;

  if (id == NOT_USED_ID)
    return false;

  if (groupDataSyncRead[group_num].data_list_length_ != 0)
    _data_num = groupSyncReadFind(group_num, id);

  if (groupDataSyncRead[group_num].data_list_length_ == _data_num)
  {
    groupDataSyncRead[group_num].data_list_length_++;
    groupDataSyncRead[group_num].data_list_ = (DataListSyncRead *)realloc(groupDataSyncRead[group_num].data_list_, groupDataSyncRead[group_num].data_list_length_ * sizeof(DataListSyncRead));

    groupDataSyncRead[group_num].data_list_[_data_num].id_ = id;
    groupDataSyncRead[group_num].data_list_[_data_num].data_ = (uint8_t *)calloc(groupDataSyncRead[group_num].data_length_, sizeof(uint8_t));
  }

  groupDataSyncRead[group_num].is_param_changed_ = true;
  return true;
}
void groupSyncReadRemoveParam(int group_num, uint8_t id)
{
  int _data_num = groupSyncReadFind(group_num, id);

  if (groupDataSyncRead[group_num].protocol_version == 1)
    return;

  if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)  // NOT exist
    return;

  groupDataSyncRead[group_num].data_list_[_data_num].data_ = 0;

  groupDataSyncRead[group_num].data_list_[_data_num].id_ = NOT_USED_ID;

  groupDataSyncRead[group_num].is_param_changed_ = true;
}
void groupSyncReadClearParam(int group_num)
{
  int _port_num = groupDataSyncRead[group_num].port_num;

  if (groupDataSyncRead[group_num].protocol_version == 1)
    return;

  if (groupSyncReadSize(group_num) == 0)
    return;

  groupDataSyncRead[group_num].data_list_ = 0;

  packetData[_port_num].data_write_ = 0;

  groupDataSyncRead[group_num].data_list_length_ = 0;
}

void groupSyncReadTxPacket(int group_num)
{
  int _port_num = groupDataSyncRead[group_num].port_num;

  if (groupDataSyncRead[group_num].protocol_version == 1)
  {
    packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return;
  }

  if (groupSyncReadSize(group_num) == 0)
  {
    packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return;
  }

  if (groupDataSyncRead[group_num].is_param_changed_ == true)
    groupSyncReadMakeParam(group_num);

  syncReadTx(groupDataSyncRead[group_num].port_num
    , groupDataSyncRead[group_num].protocol_version
    , groupDataSyncRead[group_num].start_address_
    , groupDataSyncRead[group_num].data_length_
    , (groupSyncReadSize(group_num) * 1));
}

void groupSyncReadRxPacket(int group_num)
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

  if (groupSyncReadSize(group_num) == 0)
  {
    packetData[groupDataSyncRead[group_num].port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return;
  }

  for (_data_num = 0; _data_num < groupDataSyncRead[group_num].data_list_length_; _data_num++)
  {
    if (groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
      continue;

      packetData[_port_num].data_read_
        = (uint8_t *)realloc(packetData[_port_num].data_read_, groupDataSyncRead[group_num].data_length_ * sizeof(uint8_t));

      readRx(groupDataSyncRead[group_num].port_num, groupDataSyncRead[group_num].protocol_version, groupDataSyncRead[group_num].data_length_);
      if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
        return;

      for (_c = 0; _c < groupDataSyncRead[group_num].data_length_; _c++)
        groupDataSyncRead[group_num].data_list_[_data_num].data_[_c] = packetData[_port_num].data_read_[_c];
  }

  if (packetData[_port_num].communication_result_ == COMM_SUCCESS)
    groupDataSyncRead[group_num].last_result_ = true;
}

void groupSyncReadTxRxPacket(int group_num)
{
  int _port_num = groupDataSyncRead[group_num].port_num;

  if (groupDataSyncRead[group_num].protocol_version == 1)
  {
    packetData[_port_num].communication_result_ = COMM_NOT_AVAILABLE;
    return;
  }

  packetData[_port_num].communication_result_ = COMM_TX_FAIL;

  groupSyncReadTxPacket(group_num);
  if (packetData[_port_num].communication_result_ != COMM_SUCCESS)
    return;

  groupSyncReadRxPacket(group_num);
}

bool groupSyncReadIsAvailable(int group_num, uint8_t id, uint16_t address, uint16_t data_length)
{
  int _data_num = groupSyncReadFind(group_num, id);

  if (groupDataSyncRead[group_num].protocol_version == 1 || groupDataSyncRead[group_num].last_result_ == false || groupDataSyncRead[group_num].data_list_[_data_num].id_ == NOT_USED_ID)
    return false;

  if (address < groupDataSyncRead[group_num].start_address_ || groupDataSyncRead[group_num].start_address_ + groupDataSyncRead[group_num].data_length_ - data_length < address) {
    return false;
  }
  return true;
}

uint32_t groupSyncReadGetData(int group_num, uint8_t id, uint16_t address, uint16_t data_length)
{
  int _data_num = groupSyncReadFind(group_num, id);

  if (groupSyncReadIsAvailable(group_num, id, address, data_length) == false)
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

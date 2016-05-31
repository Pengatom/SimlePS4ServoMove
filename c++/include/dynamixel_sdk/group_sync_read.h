/*
 * group_sync_read.h
 *
 *  Created on: 2016. 2. 2.
 *      Author: zerom, leon
 */

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_H_


#include <map>
#include <vector>
#include "port_handler.h"
#include "packet_handler.h"

namespace dynamixel
{

class WINDECLSPEC GroupSyncRead
{
 private:
  PortHandler    *port_;
  PacketHandler  *ph_;

  std::vector<uint8_t>            id_list_;
  std::map<uint8_t, uint8_t* >    data_list_; // <id, data>

  bool            last_result_;
  bool            is_param_changed_;

  uint8_t        *param_;
  uint16_t        start_address_;
  uint16_t        data_length_;

  void    makeParam();

 public:
  GroupSyncRead(PortHandler *port, PacketHandler *ph, uint16_t start_address, uint16_t data_length);
  ~GroupSyncRead() { clearParam(); }

  PortHandler     *getPortHandler()   { return port_; }
  PacketHandler   *getPacketHandler() { return ph_; }

  bool    addParam    (uint8_t id);
  void    removeParam (uint8_t id);
  void    clearParam  ();

  int     txPacket();
  int     rxPacket();
  int     txRxPacket();

  bool        isAvailable (uint8_t id, uint16_t address, uint16_t data_length);
  uint32_t    getData     (uint8_t id, uint16_t address, uint16_t data_length);
};

}


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPSYNCREAD_H_ */

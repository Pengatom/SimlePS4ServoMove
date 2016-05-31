/*
 * group_bulk_read.h
 *
 *  Created on: 2016. 1. 28.
 *      Author: zerom, leon
 */

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_H_


#include <map>
#include <vector>
#include "port_handler.h"
#include "packet_handler.h"

namespace dynamixel
{

class WINDECLSPEC GroupBulkRead
{
 private:
  PortHandler    *port_;
  PacketHandler  *ph_;

  std::vector<uint8_t>            id_list_;
  std::map<uint8_t, uint16_t>     address_list_;  // <id, start_address>
  std::map<uint8_t, uint16_t>     length_list_;   // <id, data_length>
  std::map<uint8_t, uint8_t *>    data_list_;     // <id, data>

  bool            last_result_;
  bool            is_param_changed_;

  uint8_t        *param_;

  void    makeParam();

 public:
  GroupBulkRead(PortHandler *port, PacketHandler *ph);
  ~GroupBulkRead() { clearParam(); }

  PortHandler     *getPortHandler()   { return port_; }
  PacketHandler   *getPacketHandler() { return ph_; }

  bool    addParam    (uint8_t id, uint16_t start_address, uint16_t data_length);
  void    removeParam (uint8_t id);
  void    clearParam  ();

  int     txPacket();
  int     rxPacket();
  int     txRxPacket();

  bool        isAvailable (uint8_t id, uint16_t address, uint16_t data_length);
  uint32_t    getData     (uint8_t id, uint16_t address, uint16_t data_length);
};

}


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKREAD_H_ */

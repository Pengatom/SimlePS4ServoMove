/*
 * group_bulk_write.h
 *
 *  Created on: 2016. 2. 2.
 *      Author: zerom, leon
 */

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_H_


#include <map>
#include <vector>
#include "port_handler.h"
#include "packet_handler.h"

namespace dynamixel
{

class WINDECLSPEC GroupBulkWrite
{
 private:
  PortHandler    *port_;
  PacketHandler  *ph_;

  std::vector<uint8_t>            id_list_;
  std::map<uint8_t, uint16_t>     address_list_;  // <id, start_address>
  std::map<uint8_t, uint16_t>     length_list_;   // <id, data_length>
  std::map<uint8_t, uint8_t *>    data_list_;     // <id, data>

  bool            is_param_changed_;

  uint8_t        *param_;
  uint16_t        param_length_;

  void    makeParam();

 public:
  GroupBulkWrite(PortHandler *port, PacketHandler *ph);
  ~GroupBulkWrite() { clearParam(); }

  PortHandler     *getPortHandler()   { return port_; }
  PacketHandler   *getPacketHandler() { return ph_; }

  bool    addParam    (uint8_t id, uint16_t start_address, uint16_t data_length, uint8_t *data);
  void    removeParam (uint8_t id);
  bool    changeParam (uint8_t id, uint16_t start_address, uint16_t data_length, uint8_t *data);
  void    clearParam  ();

  int     txPacket();
};

}


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_GROUPBULKWRITE_H_ */

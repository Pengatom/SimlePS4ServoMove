/*
 * packet_handler.cpp
 *
 *  Created on: 2016. 1. 26.
 *      Author: zerom, leon
 */
#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include "dynamixel_sdk/packet_handler.h"
#include "dynamixel_sdk/protocol1_packet_handler.h"
#include "dynamixel_sdk/protocol2_packet_handler.h"

using namespace dynamixel;

PacketHandler *PacketHandler::getPacketHandler(float protocol_version)
{
  if (protocol_version == 1.0)
  {
    return (PacketHandler *)(Protocol1PacketHandler::getInstance());
  }
  else if (protocol_version == 2.0)
  {
    return (PacketHandler *)(Protocol2PacketHandler::getInstance());
  }

  return (PacketHandler *)(Protocol2PacketHandler::getInstance());
}

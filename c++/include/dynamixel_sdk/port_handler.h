/*
 * port_handler.h
 *
 *  Created on: 2016. 1. 26.
 *      Author: zerom, leon
 */

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_H_

#ifdef __linux__
#define WINDECLSPEC
#elif defined(_WIN32) || defined(_WIN64)
#ifdef WINDLLEXPORT
#define WINDECLSPEC __declspec(dllexport)
#else
#define WINDECLSPEC __declspec(dllimport)
#endif
#endif

#include <stdint.h>

namespace dynamixel
{

class WINDECLSPEC PortHandler
{
 public:
  static const int DEFAULT_BAUDRATE_ = 1000000;

  static PortHandler *getPortHandler(const char *port_name);

  bool   is_using_;

  virtual ~PortHandler() { }

  virtual bool    openPort() = 0;
  virtual void    closePort() = 0;
  virtual void    clearPort() = 0;

  virtual void    setPortName(const char* port_name) = 0;
  virtual char   *getPortName() = 0;

  virtual bool    setBaudRate(const int baudrate) = 0;
  virtual int     getBaudRate() = 0;

  virtual int     getBytesAvailable() = 0;

  virtual int     readPort(uint8_t *packet, int length) = 0;
  virtual int     writePort(uint8_t *packet, int length) = 0;

  virtual void    setPacketTimeout(uint16_t packet_length) = 0;
  virtual void    setPacketTimeout(double msec) = 0;
  virtual bool    isPacketTimeout() = 0;
};

}


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_H_ */

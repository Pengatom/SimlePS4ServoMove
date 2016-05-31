/*
* port_handler_windows.h
*
*  Created on: 2016. 4. 26.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_H_

#include <Windows.h>

#include "dynamixel_sdk/port_handler.h"

namespace dynamixel
{
class WINDECLSPEC PortHandlerWindows : public PortHandler
{
 private:
  HANDLE  serial_handle_;
  LARGE_INTEGER freq_, counter_;

  int     baudrate_;
  char    port_name_[30];

  double  packet_start_time_;
  double  packet_timeout_;
  double  tx_time_per_byte_;

  bool    setupPort(const int baudrate);

  double  getCurrentTime();
  double  getTimeSinceStart();

 public:
  PortHandlerWindows(const char *port_name);
  virtual ~PortHandlerWindows() { closePort(); }

  bool    openPort();
  void    closePort();
  void    clearPort();

  void    setPortName(const char *port_name);
  char   *getPortName();

  bool    setBaudRate(const int baudrate);
  int     getBaudRate();

  int     getBytesAvailable();

  int     readPort(uint8_t *packet, int length);
  int     writePort(uint8_t *packet, int length);

  void    setPacketTimeout(uint16_t packet_length);
  void    setPacketTimeout(double msec);
  bool    isPacketTimeout();
};

}


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERWINDOWS_H_ */

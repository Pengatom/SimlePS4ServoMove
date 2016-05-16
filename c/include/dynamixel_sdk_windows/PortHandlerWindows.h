/*
* PortHandlerWindows.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_C_H_

#include <Windows.h>
#include "dynamixel_sdk/PortHandler.h"

WINDECLSPEC bool    SetupPortWindows            (int port_num, const int baudrate);

#undef GetCurrentTime
WINDECLSPEC double  GetCurrentTimeWindows       (int port_num);
WINDECLSPEC double  GetTimeSinceStartWindows    (int port_num);

WINDECLSPEC int     PortHandlerWindows          (const char *port_name);

WINDECLSPEC bool    OpenPortWindows             (int port_num);
WINDECLSPEC void    ClosePortWindows            (int port_num);
WINDECLSPEC void    ClearPortWindows            (int port_num);

WINDECLSPEC void    SetPortNameWindows          (int port_num, const char* port_name);
WINDECLSPEC char   *GetPortNameWindows          (int port_num);

WINDECLSPEC bool    SetBaudRateWindows          (int port_num, const int baudrate);
WINDECLSPEC int     GetBaudRateWindows          (int port_num);

WINDECLSPEC int     ReadPortWindows             (int port_num, UINT8_T *packet, int length);
WINDECLSPEC int     WritePortWindows            (int port_num, UINT8_T *packet, int length);

WINDECLSPEC void    SetPacketTimeoutWindows     (int port_num, UINT16_T packet_length);
WINDECLSPEC void    SetPacketTimeoutMSecWindows (int port_num, double msec);
WINDECLSPEC bool    IsPacketTimeoutWindows      (int port_num);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERWINDOWS_C_H_ */

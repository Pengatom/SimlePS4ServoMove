/*
* PortHandler.h
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_C_H_

#ifdef __linux__
#define WINDECLSPEC
#elif defined(_WIN32) || defined(_WIN64)
#ifdef WINDLLEXPORT
#define WINDECLSPEC __declspec(dllexport)
#else
#define WINDECLSPEC __declspec(dllimport)
#endif
#endif

#include "RobotisDef.h"

static const int DEFAULT_BAUDRATE = 1000000;

int     used_port_num_;
bool    *is_using_;

WINDECLSPEC int     PortHandler             (const char *port_name);

WINDECLSPEC bool    OpenPort                (int port_num);
WINDECLSPEC void    ClosePort               (int port_num);
WINDECLSPEC void    ClearPort               (int port_num);

WINDECLSPEC void    SetPortName             (int port_num, const char* port_name);
WINDECLSPEC char   *GetPortName             (int port_num);

WINDECLSPEC bool    SetBaudRate             (int port_num, const int baudrate);
WINDECLSPEC int     GetBaudRate             (int port_num);

#if defined(_WIN32) || defined(_WIN64)
WINDECLSPEC int     GetBytesAvailable       (int port_num);
#endif

WINDECLSPEC int     ReadPort                (int port_num, UINT8_T *packet, int length);
WINDECLSPEC int     WritePort               (int port_num, UINT8_T *packet, int length);

WINDECLSPEC void    SetPacketTimeout        (int port_num, UINT16_T packet_length);
WINDECLSPEC void    SetPacketTimeoutMSec    (int port_num, double msec);
WINDECLSPEC bool    IsPacketTimeout         (int port_num);


#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_PORTHANDLER_C_H_ */

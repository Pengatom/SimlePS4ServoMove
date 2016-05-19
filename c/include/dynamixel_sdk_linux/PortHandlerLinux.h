/*
* PortHandlerLinux.h
*
*  Created on: 2016. 5. 17.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERLINUX_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERLINUX_C_H_


#include "dynamixel_sdk/PortHandler.h"

int PortHandlerLinux(const char *port_name);

bool    SetupPortLinux(int port_num, const int cflag_baud);
bool    SetCustomBaudrateLinux(int port_num, int speed);
int     GetCFlagBaud(const int baudrate);

double  GetCurrentTimeLinux();
double  GetTimeSinceStartLinux(int port_num);

bool    OpenPortLinux(int port_num);
void    ClosePortLinux(int port_num);
void    ClearPortLinux(int port_num);

void    SetPortNameLinux(int port_num, const char *port_name);
char   *GetPortNameLinux(int port_num);

bool    SetBaudRateLinux(int port_num, const int baudrate);
int     GetBaudRateLinux(int port_num);

int     GetBytesAvailableLinux(int port_num);

int     ReadPortLinux(int port_num, UINT8_T *packet, int length);
int     WritePortLinux(int port_num, UINT8_T *packet, int length);

void    SetPacketTimeoutLinux(int port_num, UINT16_T packet_length);
void    SetPacketTimeoutMSecLinux(int port_num, double msec);
bool    IsPacketTimeoutLinux(int port_num);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERLINUX_C_H_ */

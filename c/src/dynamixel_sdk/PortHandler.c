/*
* PortHandler.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include "dynamixel_sdk/PortHandler.h"

#ifdef __linux__
#include "dynamixel_sdk_linux/PortHandlerLinux.h"
#endif

#if defined(_WIN32) || defined(_WIN64)
#include "dynamixel_sdk_windows/PortHandlerWindows.h"
#endif

#ifdef __linux__
int     PortHandler         (const char *port_name) { return PortHandlerLinux(port_name); };

bool    OpenPort            (int port_num) { return OpenPortLinux(port_num); };
void    ClosePort           (int port_num) { ClosePortLinux(port_num); };
void    ClearPort           (int port_num) { ClearPortLinux(port_num); };

void    SetPortName         (int port_num, const char* port_name) { SetPortNameLinux(port_num, port_name); };
char   *GetPortName         (int port_num) { return GetPortNameLinux(port_num); };

bool    SetBaudRate         (int port_num, const int baudrate) { return SetBaudRateLinux(port_num, baudrate); };
int     GetBaudRate         (int port_num) { return GetBaudRateLinux(port_num); }

int     GetBytesAvailable   (int port_num) { return GetBytesAvailableLinux(port_num); }

int     ReadPort            (int port_num, UINT8_T *packet, int length) { return ReadPortLinux(port_num, packet, length); };
int     WritePort           (int port_num, UINT8_T *packet, int length) { return WritePortLinux(port_num, packet, length); };

void    SetPacketTimeout    (int port_num, UINT16_T packet_length) { SetPacketTimeoutLinux(port_num, packet_length); };
void    SetPacketTimeoutMSec(int port_num, double msec) { SetPacketTimeoutMSecLinux(port_num, msec); };
bool    IsPacketTimeout     (int port_num) { return IsPacketTimeoutLinux(port_num); };
#endif

#if defined(_WIN32) || defined(_WIN64)
int     PortHandler         (const char *port_name) { return PortHandlerWindows(port_name); };

bool    OpenPort            (int port_num) { return OpenPortWindows(port_num); };
void    ClosePort           (int port_num) { ClosePortWindows(port_num); };
void    ClearPort           (int port_num) { ClearPortWindows(port_num); };

void    SetPortName         (int port_num, const char* port_name) { SetPortNameWindows(port_num, port_name); };
char   *GetPortName         (int port_num) { return GetPortNameWindows(port_num); };

bool    SetBaudRate         (int port_num, const int baudrate) { return SetBaudRateWindows(port_num, baudrate); };
int     GetBaudRate         (int port_num) { return GetBaudRateWindows(port_num); }

int     ReadPort            (int port_num, UINT8_T *packet, int length) { return ReadPortWindows(port_num, packet, length); };
int     WritePort           (int port_num, UINT8_T *packet, int length) { return WritePortWindows(port_num, packet, length); };

void    SetPacketTimeout    (int port_num, UINT16_T packet_length) { SetPacketTimeoutWindows(port_num, packet_length); };
void    SetPacketTimeoutMSec(int port_num, double msec) { SetPacketTimeoutMSecWindows(port_num, msec); };
bool    IsPacketTimeout     (int port_num) { return IsPacketTimeoutWindows(port_num); };
#endif

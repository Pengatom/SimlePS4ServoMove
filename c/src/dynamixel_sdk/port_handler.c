/*******************************************************************************
* Copyright (c) 2016, ROBOTIS CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* * Neither the name of ROBOTIS nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* Author: Leon Ryu Woon Jung */

/*
* PortHandler.c
*
*  Created on: 2016. 5. 4.
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

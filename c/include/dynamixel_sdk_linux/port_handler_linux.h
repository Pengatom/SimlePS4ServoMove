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
* PortHandlerLinux.h
*
*  Created on: 2016. 5. 17.
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

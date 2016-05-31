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
* PortHandlerWindows.h
*
*  Created on: 2016. 5. 4.
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

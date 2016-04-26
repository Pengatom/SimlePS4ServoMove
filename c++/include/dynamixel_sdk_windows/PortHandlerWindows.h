/*
* PortHandlerLinux.h
*
*  Created on: 2016. 4. 26.
*      Author: leon
*/

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_WINDOWS_PORTHANDLERWINDOWS_H_

#include <Windows.h>
#include "dynamixel_sdk/PortHandler.h"

namespace ROBOTIS
{
    class WINDECLSPEC PortHandlerWindows : public PortHandler
    {
    private:
        HANDLE  serial_handle;
        LARGE_INTEGER freq, counter;

        int     baudrate_;
        char    port_name_[30];

        double  packet_start_time_;
        double  packet_timeout_;
        double  tx_time_per_byte;

        bool    SetupPort(const int baudrate);

#undef GetCurrentTime
        double  GetCurrentTime();
        double  GetTimeSinceStart();

    public:
        PortHandlerWindows(const char *port_name);
        virtual ~PortHandlerWindows() { ClosePort(); }

        bool    OpenPort();
        void    ClosePort();
        void    ClearPort();

        void    SetPortName(const char *port_name);
        char   *GetPortName();

        bool    SetBaudRate(const int baudrate);
        int     GetBaudRate();

        int     GetBytesAvailable();

        int     ReadPort(UINT8_T *packet, int length);
        int     WritePort(UINT8_T *packet, int length);

        void    SetPacketTimeout(UINT16_T packet_length);
        void    SetPacketTimeout(double msec);
        bool    IsPacketTimeout();
    };

}

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_LINUX_PORTHANDLERWINDOWS_H_ */

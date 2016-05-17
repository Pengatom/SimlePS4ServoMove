/*
* PortHandlerLinux.c
*
*  Created on: 2016. 5. 17.
*      Author: leon
*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "dynamixel_sdk_linux/PortHandlerLinux.h"

#define LATENCY_TIMER   4  // msec (USB latency timer)

typedef struct
{
    int     socket_fd_;
    int     baudrate_;
    char    port_name_[30];

    double  packet_start_time_;
    double  packet_timeout_;
    double  tx_time_per_byte;
}PortDataLinux;

PortDataLinux *portDataLinux;

int PortHandlerLinux(const char *port_name)
{
    int _port_num;

    if (portDataLinux == NULL)
    {
        _port_num = 0;
        used_port_num_ = 1;
        portDataLinux = (PortDataLinux *)calloc(1, sizeof(PortDataLinux));
        is_using_ = (bool*)calloc(1, sizeof(bool));
    }
    else
    {
        for (_port_num = 0; _port_num < used_port_num_; _port_num++)
            if (!strcmp(portDataLinux[_port_num].port_name_, port_name))
                break;

        if (_port_num == used_port_num_)
        {
            for (_port_num = 0; _port_num < used_port_num_; _port_num++)
                if (portDataLinux[_port_num].socket_fd_ != -1)
                    break;

            if (_port_num == used_port_num_)
            {
                used_port_num_++;
                portDataLinux = (PortDataLinux*)realloc(portDataLinux, used_port_num_ * sizeof(portDataLinux));
                is_using_ = (bool*)realloc(is_using_, used_port_num_ * sizeof(bool));
            }
        }
        else
            printf("[PortHandler setup] The port number %d has same device name... reinitialize port number %d!!\n", _port_num, _port_num);
    }

    portDataLinux[_port_num].socket_fd_ = -1;
    portDataLinux[_port_num].baudrate_ = DEFAULT_BAUDRATE;
    portDataLinux[_port_num].packet_start_time_ = 0.0;
    portDataLinux[_port_num].packet_timeout_ = 0.0;
    portDataLinux[_port_num].tx_time_per_byte = 0.0;

    is_using_[_port_num] = false;

    SetPortNameLinux(_port_num, port_name);

    return _port_num;
}

bool OpenPortLinux(int port_num)
{
    return SetBaudRateLinux(port_num, portDataLinux[port_num].baudrate_);
}

void ClosePortLinux(int port_num)
{
    if (portDataLinux[port_num].socket_fd_ != -1)
    {
        close(portDataLinux[port_num].socket_fd_);
        portDataLinux[port_num].socket_fd_ = -1;
    }
}

void ClearPortLinux(int port_num)
{
    tcflush(portDataLinux[port_num].socket_fd_, TCIOFLUSH);
}

void SetPortNameLinux(int port_num, const char *port_name)
{
    strcpy(portDataLinux[port_num].port_name_, port_name);
}

char *GetPortNameLinux(int port_num)
{
    return portDataLinux[port_num].port_name_;
}

bool SetBaudRateLinux(int port_num, const int baudrate)
{
    int _baud = GetCFlagBaud(baudrate);

    ClosePortLinux(port_num);

    if (_baud <= 0)   // custom baudrate
    {
        SetupPortLinux(port_num, B38400);
        portDataLinux[port_num].baudrate_ = baudrate;
        return SetCustomBaudrateLinux(port_num, baudrate);
    }
    else
    {
        portDataLinux[port_num].baudrate_ = baudrate;
        return SetupPortLinux(port_num, _baud);
    }
}

int GetBaudRateLinux(int port_num)
{
    return portDataLinux[port_num].baudrate_;
}

int GetBytesAvailableLinux(int port_num)
{
    int _bytes_available;
    ioctl(portDataLinux[port_num].socket_fd_, FIONREAD, &_bytes_available);
    return _bytes_available;
}

int ReadPortLinux(int port_num, UINT8_T *packet, int length)
{
    return read(portDataLinux[port_num].socket_fd_, packet, length);
}

int WritePortLinux(int port_num, UINT8_T *packet, int length)
{
    return write(portDataLinux[port_num].socket_fd_, packet, length);
}

void SetPacketTimeoutLinux(int port_num, UINT16_T packet_length)
{
    portDataLinux[port_num].packet_start_time_ = GetCurrentTimeLinux();
    portDataLinux[port_num].packet_timeout_ = (portDataLinux[port_num].tx_time_per_byte * (double)packet_length) + (LATENCY_TIMER * 2.0) + 2.0;
}

void SetPacketTimeoutMSecLinux(int port_num, double msec)
{
    portDataLinux[port_num].packet_start_time_ = GetCurrentTimeLinux();
    portDataLinux[port_num].packet_timeout_ = msec;
}

bool IsPacketTimeoutLinux(int port_num)
{
    if (GetTimeSinceStartLinux(port_num) > portDataLinux[port_num].packet_timeout_)
    {
        portDataLinux[port_num].packet_timeout_ = 0;
        return true;
    }
    return false;
}

double GetCurrentTimeLinux()
{
    struct timespec _tv;
    clock_gettime(CLOCK_REALTIME, &_tv);
    return ((double)_tv.tv_sec*1000.0 + (double)_tv.tv_nsec*0.001*0.001);
}

double GetTimeSinceStartLinux(int port_num)
{
    double _time;

    _time = GetCurrentTimeLinux() - portDataLinux[port_num].packet_start_time_;
    if (_time < 0.0)
        portDataLinux[port_num].packet_start_time_ = GetCurrentTimeLinux();

    return _time;
}

bool SetupPortLinux(int port_num, int cflag_baud)
{
    struct termios newtio;

    portDataLinux[port_num].socket_fd_ = open(portDataLinux[port_num].port_name_, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (portDataLinux[port_num].socket_fd_ < 0)
    {
        printf("[PortHandlerLinux::SetupPort] Error opening serial port!\n");
        return false;
    }

    bzero(&newtio, sizeof(newtio)); // clear struct for new port settings

    newtio.c_cflag = cflag_baud | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    // clean the buffer and activate the settings for the port
    tcflush(portDataLinux[port_num].socket_fd_, TCIFLUSH);
    tcsetattr(portDataLinux[port_num].socket_fd_, TCSANOW, &newtio);

    portDataLinux[port_num].tx_time_per_byte = (1000.0 / (double)portDataLinux[port_num].baudrate_) * 10.0;
    return true;
}

bool SetCustomBaudrateLinux(int port_num, int speed)
{
    // try to set a custom divisor
    struct serial_struct ss;
    if (ioctl(portDataLinux[port_num].socket_fd_, TIOCGSERIAL, &ss) != 0)
    {
        printf("[PortHandlerLinux::SetCustomBaudrate] TIOCGSERIAL failed!\n");
        return false;
    }

    ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
    ss.custom_divisor = (ss.baud_base + (speed / 2)) / speed;
    int closest_speed = ss.baud_base / ss.custom_divisor;

    if (closest_speed < speed * 98 / 100 || closest_speed > speed * 102 / 100)
    {
        printf("[PortHandlerLinux::SetCustomBaudrate] Cannot set speed to %d, closest is %d \n", speed, closest_speed);
        return false;
    }

    if (ioctl(portDataLinux[port_num].socket_fd_, TIOCSSERIAL, &ss) < 0)
    {
        printf("[PortHandlerLinux::SetCustomBaudrate] TIOCSSERIAL failed!\n");
        return false;
    }

    portDataLinux[port_num].tx_time_per_byte = (1000.0 / (double)speed) * 10.0;
    return true;
}

int GetCFlagBaud(int baudrate)
{
    switch (baudrate)
    {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return -1;
    }
}

/*
* PortHandlerWindows.c
*
*  Created on: 2016. 5. 4.
*      Author: leon
*/

#if defined(_WIN32) || defined(_WIN64)
#define WINDLLEXPORT
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dynamixel_sdk_windows/PortHandlerWindows.h"

#define LATENCY_TIMER  16 // msec (USB latency timer) 

typedef struct
{
    HANDLE  serial_handle_;
    LARGE_INTEGER freq_, counter_;

    int     baudrate_;
    char    port_name_[30];

    double  packet_start_time_;
    double  packet_timeout_;
    double  tx_time_per_byte_;
}PortDataWindows;

PortDataWindows *portDataWindows;

int PortHandlerWindows(const char *port_name)
{
    int _port_num;
    char _port_name[15];

    sprintf_s(_port_name, sizeof(_port_name), "\\\\.\\", port_name);

    if (portDataWindows == NULL)
    {
        _port_num = 0;
        used_port_num_ = 1;
        portDataWindows = (PortDataWindows*)calloc(1, sizeof(PortDataWindows));
        is_using_ = (bool*)calloc(1, sizeof(bool));
    }
    else
    {
        for (_port_num = 0; _port_num < used_port_num_; _port_num++)
            if (!strcmp(portDataWindows[_port_num].port_name_, port_name))
                break;

        if (_port_num == used_port_num_)
        {
            for (_port_num = 0; _port_num < used_port_num_; _port_num++)
                if (portDataWindows[_port_num].serial_handle_ != INVALID_HANDLE_VALUE)
                    break;

            if (_port_num == used_port_num_)
            {
                used_port_num_++;
                portDataWindows = (PortDataWindows*)realloc(portDataWindows, used_port_num_ * sizeof(PortDataWindows));
                is_using_ = (bool*)realloc(is_using_, used_port_num_ * sizeof(bool));
            }
        }
        else
            printf("[PortHandler setup] The port number %d has same device name... reinitialize port number %d!!\n", _port_num, _port_num);
    }

    portDataWindows[_port_num].serial_handle_ = INVALID_HANDLE_VALUE;
    portDataWindows[_port_num].baudrate_ = DEFAULT_BAUDRATE;
    portDataWindows[_port_num].packet_start_time_ = 0.0;
    portDataWindows[_port_num].packet_timeout_ = 0.0;
    portDataWindows[_port_num].tx_time_per_byte_ = 0.0;

    is_using_[_port_num] = false;

    SetPortNameWindows(_port_num, port_name);

    return _port_num;
}

bool OpenPortWindows(int port_num)
{
    return SetBaudRateWindows(port_num, portDataWindows[port_num].baudrate_);
}

void ClosePortWindows(int port_num)
{
    if (portDataWindows[port_num].serial_handle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(portDataWindows[port_num].serial_handle_);
        portDataWindows[port_num].serial_handle_ = INVALID_HANDLE_VALUE;
    }
}

void ClearPortWindows(int port_num)
{
    PurgeComm(portDataWindows[port_num].serial_handle_, PURGE_RXABORT | PURGE_RXCLEAR);
}

void SetPortNameWindows(int port_num, const char *port_name)
{
    strcpy_s(portDataWindows[port_num].port_name_, sizeof(portDataWindows[port_num].port_name_), port_name);
}

char *GetPortNameWindows(int port_num)
{
    return portDataWindows[port_num].port_name_;
}

bool SetBaudRateWindows(int port_num, const int baudrate)
{
    ClosePortWindows(port_num);

    portDataWindows[port_num].baudrate_ = baudrate;
    return SetupPortWindows(port_num, baudrate);
}

int GetBaudRateWindows(int port_num)
{
    return portDataWindows[port_num].baudrate_;
}

int ReadPortWindows(int port_num, UINT8_T *packet, int length)
{
    DWORD _dwRead = 0;

    if (ReadFile(portDataWindows[port_num].serial_handle_, packet, (DWORD)length, &_dwRead, NULL) == FALSE)
        return -1;

    return (int)_dwRead;
}

int WritePortWindows(int port_num, UINT8_T *packet, int length)
{
    DWORD _dwWrite = 0;

    if (WriteFile(portDataWindows[port_num].serial_handle_, packet, (DWORD)length, &_dwWrite, NULL) == FALSE)
        return -1;

    return (int)_dwWrite;
}

void SetPacketTimeoutWindows(int port_num, UINT16_T packet_length)
{
    portDataWindows[port_num].packet_start_time_ = GetCurrentTimeWindows(port_num);
    portDataWindows[port_num].packet_timeout_ = (portDataWindows[port_num].tx_time_per_byte_ * (double)packet_length) + (LATENCY_TIMER * 2.0) + 2.0;
}

void SetPacketTimeoutMSecWindows(int port_num, double msec)
{
    portDataWindows[port_num].packet_start_time_ = GetCurrentTimeWindows(port_num);
    portDataWindows[port_num].packet_timeout_ = msec;
}

bool IsPacketTimeoutWindows(int port_num)
{
    if (GetTimeSinceStartWindows(port_num) > portDataWindows[port_num].packet_timeout_)
    {
        portDataWindows[port_num].packet_timeout_ = 0;
        return true;
    }
    return false;
}
#undef GetCurrentTime
double GetCurrentTimeWindows(int port_num)
{
    QueryPerformanceCounter(&portDataWindows[port_num].counter_);
    QueryPerformanceFrequency(&portDataWindows[port_num].freq_);
    return (double)portDataWindows[port_num].counter_.QuadPart / (double)portDataWindows[port_num].freq_.QuadPart * 1000.0;
}

double GetTimeSinceStartWindows(int port_num)
{
    double _time;

    _time = GetCurrentTimeWindows(port_num) - portDataWindows[port_num].packet_start_time_;
    if (_time < 0.0)
        portDataWindows[port_num].packet_start_time_ = GetCurrentTimeWindows(port_num);

    return _time;
}

bool SetupPortWindows(int port_num, const int baudrate)
{
    DCB dcb;
    COMMTIMEOUTS timeouts;
    DWORD dwError;
    
    ClosePortWindows(port_num);
    
    portDataWindows[port_num].serial_handle_ = CreateFileA(portDataWindows[port_num].port_name_, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (portDataWindows[port_num].serial_handle_ == INVALID_HANDLE_VALUE)
    {
        printf("[PortHandlerWindows::SetupPort] Error opening serial port!\n");
        return false;
    }
    
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(portDataWindows[port_num].serial_handle_, &dcb) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    
    // Set baudrate
    dcb.BaudRate = (DWORD)baudrate;
    dcb.ByteSize = 8;                    // Data bit = 8bit
    dcb.Parity = NOPARITY;             // No parity
    dcb.StopBits = ONESTOPBIT;           // Stop bit = 1
    dcb.fParity = NOPARITY;             // No Parity check
    dcb.fBinary = 1;                    // Binary mode
    dcb.fNull = 0;                    // Get Null byte
    dcb.fAbortOnError = 0;
    dcb.fErrorChar = 0;
    // Not using XOn/XOff
    dcb.fOutX = 0;
    dcb.fInX = 0;
    // Not using H/W flow control
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fOutxCtsFlow = 0;
    
    if (SetCommState(portDataWindows[port_num].serial_handle_, &dcb) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    
    if (SetCommMask(portDataWindows[port_num].serial_handle_, 0) == FALSE) // Not using Comm event
        goto DXL_HAL_OPEN_ERROR;
    if (SetupComm(portDataWindows[port_num].serial_handle_, 4096, 4096) == FALSE) // Buffer size (Rx,Tx)
        goto DXL_HAL_OPEN_ERROR;
    if (PurgeComm(portDataWindows[port_num].serial_handle_, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
        goto DXL_HAL_OPEN_ERROR;
    if (ClearCommError(portDataWindows[port_num].serial_handle_, &dwError, NULL) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    
    if (GetCommTimeouts(portDataWindows[port_num].serial_handle_, &timeouts) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    // Timeout (Not using timeout)
    // Immediatly return
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1; // must not be zero.
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(portDataWindows[port_num].serial_handle_, &timeouts) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    
    portDataWindows[port_num].tx_time_per_byte_ = (1000.0 / (double)portDataWindows[port_num].baudrate_) * 10.0;
    return true;
    
    DXL_HAL_OPEN_ERROR:
        ClosePortWindows(port_num);

    return false;
}

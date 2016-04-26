/*
* PortHandlerWindows.cpp
*
*  Created on: 2016. 4. 06.
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

using namespace ROBOTIS;

PortHandlerWindows::PortHandlerWindows(const char *port_name)
    : serial_handle(INVALID_HANDLE_VALUE),
    baudrate_(DEFAULT_BAUDRATE),
    packet_start_time_(0.0),
    packet_timeout_(0.0),
    tx_time_per_byte(0.0)
{
    is_using = false;

    char PortName[15];
    sprintf_s(PortName, sizeof(PortName), "\\\\.\\", port_name);
    SetPortName(port_name);
}

bool PortHandlerWindows::OpenPort()
{
    return SetBaudRate(baudrate_);
}

void PortHandlerWindows::ClosePort()
{
    if (serial_handle != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(serial_handle);
        serial_handle = INVALID_HANDLE_VALUE;
    }
}

void PortHandlerWindows::ClearPort()
{
    PurgeComm(serial_handle, PURGE_RXABORT | PURGE_RXCLEAR);
}

void PortHandlerWindows::SetPortName(const char *port_name)
{
    strcpy_s(port_name_, sizeof(port_name_), port_name);
}

char *PortHandlerWindows::GetPortName()
{
    return port_name_;
}

bool PortHandlerWindows::SetBaudRate(const int baudrate)
{
    ClosePort();

    baudrate_ = baudrate;
    return SetupPort(baudrate);
}

int PortHandlerWindows::GetBaudRate()
{
    return baudrate_;
}

int PortHandlerWindows::GetBytesAvailable()
{
    DWORD retbyte = 2;
    BOOL res = DeviceIoControl(serial_handle, GENERIC_READ | GENERIC_WRITE, NULL, 0, 0, 0, &retbyte, (LPOVERLAPPED)NULL);
    
    printf("%d", (int)res);
    return (int)retbyte;
}

int PortHandlerWindows::ReadPort(UINT8_T *packet, int length)
{
    DWORD dwRead = 0;

    if (ReadFile(serial_handle, packet, (DWORD)length, &dwRead, NULL) == FALSE)
        return -1;

    return (int)dwRead;
}

int PortHandlerWindows::WritePort(UINT8_T *packet, int length)
{
    DWORD dwWrite = 0;

    if (WriteFile(serial_handle, packet, (DWORD)length, &dwWrite, NULL) == FALSE)
        return -1;

    return (int)dwWrite;
}

void PortHandlerWindows::SetPacketTimeout(UINT16_T packet_length)
{
    packet_start_time_ = GetCurrentTime();
    packet_timeout_ = (tx_time_per_byte * (double)packet_length) + (LATENCY_TIMER * 2.0) + 2.0;
}

void PortHandlerWindows::SetPacketTimeout(double msec)
{
    packet_start_time_ = GetCurrentTime();
    packet_timeout_ = msec;
}

bool PortHandlerWindows::IsPacketTimeout()
{
    if (GetTimeSinceStart() > packet_timeout_)
    {
        packet_timeout_ = 0;
        return true;
    }
    return false;
}
#undef GetCurrentTime
double PortHandlerWindows::GetCurrentTime()
{
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);
    return (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;
}

double PortHandlerWindows::GetTimeSinceStart()
{
    double _time;

    _time = GetCurrentTime() - packet_start_time_;
    if (_time < 0.0)
        packet_start_time_ = GetCurrentTime();

    return _time;
}

bool PortHandlerWindows::SetupPort(int baudrate)
{
    DCB Dcb;
    COMMTIMEOUTS Timeouts;
    DWORD dwError;

    ClosePort();

    serial_handle = CreateFileA(port_name_, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (serial_handle == INVALID_HANDLE_VALUE)
    {
        printf("[PortHandlerWindows::SetupPort] Error opening serial port!\n");
        return false;
    }

    Dcb.DCBlength = sizeof(DCB);
    if (GetCommState(serial_handle, &Dcb) == FALSE)
        goto DXL_HAL_OPEN_ERROR;

    // Set baudrate
    Dcb.BaudRate = (DWORD)baudrate;
    Dcb.ByteSize = 8;                    // Data bit = 8bit
    Dcb.Parity = NOPARITY;             // No parity
    Dcb.StopBits = ONESTOPBIT;           // Stop bit = 1
    Dcb.fParity = NOPARITY;             // No Parity check
    Dcb.fBinary = 1;                    // Binary mode
    Dcb.fNull = 0;                    // Get Null byte
    Dcb.fAbortOnError = 0;
    Dcb.fErrorChar = 0;
    // Not using XOn/XOff
    Dcb.fOutX = 0;
    Dcb.fInX = 0;
    // Not using H/W flow control
    Dcb.fDtrControl = DTR_CONTROL_DISABLE;
    Dcb.fRtsControl = RTS_CONTROL_DISABLE;
    Dcb.fDsrSensitivity = 0;
    Dcb.fOutxDsrFlow = 0;
    Dcb.fOutxCtsFlow = 0;

    if (SetCommState(serial_handle, &Dcb) == FALSE)
        goto DXL_HAL_OPEN_ERROR;

    if (SetCommMask(serial_handle, 0) == FALSE) // Not using Comm event
        goto DXL_HAL_OPEN_ERROR;
    if (SetupComm(serial_handle, 4096, 4096) == FALSE) // Buffer size (Rx,Tx)
        goto DXL_HAL_OPEN_ERROR;
    if (PurgeComm(serial_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
        goto DXL_HAL_OPEN_ERROR;
    if (ClearCommError(serial_handle, &dwError, NULL) == FALSE)
        goto DXL_HAL_OPEN_ERROR;

    if (GetCommTimeouts(serial_handle, &Timeouts) == FALSE)
        goto DXL_HAL_OPEN_ERROR;
    // Timeout (Not using timeout)
    // Immediatly return
    Timeouts.ReadIntervalTimeout = 0;
    Timeouts.ReadTotalTimeoutMultiplier = 0;
    Timeouts.ReadTotalTimeoutConstant = 1; // must not be zero.
    Timeouts.WriteTotalTimeoutMultiplier = 0;
    Timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(serial_handle, &Timeouts) == FALSE)
        goto DXL_HAL_OPEN_ERROR;

    tx_time_per_byte = (1000.0 / (double)baudrate_) * 10.0;
    return true;

DXL_HAL_OPEN_ERROR:
    ClosePort();
    return false;
}

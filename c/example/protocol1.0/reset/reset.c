/*
* reset.c
*
*  Created on: 2016. 5. 16.
*      Author: leon
*/

//
// *********     Factory Reset Example      *********
//
//
// Available Dynamixel model on this example : All models using Protocol 1.0
// This example is designed for using a Dynamixel MX-28, and an USB2DYNAMIXEL.
// To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below "#define"d variables yourself.
// Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 1000000 [1M])
//

// Be aware that:
// This example resets all properties of Dynamixel to default values, such as %% ID : 1 / Baudnum : 34 (Baudrate : 57600)
//

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdio.h>
#include "DynamixelSDK.h"                                   // Uses Dynamixel SDK library

// Control table address
#define ADDR_MX_BAUDRATE                4                   // Control table address is different in Dynamixel model

// Protocol version
#define PROTOCOL_VERSION                1.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID                          1                   // Dynamixel ID: 1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

#define FACTORYRST_DEFAULTBAUDRATE      57600               // Dynamixel baudrate set by factoryreset
#define NEW_BAUDNUM                     1                   // New baudnum to recover Dynamixel baudrate as it was
#define OPERATION_MODE                  0x00                // Mode is unavailable in Protocol 1.0 Reset

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque

#ifdef __linux__
int _getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int _kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

void msecSleep(int waitTime)
{
#ifdef __linux__
    usleep(waitTime * 1000);
#elif defined(_WIN32) || defined(_WIN64)
    Sleep(waitTime);
#endif
}

int main()
{
    // Initialize PortHandler Structs
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    int port_num = PortHandler(DEVICENAME);

    // Initialize PacketHandler Structs 
    PacketHandler();

    int dxl_comm_result = COMM_TX_FAIL;             // Communication result

    UINT8_T dxl_error = 0;                          // Dynamixel error
    UINT8_T dxl_baudnum_read;                       // Read baudnum

     // Open port
    if (OpenPort(port_num))
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    // Set port baudrate
    if (SetBaudRate(port_num, BAUDRATE))
    {
        printf("Succeeded to change the baudrate!\n");
    }
    else
    {
        printf("Failed to change the baudrate!\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    // Read present baudrate of the controller
    printf("Now the controller baudrate is : %d\n", GetBaudRate(port_num));

    // Try factoryreset
    printf("[ID:%03d] Try factoryreset : ", DXL_ID);
    FactoryReset(port_num, PROTOCOL_VERSION, DXL_ID, OPERATION_MODE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
    {
        printf("Aborted\n");
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
        return 0;
    }
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);

    // Wait for reset
    printf("Wait for reset...\n");
    msecSleep(2000);

    printf("[ID:%03d] FactoryReset Success!\n", DXL_ID);

    // Set controller baudrate to dxl default baudrate
    if (SetBaudRate(port_num, FACTORYRST_DEFAULTBAUDRATE))
    {
        printf("Succeed to change the controller baudrate to : %d\n", FACTORYRST_DEFAULTBAUDRATE);
    }
    else
    {
        printf("Failed to change the controller baudrate\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    // Read Dynamixel baudnum
    dxl_baudnum_read = Read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);
    else
        printf("[ID:%03d] Dynamixel baudnum is now : %d\n", DXL_ID, dxl_baudnum_read);

    // Write new baudnum
    Write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE, NEW_BAUDNUM);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);
    else
        printf("[ID:%03d] Set Dynamixel baudnum to : %d\n", DXL_ID, NEW_BAUDNUM);

    // Set port baudrate to BAUDRATE
    if (SetBaudRate(port_num, BAUDRATE))
    {
        printf("Succeed to change the controller baudrate to : %d\n", BAUDRATE);
    }
    else
    {
        printf("Failed to change the controller baudrate\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    msecSleep(200);

    // Read Dynamixel baudnum
    dxl_baudnum_read = Read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);
    else
        printf("[ID:%03d] Dynamixel Baudnum is now : %d\n", DXL_ID, dxl_baudnum_read);

    // Close port
    ClosePort(port_num);

    return 0;
}

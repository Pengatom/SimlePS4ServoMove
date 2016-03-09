/*
 * FactoryReset.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Factory Reset Example      *********
//
//
// Available DXL model on this example : All models using Protocol 2.0
// This example is tested with a DXL PRO 54-200, and an USB2DYNAMIXEL
// Be sure that DXL PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000) / Min voltage limit : under 110 (when using 12V power supplement)
//

// Be aware that:
// This example resets all properties of DXL to default values, such as %% ID : 1 / Baudnum : 1 (Baudrate : 57600) / Min voltage limit : 150
//

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DynamixelSDK.h"

// Control table address
#define ADDR_PRO_BAUDRATE               8

// Protocol version
#define PROTOCOL_VERSION                2.0

// Default setting
#define DXL_ID                          1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"

#define FACTORYRST_DEFAULTBAUDRATE      57600       // DXL baudrate set by factoryreset
#define NEW_BAUDNUM                     3           // New baudnum to recover DXL baudrate as it was
#define OPERATION_MODE                  0x01        // 0xFF : reset all values
                                                    // 0x01 : reset all values except ID
                                                    // 0x02 : reset all values except ID and baudrate

using namespace ROBOTIS;

#ifdef __linux__
int _getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
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

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

int main()
{
    // Initialize PortHandler instance
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    PortHandler *portHandler = PortHandler::GetPortHandler(DEVICENAME);

    // Initialize Packethandler instance
    PacketHandler *packetHandler = PacketHandler::GetPacketHandler(PROTOCOL_VERSION);

    int dxl_comm_result = COMM_TX_FAIL;         // Communication result

    UINT8_T dxl_baudnum_read;                   // Read baudnum
    UINT8_T dxl_error = 0;                      // DXL error

    // Open port
    if( portHandler->OpenPort() )
    {
        printf( "Succeeded to open the port!\n" );
    }
    else
    {
        printf( "Failed to open the port!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Set port baudrate
    if( portHandler->SetBaudRate(BAUDRATE) )
    {
        printf( "Succeeded to change the baudrate!\n" );
    }
    else
    {
        printf( "Failed to change the baudrate!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Read present baudrate of the controller
    printf("Now the controller baudrate is : %d\n", portHandler->GetBaudRate());

    // Try factoryreset
    printf("[ID:%03d] Try factoryreset : ", DXL_ID);
    dxl_comm_result = packetHandler->FactoryReset(portHandler, DXL_ID, OPERATION_MODE, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Wait for reset
    printf("Wait for reset...\n");
    usleep(2000 * 1000);

    printf("[ID:%03d] FactoryReset Success!\n", DXL_ID);

    // Set port baudrate to factoryreset default baudrate
    if(portHandler->SetBaudRate(FACTORYRST_DEFAULTBAUDRATE))
    {
    	printf( "Succeed to change the controller baudrate to : %d\n", FACTORYRST_DEFAULTBAUDRATE );
    }
    else
    {
        printf( "Failed to change the controller baudrate\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Read DXL baudnum
    dxl_comm_result = packetHandler->Read1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_BAUDRATE, &dxl_baudnum_read, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Write new baudnum
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_BAUDRATE, NEW_BAUDNUM, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Set controller baudrate to 1000000
    if(portHandler->SetBaudRate(BAUDRATE))
    {
    	printf( "Succeed to change the controller baudrate to : %d\n", BAUDRATE );
    }
    else
    {
        printf( "Failed to change the controller baudrate\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Read DXL baudnum
    dxl_comm_result = packetHandler->Read1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_BAUDRATE, &dxl_baudnum_read, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Close port
    portHandler->ClosePort();

    printf( "Press Enter key to terminate...\n" );
    _getch();
    return 0;
}

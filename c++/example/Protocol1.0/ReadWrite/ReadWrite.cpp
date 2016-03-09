/*
 * ReadWrite.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Read and Write Example      *********
//
//
// Available DXL model on this example : All models using Protocol 1.0
// This example is tested with a DXL MX-28, and an USB2DYNAMIXEL
// Be sure that DXL MX properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 1000000)
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
#define ADDR_MX_TORQUE_ENABLE           24
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_PRESENT_POSITION        36
#define ADDR_MX_MOVING                  46

// Protocol version
#define PROTOCOL_VERSION                1.0

// Default setting
#define DXL_ID                          1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"

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

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;             // Communication result
    int dxl_goal_position[2] = {100, 1000};         // Goal position

    UINT8_T dxl_torque_enable = 1;                  // Value for torque enable
	UINT8_T dxl_torque_disable = 0;	                // Torque unable
    UINT8_T dxl_error = 0;                          // DXL error
    UINT8_T	dxl_moving = 0;                         // DXL moving status
    UINT16_T dxl_present_position = 0;              // Present position

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

    // Enable DXL Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Write goal position
        dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        do
        {
            // Read present position
            dxl_comm_result = packetHandler->Read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);
            else if(dxl_error != 0)
                packetHandler->PrintRxPacketError(dxl_error);

            printf("[ID:%03d] GoalPos:%03d  PresPos:%03d\n", DXL_ID, dxl_goal_position[index], dxl_present_position);

            // Read Moving status
            dxl_comm_result = packetHandler->Read1ByteTxRx(portHandler, DXL_ID, ADDR_MX_MOVING, &dxl_moving, &dxl_error);
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);
            else if(dxl_error != 0)
                packetHandler->PrintRxPacketError(dxl_error);
        }while(dxl_moving == 1);

        // Change goal position
        if( index == 0 )
            index = 1;
        else
            index = 0;
    }

    // Disable DXL Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
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

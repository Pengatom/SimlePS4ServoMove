/*
 * MultiPort.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     MultiPort Example      *********
//
//
// Available DXL model on this example : All models using Protocol 2.0
// This example is tested with two DXL PRO 54-200, and two USB2DYNAMIXEL
// Be sure that DXL PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000) / Min voltage limit : under 110 (when using 12V power supplement)
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
#define ADDR_PRO_TORQUE_ENABLE          562
#define ADDR_PRO_GOAL_POSITION          596
#define ADDR_PRO_PRESENT_POSITION       611

// Protocol version
#define PROTOCOL_VERSION                2.0

// Default setting
#define DXL1_ID                         1                   // DXL#1 ID:1
#define DXL2_ID                         2                   // DXL#2 ID:2
#define BAUDRATE                            1000000
#define DEVICENAME1                      "/dev/ttyUSB0"
#define DEVICENAME2                      "/dev/ttyUSB1"

#define STOP_MOVING_MARGIN              20                  // Criteria for considering DXL moving status as stopped

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
    // Initialize Porthandlers instance
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    PortHandler *portHandler1 = (PortHandler*)PortHandler::GetPortHandler(DEVICENAME1);
    PortHandler *portHandler2 = (PortHandler*)PortHandler::GetPortHandler(DEVICENAME2);

    // Initialize Packethandler instance
    // Get Protocol2PacketHandler methods and members
    PacketHandler *packetHandler = PacketHandler::GetPacketHandler(PROTOCOL_VERSION);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result
    int dxl_goal_position[2] = {-150000, 150000};           // Goal position

    UINT8_T dxl_torque_enable = 1;                          // Value for torque enable
	UINT8_T dxl_torque_disable = 0;                         // Value for torque disable
    UINT8_T dxl_error = 0;                                  // DXL error
    INT32_T dxl_present_position1, dxl_present_position2;   // Present position

    // Open port1
    if( portHandler1->OpenPort() )
    {
        printf( "Succeeded to open the port1!\n" );
    }
    else
    {
        printf( "Failed to open the port1!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Open port2
    if( portHandler2->OpenPort() )
    {
        printf( "Succeeded to open the port2!\n" );
    }
    else
    {
        printf( "Failed to open the port2!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Set port1 baudrate
    if(portHandler1->SetBaudRate(BAUDRATE) )
    {
    	printf( "Succeed to change the baudrate of port1!\n" );
    }
    else
    {
        printf( "Failed to change the baudrate of port1!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Set port2 baudrate
    if(portHandler2->SetBaudRate(BAUDRATE) )
    {
    	printf( "Succeed to change the baudrate of port2!\n" );
    }
    else
    {
        printf( "Failed to change the baudrate of port2!\n" );
        printf( "Press any key to terminate...\n" );
        _getch();
        return 0;
    }

    // Enable DXL#1 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler1, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL1_ID);

    // Enable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler2, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL2_ID);

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Write DXL#1 goal position
        dxl_comm_result = packetHandler->Write4ByteTxRx(portHandler1, DXL1_ID, ADDR_PRO_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        // Write DXL#2 goal position
        dxl_comm_result = packetHandler->Write4ByteTxRx(portHandler2, DXL2_ID, ADDR_PRO_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        do
        {
            usleep(50*1000);

            // Read DXL#1 present position
            dxl_comm_result = packetHandler->Read4ByteTxRx(portHandler1, DXL1_ID, ADDR_PRO_PRESENT_POSITION, (UINT32_T*)&dxl_present_position1, &dxl_error);
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);
            else if(dxl_error != 0)
                packetHandler->PrintRxPacketError(dxl_error);

            // Read DXL#2 present position
            dxl_comm_result = packetHandler->Read4ByteTxRx(portHandler2, DXL2_ID, ADDR_PRO_PRESENT_POSITION, (UINT32_T*)&dxl_present_position2, &dxl_error);
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);
            else if(dxl_error != 0)
                packetHandler->PrintRxPacketError(dxl_error);

            printf("[ID:%03d] GoalPos:%03d  PresPos:%03d [ID:%03d] GoalPos:%03d  PresPos:%03d\n", DXL1_ID, dxl_goal_position[index], dxl_present_position1, DXL2_ID, dxl_goal_position[index], dxl_present_position2);
        }while((abs(dxl_goal_position[index] - dxl_present_position1) > STOP_MOVING_MARGIN) || (abs(dxl_goal_position[index] - dxl_present_position2) > STOP_MOVING_MARGIN));

        // Change goal position
        if( index == 0 )
            index = 1;
        else
            index = 0;
    }

    // Disable DXL#1 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler1, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Disable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler2, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Close port1
    portHandler1->ClosePort();

    // Close port2
    portHandler2->ClosePort();

    printf( "Press Enter key to terminate...\n" );
    _getch();
    return 0;
}

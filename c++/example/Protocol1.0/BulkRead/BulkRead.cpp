/*
 * BulkRead.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Bulk Read Example      *********
//
//
// Available DXL model on this example : MX or X series set to Protocol 1.0
// This example is tested with two DXL MX-28, and an USB2DYNAMIXEL
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

// Data Byte Length
#define LEN_MX_TORQUE_ENABLE            1
#define LEN_MX_GOAL_POSITION            2
#define LEN_MX_PRESENT_POSITION         2
#define LEN_MX_MOVING                   1

// Protocol version
#define PROTOCOL_VERSION                1.0

// Default setting
#define DXL1_ID                         1                   // DXL#1 ID:1
#define DXL2_ID                         2                   // DXL#2 ID:2
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"

#define STOP_MOVING_MARGIN              10                  // Criteria for considering DXL moving status as stopped

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

    // Initialize Groupbulkread instance
    GroupBulkRead groupBulkRead(portHandler, packetHandler);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result
    int dxl_addparam_result = false;                        // AddParam result
    int dxl_getdata_result = false;                         // GetParam result
    int dxl_goal_position[2] = {100, 1000};                 // Goal position

    UINT8_T dxl_torque_enable = 1;                          // Value for torque enable
    UINT8_T dxl_torque_disable = 0;                         // Value for torque disable
    UINT8_T dxl_error = 0;                                  // DXL error
    UINT8_T dxl_moving2 = 0;                                // DXL moving status
    UINT16_T dxl_present_position1 = 0;                     // Present position

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

    // Enable DXL#1 Torque
    dxl_comm_result= packetHandler->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL1_ID);


    // Enable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL2_ID);


    // Add parameter storage for DXL#1 present position value
    dxl_addparam_result = groupBulkRead.AddParam(DXL1_ID, ADDR_MX_PRESENT_POSITION, LEN_MX_PRESENT_POSITION);
    if( dxl_addparam_result != true )
    {
        fprintf(stderr, "[ID:%03d] grouBulkRead addparam failed", DXL1_ID);
        return 0;
    }

    // Add parameter storage for DXL#2 present moving value
    dxl_addparam_result = groupBulkRead.AddParam(DXL2_ID, ADDR_MX_MOVING, LEN_MX_MOVING);
    if( dxl_addparam_result != true )
    {
        fprintf(stderr, "[ID:%03d] grouBulkRead addparam failed", DXL2_ID);
        return 0;
    }

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Write DXL#1 goal position
        dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL1_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        // Write DXL#2 goal position
        dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL2_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        do{
            // Bulkread present position and moving status
            dxl_comm_result = groupBulkRead.TxRxPacket();
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);

            // Get DXL#1 present position value
            dxl_getdata_result = groupBulkRead.GetData(DXL1_ID, ADDR_MX_PRESENT_POSITION, &dxl_present_position1);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupBulkRead getdata failed", DXL1_ID);
                return 0;
            }

            // Get DXL#2 moving status value
            dxl_getdata_result = groupBulkRead.GetData(DXL2_ID, ADDR_MX_MOVING, &dxl_moving2);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupBulkRead getdata failed", DXL2_ID);
                return 0;
            }

            printf("[ID:%03d] Present Position : %d \t [ID:%03d] Is Moving : %d\n", DXL1_ID, dxl_present_position1, DXL2_ID, dxl_moving2);
        }while((abs(dxl_goal_position[index] - dxl_present_position1) > STOP_MOVING_MARGIN) || (dxl_moving2 == 1));

        // Change goal position
        if( index == 0 )
            index = 1;
        else
            index = 0;
    }
    // Clear bulkread parameter storage
    groupBulkRead.ClearParam();

    // Disable DXL#1 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Disable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
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

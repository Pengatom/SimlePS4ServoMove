/*
 * SyncReadWrite.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Sync Read and Sync Write Example      *********
//
//
// Available DXL model on this example : All models using Protocol 2.0
// This example is tested with two DXL PRO 54-200, and an USB2DYNAMIXEL
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

// Data Byte Length
#define LEN_PRO_TORQUE_ENABLE           1
#define LEN_PRO_GOAL_POSITION           4
#define LEN_PRO_PRESENT_POSITION        4

// Protocol version
#define PROTOCOL_VERSION                2.0

// Default setting
#define DXL1_ID                         1                   // DXL#1 ID:1
#define DXL2_ID                         2                   // DXL#2 ID:2
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"

#define STOP_MOVING_MARGIN				20                  // Criteria for considering DXL moving status as stopped

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

    // Initialize Groupsyncwrite instance for Goal Position
    GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION);

    // Initialize Groupsyncread instance for Present Position
    GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR_PRO_PRESENT_POSITION, LEN_PRO_PRESENT_POSITION);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result
    int dxl_addparam_result = false;                        // AddParam result
    int dxl_getdata_result = false;                         // GetParam result
    int dxl_goal_position[2] = {-150000, 150000};           // Goal position

    UINT8_T dxl_torque_enable = 1;                          // Value for torque enable
    UINT8_T dxl_torque_disable = 0;                         // Value for torque unable
    UINT8_T dxl_error = 0;                                  // DXL error
    UINT8_T param_goal_position[4];
    INT32_T dxl_present_position1, dxl_present_position2;  // Present position

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
	dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
	if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL1_ID);

    // Enable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);
    printf("DXL#%d successfully connected\n", DXL2_ID);

    // Add parameter storage for DXL#1 present position value
    dxl_addparam_result = groupSyncRead.AddParam(DXL1_ID);
    if( dxl_addparam_result != true )
    {
        fprintf(stderr, "[ID:%03d] groupSyncRead addparam failed", DXL1_ID);
        return 0;
    }

    // Add parameter storage for DXL#2 present position value
    dxl_addparam_result = groupSyncRead.AddParam(DXL2_ID);
    if( dxl_addparam_result != true )
    {
        fprintf(stderr, "[ID:%03d] groupSyncRead addparam failed", DXL2_ID);
        return 0;
    }

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Allocate goal position value into byte array
        param_goal_position[0] = DXL_LOBYTE(DXL_LOWORD(dxl_goal_position[index]));
        param_goal_position[1] = DXL_HIBYTE(DXL_LOWORD(dxl_goal_position[index]));
        param_goal_position[2] = DXL_LOBYTE(DXL_HIWORD(dxl_goal_position[index]));
        param_goal_position[3] = DXL_HIBYTE(DXL_HIWORD(dxl_goal_position[index]));

        // Add DXL#1 goal position value to the Syncwrite storage
        dxl_addparam_result = groupSyncWrite.AddParam(DXL1_ID, param_goal_position);
        if( dxl_addparam_result != true )
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", DXL1_ID);
            return 0;
        }

        // Add DXL#2 goal position value to the Syncwrite storage
        dxl_addparam_result = groupSyncWrite.AddParam(DXL2_ID, param_goal_position);
        if( dxl_addparam_result != true )
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", DXL2_ID);
            return 0;
        }

        // Syncwrite goal position
        dxl_comm_result = groupSyncWrite.TxPacket();
        if(dxl_comm_result != COMM_SUCCESS)
            packetHandler->PrintTxRxResult(dxl_comm_result);
        else if(dxl_error != 0)
            packetHandler->PrintRxPacketError(dxl_error);

        // Clear syncwrite parameter storage
        groupSyncWrite.ClearParam();

        do{
            //usleep(40*1000);

            // Syncread present position
            dxl_comm_result = groupSyncRead.TxRxPacket();
            if(dxl_comm_result != COMM_SUCCESS)
                packetHandler->PrintTxRxResult(dxl_comm_result);
            else if(dxl_error != 0)
                packetHandler->PrintRxPacketError(dxl_error);

            // Get DXL#1 present position value
            dxl_getdata_result = groupSyncRead.GetData(DXL1_ID, ADDR_PRO_PRESENT_POSITION, (UINT32_T*)&dxl_present_position1);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupSyncRead getdata failed", DXL2_ID);
                return 0;
            }

            // Get DXL#2 present position value
            dxl_getdata_result = groupSyncRead.GetData(DXL2_ID, ADDR_PRO_PRESENT_POSITION, (UINT32_T*)&dxl_present_position2);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupSyncRead getdata failed", DXL2_ID);
                return 0;
            }

            printf("[ID:%03d] GoalPos:%03d  PresPos:%03d [ID:%03d] GoalPos:%03d  PresPos:%03d\n", DXL1_ID, dxl_goal_position[index], dxl_present_position1, DXL2_ID, dxl_goal_position[index], dxl_present_position2);
        }while((abs(dxl_goal_position[index] - dxl_present_position1) > STOP_MOVING_MARGIN) || (abs(dxl_goal_position[index] - dxl_present_position2) > STOP_MOVING_MARGIN));

        if( index == 0 )
            index = 1;
        else
            index = 0;
    }

    // Clear syncread parameter storage
    groupSyncRead.ClearParam();

    // Disable DXL#1 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if(dxl_comm_result != COMM_SUCCESS)
        packetHandler->PrintTxRxResult(dxl_comm_result);
    else if(dxl_error != 0)
        packetHandler->PrintRxPacketError(dxl_error);

    // Disable DXL#2 Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
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

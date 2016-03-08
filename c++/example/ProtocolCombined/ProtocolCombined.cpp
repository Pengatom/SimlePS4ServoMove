/*
 * ProtocolCombined.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Protocol Combined Example      *********
//
//
// Available DXL model on this example : All models using Protocol 1.0 and 2.0
// This example is tested with a DXL MX-28, a DXL PRO 54-200 and an USB2DYNAMIXEL
// Be sure that properties of DXL MX and PRO are already set as %% MX - ID : 1 / Baudnum : 1 (Baudrate : 1000000) , PRO - ID : 1 / Baudnum : 3 (Baudrate : 1000000)
//

// Be aware that:
// This example configures two different control tables (especially, if it uses DXL and DXL PRO). It may modify critical DXL parameter on the control table, if DXLs have wrong ID.
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

// DXL Error bit for Protocol 1.0
#define ERRBIT_PROTOCOL1_VOLTAGE        1
#define ERRBIT_PROTOCOL1_ANGLE          2
#define ERRBIT_PROTOCOL1_OVERHEAT       4
#define ERRBIT_PROTOCOL1_RANGE          8
#define ERRBIT_PROTOCOL1_CHECKSUM       16
#define ERRBIT_PROTOCOL1_OVERLOAD       32
#define ERRBIT_PROTOCOL1_INSTRUCTION    64

// DXL Error bit for Protocol 2.0
#define ERRBIT_PROTOCOL2_RESULTFAIL     1
#define ERRBIT_PROTOCOL2_INSTRUCTION    2
#define ERRBIT_PROTOCOL2_CRC            3
#define ERRBIT_PROTOCOL2_DATARANGE      4
#define ERRBIT_PROTOCOL2_DATALENGTH     5
#define ERRBIT_PROTOCOL2_DATALIMIT      6
#define ERRBIT_PROTOCOL2_ADDRESS        7

// Control table address for DXL MX
#define ADDR_MX_TORQUE_ENABLE           24
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_PRESENT_POSITION        36

// Control table address for DXL PRO
#define ADDR_PRO_TORQUE_ENABLE          562
#define ADDR_PRO_GOAL_POSITION          596
#define ADDR_PRO_PRESENT_POSITION       611

// Protocol versions
#define PROTOCOL_VERSION1               1.0
#define PROTOCOL_VERSION2               2.0

// Default setting
#define DXL1_ID                         1               // MX
#define DXL2_ID                         2               // PRO
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"

#define STOP_MOVING_MARGIN1             10
#define STOP_MOVING_MARGIN2             20

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

// Print communication result
void PrintCommStatus(int CommStatus)
{
    switch(CommStatus)
    {
    case COMM_PORT_BUSY:
        printf("COMM_PORT_BUSY: Port is in use!\n");
        break;

    case COMM_TX_FAIL:
        printf("COMM_TXFAIL: Failed transmit instruction packet!\n");
        break;

    case COMM_RX_FAIL:
        printf("COMM_RXFAIL: Failed get status packet from device!\n");
        break;

    case COMM_TX_ERROR:
        printf("COMM_TXERROR: Incorrect instruction packet!\n");
        break;

    case COMM_RX_WAITING:
        printf("COMM_RXWAITING: Now recieving status packet!\n");
        break;

    case COMM_RX_TIMEOUT:
        printf("COMM_RXTIMEOUT: There is no status packet!\n");
        break;

    case COMM_RX_CORRUPT:
        printf("COMM_RXCORRUPT: Incorrect status packet!\n");
        break;

    case COMM_NOT_AVAILABLE:
        printf("COMM_NOT_AVAILABLE: Protocol does not support This function!\n");
        break;

    default:
        printf("This is unknown error code!\n");
        break;
    }
}

// Print error bit of status packet for Protocol 1.0
void PrintErrorCode_Protocol1(int ErrorCode)
{
    if(ErrorCode & ERRBIT_PROTOCOL1_VOLTAGE)
        printf("Input voltage error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_ANGLE)
        printf("Angle limit error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_OVERHEAT)
        printf("Overheat error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_RANGE)
        printf("Out of range error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_CHECKSUM)
        printf("Checksum error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_OVERLOAD)
        printf("Overload error!\n");

    if(ErrorCode & ERRBIT_PROTOCOL1_INSTRUCTION)
        printf("Instruction code error!\n");
}

// Print error bit of status packet for Protocol 2.0
void PrintErrorCode_Protocol2(int ErrorCode)
{
    if(ErrorCode & 0x80)
        printf("Hardware error occurred. Check the error at Control Table (Hardware Error Status)!\n");

    int err = ErrorCode & 0x7F;

    switch(err)
    {
    case ERRBIT_PROTOCOL2_RESULTFAIL:
        printf("Failed to deal with instruction packet!\n");
        break;

    case ERRBIT_PROTOCOL2_INSTRUCTION:
        printf("Undefined instruction!\n");
        break;

    case ERRBIT_PROTOCOL2_CRC:
        printf("CRC doesn't match!\n");
        break;

    case ERRBIT_PROTOCOL2_DATARANGE:
        printf("Data is out of range!\n");
        break;

    case ERRBIT_PROTOCOL2_DATALENGTH:
        printf("Data is shorter than expected!\n");
        break;
          
    case ERRBIT_PROTOCOL2_DATALIMIT:
        printf("Data is too long!\n");
        break;

    case ERRBIT_PROTOCOL2_ADDRESS:
        printf("Writing or Reading is not available to target address!\n");
        break;

    default:
        printf("Unknown error code!\n");
        break;
    }
}

int main()
{
    // Initialize PortHandler instance
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    PortHandler *portHandler = (PortHandler*)PortHandler::GetPortHandler(DEVICENAME);

    // Initialize Packethandler instance for Protocol 1.0
    // Get Protocol1PacketHandler methods and members
    PacketHandler *packetHandler1 = PacketHandler::GetPacketHandler(PROTOCOL_VERSION1);

    // Initialize Packethandler instance for Protocol 2.0
    // Get Protocol1PacketHandler methods and members
    PacketHandler *packetHandler2 = PacketHandler::GetPacketHandler(PROTOCOL_VERSION2);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result
    int dxl_goal_position1[2] = { 100, 1000 };              // Goal position1
    int dxl_goal_position2[2] = {-150000, 150000 };         // Goal position2

    UINT8_T dxl_torque_enable = 1;                          // Value for torque enable
    UINT8_T dxl_torque_disable = 0;                         // Value for torque unable
    UINT8_T dxl_error = 0;                                  // DXL error
    UINT16_T dxl_present_position1;                         // Present position1
    INT32_T dxl_present_position2;                          // Present position2

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

    // Enable DXL#1 torque
    dxl_comm_result = packetHandler1->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result == COMM_SUCCESS)
    {
    	if(dxl_error != 0)
    	{
    		PrintErrorCode_Protocol1(dxl_error);
    		return 0;
    	}
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }
    printf("DXL#%d successfully connected\n", DXL1_ID);

    // Enable DXL#2 torque
    dxl_comm_result = packetHandler2->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if(dxl_comm_result == COMM_SUCCESS)
    {
        if(dxl_error != 0)
        {
            PrintErrorCode_Protocol2(dxl_error);
            return 0;
        }
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }
    printf("DXL#%d successfully connected\n", DXL2_ID);

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Write DXL#1 goal position
        dxl_comm_result = packetHandler1->Write2ByteTxRx(portHandler, DXL1_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position1[index], &dxl_error);
        if(dxl_comm_result == COMM_SUCCESS)
        {
            if(dxl_error != 0)
            {
                PrintErrorCode_Protocol1(dxl_error);
                return 0;
            }
        }
        else
        {
            PrintCommStatus(dxl_comm_result);
            return 0;
        }

        // Write DXL#2 goal position
        dxl_comm_result = packetHandler2->Write4ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_GOAL_POSITION, dxl_goal_position2[index], &dxl_error);
        if(dxl_comm_result == COMM_SUCCESS)
        {
            if(dxl_error != 0)
            {
                PrintErrorCode_Protocol2(dxl_error);
                return 0;
            }
        }
        else
        {
            PrintCommStatus(dxl_comm_result);
            return 0;
        }

        do
        {
            usleep(40*1000);

            // Read DXL#1 present position
            dxl_comm_result = packetHandler1->Read2ByteTxRx(portHandler, DXL1_ID, ADDR_MX_PRESENT_POSITION, &dxl_present_position1, &dxl_error);
            if(dxl_comm_result == COMM_SUCCESS)
            {
                if(dxl_error != 0)
                {
                    PrintErrorCode_Protocol1(dxl_error);
                    return 0;
                }
            }
            else
            {
                PrintCommStatus(dxl_comm_result);
                return 0;
            }

            // Read DXL#2 present position
            dxl_comm_result = packetHandler2->Read4ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_PRESENT_POSITION, (UINT32_T*)&dxl_present_position2, &dxl_error);
            if(dxl_comm_result == COMM_SUCCESS)
            {
                if(dxl_error != 0)
                {
                    PrintErrorCode_Protocol2(dxl_error);
                    return 0;
                }
            }
            else
            {
                PrintCommStatus(dxl_comm_result);
                return 0;
            }

            printf("[ID:%03d] GoalPos:%03d  PresPos:%03d [ID:%03d] GoalPos:%03d  PresPos:%03d\n", DXL1_ID, dxl_goal_position1[index], dxl_present_position1, DXL2_ID, dxl_goal_position2[index], dxl_present_position2);
        }while((abs(dxl_goal_position1[index] - dxl_present_position1) > STOP_MOVING_MARGIN1) || (abs(dxl_goal_position2[index] - dxl_present_position2) > STOP_MOVING_MARGIN2));

        // Change goal position
        if( index == 0 )
            index = 1;
        else
            index = 0;
    }

    // Disable DXL#1 Torque
    dxl_comm_result = packetHandler1->Write1ByteTxRx(portHandler, DXL1_ID, ADDR_MX_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if( dxl_comm_result == COMM_SUCCESS )
    {
        if(dxl_error != 0)
        {
            PrintErrorCode_Protocol2(dxl_error);
            return 0;
        }
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }

    // Disable DXL#2 Torque
    dxl_comm_result = packetHandler2->Write1ByteTxRx(portHandler, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if( dxl_comm_result == COMM_SUCCESS )
    {
        if(dxl_error != 0)
        {
            PrintErrorCode_Protocol2(dxl_error);
            return 0;
        }
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }

	// Close port
    portHandler->ClosePort();

    printf( "Press Enter key to terminate...\n" );
    _getch();
    return 0;
}

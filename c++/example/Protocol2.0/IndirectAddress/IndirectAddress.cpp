/*
 * IndirectAddress.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Indirect Address Example      *********
//
//
// Available DXL model on this example : All models using Protocol 2.0
// This example is tested with a DXL PRO 54-200, and an USB2DYNAMIXEL
// Be sure that DXL PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000) / Min voltage limit : under 110 (DXL hardware error would be occurred when it is supplied by 12V power supplement)
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

// DXL Error bit for Protocol 2.0
#define ERRBIT_PROTOCOL2_RESULTFAIL     1
#define ERRBIT_PROTOCOL2_INSTRUCTION    2
#define ERRBIT_PROTOCOL2_CRC            3
#define ERRBIT_PROTOCOL2_DATARANGE      4
#define ERRBIT_PROTOCOL2_DATALENGTH     5
#define ERRBIT_PROTOCOL2_DATALIMIT      6
#define ERRBIT_PROTOCOL2_ADDRESS        7

// Control table address
#define ADDR_PRO_INDIRECTADDRESS_FOR_WRITE      49          // EEPROM region
#define ADDR_PRO_INDIRECTADDRESS_FOR_READ       59          // EEPROM region
#define ADDR_PRO_TORQUE_ENABLE                  562
#define ADDR_PRO_LED_RED                        563
#define ADDR_PRO_GOAL_POSITION                  596
#define ADDR_PRO_MOVING                         610
#define ADDR_PRO_PRESENT_POSITION               611
#define ADDR_PRO_INDIRECTDATA_FOR_WRITE         634
#define ADDR_PRO_INDIRECTDATA_FOR_READ          639

// Data Byte Length
#define LEN_PRO_INDIRECTDATA_FOR_WRITE          5
#define LEN_PRO_INDIRECTDATA_FOR_READ           5

// Protocol version
#define PROTOCOL_VERSION                        2.0

// Default setting
#define DXL_ID                                  1
#define BAUDRATE                                1000000
#define DEVICENAME                              "/dev/ttyUSB0"

#define STOP_MOVING_MARGIN	                    20               // Criteria for considering DXL moving status as stopped

using namespace ROBOTIS;

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
    PortHandler *portHandler = PortHandler::GetPortHandler(DEVICENAME);

    // Initialize Packethandler instance
    PacketHandler *packetHandler = PacketHandler::GetPacketHandler(PROTOCOL_VERSION);

    // Initialize Groupsyncwrite instance
    GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_PRO_INDIRECTDATA_FOR_WRITE, LEN_PRO_INDIRECTDATA_FOR_WRITE);

    // Initialize Groupsyncread instance
    GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR_PRO_INDIRECTDATA_FOR_READ, LEN_PRO_INDIRECTDATA_FOR_READ);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result
    int dxl_addparam_result = false;                        // AddParam result
    int dxl_getdata_result = false;                         // GetParam result
    int dxl_goal_position[2] = {-150000, 150000};           // Goal Position

    UINT8_T dxl_torque_enable = 1;                          // Value for torque enable
    UINT8_T dxl_torque_disable = 0;                         // Value for torque disable
    UINT8_T dxl_error = 0;                                  // DXL error
    UINT8_T dxl_moving;                                     // DXL moving status
    INT32_T dxl_present_position;                           // Present position
    UINT8_T dxl_led_value[2] = {0x00, 0xFF};                // LED value
    UINT8_T param_indirect_data_for_write[LEN_PRO_INDIRECTDATA_FOR_WRITE];

    // Open port
    if(portHandler->OpenPort())
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
    if(portHandler->SetBaudRate(BAUDRATE))
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

    // Disable DXL Torque :
    // Indirect address would not accessible when the torque is already enabled
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
    if( dxl_comm_result == COMM_SUCCESS )
    {
        if(dxl_error != 0)
        {
            PrintErrorCode_Protocol2(dxl_error);
            return 0;
        }
        printf("DXL torque is unabled\n");
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }

    // INDIRECTDATA parameter storages replace LED, goal position, present position and moving status storages
    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_WRITE + 0, ADDR_PRO_GOAL_POSITION + 0, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_WRITE + 2, ADDR_PRO_GOAL_POSITION + 1, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_WRITE + 4, ADDR_PRO_GOAL_POSITION + 2, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_WRITE + 6, ADDR_PRO_GOAL_POSITION + 3, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_WRITE + 8, ADDR_PRO_LED_RED, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_READ + 0, ADDR_PRO_PRESENT_POSITION + 0, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_READ + 2, ADDR_PRO_PRESENT_POSITION + 1, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_READ + 4, ADDR_PRO_PRESENT_POSITION + 2, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_READ + 6, ADDR_PRO_PRESENT_POSITION + 3, &dxl_error);
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

    dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, DXL_ID, ADDR_PRO_INDIRECTADDRESS_FOR_READ + 8, ADDR_PRO_MOVING, &dxl_error);
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

    // Enable DXL Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_enable, &dxl_error);
    if( dxl_comm_result == COMM_SUCCESS )
    {
        if(dxl_error != 0)
        {
            PrintErrorCode_Protocol2(dxl_error);
            return 0;
        }
        printf("DXL torque is enabled\n");
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        return 0;
    }

    // Add parameter storage for the present position value
    dxl_addparam_result = groupSyncRead.AddParam(DXL_ID);
    if( dxl_addparam_result != true )
    {
        fprintf(stderr, "[ID:%03d] groupsyncread addparam failed\n", DXL_ID);
        return 0;
    }

    while(1)
    {
        printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
        if(_getch() == 0x1b)
            break;

        // Allocate LED and goal position value into byte array
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(dxl_goal_position[index]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(dxl_goal_position[index]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(dxl_goal_position[index]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(dxl_goal_position[index]));
        param_indirect_data_for_write[4] = dxl_led_value[index];

        // Add values to the Syncwrite storage
        dxl_addparam_result = groupSyncWrite.AddParam(DXL_ID, param_indirect_data_for_write);
        if( dxl_addparam_result != true )
        {
            fprintf(stderr, "[ID:%03d] groupsyncread addparam failed\n", DXL_ID);
            return 0;
        }

        // Syncwrite all
        dxl_comm_result = groupSyncWrite.TxPacket();
        if( dxl_comm_result == COMM_SUCCESS ) {}
        else
        {
            PrintCommStatus(dxl_comm_result);
            return 0;
        }

        // Clear syncwrite parameter storage
        groupSyncWrite.ClearParam();

        do{
            usleep(50*1000);

            // Syncread present position from indirectdata2
            dxl_comm_result = groupSyncRead.TxRxPacket();
            if( dxl_comm_result == COMM_SUCCESS ) {}
            else
            {
                PrintCommStatus(dxl_comm_result);
                return 0;
            }

            // Get DXL present position value
            dxl_getdata_result = groupSyncRead.GetData(DXL_ID, ADDR_PRO_INDIRECTDATA_FOR_READ, (UINT32_T*)&dxl_present_position);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupSyncRead getdata failed", DXL_ID);
                return 0;
            }

            // Get DXL moving status value
            dxl_getdata_result = groupSyncRead.GetData(DXL_ID, ADDR_PRO_INDIRECTDATA_FOR_READ + 4, &dxl_moving);
            if( dxl_getdata_result != true )
            {
                fprintf(stderr, "[ID:%03d] groupSyncRead getdata failed", DXL_ID);
                return 0;
            }

            printf("[ID:%03d] GoalPos:%03d  PresPos:%03d  IsMoving:%d\n", DXL_ID, dxl_goal_position[index], dxl_present_position, dxl_moving);
        }while(abs(dxl_goal_position[index] - dxl_present_position) > STOP_MOVING_MARGIN);

        if( index == 0 )
            index = 1;
        else
            index = 0;
    }
    // Clear syncread parameter storage
    groupSyncRead.ClearParam();

    // Disable DXL Torque
    dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, dxl_torque_disable, &dxl_error);
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

/*
 * Ping.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Ping Example      *********
//
//
// Available DXL model on this example : All models using Protocol 2.0
// This example is tested with a DXL PRO 54-200, and an USB2DYNAMIXEL
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

// DXL Error bit for Protocol 2.0
#define ERRBIT_PROTOCOL2_RESULTFAIL     1
#define ERRBIT_PROTOCOL2_INSTRUCTION    2
#define ERRBIT_PROTOCOL2_CRC            3
#define ERRBIT_PROTOCOL2_DATARANGE      4
#define ERRBIT_PROTOCOL2_DATALENGTH     5
#define ERRBIT_PROTOCOL2_DATALIMIT      6
#define ERRBIT_PROTOCOL2_ADDRESS        7

// Protocol version
#define PROTOCOL_VERSION				2.0

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
    // Initialize Porthandler instance
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    PortHandler *portHandler = (PortHandler*)PortHandler::GetPortHandler(DEVICENAME);

    // Initialize Packethandler instance
    PacketHandler *packetHandler = PacketHandler::GetPacketHandler(PROTOCOL_VERSION);

    int dxl_comm_result = COMM_TX_FAIL;                     // Communication result

    UINT8_T dxl_error = 0;                                  // DXL error
    UINT16_T dxl_model_number;                              // DXL model number

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

    // Try to ping the DXL
    // Get DXL model number from the DXL
    dxl_comm_result = packetHandler->Ping(portHandler, DXL_ID, &dxl_model_number, &dxl_error);
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

	printf("[ID:%03d] Ping Succeeded. DXL model number : %d\n", DXL_ID, dxl_model_number);

	// Close port
    portHandler->ClosePort();

	printf( "Press Enter key to terminate...\n" );
	_getch();
	return 0;
}

/*
 * DXLMonitor.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     DXL Monitor Example      *********
//
//
// Available DXL model on this example : All models using Protocol 1.0 and 2.0
// This example is tested with a DXL MX-28, a DXL PRO 54-200 and an USB2DYNAMIXEL
// Be sure that properties of DXL MX and PRO are already set as %% MX - ID : 1 / Baudnum : 1 (Baudrate : 1000000) , PRO - ID : 1 / Baudnum : 3 (Baudrate : 1000000)
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
#include <vector>
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

// Protocol version
#define PROTOCOL_VERSION1               1.0
#define PROTOCOL_VERSION2               2.0

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

void Help()
{
    printf("\n");
    printf("                    .----------------------------.\n");
    printf("                    |  DXL Monitor Command List  |\n");
    printf("                    '----------------------------'\n");
    printf(" =========================== Common Commands ===========================\n");
    printf(" \n");
    printf(" help |h |?                  :Displays help information\n");
    printf(" baud [BAUD_RATE]            :Changes baudrate to [BAUD_RATE] \n" );
    printf("                               ex) baud 2400 (2400 bps) \n");
    printf("                               ex) baud 1000000 (1 Mbps)  \n");
    printf(" exit                        :Exit this program\n");
    printf(" scan                        :Outputs the current status of all DXLs\n");
    printf(" ping [ID] [ID] ...          :Outputs the current status of [ID]s \n");
    printf(" bp                          :Broadcast ping (DXL Protocol 2.0 only)\n");
    printf(" \n");
    printf(" ==================== Commands for DXL Protocol 1.0 ====================\n");
    printf(" \n");
    printf(" wrb1|w1 [ID] [ADDR] [VALUE] :Write byte [VALUE] to [ADDR] of [ID]\n");
    printf(" wrw1 [ID] [ADDR] [VALUE]    :Write word [VALUE] to [ADDR] of [ID]\n");
    printf(" rdb1 [ID] [ADDR]            :Read byte value from [ADDR] of [ID]\n");
    printf(" rdw1 [ID] [ADDR]            :Read word value from [ADDR] of [ID]\n");
    printf(" r1 [ID] [ADDR] [LENGTH]     :Dumps the control table of [ID]\n");
    printf("                               ([LENGTH] bytes from [ADDR])\n");
    printf(" reset1|rst1 [ID]            :Factory reset the Dynamixel of [ID]\n");
    printf(" \n");
    printf(" ==================== Commands for DXL Protocol 2.0 ====================\n");
    printf(" \n");
    printf(" wrb2|w2 [ID] [ADDR] [VALUE] :Write byte [VALUE] to [ADDR] of [ID]\n");
    printf(" wrw2 [ID] [ADDR] [VALUE]    :Write word [VALUE] to [ADDR] of [ID]\n");
    printf(" wrd2 [ID] [ADDR] [VALUE]    :Write dword [VALUE] to [ADDR] of [ID]\n");
    printf(" rdb2 [ID] [ADDR]            :Read byte value from [ADDR] of [ID]\n");
    printf(" rdw2 [ID] [ADDR]            :Read word value from [ADDR] of [ID]\n");
    printf(" rdd2 [ID] [ADDR]            :Read dword value from [ADDR] of [ID]\n");
    printf(" r2 [ID] [ADDR] [LENGTH]     :Dumps the control table of [ID]\n");
    printf("                               ([LENGTH] bytes from [ADDR])\n");
    printf(" reboot2|rbt2 [ID]           :Reboot the Dynamixel of [ID]\n");
    printf(" reset2|rst2 [ID] [OPTION]   :Factory reset the Dynamixel of [ID]\n");
    printf("                               OPTION: 255(All), 1(Except ID), 2(Except ID&Baud)\n");

    printf("\n");
}

void Scan(PortHandler *portHandler, PacketHandler* packetHandler1, PacketHandler* packetHandler2)
{
    UINT8_T dxl_error;
    UINT16_T dxl_model_num;

    fprintf(stderr, "\n");
    fprintf(stderr, "Scan DXL Using Protocol 1.0\n");
    for(int id = 1; id < 253; id++)
    {
        if( packetHandler1-> Ping(portHandler, id, &dxl_model_num, &dxl_error)== COMM_SUCCESS)
        {
            fprintf(stderr, "\n                                          ... SUCCESS \r");
            fprintf(stderr, " [ID:%.3d] Model No : %.5d \n",
                    id, dxl_model_num);
        }
        else
            fprintf(stderr, ".");

        if(_kbhit())
        {
            char c = _getch();
            if(c == 0x1b)
                break;
        }
    }
    fprintf(stderr, "\n\n");

    fprintf(stderr, "Scan DXL Using Protocol 2.0\n");
    for(int id = 1; id < 253; id++)
    {
        if(packetHandler2-> Ping(portHandler, id, &dxl_model_num, &dxl_error)== COMM_SUCCESS)
        {
            fprintf(stderr, "\n                                          ... SUCCESS \r");
            fprintf(stderr, " [ID:%.3d] Model No : %.5d \n",
                    id, dxl_model_num);
        }
        else
            fprintf(stderr, ".");

        if(_kbhit())
        {
            char c = _getch();
            if(c == 0x1b) {
                break;
            }
        }
    }
    fprintf(stderr, "\n\n");
}

void Write(PortHandler *portHandler, PacketHandler *packetHandler, UINT8_T id, UINT16_T addr, UINT16_T length, UINT32_T value)
{
    UINT8_T dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;

    if(length == 1)
        dxl_comm_result = packetHandler->Write1ByteTxRx(portHandler, id, addr, (UINT8_T)value, &dxl_error);
    else if(length == 2)
        dxl_comm_result = packetHandler->Write2ByteTxRx(portHandler, id, addr, (UINT16_T)value, &dxl_error);
    else if(length == 4)
        dxl_comm_result = packetHandler->Write4ByteTxRx(portHandler, id, addr, (UINT32_T)value, &dxl_error);

    if(dxl_comm_result == COMM_SUCCESS)
    {
        if(dxl_error != 0)
        {
            if(packetHandler->GetProtocolVersion() == 1.0)
                PrintErrorCode_Protocol1(dxl_error);
            else
                PrintErrorCode_Protocol2(dxl_error);
        }
        fprintf(stderr, "\n Success to write\n\n");
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        fprintf(stderr, "\n Fail to write! \n\n");
    }
}

void Read(PortHandler *portHandler, PacketHandler *packetHandler, UINT8_T id, UINT16_T addr, UINT16_T length)
{
    UINT8_T dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;

    INT8_T    value8    = 0;
    INT16_T    value16    = 0;
    INT32_T value32    = 0;


    if(length == 1)
        dxl_comm_result = packetHandler->Read1ByteTxRx(portHandler, id, addr, (UINT8_T*)&value8, &dxl_error);
    else if(length == 2)
        dxl_comm_result = packetHandler->Read2ByteTxRx(portHandler, id, addr, (UINT16_T*)&value16, &dxl_error);
    else if(length == 4)
        dxl_comm_result = packetHandler->Read4ByteTxRx(portHandler, id, addr, (UINT32_T*)&value32, &dxl_error);

    if(dxl_comm_result == COMM_SUCCESS)
    {
        if(dxl_error != 0)
        {
            if(packetHandler->GetProtocolVersion() == 1.0)
                PrintErrorCode_Protocol1(dxl_error);
            else
                PrintErrorCode_Protocol2(dxl_error);
        }

        if(length == 1)
            fprintf(stderr, "\n READ VALUE : (UNSIGNED) %u , (SIGNED) %d \n\n", (UINT8_T)value8, value8);
        else if(length == 2)
            fprintf(stderr, "\n READ VALUE : (UNSIGNED) %u , (SIGNED) %d \n\n", (UINT16_T)value16, value16);
        else if(length == 4)
            fprintf(stderr, "\n READ VALUE : (UNSIGNED) %u , (SIGNED) %d \n\n", (UINT32_T)value32, value32);

    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        fprintf(stderr, "\n Fail to read! \n\n");
    }
}

void Dump(PortHandler *portHandler, PacketHandler *packetHandler, UINT8_T id, UINT16_T addr, UINT16_T len)
{
    UINT8_T dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;
    UINT8_T* data = (UINT8_T*)calloc(len, sizeof(UINT8_T));

    dxl_comm_result = packetHandler->ReadTxRx(portHandler, id, addr, len, data, &dxl_error);
    if(dxl_comm_result == COMM_SUCCESS)
    {
        if(dxl_error != 0)
        {
            if(packetHandler->GetProtocolVersion() == 1.0)
                PrintErrorCode_Protocol1(dxl_error);
            else
                PrintErrorCode_Protocol2(dxl_error);
        }

        if(id != BROADCAST_ID)
        {
            fprintf(stderr, "\n");
            for(int i = addr; i < addr+len; i++)
                fprintf(stderr, "ADDR %.3d [0x%.4X] :     %.3d [0x%.2X] \n", i, i, data[i-addr], data[i-addr]);
            fprintf(stderr, "\n");
        }
    }
    else
    {
        PrintCommStatus(dxl_comm_result);
        fprintf(stderr, "\n Fail to read! \n\n");
    }

    free(data);
}

int main(int argc, char *argv[])
{
	// Initialize PortHandler instance
	// Set the port path
	// Get methods and members of PortHandlerLinux or PortHandlerWindows
	PortHandler *portHandler = PortHandler::GetPortHandler(DEVICENAME);

    // Initialize Packethandler1 instance
    PacketHandler *packetHandler1 = PacketHandler::GetPacketHandler(PROTOCOL_VERSION1);

    // Initialize Packethandler2 instance
    PacketHandler *packetHandler2 = PacketHandler::GetPacketHandler(PROTOCOL_VERSION2);

    fprintf(stderr, "\n***********************************************************************\n");
    fprintf(stderr,   "*                            DXL Monitor                              *\n");
    fprintf(stderr,   "***********************************************************************\n\n");

    // Open port
    if(portHandler->OpenPort())
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n" );
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    char input[128];
    char cmd[80];
    char param[20][30];
    int num_param;
    char* token;
    UINT8_T dxl_error;

    while(1)
    {
        printf("[CMD] ");
        fgets(input, sizeof(input), stdin);
        char *p;
        if((p = strchr(input, '\n'))!= NULL) *p = '\0';
        fflush(stdin);

        if(strlen(input) == 0)
            continue;

        token = strtok(input, " ");
        if(token == 0)
            continue;

        strcpy(cmd, token);
        token = strtok(0, " ");
        num_param = 0;
        while(token != 0)
        {
            strcpy(param[num_param++], token);
            token = strtok(0, " ");
        }

        if(strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0 || strcmp(cmd, "?") == 0)
        {
            Help();
        }
        else if(strcmp(cmd, "baud") == 0)
        {
            if(num_param == 1)
            {
                if(portHandler->SetBaudRate(atoi(param[0])) == false)
                    fprintf(stderr, " Failed to change baudrate! \n");
                else
                    fprintf(stderr, " Success to change baudrate! [ BAUD RATE: %d ]\n", atoi(param[0]));
            }
            else
            {
                fprintf(stderr, " Invalid parameters! \n");
                continue;
            }
        }
        else if(strcmp(cmd, "exit") == 0)
        {
            portHandler->ClosePort();
            return 0;
        }
        else if(strcmp(cmd, "scan") == 0)
        {
            Scan(portHandler, packetHandler1, packetHandler2);
        }
        else if(strcmp(cmd, "ping") == 0)
        {
            UINT16_T dxl_model_num;

            if(num_param == 0)
            {
                fprintf(stderr, " Invalid parameters! \n");
                continue;
            }

            fprintf(stderr, "\n");
            fprintf(stderr, "Ping Using Protocol 1.0\n");
            for(int i = 0; i < num_param; i++)
            {
                if(packetHandler1->Ping(portHandler, atoi(param[i]), &dxl_model_num, &dxl_error) == COMM_SUCCESS)
                {
                    fprintf(stderr, "\n                                          ... SUCCESS \r");
                    fprintf(stderr, " [ID:%.3d] Model No : %.5d \n",
                            atoi(param[i]), dxl_model_num);
                }
                else
                {
                    fprintf(stderr, "\n                                          ... FAIL \r");
                    fprintf(stderr, " [ID:%.3d] \n", atoi(param[i]));
                }
            }
            fprintf(stderr, "\n");

            fprintf(stderr, "\n");
            fprintf(stderr, "Ping Using Protocol 2.0\n");
            for(int i = 0; i < num_param; i++)
            {
                if(packetHandler2->Ping(portHandler, atoi(param[i]), &dxl_model_num, &dxl_error) == COMM_SUCCESS)
                {
                    fprintf(stderr, "\n                                          ... SUCCESS \r");
                    fprintf(stderr, " [ID:%.3d] Model No : %.5d \n",
                            atoi(param[i]), dxl_model_num);
                }
                else
                {
                    fprintf(stderr, "\n                                          ... FAIL \r");
                    fprintf(stderr, " [ID:%.3d] \n", atoi(param[i]));
                }
            }
            fprintf(stderr, "\n");
        }
        else if(strcmp(cmd, "bp") == 0)
        {
            if(num_param == 0)
            {
                std::vector<unsigned char> vec;

                int dxl_comm_result = packetHandler2->BroadcastPing(portHandler, vec);
                if(dxl_comm_result == COMM_SUCCESS)
                {
                    if(dxl_error != 0)
                    {
                        PrintErrorCode_Protocol1(dxl_error);
                    }
                }
                else
                {
                    PrintCommStatus(dxl_comm_result);
                }

                for(unsigned int i = 0; i < vec.size(); i++)
                {
                    fprintf(stderr, "\n                                          ... SUCCESS \r");
                    fprintf(stderr, " [ID:%.3d] \n", vec.at(i));
                }
                printf("\n");
            }
            else
            {
                fprintf(stderr, " Invalid parameters! \n");
            }

        }
        else if(strcmp(cmd, "wrb1") == 0 || strcmp(cmd, "w1") == 0)
        {
            if(num_param == 3)
                Write(portHandler, packetHandler1, atoi(param[0]), atoi(param[1]), 1, atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "wrb2") == 0 || strcmp(cmd, "w2") == 0)
        {
            if(num_param == 3)
                Write(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 1, atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "wrw1") == 0)
        {
            if(num_param == 3)
                Write(portHandler, packetHandler1, atoi(param[0]), atoi(param[1]), 2, atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "wrw2") == 0)
        {
            if(num_param == 3)
                Write(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 2, atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "wrd2") == 0)
        {
            if(num_param == 3)
                Write(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 4, atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "rdb1") == 0)
        {
            if(num_param == 2)
                Read(portHandler, packetHandler1, atoi(param[0]), atoi(param[1]), 1);
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "rdb2") == 0)
        {
            if(num_param == 2)
                Read(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 1);
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "rdw1") == 0)
        {
            if(num_param == 2)
                Read(portHandler, packetHandler1, atoi(param[0]), atoi(param[1]), 2);
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "rdw2") == 0)
        {
            if(num_param == 2)
                Read(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 2);
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "rdd2") == 0)
        {
            if(num_param == 2)
                Read(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), 4);
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "r1") == 0)
        {
            if(num_param == 3)
                Dump(portHandler, packetHandler1, atoi(param[0]), atoi(param[1]), atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "r2") == 0)
        {
            if(num_param == 3)
                Dump(portHandler, packetHandler2, atoi(param[0]), atoi(param[1]), atoi(param[2]));
            else
                fprintf(stderr, " Invalid parameters! \n");
        }
        else if(strcmp(cmd, "reboot2") == 0 || strcmp(cmd, "rbt2") == 0)
        {
            if(num_param == 1)
            {
                int dxl_comm_result = packetHandler2->Reboot(portHandler, atoi(param[0]), &dxl_error);
                if( dxl_comm_result == COMM_SUCCESS )
                {
                    if(dxl_error != 0)
                    {
                        PrintErrorCode_Protocol2(dxl_error);
                    }
                    fprintf(stderr, "\n Success to reboot! \n\n");
                }
                else
                {
                    PrintCommStatus(dxl_comm_result);
                    fprintf(stderr, "\n Fail to reboot! \n\n");
                }
            }
            else
            {
                fprintf(stderr, " Invalid parameters! \n");
            }
        }
        else if(strcmp(cmd, "reset1") == 0 || strcmp(cmd, "rst1") == 0)
        {
            if(num_param == 1)
            {
                int dxl_comm_result = packetHandler1->FactoryReset(portHandler, atoi(param[0]), 0x00, &dxl_error);
                if( dxl_comm_result == COMM_SUCCESS )
                {
                    if(dxl_error != 0)
                    {
                        PrintErrorCode_Protocol1(dxl_error);
                    }
                    fprintf(stderr, "\n Success to reset! \n\n");
                }
                else
                {
                    PrintCommStatus(dxl_comm_result);
                    fprintf(stderr, "\n Fail to reset! \n\n");
                }
            }
            else
            {
                fprintf(stderr, " Invalid parameters! \n");
            }
        }
        else if(strcmp(cmd, "reset2") == 0 || strcmp(cmd, "rst2") == 0)
        {
            if(num_param == 2)
            {
                int dxl_comm_result = packetHandler2->FactoryReset(portHandler, atoi(param[0]), atoi(param[1]), &dxl_error);
                if( dxl_comm_result == COMM_SUCCESS )
                {
                    if(dxl_error != 0)
                    {
                        PrintErrorCode_Protocol2(dxl_error);
                    }
                    fprintf(stderr, "\n Success to reset! \n\n");
                }
                else
                {
                    PrintCommStatus(dxl_comm_result);
                    fprintf(stderr, "\n Fail to reset! \n\n");
                }
            }
            else
            {
                fprintf(stderr, " Invalid parameters! \n");
            }
        }
        else
        {
            printf(" Bad command! Please input 'help'.\n");
        }
    }
}

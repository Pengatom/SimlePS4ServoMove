#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# broadcast_ping.py
#
#  Created on: 2016. 6. 16.
#      Author: Ryu Woon Jung (Leon)
#

#
# *********     ping Example      *********
#
#
# Available Dynamixel model on this example : All models using Protocol 2.0
# This example is designed for using a Dynamixel PRO 54-200, and an USB2DYNAMIXEL.
# To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below variables yourself.
# Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000 [1M])
#

import msvcrt
import ctypes
import init_path
from dynamixel_functions_py import dynamixel_functions as dynamixel                      # Uses Dynamixel SDK library

# Protocol version
PROTOCOL_VERSION            = 2                             # See which protocol version is used in the Dynamixel

# Default setting
DXL_ID                      = 1                             # Dynamixel ID: 1
BAUDRATE                    = 1000000
DEVICENAME                  = "COM1".encode('utf-8')        # Check which port is being used on your controller
                                                            # ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

MAX_ID                      = 252                           # Maximum ID value
COMM_SUCCESS                = 0                             # Communication Success result value
COMM_TX_FAIL                = -1001                         # Communication Tx Failed

# Initialize PortHandler Structs
# Set the port path
# Get methods and members of PortHandlerLinux or PortHandlerWindows
port_num = dynamixel.portHandler(DEVICENAME)

# Initialize PacketHandler Structs
dynamixel.packetHandler()

dxl_comm_result = COMM_TX_FAIL                              # Communication result

# Open port
if dynamixel.openPort(port_num):
    print("Succeeded to open the port!")
else:
    print("Failed to open the port!")
    print("Press any key to terminate...")
    msvcrt.getch()
    quit()

# Set port baudrate
if dynamixel.setBaudRate(port_num, BAUDRATE):
    print("Succeeded to change the baudrate!")
else:
    print("Failed to change the baudrate!")
    print("Press any key to terminate...")
    msvcrt.getch()
    quit()

# Try to broadcast ping the Dynamixel
dynamixel.broadcastPing(port_num, PROTOCOL_VERSION)
if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
    dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))

print("Detected Dynamixel : ")
for id in range(0, MAX_ID):
    if ctypes.c_ubyte(dynamixel.getBroadcastPingResult(port_num, PROTOCOL_VERSION, id)).value:
        print("[ID:%03d]" % (id))

# Close port
dynamixel.closePort(port_num)

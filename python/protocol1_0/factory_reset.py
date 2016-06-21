#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# factory_reset.py
#
#  Created on: 2016. 5. 16.
#      Author: Leon Ryu Woon Jung
#

#
# *********     Factory Reset Example      *********
#
#
# Available Dynamixel model on this example : All models using Protocol 1.0
# This example is designed for using a Dynamixel MX-28, and an USB2DYNAMIXEL.
# To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below variables yourself.
# Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 1000000 [1M])
#

# Be aware that:
# This example resets all properties of Dynamixel to default values, such as %% ID : 1 / Baudnum : 34 (Baudrate : 57600)
#

from time import sleep
import msvcrt
import ctypes
import init_path
from dynamixel_functions_py import dynamixel_functions as dynamixel                      # Uses Dynamixel SDK library

# Control table address
ADDR_MX_BAUDRATE            = 4            # Control table address is different in Dynamixel model

# Protocol version
PROTOCOL_VERSION            = 1            # See which protocol version is used in the Dynamixel

# Default setting
DXL_ID                      = 1            # Dynamixel ID: 1
BAUDRATE                    = 1000000
DEVICENAME                  = "COM1".encode("utf-8")       # Check which port is being used on your controller
                                           # ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

FACTORYRST_DEFAULTBAUDRATE  = 57600        # Dynamixel baudrate set by factoryreset
NEW_BAUDNUM                 = 1            # New baudnum to recover Dynamixel baudrate as it was
OPERATION_MODE              = 0x00         # Mode is unavailable in Protocol 1.0 Reset

COMM_SUCCESS                = 0            # Communication Success result value
COMM_TX_FAIL                = -1001        # Communication Tx Failed

# Initialize PortHandler Structs
# Set the port path
# Get methods and members of PortHandlerLinux or PortHandlerWindows
port_num = dynamixel.portHandler(DEVICENAME)

# Initialize PacketHandler Structs
dynamixel.packetHandler()

dxl_comm_result = COMM_TX_FAIL           # Communication result

dxl_error = 0                            # Dynamixel error
dxl_baudnum_read = 0                     # Read baudnum

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


# Read present baudrate of the controller
print("Now the controller baudrate is : %d" % (dynamixel.getBaudRate(port_num)))

# Try factoryreset
print("[ID:%03d] Try factoryreset : " % (DXL_ID))
dynamixel.factoryReset(port_num, PROTOCOL_VERSION, DXL_ID, OPERATION_MODE)
if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
    print("Aborted")
    dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
    quit()
elif dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION) != 0:
    dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))


# Wait for reset
print("Wait for reset...")
sleep(2)

print("[ID:%03d] factoryReset Success!" % (DXL_ID))

# Set controller baudrate to dxl default baudrate
if dynamixel.setBaudRate(port_num, FACTORYRST_DEFAULTBAUDRATE):
    print("Succeed to change the controller baudrate to : %d" % (FACTORYRST_DEFAULTBAUDRATE))
else:
    print("Failed to change the controller baudrate")
    msvcrt.getch()
    quit()

# Read Dynamixel baudnum
dxl_baudnum_read = dynamixel.read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE)
if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
    dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
elif dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION) != 0:
    dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
else:
  print("[ID:%03d] Dynamixel baudnum is now : %d" % (DXL_ID, dxl_baudnum_read))

# Write new baudnum
dynamixel.write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE, NEW_BAUDNUM)
if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
    dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
elif dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION) != 0:
    dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
else:
  print("[ID:%03d] Set Dynamixel baudnum to : %d" % (DXL_ID, NEW_BAUDNUM))

# Set port baudrate to BAUDRATE
if dynamixel.setBaudRate(port_num, BAUDRATE):
    print("Succeed to change the controller baudrate to : %d" % (BAUDRATE))
else:
    print("Failed to change the controller baudrate")
    msvcrt.getch()
    quit()

sleep(0.2)

# Read Dynamixel baudnum
dxl_baudnum_read = dynamixel.read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_MX_BAUDRATE)
if dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION) != COMM_SUCCESS:
    dynamixel.printTxRxResult(PROTOCOL_VERSION, dynamixel.getLastTxRxResult(port_num, PROTOCOL_VERSION))
elif dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION) != 0:
    dynamixel.printRxPacketError(PROTOCOL_VERSION, dynamixel.getLastRxPacketError(port_num, PROTOCOL_VERSION))
else:
  print("[ID:%03d] Dynamixel baudnum is now : %d" % (DXL_ID, dxl_baudnum_read))


# Close port
dynamixel.closePort(port_num)

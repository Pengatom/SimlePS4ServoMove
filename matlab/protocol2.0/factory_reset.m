%
% factory_reset.m
%
%  Created on: 2016. 5. 16.
%      Author: Leon Ryu Woon Jung
%

%
% *********     Factory Reset Example      *********
%
%
% Available Dynamixel model on this example : All models using Protocol 2.0
% This example is designed for using a Dynamixel PRO 54-200, and an USB2DYNAMIXEL.
% To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below "#define"d variables yourself.
% Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000 [1M])
%

% Be aware that:
% This example resets all properties of Dynamixel to default values, such as %% ID : 1 / Baudnum : 1 (Baudrate : 57600)
%

clc;
clear all;

% Load Libraries
if ~libisloaded('dxl_x86_c');
  [notfound, warnings] = loadlibrary('dxl_x86_c', 'dynamixel_sdk.h', 'addheader', 'port_handler.h', 'addheader', 'packet_handler.h');
end

% Control table address
ADDR_PRO_BAUDRATE               = 8;            % Control table address is different in Dynamixel model

% Protocol version
PROTOCOL_VERSION                = 2.0;          % See which protocol version is used in the Dynamixel

% Default setting
DXL_ID                          = 1;            % Dynamixel ID: 1
BAUDRATE                        = 1000000;
DEVICENAME                      = 'COM8';       % Check which port is being used on your controller
                                                % ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

FACTORYRST_DEFAULTBAUDRATE      = 57600;        % Dynamixel baudrate set by factoryreset
NEW_BAUDNUM                     = 3;            % New baudnum to recover Dynamixel baudrate as it was
OPERATION_MODE                  = 1;            % 0xFF : reset all values
                                                % 0x01 : reset all values except ID
                                                % 0x02 : reset all values except ID and baudrate

TORQUE_ENABLE                   = 1;            % Value for enabling the torque
TORQUE_DISABLE                  = 0;            % Value for disabling the torque

COMM_SUCCESS                    = 0;            % Communication Success result value
COMM_TX_FAIL                    = -1001;        % Communication Tx Failed

% Initialize PortHandler Structs
% Set the port path
% Get methods and members of PortHandlerLinux or PortHandlerWindows
port_num = portHandler(DEVICENAME);

% Initialize PacketHandler Structs
packetHandler();

dxl_comm_result = COMM_TX_FAIL;                 % Communication result

dxl_error = 0;                                  % Dynamixel error
dxl_baudnum_read = 0;                           % Read baudnum

% Open port
if (openPort(port_num))
  fprintf('Succeeded to open the port!\n');
else
  unloadlibrary('dxl_x86_c');
  fprintf('Failed to open the port!\n');
  input('Press any key to terminate...\n');
  return;
end

% Set port baudrate
if (setBaudRate(port_num, BAUDRATE))
  fprintf('Succeeded to change the baudrate!\n');
else
  unloadlibrary('dxl_x86_c');
  fprintf('Failed to change the baudrate!\n');
  input('Press any key to terminate...\n');
  return;
end

% Read present baudrate of the controller
fprintf('Now the controller baudrate is : %d\n', getBaudRate(port_num));

% Try factoryreset
fprintf('[ID:%03d] Try factoryreset : ', DXL_ID);
factoryReset(port_num, PROTOCOL_VERSION, DXL_ID, OPERATION_MODE);
if getLastTxRxResult(port_num, PROTOCOL_VERSION) ~= COMM_SUCCESS
  fprintf('Aborted\n');
  printTxRxResult(PROTOCOL_VERSION, getLastTxRxResult(port_num, PROTOCOL_VERSION));
  return;
elseif getLastRxPacketError(port_num, PROTOCOL_VERSION) ~= 0
  printRxPacketError(PROTOCOL_VERSION, getLastRxPacketError(port_num, PROTOCOL_VERSION));
end

% Wait for reset
fprintf('Wait for reset...\n');
pause(2);

fprintf('[ID:%03d] factoryReset Success!\n', DXL_ID);

% Set controller baudrate to dxl default baudrate
if (setBaudRate(port_num, FACTORYRST_DEFAULTBAUDRATE))
  fprintf('Succeed to change the controller baudrate to : %d\n', FACTORYRST_DEFAULTBAUDRATE);
else
  fprintf('Failed to change the controller baudrate\n');
  input('Press any key to terminate...\n');
  return;
end

% Read Dynamixel baudnum
dxl_baudnum_read = read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_BAUDRATE);
if getLastTxRxResult(port_num, PROTOCOL_VERSION) ~= COMM_SUCCESS
  printTxRxResult(PROTOCOL_VERSION, getLastTxRxResult(port_num, PROTOCOL_VERSION));
elseif getLastRxPacketError(port_num, PROTOCOL_VERSION) ~= 0
  printRxPacketError(PROTOCOL_VERSION, getLastRxPacketError(port_num, PROTOCOL_VERSION));
else
  fprintf('[ID:%03d] Dynamixel baudnum is now : %d\n', DXL_ID, dxl_baudnum_read);
end

% Write new baudnum
write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_BAUDRATE, NEW_BAUDNUM);
if getLastTxRxResult(port_num, PROTOCOL_VERSION) ~= COMM_SUCCESS
  printTxRxResult(PROTOCOL_VERSION, getLastTxRxResult(port_num, PROTOCOL_VERSION));
elseif getLastRxPacketError(port_num, PROTOCOL_VERSION) ~= 0
  printRxPacketError(PROTOCOL_VERSION, getLastRxPacketError(port_num, PROTOCOL_VERSION));
else
  fprintf('[ID:%03d] Set Dynamixel baudnum to : %d\n', DXL_ID, NEW_BAUDNUM);
end

  % Set port baudrate to BAUDRATE
if (setBaudRate(port_num, BAUDRATE))
  fprintf('Succeed to change the controller baudrate to : %d\n', BAUDRATE);
else
  fprintf('Failed to change the controller baudrate\n');
  input('Press any key to terminate...\n');
  return;
end

pause(0.2);

% Read Dynamixel baudnum
dxl_baudnum_read = read1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_BAUDRATE);
if getLastTxRxResult(port_num, PROTOCOL_VERSION) ~= COMM_SUCCESS
  printTxRxResult(PROTOCOL_VERSION, getLastTxRxResult(port_num, PROTOCOL_VERSION));
elseif getLastRxPacketError(port_num, PROTOCOL_VERSION) ~= 0
  printRxPacketError(PROTOCOL_VERSION, getLastRxPacketError(port_num, PROTOCOL_VERSION));
else
  fprintf('[ID:%03d] Dynamixel baudnum is now : %d\n', DXL_ID, dxl_baudnum_read);
end

% Close port
closePort(port_num);

% Unload Library
unloadlibrary('dxl_x86_c');

close all;
clear all;

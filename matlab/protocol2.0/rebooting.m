%
% reboot.m
%
%  Created on: 2016. 5. 16.
%      Author: Leon Ryu Woon Jung
%

%
% *********     Reboot Example      *********
%
%
% Available Dynamixel model on this example : All models using Protocol 2.0
% This example is designed for using a Dynamixel PRO 54-200, and an USB2DYNAMIXEL.
% To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below '#define'd variables yourself.
% Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 3 (Baudrate : 1000000 [1M])
%

clc;
clear all;

% Load Libraries
if ~libisloaded('dxl_x86_c');
    [notfound, warnings] = loadlibrary('dxl_x86_c', 'dynamixel_sdk.h', 'addheader', 'port_handler.h', 'addheader', 'packet_handler.h');
end

% Protocol version
PROTOCOL_VERSION                = 2.0;          % See which protocol version is used in the Dynamixel

% Default setting
DXL_ID                          = 1;            % Dynamixel ID: 1
BAUDRATE                        = 1000000;
DEVICENAME                      = 'COM8';       % Check which port is being used on your controller
                                                % ex) Windows: 'COM1'   Linux: '/dev/ttyUSB0'

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

% Trigger
input('Press any key to reboot\n');

fprintf('See the Dynamixel LED flickering\n');
% Try reboot
% Dynamixel LED will flicker while it reboots
reboot(port_num, PROTOCOL_VERSION, DXL_ID);
if getLastTxRxResult(port_num, PROTOCOL_VERSION) ~= COMM_SUCCESS
    printTxRxResult(PROTOCOL_VERSION, getLastTxRxResult(port_num, PROTOCOL_VERSION));
elseif getLastRxPacketError(port_num, PROTOCOL_VERSION) ~= 0
    printRxPacketError(PROTOCOL_VERSION, getLastRxPacketError(port_num, PROTOCOL_VERSION));
end

fprintf('[ID:%03d] reboot Succeeded\n', DXL_ID);

% Close port
closePort(port_num);

% Unload Library
unloadlibrary('dxl_x86_c');

close all;
clear all;

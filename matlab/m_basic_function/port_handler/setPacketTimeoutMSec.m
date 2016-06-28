function [] = setPacketTimeoutMSec( port_num, msec)

calllib('dxl_x86_c', 'setPacketTimeoutMSec', port_num, msec);
end


function [VALUE] = getLastRxPacketError( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'getLastRxPacketError', port_num, protocol_version);
end


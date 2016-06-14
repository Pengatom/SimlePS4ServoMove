function [] = txrxPacket( port_num, protocol_version )

calllib('dxl_x86_c', 'txRxPacket', port_num, protocol_version);
end


function [] = txPacket( port_num, protocol_version )

calllib('dxl_x86_c', 'txPacket', port_num, protocol_version);
end


function [] = rxPacket( port_num, protocol_version )

calllib('dxl_x86_c', 'rxPacket', port_num, protocol_version);
end


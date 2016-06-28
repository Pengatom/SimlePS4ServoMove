function [] = setPacketTimeout( port_num, packet_length)

calllib('dxl_x86_c', 'setPacketTimeout', port_num, packet_length);
end


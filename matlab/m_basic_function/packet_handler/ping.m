function [] = ping( port_num, protocol_version, id )

calllib('dxl_x86_c', 'ping', port_num, protocol_version, id);
end


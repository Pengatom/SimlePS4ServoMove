function [] = readRx( port_num, protocol_version, length )

calllib('dxl_x86_c', 'readRx', port_num, protocol_version, length);
end


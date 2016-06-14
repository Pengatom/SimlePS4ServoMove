function [] = read4ByteTx( port_num, protocol_version, id, address )

calllib('dxl_x86_c', 'read4ByteTx', port_num, protocol_version, id, address);
end


function [] = read1ByteTx( port_num, protocol_version, id, address )

calllib('dxl_x86_c', 'read1ByteTx', port_num, protocol_version, id, address);
end


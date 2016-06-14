function [] = readTx( port_num, protocol_version, id, address, length )

calllib('dxl_x86_c', 'readTx', port_num, protocol_version, id, address, length);
end


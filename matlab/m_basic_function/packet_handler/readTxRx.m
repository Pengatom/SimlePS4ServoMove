function [] = readTxRx( port_num, protocol_version, id, address, length )


calllib('dxl_x86_c', 'readTxRx', port_num, protocol_version, id, address, length);
end


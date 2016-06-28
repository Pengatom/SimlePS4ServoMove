function [] = writeTxRx( port_num, protocol_version, id, address, length )

calllib('dxl_x86_c', 'writeTxRx', port_num, protocol_version, id, address, length);
end


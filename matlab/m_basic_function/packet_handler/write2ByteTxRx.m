function [] = write2ByteTxRx( port_num, protocol_version, id, address, data )

calllib('dxl_x86_c', 'write2ByteTxRx', port_num, protocol_version, id, address, data);
end


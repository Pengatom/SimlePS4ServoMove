function [] = write2ByteTxOnly( port_num, protocol_version, id, address, data )

calllib('dxl_x86_c', 'write2ByteTxOnly', port_num, protocol_version, id, address, data);
end


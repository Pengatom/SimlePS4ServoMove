function [] = write1ByteTxOnly( port_num, protocol_version, id, address, data )

calllib('dxl_x86_c', 'write1ByteTxOnly', port_num, protocol_version, id, address, data);
end


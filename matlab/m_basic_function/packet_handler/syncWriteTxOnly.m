function [] = syncWriteTxOnly( port_num, protocol_version, start_addres, data_length, param_length )

calllib('dxl_x86_c', 'syncWriteTxOnly', port_num, protocol_version, start_addres, data_length, param_length);
end


function [] = bulkWriteTxOnly( port_num, protocol_version, param_length )

calllib('dxl_x86_c', 'bulkWriteTxOnly', port_num, protocol_version, param_length);
end


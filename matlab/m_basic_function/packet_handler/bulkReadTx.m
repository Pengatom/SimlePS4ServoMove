function [] = bulkReadTx( port_num, protocol_version, param_length )

calllib('dxl_x86_c', 'bulkReadTx', port_num, protocol_version, param_length);
end


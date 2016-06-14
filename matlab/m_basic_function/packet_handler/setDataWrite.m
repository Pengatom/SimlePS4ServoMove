function [] = setDataWrite( port_num, protocol_version, data_length, data_pos, data )

calllib('dxl_x86_c', 'setDataWrite', port_num, protocol_version, data_length, data_pos, data);
end


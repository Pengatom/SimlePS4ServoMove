function [VALUE] = getDataRead( port_num, protocol_version, data_length, data_pos )

VALUE = calllib('dxl_x86_c', 'getDataRead', port_num, protocol_version, data_length, data_pos);
end


function [VALUE] = groupSyncRead( port_num, protocol_version, start_address, data_length )

VALUE = calllib('dxl_x86_c', 'groupSyncRead', port_num, protocol_version, start_address, data_length);
end


function [VALUE] = groupSyncWrite( group_num, protocol_version, start_address, data_length )

VALUE = calllib('dxl_x86_c', 'groupSyncWrite', group_num, protocol_version, start_address, data_length);
end


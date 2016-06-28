function [VALUE] = groupSyncReadGetData( group_num, id, address, data_length )

VALUE = calllib('dxl_x86_c', 'groupSyncReadGetData', group_num, id, address, data_length);
end


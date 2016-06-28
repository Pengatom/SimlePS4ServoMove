function [VALUE] = groupBulkReadGetData( group_num, id, address, data_length)

VALUE = calllib('dxl_x86_c', 'groupBulkReadGetData', group_num, id, address, data_length);
end


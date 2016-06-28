function [VALUE] = groupBulkReadAddParam( group_num, id, start_address, data_length )

VALUE = calllib('dxl_x86_c', 'groupBulkReadAddParam', group_num, id, start_address, data_length);
end


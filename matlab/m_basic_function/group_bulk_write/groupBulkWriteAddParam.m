function [VALUE] = groupBulkWriteAddParam( group_num, id, start_address, data_length, data, input_length )

VALUE = calllib('dxl_x86_c', 'groupBulkWriteAddParam', group_num, id, start_address, data_length, data, input_length);
end


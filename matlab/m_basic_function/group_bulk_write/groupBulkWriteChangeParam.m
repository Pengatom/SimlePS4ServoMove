function [VALUE] = groupBulkWriteChangeParam( group_num, id, start_address, data_length, data, input_length, data_pos )

VALUE = calllib('dxl_x86_c', 'groupBulkWriteChangeParam', group_num, id, start_address, data_length, data, input_length, data_pos);
end


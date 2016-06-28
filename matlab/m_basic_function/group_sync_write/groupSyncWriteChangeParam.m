function [VALUE] = groupSyncWriteChangeParam( group_num, id, data, data_length, data_pos )

VALUE = calllib('dxl_x86_c', 'groupSyncWriteChangeParam', group_num, id, data, data_length, data_pos);
end


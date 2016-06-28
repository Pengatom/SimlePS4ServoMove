function [VALUE] = groupSyncWriteAddParam( group_num, id, data, data_length )

VALUE = calllib('dxl_x86_c', 'groupSyncWriteAddParam', group_num, id, data, data_length);
end


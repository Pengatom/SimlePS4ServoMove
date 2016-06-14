function [] = groupSyncWriteRemoveParam( group_num, id )

calllib('dxl_x86_c', 'groupSyncWriteRemoveParam', group_num, id);
end


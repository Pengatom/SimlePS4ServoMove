function [] = groupBulkWriteRemoveParam( group_num, id )

calllib('dxl_x86_c', 'groupBulkWriteRemoveParam', group_num, id);
end


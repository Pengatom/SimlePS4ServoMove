function [] = groupBulkReadRemoveParam( group_num, id )

calllib('dxl_x86_c', 'groupBulkReadRemoveParam', group_num, id);
end


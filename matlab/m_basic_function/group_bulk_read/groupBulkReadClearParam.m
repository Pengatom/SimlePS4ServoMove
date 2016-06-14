function [] = groupBulkReadClearParam( group_num )

calllib('dxl_x86_c', 'groupBulkReadClearParam', group_num);
end


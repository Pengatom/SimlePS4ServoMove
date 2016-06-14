function [] = groupSyncReadClearParam( port_num )

calllib('dxl_x86_c', 'groupSyncReadClearParam', port_num);
end


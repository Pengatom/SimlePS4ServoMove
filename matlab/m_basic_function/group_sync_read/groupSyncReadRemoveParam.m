function [] = groupSyncReadRemoveParam( port_num, id )

calllib('dxl_x86_c', 'groupSyncReadRemoveParam', port_num, id);
end


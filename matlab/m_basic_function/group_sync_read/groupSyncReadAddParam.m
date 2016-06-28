function [VALUE] = groupSyncReadAddParam( port_num, id )

VALUE = calllib('dxl_x86_c', 'groupSyncReadAddParam', port_num, id);
end


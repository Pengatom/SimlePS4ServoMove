function [] = reBoot( port_num, protocol_version, id )

calllib('dxl_x86_c', 'reboot', port_num, protocol_version, id);
end


function [] = factoryReset( port_num, protocol_version, id, option )

calllib('dxl_x86_c', 'factoryReset', port_num, protocol_version, id, option);
end


function [] = broadcastPing( port_num, protocol_version )

calllib('dxl_x86_c', 'broadcastPing', port_num, protocol_version);
end


function [] = groupSyncReadRxPacket( port_num )

calllib('dxl_x86_c', 'groupSyncReadRxPacket', port_num);
end


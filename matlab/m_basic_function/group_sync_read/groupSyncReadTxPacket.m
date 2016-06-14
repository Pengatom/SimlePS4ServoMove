function [] = groupSyncReadTxPacket( port_num )

calllib('dxl_x86_c', 'groupSyncReadTxPacket', port_num);
end


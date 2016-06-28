function [] = groupSyncReadTxRxPacket( port_num )

calllib('dxl_x86_c', 'groupSyncReadTxRxPacket', port_num);
end


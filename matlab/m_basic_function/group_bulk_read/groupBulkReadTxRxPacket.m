function [] = groupBulkReadTxRxPacket( group_num )

calllib('dxl_x86_c', 'groupBulkReadTxRxPacket', group_num);
end


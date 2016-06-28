function [] = groupBulkReadTxPacket( group_num )

calllib('dxl_x86_c', 'groupBulkReadTxPacket', group_num);
end


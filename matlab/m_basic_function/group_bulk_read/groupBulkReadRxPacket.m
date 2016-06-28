function [] = groupBulkReadRxPacket( group_num )

calllib('dxl_x86_c', 'GroupBulkReadRxPacket', group_num);
end


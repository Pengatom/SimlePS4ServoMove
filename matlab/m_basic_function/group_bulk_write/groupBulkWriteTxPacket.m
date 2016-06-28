function [] = groupBulkWriteTxPacket( group_num )

calllib('dxl_x86_c', 'groupBulkWriteTxPacket', group_num);
end


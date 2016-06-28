function [] = groupSyncWriteTxPacket( group_num )

calllib('dxl_x86_c', 'groupSyncWriteTxPacket', group_num);
end


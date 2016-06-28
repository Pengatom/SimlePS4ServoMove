function [VALUE] = groupSyncReadIsAvailable( group_num, id, address, data_length )

VALUE = calllib('dxl_x86_c', 'groupSyncReadIsAvailable', group_num, id, address, data_length);
end


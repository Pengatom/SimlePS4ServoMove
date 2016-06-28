function [VALUE] = groupBulkReadIsAvailable( group_num, id, address, data_length)

VALUE = calllib('dxl_x86_c', 'groupBulkReadIsAvailable', group_num, id, address, data_length);
end


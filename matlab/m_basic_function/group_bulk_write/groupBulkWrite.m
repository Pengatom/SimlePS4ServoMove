function [VALUE] = groupBulkWrite( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'groupBulkWrite', port_num, protocol_version);
end


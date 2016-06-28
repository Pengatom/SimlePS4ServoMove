function [VALUE] = groupBulkRead( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'groupBulkRead', port_num, protocol_version);
end


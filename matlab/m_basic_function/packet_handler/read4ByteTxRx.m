function [VALUE] = read4ByteTxRx( port_num, protocol_version, id, address )

VALUE = calllib('dxl_x86_c', 'read4ByteTxRx', port_num, protocol_version, id, address);
end


function [VALUE] = read4ByteRx( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'read4ByteRx', port_num, protocol_version);
end


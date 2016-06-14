function [VALUE] = read1ByteRx( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'read1ByteRx', port_num, protocol_version);
end


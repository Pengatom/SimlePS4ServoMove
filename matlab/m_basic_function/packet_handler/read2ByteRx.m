function [VALUE] = read2ByteRx( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'read2ByteRx', port_num, protocol_version);
end


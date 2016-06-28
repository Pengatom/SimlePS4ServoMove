function [VALUE] = read1ByteTxRx( port_num, protocol_version, id, address )

VALUE = calllib('dxl_x86_c', 'read1ByteTxRx', port_num, protocol_version, id, address);
end


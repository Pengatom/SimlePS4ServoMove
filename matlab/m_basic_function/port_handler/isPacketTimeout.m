function [VALUE] = isPacketTimeout( port_num)

VALUE = calllib('dxl_x86_c', 'isPacketTimeout', port_num);
end


function [STRING] = getPortName( port_num )

STRING = calllib('dxl_x86_c', 'getPortName', port_num);
end


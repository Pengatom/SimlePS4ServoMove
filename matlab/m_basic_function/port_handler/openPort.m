function [VALUE] = openPort( port_num )

VALUE = calllib('dxl_x86_c', 'openPort', port_num);
end


function [VALUE] = getBaudRate( port_num )

VALUE = calllib('dxl_x86_c', 'getBaudRate', port_num);
end


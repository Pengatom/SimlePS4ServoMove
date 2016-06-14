function [VALUE] = setBaudRate( port_num, baudrate)

VALUE = calllib('dxl_x86_c', 'setBaudRate', port_num, baudrate);
end


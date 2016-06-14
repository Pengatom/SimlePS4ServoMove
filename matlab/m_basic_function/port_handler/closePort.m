function [] = closePort( port_num )

calllib('dxl_x86_c', 'closePort', port_num);

end
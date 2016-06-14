function [] = clearPort( port_num )

calllib('dxl_x86_c', 'clearPort', port_num);
end
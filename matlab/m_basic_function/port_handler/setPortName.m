function [] = setPortName( port_num, port_name )

calllib('dxl_x86_c', 'setPortName', port_num, port_name);
end


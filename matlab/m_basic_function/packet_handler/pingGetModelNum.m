function [VALUE] = pingGetModelNum( port_num, protocol_version, id )

VALUE = calllib('dxl_x86_c', 'pingGetModelNum', port_num, protocol_version, id);
end


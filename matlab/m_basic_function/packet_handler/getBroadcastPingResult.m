function [VALUE] = getBroadcastPingResult( port_num, protocol_version, id )

VALUE = calllib('dxl_x86_c', 'getBroadcastPingResult', port_num, protocol_version, id);
end


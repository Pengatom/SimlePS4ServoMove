function [VALUE] = getLastTxRxResult( port_num, protocol_version )

VALUE = calllib('dxl_x86_c', 'getLastTxRxResult', port_num, protocol_version);
end


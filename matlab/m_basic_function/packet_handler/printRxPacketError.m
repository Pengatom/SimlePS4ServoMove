function [] = printRxPacketError( protocol_version, error )

calllib('dxl_x86_c', 'printRxPacketError', protocol_version, error);
end


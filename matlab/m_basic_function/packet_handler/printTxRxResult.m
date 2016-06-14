function [] = printTxRxResult( protocol_version, result )

calllib('dxl_x86_c', 'printTxRxResult', protocol_version, result);
end


function [VALUE] = portHandler( DEVICE_NAME )

VALUE = calllib('dxl_x86_c', 'portHandler', DEVICE_NAME);
end


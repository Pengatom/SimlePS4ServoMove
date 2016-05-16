##################################################
# PROJECT: DynamixelSDK - ROBOTIS Ltd.
##################################################

#---------------------------------------------------------------------
# C++ COMPILER, COMPILER FLAGS, AND TARGET PROGRAM NAME
#---------------------------------------------------------------------
DIR_DXL     = ../..
DIR_OBJS    = ./.objects

INSTALL_ROOT = /usr/local

MAJ_VERSION = 2
MIN_VERSION = 0
REV_VERSION = 0

TARGET      = libdxl_x86_c.so
TARGET1     = $(TARGET).$(MAJ_VERSION)
TARGET2     = $(TARGET).$(MAJ_VERSION).$(MIN_VERSION)
TARGET3     = $(TARGET).$(MAJ_VERSION).$(MIN_VERSION).$(REV_VERSION)

CHK_DIR_EXISTS = test -d
PRINT       = echo
STRIP       = strip
AR          = ar
ARFLAGS     = cr
LD          = g++
LDFLAGS     = -shared -fPIC $(FORMAT)#-Wl,-soname,dxl
LD_CONFIG   = ldconfig
CP          = cp
CP_ALL      = cp -r
RM          = rm
RM_ALL      = rm -rf
SYMLINK     = ln -s
MKDIR       = mkdir
CC          = gcc
CX          = g++
CCFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall -c $(INCLUDES) $(FORMAT) -fPIC -g
CXFLAGS     = -O2 -O3 -DLINUX -D_GNU_SOURCE -Wall -c $(INCLUDES) $(FORMAT) -fPIC -g
FORMAT     = -m32
INCLUDES    += -I$(DIR_DXL)/include

#---------------------------------------------------------------------
# Required external libraries
#---------------------------------------------------------------------
LIBRARIES  += -lrt

#---------------------------------------------------------------------
# SDK Files
#---------------------------------------------------------------------
SOURCES  = src/dynamixel_sdk/GroupBulkRead.c \
           src/dynamixel_sdk/GroupBulkWrite.c \
           src/dynamixel_sdk/GroupSyncRead.c \
           src/dynamixel_sdk/GroupSyncWrite.c \
           src/dynamixel_sdk/PacketHandler.c \
           src/dynamixel_sdk/PortHandler.c \
           src/dynamixel_sdk/Protocol1PacketHandler.c \
           src/dynamixel_sdk/Protocol2PacketHandler.c \
           src/dynamixel_sdk_linux/PortHandlerLinux.c \


OBJECTS=$(addsuffix .o,$(addprefix $(DIR_OBJS)/,$(basename $(notdir $(SOURCES)))))


#---------------------------------------------------------------------
# COMPILING RULES
#---------------------------------------------------------------------
$(TARGET): makedirs $(OBJECTS)
	$(LD) $(LDFLAGS) -o ./$(TARGET) $(OBJECTS) $(LIBRARIES)

makedirs:
	mkdir -p $(DIR_OBJS)/

clean:
	rm -f $(OBJECTS) ./$(TARGET)

install: $(TARGET)
    # copy the binaries into the lib directory
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/lib || $(MKDIR) $(INSTALL_ROOT)/lib
	-$(CP) "./$(TARGET)" "$(INSTALL_ROOT)/lib/$(TARGET)"
	-$(SYMLINK) "$(INSTALL_ROOT)/lib/$(TARGET)" "$(INSTALL_ROOT)/lib/$(TARGET1)"
	-$(SYMLINK) "$(INSTALL_ROOT)/lib/$(TARGET)" "$(INSTALL_ROOT)/lib/$(TARGET2)"
	-$(SYMLINK) "$(INSTALL_ROOT)/lib/$(TARGET)" "$(INSTALL_ROOT)/lib/$(TARGET3)"

    # copy the headers into the include directory
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/include || $(MKDIR) $(INSTALL_ROOT)/include
	$(CP_ALL) $(DIR_DXL)/include/* $(INSTALL_ROOT)/include/

	$(LD_CONFIG)

uninstall:
	$(RM) $(INSTALL_ROOT)/lib/$(TARGET)
	$(RM) $(INSTALL_ROOT)/lib/$(TARGET1)
	$(RM) $(INSTALL_ROOT)/lib/$(TARGET2)
	$(RM) $(INSTALL_ROOT)/lib/$(TARGET3)

	$(RM) $(INSTALL_ROOT)/include/DynamixelSDK.h
	$(RM_ALL) $(INSTALL_ROOT)/include/dynamixel_sdk*

reinstall: uninstall install


#---------------------------------------------------------------------
# Make rules for all .c and .cpp files in each directory
#---------------------------------------------------------------------

$(DIR_OBJS)/%.o: $(DIR_DXL)/src/dynamixel_sdk/%.c
	$(CC) $(CCFLAGS) -c $? -o $@

$(DIR_OBJS)/%.o: $(DIR_DXL)/src/dynamixel_sdk/%.cpp
	$(CX) $(CXFLAGS) -c $? -o $@

$(DIR_OBJS)/%.o: $(DIR_DXL)/src/dynamixel_sdk_linux/%.c
	$(CC) $(CCFLAGS) -c $? -o $@

$(DIR_OBJS)/%.o: $(DIR_DXL)/src/dynamixel_sdk_linux/%.cpp
	$(CX) $(CXFLAGS) -c $? -o $@

#---------------------------------------------------------------------
# END OF MAKEFILE
#---------------------------------------------------------------------

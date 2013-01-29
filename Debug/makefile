################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include test/util/subdir.mk
-include test/server/subdir.mk
-include test/net/subdir.mk
-include test/misc/subdir.mk
-include test/ipc/subdir.mk
-include test/hb/subdir.mk
-include test/framework/subdir.mk
-include test/dispatcher2/subdir.mk
-include test/dispatcher/subdir.mk
-include test/database/subdir.mk
-include test/concurrent/subdir.mk
-include test/client/subdir.mk
-include test/channel/subdir.mk
-include test/buffer/subdir.mk
-include src/util/so/subdir.mk
-include src/util/container/subdir.mk
-include src/util/subdir.mk
-include src/timer/subdir.mk
-include src/thread/subdir.mk
-include src/protocol/redis/subdir.mk
-include src/process/subdir.mk
-include src/net/subdir.mk
-include src/misc/sds/subdir.mk
-include src/misc/http/subdir.mk
-include src/misc/crypto/subdir.mk
-include src/misc/cjson/subdir.mk
-include src/mem/mm/subdir.mk
-include src/mem/subdir.mk
-include src/logging/subdir.mk
-include src/ipc/subdir.mk
-include src/framework/vcs/subdir.mk
-include src/framework/process/subdir.mk
-include src/framework/handler/subdir.mk
-include src/framework/event/subdir.mk
-include src/framework/admin/subdir.mk
-include src/exception/subdir.mk
-include src/database/subdir.mk
-include src/concurrent/ngx/subdir.mk
-include src/concurrent/subdir.mk
-include src/common/subdir.mk
-include src/channel/timer/subdir.mk
-include src/channel/socket/subdir.mk
-include src/channel/signal/subdir.mk
-include src/channel/redis/subdir.mk
-include src/channel/inotify/subdir.mk
-include src/channel/http/subdir.mk
-include src/channel/fifo/subdir.mk
-include src/channel/codec/subdir.mk
-include src/channel/subdir.mk
-include src/buffer/subdir.mk
-include 3rd/sqlite3/subdir.mk
-include 3rd/kamailio/mem/subdir.mk
-include 3rd/kamailio/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: carch

# Tool invocations
carch: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++  -o "carch" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) carch
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

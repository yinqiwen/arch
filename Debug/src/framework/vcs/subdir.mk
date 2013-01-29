################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/vcs/virtual_channel_helper.cpp \
../src/framework/vcs/virtual_client_connection_manager.cpp 

OBJS += \
./src/framework/vcs/virtual_channel_helper.o \
./src/framework/vcs/virtual_client_connection_manager.o 

CPP_DEPS += \
./src/framework/vcs/virtual_channel_helper.d \
./src/framework/vcs/virtual_client_connection_manager.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/vcs/%.o: ../src/framework/vcs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



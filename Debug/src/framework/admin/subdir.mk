################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/admin/admin_command_handler.cpp \
../src/framework/admin/admin_message_handler.cpp 

OBJS += \
./src/framework/admin/admin_command_handler.o \
./src/framework/admin/admin_message_handler.o 

CPP_DEPS += \
./src/framework/admin/admin_command_handler.d \
./src/framework/admin/admin_message_handler.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/admin/%.o: ../src/framework/admin/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



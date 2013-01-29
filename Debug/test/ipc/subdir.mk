################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/ipc/ipc_ut.cpp 

OBJS += \
./test/ipc/ipc_ut.o 

CPP_DEPS += \
./test/ipc/ipc_ut.d 


# Each subdirectory must supply rules for building sources it contributes
test/ipc/%.o: ../test/ipc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



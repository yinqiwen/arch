################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocol/redis/redis_command_manager.cpp \
../src/protocol/redis/redis_frame_decoder.cpp 

OBJS += \
./src/protocol/redis/redis_command_manager.o \
./src/protocol/redis/redis_frame_decoder.o 

CPP_DEPS += \
./src/protocol/redis/redis_command_manager.d \
./src/protocol/redis/redis_frame_decoder.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocol/redis/%.o: ../src/protocol/redis/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/fifo/fifo_channel.cpp \
../src/channel/fifo/shm_fifo_channel.cpp 

OBJS += \
./src/channel/fifo/fifo_channel.o \
./src/channel/fifo/shm_fifo_channel.o 

CPP_DEPS += \
./src/channel/fifo/fifo_channel.d \
./src/channel/fifo/shm_fifo_channel.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/fifo/%.o: ../src/channel/fifo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/signal/signal_channel.cpp \
../src/channel/signal/soft_signal_channel.cpp 

OBJS += \
./src/channel/signal/signal_channel.o \
./src/channel/signal/soft_signal_channel.o 

CPP_DEPS += \
./src/channel/signal/signal_channel.d \
./src/channel/signal/soft_signal_channel.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/signal/%.o: ../src/channel/signal/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



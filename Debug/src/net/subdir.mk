################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/net/datagram_packet.cpp \
../src/net/network_helper.cpp \
../src/net/socket_address.cpp 

OBJS += \
./src/net/datagram_packet.o \
./src/net/network_helper.o \
./src/net/socket_address.o 

CPP_DEPS += \
./src/net/datagram_packet.d \
./src/net/network_helper.d \
./src/net/socket_address.d 


# Each subdirectory must supply rules for building sources it contributes
src/net/%.o: ../src/net/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/socket/clientsocket_channel.cpp \
../src/channel/socket/datagram_channel.cpp \
../src/channel/socket/serversocket_channel.cpp \
../src/channel/socket/socket_channel.cpp 

OBJS += \
./src/channel/socket/clientsocket_channel.o \
./src/channel/socket/datagram_channel.o \
./src/channel/socket/serversocket_channel.o \
./src/channel/socket/socket_channel.o 

CPP_DEPS += \
./src/channel/socket/clientsocket_channel.d \
./src/channel/socket/datagram_channel.d \
./src/channel/socket/serversocket_channel.d \
./src/channel/socket/socket_channel.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/socket/%.o: ../src/channel/socket/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



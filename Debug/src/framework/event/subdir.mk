################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/event/ipc_ctrl_event.cpp \
../src/framework/event/ipc_event.cpp \
../src/framework/event/ipc_event_factory.cpp \
../src/framework/event/ipc_message_event.cpp \
../src/framework/event/socket_ctrl_event.cpp \
../src/framework/event/socket_message_event.cpp 

OBJS += \
./src/framework/event/ipc_ctrl_event.o \
./src/framework/event/ipc_event.o \
./src/framework/event/ipc_event_factory.o \
./src/framework/event/ipc_message_event.o \
./src/framework/event/socket_ctrl_event.o \
./src/framework/event/socket_message_event.o 

CPP_DEPS += \
./src/framework/event/ipc_ctrl_event.d \
./src/framework/event/ipc_event.d \
./src/framework/event/ipc_event_factory.d \
./src/framework/event/ipc_message_event.d \
./src/framework/event/socket_ctrl_event.d \
./src/framework/event/socket_message_event.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/event/%.o: ../src/framework/event/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/handler/dispatcher_handler.cpp \
../src/framework/handler/dispatcher_ipc_event_handler.cpp \
../src/framework/handler/io_ipc_event_handler.cpp \
../src/framework/handler/ipc_event_decoder.cpp \
../src/framework/handler/ipc_event_encoder.cpp \
../src/framework/handler/manager_handler.cpp \
../src/framework/handler/manager_ipc_event_handler.cpp \
../src/framework/handler/service_handler.cpp \
../src/framework/handler/service_ipc_event_handler.cpp 

OBJS += \
./src/framework/handler/dispatcher_handler.o \
./src/framework/handler/dispatcher_ipc_event_handler.o \
./src/framework/handler/io_ipc_event_handler.o \
./src/framework/handler/ipc_event_decoder.o \
./src/framework/handler/ipc_event_encoder.o \
./src/framework/handler/manager_handler.o \
./src/framework/handler/manager_ipc_event_handler.o \
./src/framework/handler/service_handler.o \
./src/framework/handler/service_ipc_event_handler.o 

CPP_DEPS += \
./src/framework/handler/dispatcher_handler.d \
./src/framework/handler/dispatcher_ipc_event_handler.d \
./src/framework/handler/io_ipc_event_handler.d \
./src/framework/handler/ipc_event_decoder.d \
./src/framework/handler/ipc_event_encoder.d \
./src/framework/handler/manager_handler.d \
./src/framework/handler/manager_ipc_event_handler.d \
./src/framework/handler/service_handler.d \
./src/framework/handler/service_ipc_event_handler.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/handler/%.o: ../src/framework/handler/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



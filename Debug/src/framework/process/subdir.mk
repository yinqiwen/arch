################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/process/dispatcher_process.cpp \
../src/framework/process/manager_process.cpp \
../src/framework/process/service_process.cpp \
../src/framework/process/service_process_factory.cpp \
../src/framework/process/service_process_status.cpp 

OBJS += \
./src/framework/process/dispatcher_process.o \
./src/framework/process/manager_process.o \
./src/framework/process/service_process.o \
./src/framework/process/service_process_factory.o \
./src/framework/process/service_process_status.o 

CPP_DEPS += \
./src/framework/process/dispatcher_process.d \
./src/framework/process/manager_process.d \
./src/framework/process/service_process.d \
./src/framework/process/service_process_factory.d \
./src/framework/process/service_process_status.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/process/%.o: ../src/framework/process/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



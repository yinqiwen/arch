################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ipc/fifo.cpp \
../src/ipc/shm.cpp 

OBJS += \
./src/ipc/fifo.o \
./src/ipc/shm.o 

CPP_DEPS += \
./src/ipc/fifo.d \
./src/ipc/shm.d 


# Each subdirectory must supply rules for building sources it contributes
src/ipc/%.o: ../src/ipc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



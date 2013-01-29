################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/timer/timer.cpp \
../src/timer/timer_task_queue.cpp 

OBJS += \
./src/timer/timer.o \
./src/timer/timer_task_queue.o 

CPP_DEPS += \
./src/timer/timer.d \
./src/timer/timer_task_queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/timer/%.o: ../src/timer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



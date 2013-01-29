################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/thread/thread.cpp \
../src/thread/thread_pool.cpp \
../src/thread/thread_pool_timer.cpp 

OBJS += \
./src/thread/thread.o \
./src/thread/thread_pool.o \
./src/thread/thread_pool_timer.o 

CPP_DEPS += \
./src/thread/thread.d \
./src/thread/thread_pool.d \
./src/thread/thread_pool_timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/thread/%.o: ../src/thread/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



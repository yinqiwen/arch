################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/buffer/buffer.cpp \
../src/buffer/buffer_factory.cpp \
../src/buffer/buffer_helper.cpp \
../src/buffer/shm_byte_array.cpp 

OBJS += \
./src/buffer/buffer.o \
./src/buffer/buffer_factory.o \
./src/buffer/buffer_helper.o \
./src/buffer/shm_byte_array.o 

CPP_DEPS += \
./src/buffer/buffer.d \
./src/buffer/buffer_factory.d \
./src/buffer/buffer_helper.d \
./src/buffer/shm_byte_array.d 


# Each subdirectory must supply rules for building sources it contributes
src/buffer/%.o: ../src/buffer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/logging/appender.cpp \
../src/logging/layout.cpp \
../src/logging/logger.cpp 

OBJS += \
./src/logging/appender.o \
./src/logging/layout.o \
./src/logging/logger.o 

CPP_DEPS += \
./src/logging/appender.d \
./src/logging/layout.d \
./src/logging/logger.d 


# Each subdirectory must supply rules for building sources it contributes
src/logging/%.o: ../src/logging/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



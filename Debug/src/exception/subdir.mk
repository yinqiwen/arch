################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/exception/api_exception.cpp 

OBJS += \
./src/exception/api_exception.o 

CPP_DEPS += \
./src/exception/api_exception.d 


# Each subdirectory must supply rules for building sources it contributes
src/exception/%.o: ../src/exception/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



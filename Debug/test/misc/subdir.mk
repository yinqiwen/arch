################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/misc/misc_test.cpp \
../test/misc/utils_ut.cpp 

OBJS += \
./test/misc/misc_test.o \
./test/misc/utils_ut.o 

CPP_DEPS += \
./test/misc/misc_test.d \
./test/misc/utils_ut.d 


# Each subdirectory must supply rules for building sources it contributes
test/misc/%.o: ../test/misc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



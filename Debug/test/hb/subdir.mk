################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/hb/http_benchmark.cpp \
../test/hb/main.cpp 

OBJS += \
./test/hb/http_benchmark.o \
./test/hb/main.o 

CPP_DEPS += \
./test/hb/http_benchmark.d \
./test/hb/main.d 


# Each subdirectory must supply rules for building sources it contributes
test/hb/%.o: ../test/hb/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



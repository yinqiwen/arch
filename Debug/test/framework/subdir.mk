################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/framework/MyFramework.cpp \
../test/framework/MyManagerHandler.cpp \
../test/framework/MyManagerProcess.cpp \
../test/framework/MyServiceHandler.cpp 

OBJS += \
./test/framework/MyFramework.o \
./test/framework/MyManagerHandler.o \
./test/framework/MyManagerProcess.o \
./test/framework/MyServiceHandler.o 

CPP_DEPS += \
./test/framework/MyFramework.d \
./test/framework/MyManagerHandler.d \
./test/framework/MyManagerProcess.d \
./test/framework/MyServiceHandler.d 


# Each subdirectory must supply rules for building sources it contributes
test/framework/%.o: ../test/framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/dispatcher2/MyDispatcherHandler.cpp \
../test/dispatcher2/MyFramework.cpp \
../test/dispatcher2/MyManagerProcess.cpp \
../test/dispatcher2/MyNetworkEventDispatcherHandler.cpp \
../test/dispatcher2/MyServiceHandler.cpp 

OBJS += \
./test/dispatcher2/MyDispatcherHandler.o \
./test/dispatcher2/MyFramework.o \
./test/dispatcher2/MyManagerProcess.o \
./test/dispatcher2/MyNetworkEventDispatcherHandler.o \
./test/dispatcher2/MyServiceHandler.o 

CPP_DEPS += \
./test/dispatcher2/MyDispatcherHandler.d \
./test/dispatcher2/MyFramework.d \
./test/dispatcher2/MyManagerProcess.d \
./test/dispatcher2/MyNetworkEventDispatcherHandler.d \
./test/dispatcher2/MyServiceHandler.d 


# Each subdirectory must supply rules for building sources it contributes
test/dispatcher2/%.o: ../test/dispatcher2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



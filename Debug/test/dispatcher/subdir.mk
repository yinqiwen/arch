################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/dispatcher/MyDispatcherHandler.cpp \
../test/dispatcher/MyFramework.cpp \
../test/dispatcher/MyManagerProcess.cpp \
../test/dispatcher/MyNetworkEventDispatcherHandler.cpp \
../test/dispatcher/MyServiceHandler.cpp 

OBJS += \
./test/dispatcher/MyDispatcherHandler.o \
./test/dispatcher/MyFramework.o \
./test/dispatcher/MyManagerProcess.o \
./test/dispatcher/MyNetworkEventDispatcherHandler.o \
./test/dispatcher/MyServiceHandler.o 

CPP_DEPS += \
./test/dispatcher/MyDispatcherHandler.d \
./test/dispatcher/MyFramework.d \
./test/dispatcher/MyManagerProcess.d \
./test/dispatcher/MyNetworkEventDispatcherHandler.d \
./test/dispatcher/MyServiceHandler.d 


# Each subdirectory must supply rules for building sources it contributes
test/dispatcher/%.o: ../test/dispatcher/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



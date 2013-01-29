################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/channel.cpp \
../src/channel/channel_buffer_pool.cpp \
../src/channel/channel_handler_context.cpp \
../src/channel/channel_helper.cpp \
../src/channel/channel_pipeline.cpp \
../src/channel/channel_service.cpp 

OBJS += \
./src/channel/channel.o \
./src/channel/channel_buffer_pool.o \
./src/channel/channel_handler_context.o \
./src/channel/channel_helper.o \
./src/channel/channel_pipeline.o \
./src/channel/channel_service.o 

CPP_DEPS += \
./src/channel/channel.d \
./src/channel/channel_buffer_pool.d \
./src/channel/channel_handler_context.d \
./src/channel/channel_helper.d \
./src/channel/channel_pipeline.d \
./src/channel/channel_service.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/%.o: ../src/channel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



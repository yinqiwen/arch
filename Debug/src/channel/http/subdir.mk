################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/http/http_decoder.cpp \
../src/channel/http/http_encoder.cpp \
../src/channel/http/http_message.cpp 

OBJS += \
./src/channel/http/http_decoder.o \
./src/channel/http/http_encoder.o \
./src/channel/http/http_message.o 

CPP_DEPS += \
./src/channel/http/http_decoder.d \
./src/channel/http/http_encoder.d \
./src/channel/http/http_message.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/http/%.o: ../src/channel/http/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



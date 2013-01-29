################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/channel/codec/delimiter_frame_decoder.cpp \
../src/channel/codec/int_header_frame_decoder.cpp 

OBJS += \
./src/channel/codec/delimiter_frame_decoder.o \
./src/channel/codec/int_header_frame_decoder.o 

CPP_DEPS += \
./src/channel/codec/delimiter_frame_decoder.d \
./src/channel/codec/int_header_frame_decoder.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/codec/%.o: ../src/channel/codec/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



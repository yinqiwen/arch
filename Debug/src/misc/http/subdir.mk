################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/misc/http/http_parser.c 

OBJS += \
./src/misc/http/http_parser.o 

C_DEPS += \
./src/misc/http/http_parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/misc/http/%.o: ../src/misc/http/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



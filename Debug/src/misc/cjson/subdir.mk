################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/misc/cjson/cJSON.c 

OBJS += \
./src/misc/cjson/cJSON.o 

C_DEPS += \
./src/misc/cjson/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
src/misc/cjson/%.o: ../src/misc/cjson/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/misc/sds/sds.c 

OBJS += \
./src/misc/sds/sds.o 

C_DEPS += \
./src/misc/sds/sds.d 


# Each subdirectory must supply rules for building sources it contributes
src/misc/sds/%.o: ../src/misc/sds/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



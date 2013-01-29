################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/concurrent/ngx/ngx_shm_lock.c 

OBJS += \
./src/concurrent/ngx/ngx_shm_lock.o 

C_DEPS += \
./src/concurrent/ngx/ngx_shm_lock.d 


# Each subdirectory must supply rules for building sources it contributes
src/concurrent/ngx/%.o: ../src/concurrent/ngx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



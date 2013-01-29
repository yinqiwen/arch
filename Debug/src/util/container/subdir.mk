################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/util/container/adlist.c \
../src/util/container/endian.c \
../src/util/container/intset.c 

OBJS += \
./src/util/container/adlist.o \
./src/util/container/endian.o \
./src/util/container/intset.o 

C_DEPS += \
./src/util/container/adlist.d \
./src/util/container/endian.d \
./src/util/container/intset.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/container/%.o: ../src/util/container/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



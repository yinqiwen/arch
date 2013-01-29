################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mem/mm/mm_alloc.c \
../src/mem/mm/mm_core.c \
../src/mem/mm/mm_ext.c \
../src/mem/mm/mm_global.c \
../src/mem/mm/mm_lib.c \
../src/mem/mm/mm_vers.c 

OBJS += \
./src/mem/mm/mm_alloc.o \
./src/mem/mm/mm_core.o \
./src/mem/mm/mm_ext.o \
./src/mem/mm/mm_global.o \
./src/mem/mm/mm_lib.o \
./src/mem/mm/mm_vers.o 

C_DEPS += \
./src/mem/mm/mm_alloc.d \
./src/mem/mm/mm_core.d \
./src/mem/mm/mm_ext.d \
./src/mem/mm/mm_global.d \
./src/mem/mm/mm_lib.d \
./src/mem/mm/mm_vers.d 


# Each subdirectory must supply rules for building sources it contributes
src/mem/mm/%.o: ../src/mem/mm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



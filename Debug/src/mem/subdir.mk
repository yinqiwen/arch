################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mem/bit_scan.c \
../src/mem/mem_malloc.c \
../src/mem/shm_malloc.c 

CC_SRCS += \
../src/mem/ngx_slab.cc 

OBJS += \
./src/mem/bit_scan.o \
./src/mem/mem_malloc.o \
./src/mem/ngx_slab.o \
./src/mem/shm_malloc.o 

C_DEPS += \
./src/mem/bit_scan.d \
./src/mem/mem_malloc.d \
./src/mem/shm_malloc.d 

CC_DEPS += \
./src/mem/ngx_slab.d 


# Each subdirectory must supply rules for building sources it contributes
src/mem/%.o: ../src/mem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mem/%.o: ../src/mem/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



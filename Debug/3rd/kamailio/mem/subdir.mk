################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rd/kamailio/mem/dl_malloc.c \
../3rd/kamailio/mem/f_malloc.c \
../3rd/kamailio/mem/ll_malloc.c \
../3rd/kamailio/mem/mem.c \
../3rd/kamailio/mem/memtest.c \
../3rd/kamailio/mem/q_malloc.c \
../3rd/kamailio/mem/sf_malloc.c \
../3rd/kamailio/mem/shm_mem.c 

OBJS += \
./3rd/kamailio/mem/dl_malloc.o \
./3rd/kamailio/mem/f_malloc.o \
./3rd/kamailio/mem/ll_malloc.o \
./3rd/kamailio/mem/mem.o \
./3rd/kamailio/mem/memtest.o \
./3rd/kamailio/mem/q_malloc.o \
./3rd/kamailio/mem/sf_malloc.o \
./3rd/kamailio/mem/shm_mem.o 

C_DEPS += \
./3rd/kamailio/mem/dl_malloc.d \
./3rd/kamailio/mem/f_malloc.d \
./3rd/kamailio/mem/ll_malloc.d \
./3rd/kamailio/mem/mem.d \
./3rd/kamailio/mem/memtest.d \
./3rd/kamailio/mem/q_malloc.d \
./3rd/kamailio/mem/sf_malloc.d \
./3rd/kamailio/mem/shm_mem.d 


# Each subdirectory must supply rules for building sources it contributes
3rd/kamailio/mem/%.o: ../3rd/kamailio/mem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rd/kamailio/atomic_ops.c \
../3rd/kamailio/bit_count.c \
../3rd/kamailio/bit_scan.c \
../3rd/kamailio/crc.c \
../3rd/kamailio/daemonize.c \
../3rd/kamailio/dprint.c \
../3rd/kamailio/hash_func.c \
../3rd/kamailio/io_wait.c \
../3rd/kamailio/ip_addr.c \
../3rd/kamailio/local_timer.c \
../3rd/kamailio/lock_ops.c \
../3rd/kamailio/md5.c \
../3rd/kamailio/md5utils.c \
../3rd/kamailio/shm_init.c \
../3rd/kamailio/tcp_options.c 

OBJS += \
./3rd/kamailio/atomic_ops.o \
./3rd/kamailio/bit_count.o \
./3rd/kamailio/bit_scan.o \
./3rd/kamailio/crc.o \
./3rd/kamailio/daemonize.o \
./3rd/kamailio/dprint.o \
./3rd/kamailio/hash_func.o \
./3rd/kamailio/io_wait.o \
./3rd/kamailio/ip_addr.o \
./3rd/kamailio/local_timer.o \
./3rd/kamailio/lock_ops.o \
./3rd/kamailio/md5.o \
./3rd/kamailio/md5utils.o \
./3rd/kamailio/shm_init.o \
./3rd/kamailio/tcp_options.o 

C_DEPS += \
./3rd/kamailio/atomic_ops.d \
./3rd/kamailio/bit_count.d \
./3rd/kamailio/bit_scan.d \
./3rd/kamailio/crc.d \
./3rd/kamailio/daemonize.d \
./3rd/kamailio/dprint.d \
./3rd/kamailio/hash_func.d \
./3rd/kamailio/io_wait.d \
./3rd/kamailio/ip_addr.d \
./3rd/kamailio/local_timer.d \
./3rd/kamailio/lock_ops.d \
./3rd/kamailio/md5.d \
./3rd/kamailio/md5utils.d \
./3rd/kamailio/shm_init.d \
./3rd/kamailio/tcp_options.d 


# Each subdirectory must supply rules for building sources it contributes
3rd/kamailio/%.o: ../3rd/kamailio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



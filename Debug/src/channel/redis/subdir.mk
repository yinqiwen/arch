################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/channel/redis/ae.c 

CC_SRCS += \
../src/channel/redis/ae_epoll.cc \
../src/channel/redis/ae_kqueue.cc \
../src/channel/redis/ae_select.cc 

OBJS += \
./src/channel/redis/ae.o \
./src/channel/redis/ae_epoll.o \
./src/channel/redis/ae_kqueue.o \
./src/channel/redis/ae_select.o 

C_DEPS += \
./src/channel/redis/ae.d 

CC_DEPS += \
./src/channel/redis/ae_epoll.d \
./src/channel/redis/ae_kqueue.d \
./src/channel/redis/ae_select.d 


# Each subdirectory must supply rules for building sources it contributes
src/channel/redis/%.o: ../src/channel/redis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/channel/redis/%.o: ../src/channel/redis/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



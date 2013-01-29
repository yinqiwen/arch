################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/concurrent/process_rwlock.cpp \
../src/concurrent/semaphore.cpp \
../src/concurrent/thread_condition.cpp \
../src/concurrent/thread_mutex.cpp \
../src/concurrent/thread_mutex_lock.cpp \
../src/concurrent/thread_rwlock.cpp 

C_SRCS += \
../src/concurrent/atomic_ops.c \
../src/concurrent/lock_ops.c 

OBJS += \
./src/concurrent/atomic_ops.o \
./src/concurrent/lock_ops.o \
./src/concurrent/process_rwlock.o \
./src/concurrent/semaphore.o \
./src/concurrent/thread_condition.o \
./src/concurrent/thread_mutex.o \
./src/concurrent/thread_mutex_lock.o \
./src/concurrent/thread_rwlock.o 

C_DEPS += \
./src/concurrent/atomic_ops.d \
./src/concurrent/lock_ops.d 

CPP_DEPS += \
./src/concurrent/process_rwlock.d \
./src/concurrent/semaphore.d \
./src/concurrent/thread_condition.d \
./src/concurrent/thread_mutex.d \
./src/concurrent/thread_mutex_lock.d \
./src/concurrent/thread_rwlock.d 


# Each subdirectory must supply rules for building sources it contributes
src/concurrent/%.o: ../src/concurrent/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/concurrent/%.o: ../src/concurrent/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



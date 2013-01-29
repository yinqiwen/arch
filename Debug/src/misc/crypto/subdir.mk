################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/misc/crypto/md5.cpp 

C_SRCS += \
../src/misc/crypto/crc32.c \
../src/misc/crypto/rc4.c \
../src/misc/crypto/se.c \
../src/misc/crypto/sha1.c 

OBJS += \
./src/misc/crypto/crc32.o \
./src/misc/crypto/md5.o \
./src/misc/crypto/rc4.o \
./src/misc/crypto/se.o \
./src/misc/crypto/sha1.o 

C_DEPS += \
./src/misc/crypto/crc32.d \
./src/misc/crypto/rc4.d \
./src/misc/crypto/se.d \
./src/misc/crypto/sha1.d 

CPP_DEPS += \
./src/misc/crypto/md5.d 


# Each subdirectory must supply rules for building sources it contributes
src/misc/crypto/%.o: ../src/misc/crypto/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/misc/crypto/%.o: ../src/misc/crypto/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



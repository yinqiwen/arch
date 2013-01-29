################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rd/sqlite3/shell.c \
../3rd/sqlite3/sqlite3.c 

OBJS += \
./3rd/sqlite3/shell.o \
./3rd/sqlite3/sqlite3.o 

C_DEPS += \
./3rd/sqlite3/shell.d \
./3rd/sqlite3/sqlite3.d 


# Each subdirectory must supply rules for building sources it contributes
3rd/sqlite3/%.o: ../3rd/sqlite3/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



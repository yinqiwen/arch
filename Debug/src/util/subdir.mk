################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/config_helper.cpp \
../src/util/file_helper.cpp \
../src/util/math_helper.cpp \
../src/util/string_helper.cpp \
../src/util/system_helper.cpp \
../src/util/time_helper.cpp 

C_SRCS += \
../src/util/perftools_helper.c \
../src/util/proctitle_helper.c \
../src/util/regular_expression_helper.c \
../src/util/stringsearch.c \
../src/util/zmalloc.c 

OBJS += \
./src/util/config_helper.o \
./src/util/file_helper.o \
./src/util/math_helper.o \
./src/util/perftools_helper.o \
./src/util/proctitle_helper.o \
./src/util/regular_expression_helper.o \
./src/util/string_helper.o \
./src/util/stringsearch.o \
./src/util/system_helper.o \
./src/util/time_helper.o \
./src/util/zmalloc.o 

C_DEPS += \
./src/util/perftools_helper.d \
./src/util/proctitle_helper.d \
./src/util/regular_expression_helper.d \
./src/util/stringsearch.d \
./src/util/zmalloc.d 

CPP_DEPS += \
./src/util/config_helper.d \
./src/util/file_helper.d \
./src/util/math_helper.d \
./src/util/string_helper.d \
./src/util/system_helper.d \
./src/util/time_helper.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util/%.o: ../src/util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



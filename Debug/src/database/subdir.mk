################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/database/database_connection_manager.cpp \
../src/database/mysql_db_impl.cpp \
../src/database/sqlite3_db_impl.cpp 

OBJS += \
./src/database/database_connection_manager.o \
./src/database/mysql_db_impl.o \
./src/database/sqlite3_db_impl.o 

CPP_DEPS += \
./src/database/database_connection_manager.d \
./src/database/mysql_db_impl.d \
./src/database/sqlite3_db_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/database/%.o: ../src/database/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/clasificar.cpp \
../src/svm.cpp 

OBJS += \
./src/clasificar.o \
./src/svm.o 

CPP_DEPS += \
./src/clasificar.d \
./src/svm.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/mrpt/base/include/ -I/usr/local/include/mrpt/mrpt-config/ -I/usr/local/include/mrpt/gui/include/ -I/usr/local/include/mrpt/opengl/include/ -I/usr/include/mrpt/gui/ -I/usr/include -I../include -I/usr/include/mrpt/base/include -O0 -g3 -Wall -c -fmessage-length=0 -fpermissive -Wno-enum-compare -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



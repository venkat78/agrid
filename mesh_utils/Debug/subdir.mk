################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../mesh_utils.cpp 

OBJS += \
./mesh_utils.o 

CPP_DEPS += \
./mesh_utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/yury/git/agrid/utils" -I"/home/yury/git/agrid/spatial_trees" -I"/home/yury/git/agrid/geom" -I"/home/yury/git/agrid/tmesh" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



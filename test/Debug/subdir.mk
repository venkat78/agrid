################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test.cpp 

OBJS += \
./test.o 

CPP_DEPS += \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/Bujji/git/agrid/mesh_utils" -I"/home/Bujji/git/agrid/spatial_trees" -I"/home/Bujji/git/agrid/agrid-boolean" -I"/home/Bujji/git/agrid/agrid-mesh" -I"/home/Bujji/git/agrid/surface_mesh" -I"/home/Bujji/git/agrid/tmesh" -I"/home/Bujji/git/agrid/utils" -I"/home/Bujji/git/agrid/io" -I"/home/Bujji/git/agrid/geom" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



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
	g++ -I"/home/yury/git/agrid/mesh_utils" -I"/home/yury/git/agrid/base-grid" -I"/home/yury/git/agrid/grid-gen" -I"/home/yury/git/agrid/model" -I"/home/yury/git/agrid/spatial_trees" -I"/home/yury/git/agrid/agrid-boolean" -I"/home/yury/git/agrid/agrid-mesh" -I"/home/yury/git/agrid/surface_mesh" -I"/home/yury/git/agrid/tmesh" -I"/home/yury/git/agrid/utils" -I"/home/yury/git/agrid/io" -I"/home/yury/git/agrid/geom" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cut_cell.cpp \
../cut_cell_clay.cpp \
../volumetric_grid.cpp 

OBJS += \
./cut_cell.o \
./cut_cell_clay.o \
./volumetric_grid.o 

CPP_DEPS += \
./cut_cell.d \
./cut_cell_clay.d \
./volumetric_grid.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/Bujji/git/agrid/geom" -I"/home/Bujji/git/agrid/spatial_trees" -I"/home/Bujji/git/agrid/tmesh" -I"/home/Bujji/git/agrid/mesh_utils" -I"/home/Bujji/git/agrid/surface_mesh" -I"/home/Bujji/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



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
	g++ -I"/home/venkat/git/agrid/geom" -I"/home/venkat/git/agrid/base-grid" -I"/home/venkat/git/agrid/spatial_trees" -I"/home/venkat/git/agrid/tmesh" -I"/home/venkat/git/agrid/mesh_utils" -I"/home/venkat/git/agrid/surface_mesh" -I"/home/venkat/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../base_grid.cpp \
../grid.cpp \
../grid_cell.cpp \
../grid_object_factory.cpp 

OBJS += \
./base_grid.o \
./grid.o \
./grid_cell.o \
./grid_object_factory.o 

CPP_DEPS += \
./base_grid.d \
./grid.d \
./grid_cell.d \
./grid_object_factory.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/venkat/git/agrid/agrid-mesh" -I"/home/venkat/git/agrid/geom" -I"/home/venkat/git/agrid/mesh_utils" -I"/home/venkat/git/agrid/model" -I"/home/venkat/git/agrid/spatial_trees" -I"/home/venkat/git/agrid/surface_mesh" -I"/home/venkat/git/agrid/tmesh" -I"/home/venkat/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



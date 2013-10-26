################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../bob.cpp \
../cell.cpp \
../convex_decomposer.cpp \
../cut_cell_builder.cpp \
../cut_cells_adhesive.cpp \
../cut_cells_boolean.cpp \
../cut_cells_generator.cpp \
../foam_export.cpp \
../grid_object_factory.cpp \
../planar_cutter.cpp \
../rectilinear_grid.cpp \
../vol_grid_repair.cpp 

OBJS += \
./bob.o \
./cell.o \
./convex_decomposer.o \
./cut_cell_builder.o \
./cut_cells_adhesive.o \
./cut_cells_boolean.o \
./cut_cells_generator.o \
./foam_export.o \
./grid_object_factory.o \
./planar_cutter.o \
./rectilinear_grid.o \
./vol_grid_repair.o 

CPP_DEPS += \
./bob.d \
./cell.d \
./convex_decomposer.d \
./cut_cell_builder.d \
./cut_cells_adhesive.d \
./cut_cells_boolean.d \
./cut_cells_generator.d \
./foam_export.d \
./grid_object_factory.d \
./planar_cutter.d \
./rectilinear_grid.d \
./vol_grid_repair.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/yury/git/agrid/surface_mesh" -I"/home/yury/git/agrid/base-grid" -I"/home/yury/git/agrid/grid-gen" -I"/home/yury/git/agrid/model" -I"/home/yury/git/agrid/spatial_trees" -I"/home/yury/git/agrid/agrid-mesh" -I"/home/yury/git/agrid/mesh_utils" -I"/home/yury/git/agrid/tmesh" -I"/home/yury/git/agrid/geom" -I"/home/yury/git/agrid/io" -I"/home/yury/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



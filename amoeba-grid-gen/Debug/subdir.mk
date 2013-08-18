################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../amoeba_grid_clay.cpp \
../amoeba_grid_generator.cpp \
../generic_model_grid_clay.cpp \
../generic_model_grid_generator.cpp \
../test.cpp 

OBJS += \
./amoeba_grid_clay.o \
./amoeba_grid_generator.o \
./generic_model_grid_clay.o \
./generic_model_grid_generator.o \
./test.o 

CPP_DEPS += \
./amoeba_grid_clay.d \
./amoeba_grid_generator.d \
./generic_model_grid_clay.d \
./generic_model_grid_generator.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__=1 -I"/home/venkat/git/agrid/base-grid" -I"/home/venkat/git/agrid/agrid-mesh" -I"/home/venkat/git/agrid/geom" -I"/home/venkat/git/agrid/grid-gen" -I"/home/venkat/git/agrid/io" -I"/home/venkat/git/agrid/mesh_utils" -I"/home/venkat/git/agrid/model" -I"/home/venkat/git/agrid/spatial_trees" -I"/home/venkat/git/agrid/surface_mesh" -I"/home/venkat/git/agrid/tmesh" -I"/home/venkat/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



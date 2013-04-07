################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cell_off_writer.cpp \
../cell_record_generator.cpp \
../foam_cell_to_off.cpp \
../foam_reader.cpp \
../off.cpp \
../off_merger.cpp \
../off_reader.cpp \
../stl.cpp 

OBJS += \
./cell_off_writer.o \
./cell_record_generator.o \
./foam_cell_to_off.o \
./foam_reader.o \
./off.o \
./off_merger.o \
./off_reader.o \
./stl.o 

CPP_DEPS += \
./cell_off_writer.d \
./cell_record_generator.d \
./foam_cell_to_off.d \
./foam_reader.d \
./off.d \
./off_merger.d \
./off_reader.d \
./stl.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/yury/git/agrid/surface_mesh" -I"/home/yury/git/agrid/spatial_trees" -I"/home/yury/git/agrid/mesh_utils" -I"/home/yury/git/agrid/tmesh" -I"/home/yury/git/agrid/geom" -I"/home/yury/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



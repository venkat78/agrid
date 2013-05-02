################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../amoeba_io.cpp \
../cell_off_writer.cpp \
../cell_record_generator.cpp \
../foam_cell_to_off.cpp \
../foam_reader.cpp \
../off.cpp \
../off_merger.cpp \
../off_reader.cpp \
../stl.cpp 

OBJS += \
./amoeba_io.o \
./cell_off_writer.o \
./cell_record_generator.o \
./foam_cell_to_off.o \
./foam_reader.o \
./off.o \
./off_merger.o \
./off_reader.o \
./stl.o 

CPP_DEPS += \
./amoeba_io.d \
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
	g++ -I"/home/Bujji/git/agrid/surface_mesh" -I"/home/Bujji/git/agrid/model" -I"/home/Bujji/git/agrid/spatial_trees" -I"/home/Bujji/git/agrid/mesh_utils" -I"/home/Bujji/git/agrid/tmesh" -I"/home/Bujji/git/agrid/geom" -I"/home/Bujji/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



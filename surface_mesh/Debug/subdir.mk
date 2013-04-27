################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cleanup.cpp \
../cylinder.cpp \
../facet_merger.cpp \
../hole_closer.cpp \
../hole_filler.cpp \
../surface_mesh.cpp 

OBJS += \
./cleanup.o \
./cylinder.o \
./facet_merger.o \
./hole_closer.o \
./hole_filler.o \
./surface_mesh.o 

CPP_DEPS += \
./cleanup.d \
./cylinder.d \
./facet_merger.d \
./hole_closer.d \
./hole_filler.d \
./surface_mesh.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/Bujji/git/agrid/utils" -I"/home/Bujji/git/agrid/spatial_trees" -I"/home/Bujji/git/agrid/mesh_utils" -I"/home/Bujji/git/agrid/geom" -I"/home/Bujji/git/agrid/tmesh" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



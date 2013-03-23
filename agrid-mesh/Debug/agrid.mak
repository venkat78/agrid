RM := rm -rf
CC := /usr/bin/g++

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
	g++ -I"../../geom" -I"../../spatial_trees" -I"../../tmesh" -I"../../mesh_utils" -I"../../surface_mesh" -I"../../utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: libagrid-mesh.a

# Tool invocations
libagrid-mesh.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libagrid-mesh.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) libagrid-mesh.a
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

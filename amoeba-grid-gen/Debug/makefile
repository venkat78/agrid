################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: amoeba-grid-gen

# Tool invocations
amoeba-grid-gen: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L"/home/venkat/git/agrid/base-grid/Debug" -L"/home/venkat/git/agrid/grid-gen/Debug" -L"/home/venkat/git/agrid/agrid-mesh/Debug" -L"/home/venkat/git/agrid/geom/Debug" -L"/home/venkat/git/agrid/grid-gen/Debug" -L"/home/venkat/git/agrid/io/Debug" -L"/home/venkat/git/agrid/mesh_utils/Debug" -L"/home/venkat/git/agrid/model/Debug" -L"/home/venkat/git/agrid/spatial_trees/Debug" -L"/home/venkat/git/agrid/surface_mesh/Debug" -L"/home/venkat/git/agrid/tmesh/Debug" -L"/home/venkat/git/agrid/utils/Debug" -o "amoeba-grid-gen" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) amoeba-grid-gen
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

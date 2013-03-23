RM := rm -rf
CC := /usr/bin/g++

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../geom_methods.cpp \
../spatial_trees.cpp 

OBJS += \
./geom_methods.o \
./spatial_trees.o 

CPP_DEPS += \
./geom_methods.d \
./spatial_trees.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) -I"../../utils" -I"../../geom" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: libspatial_trees.a

# Tool invocations
libspatial_trees.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libspatial_trees.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) libspatial_trees.a
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

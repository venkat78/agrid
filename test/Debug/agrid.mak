RM := rm -rf
CC := /usr/bin/g++

CPP_SRCS += \
../test.cpp 

OBJS += \
./test.o 

CPP_DEPS += \
./test.d 

LIBS := -lagrid-boolean -lagrid-mesh -lio -lmesh_utils -lmesh -ltmesh -lspatial_trees -lgeom -lutils

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) -I"../../mesh_utils" -I"../../spatial_trees" -I"../../agrid-boolean" -I"../../agrid-mesh" -I"../../surface_mesh" -I"../../tmesh" -I"../../utils" -I"../../io" -I"../../geom" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
all: test

# Tool invocations
test: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) -L"../../io/Debug" -L"../../agrid-mesh/Debug" -L"../../agrid-boolean/Debug" -L"../../surface_mesh/Debug" -L"../../mesh_utils/Debug" -L"../../tmesh/Debug" -L"../../spatial_trees/Debug" -L"../../geom/Debug" -L"../../utils/Debug" -o"test" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) test
	-@echo ' '

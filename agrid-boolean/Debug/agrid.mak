RM := rm -rf
CC := /usr/bin/g++


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
	$(CC) -I"../../surface_mesh" -I"../../spatial_trees" -I"../../agrid-mesh" -I"../../mesh_utils" -I"../../tmesh" -I"../../geom" -I"../../io" -I"../../utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: libagrid-boolean.a

# Tool invocations
libagrid-boolean.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libagrid-boolean.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) libagrid-boolean.a
	-@echo ' '


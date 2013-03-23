RM := rm -rf
CC := /usr/bin/g++

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
	$(CC) -I"../../surface_mesh" -I"../../spatial_trees" -I"../../mesh_utils" -I"../../tmesh" -I"../../geom" -I"../../utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

all: libio.a

# Tool invocations
libio.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libio.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) libio.a
	-@echo ' '


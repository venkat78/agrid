################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../add_facet.cpp \
../construct_manifolds.cpp \
../delete_facet.cpp \
../dump.cpp \
../edge.cpp \
../euler_operators.cpp \
../facet.cpp \
../geom_info.cpp \
../geom_ops.cpp \
../half_edge.cpp \
../manifold.cpp \
../new_vertex_trigger.cpp \
../tmesh.cpp \
../tmesh_clipper.cpp \
../tmesh_facet.cpp \
../tmesh_facets_ops.cpp \
../tmesh_gen.cpp \
../tmesh_geom_queries.cpp \
../tmesh_manifold.cpp \
../tmesh_modifiers.cpp \
../tmesh_object_factory.cpp \
../tmesh_vertex.cpp \
../topo_operators.cpp \
../verify.cpp \
../vertex.cpp \
../vertex_use.cpp 

OBJS += \
./add_facet.o \
./construct_manifolds.o \
./delete_facet.o \
./dump.o \
./edge.o \
./euler_operators.o \
./facet.o \
./geom_info.o \
./geom_ops.o \
./half_edge.o \
./manifold.o \
./new_vertex_trigger.o \
./tmesh.o \
./tmesh_clipper.o \
./tmesh_facet.o \
./tmesh_facets_ops.o \
./tmesh_gen.o \
./tmesh_geom_queries.o \
./tmesh_manifold.o \
./tmesh_modifiers.o \
./tmesh_object_factory.o \
./tmesh_vertex.o \
./topo_operators.o \
./verify.o \
./vertex.o \
./vertex_use.o 

CPP_DEPS += \
./add_facet.d \
./construct_manifolds.d \
./delete_facet.d \
./dump.d \
./edge.d \
./euler_operators.d \
./facet.d \
./geom_info.d \
./geom_ops.d \
./half_edge.d \
./manifold.d \
./new_vertex_trigger.d \
./tmesh.d \
./tmesh_clipper.d \
./tmesh_facet.d \
./tmesh_facets_ops.d \
./tmesh_gen.d \
./tmesh_geom_queries.d \
./tmesh_manifold.d \
./tmesh_modifiers.d \
./tmesh_object_factory.d \
./tmesh_vertex.d \
./topo_operators.d \
./verify.d \
./vertex.d \
./vertex_use.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/venkat/git/agrid/utils" -I"/home/venkat/git/agrid/spatial_trees" -I"/home/venkat/git/agrid/geom" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../box2.cpp \
../box3.cpp \
../box3i.cpp \
../do_intersect.cpp \
../fpcube.cpp \
../intersection.cpp \
../line2.cpp \
../newell_normal_builder.cpp \
../numerics2d.cpp \
../numerics3d.cpp \
../pcube.cpp \
../plane3.cpp \
../point3.cpp \
../polygon_splitter.cpp \
../segment2.cpp \
../segment3.cpp \
../transform3.cpp \
../triangle2.cpp \
../triangle3.cpp \
../vector3.cpp 

OBJS += \
./box2.o \
./box3.o \
./box3i.o \
./do_intersect.o \
./fpcube.o \
./intersection.o \
./line2.o \
./newell_normal_builder.o \
./numerics2d.o \
./numerics3d.o \
./pcube.o \
./plane3.o \
./point3.o \
./polygon_splitter.o \
./segment2.o \
./segment3.o \
./transform3.o \
./triangle2.o \
./triangle3.o \
./vector3.o 

CPP_DEPS += \
./box2.d \
./box3.d \
./box3i.d \
./do_intersect.d \
./fpcube.d \
./intersection.d \
./line2.d \
./newell_normal_builder.d \
./numerics2d.d \
./numerics3d.d \
./pcube.d \
./plane3.d \
./point3.d \
./polygon_splitter.d \
./segment2.d \
./segment3.d \
./transform3.d \
./triangle2.d \
./triangle3.d \
./vector3.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/Bujji/git/agrid/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



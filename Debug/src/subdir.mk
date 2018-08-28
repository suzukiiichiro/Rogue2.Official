################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/display.c \
../src/hit.c \
../src/init.c \
../src/invent.c \
../src/level.c \
../src/machdep.c \
../src/main.c \
../src/message.c \
../src/monster.c \
../src/move.c \
../src/object.c \
../src/pack.c \
../src/play.c \
../src/random.c \
../src/ring.c \
../src/room.c \
../src/save.c \
../src/score.c \
../src/spechit.c \
../src/throw.c \
../src/trap.c \
../src/use.c \
../src/zap.c 

OBJS += \
./src/display.o \
./src/hit.o \
./src/init.o \
./src/invent.o \
./src/level.o \
./src/machdep.o \
./src/main.o \
./src/message.o \
./src/monster.o \
./src/move.o \
./src/object.o \
./src/pack.o \
./src/play.o \
./src/random.o \
./src/ring.o \
./src/room.o \
./src/save.o \
./src/score.o \
./src/spechit.o \
./src/throw.o \
./src/trap.o \
./src/use.o \
./src/zap.o 

C_DEPS += \
./src/display.d \
./src/hit.d \
./src/init.d \
./src/invent.d \
./src/level.d \
./src/machdep.d \
./src/main.d \
./src/message.d \
./src/monster.d \
./src/move.d \
./src/object.d \
./src/pack.d \
./src/play.d \
./src/random.d \
./src/ring.d \
./src/room.d \
./src/save.d \
./src/score.d \
./src/spechit.d \
./src/throw.d \
./src/trap.d \
./src/use.d \
./src/zap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



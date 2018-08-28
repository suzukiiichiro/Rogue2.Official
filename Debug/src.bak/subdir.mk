################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src.bak/display.c \
../src.bak/hit.c \
../src.bak/init.c \
../src.bak/invent.c \
../src.bak/level.c \
../src.bak/machdep.c \
../src.bak/main.c \
../src.bak/message.c \
../src.bak/monster.c \
../src.bak/move.c \
../src.bak/object.c \
../src.bak/pack.c \
../src.bak/play.c \
../src.bak/random.c \
../src.bak/ring.c \
../src.bak/room.c \
../src.bak/save.c \
../src.bak/score.c \
../src.bak/spechit.c \
../src.bak/throw.c \
../src.bak/trap.c \
../src.bak/use.c \
../src.bak/zap.c 

OBJS += \
./src.bak/display.o \
./src.bak/hit.o \
./src.bak/init.o \
./src.bak/invent.o \
./src.bak/level.o \
./src.bak/machdep.o \
./src.bak/main.o \
./src.bak/message.o \
./src.bak/monster.o \
./src.bak/move.o \
./src.bak/object.o \
./src.bak/pack.o \
./src.bak/play.o \
./src.bak/random.o \
./src.bak/ring.o \
./src.bak/room.o \
./src.bak/save.o \
./src.bak/score.o \
./src.bak/spechit.o \
./src.bak/throw.o \
./src.bak/trap.o \
./src.bak/use.o \
./src.bak/zap.o 

C_DEPS += \
./src.bak/display.d \
./src.bak/hit.d \
./src.bak/init.d \
./src.bak/invent.d \
./src.bak/level.d \
./src.bak/machdep.d \
./src.bak/main.d \
./src.bak/message.d \
./src.bak/monster.d \
./src.bak/move.d \
./src.bak/object.d \
./src.bak/pack.d \
./src.bak/play.d \
./src.bak/random.d \
./src.bak/ring.d \
./src.bak/room.d \
./src.bak/save.d \
./src.bak/score.d \
./src.bak/spechit.d \
./src.bak/throw.d \
./src.bak/trap.d \
./src.bak/use.d \
./src.bak/zap.d 


# Each subdirectory must supply rules for building sources it contributes
src.bak/%.o: ../src.bak/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



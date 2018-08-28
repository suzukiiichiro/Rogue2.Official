################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src.utf8/display.c \
../src.utf8/hit.c \
../src.utf8/init.c \
../src.utf8/invent.c \
../src.utf8/level.c \
../src.utf8/machdep.c \
../src.utf8/main.c \
../src.utf8/message.c \
../src.utf8/monster.c \
../src.utf8/move.c \
../src.utf8/object.c \
../src.utf8/pack.c \
../src.utf8/play.c \
../src.utf8/random.c \
../src.utf8/ring.c \
../src.utf8/room.c \
../src.utf8/save.c \
../src.utf8/score.c \
../src.utf8/spechit.c \
../src.utf8/throw.c \
../src.utf8/trap.c \
../src.utf8/use.c \
../src.utf8/zap.c 

O_SRCS += \
../src.utf8/rogue-display.o \
../src.utf8/rogue-hit.o \
../src.utf8/rogue-init.o \
../src.utf8/rogue-invent.o \
../src.utf8/rogue-level.o \
../src.utf8/rogue-machdep.o \
../src.utf8/rogue-main.o \
../src.utf8/rogue-message.o \
../src.utf8/rogue-monster.o \
../src.utf8/rogue-move.o \
../src.utf8/rogue-object.o \
../src.utf8/rogue-pack.o \
../src.utf8/rogue-play.o \
../src.utf8/rogue-random.o \
../src.utf8/rogue-ring.o \
../src.utf8/rogue-room.o \
../src.utf8/rogue-save.o \
../src.utf8/rogue-score.o \
../src.utf8/rogue-spechit.o \
../src.utf8/rogue-throw.o \
../src.utf8/rogue-trap.o \
../src.utf8/rogue-use.o \
../src.utf8/rogue-zap.o 

OBJS += \
./src.utf8/display.o \
./src.utf8/hit.o \
./src.utf8/init.o \
./src.utf8/invent.o \
./src.utf8/level.o \
./src.utf8/machdep.o \
./src.utf8/main.o \
./src.utf8/message.o \
./src.utf8/monster.o \
./src.utf8/move.o \
./src.utf8/object.o \
./src.utf8/pack.o \
./src.utf8/play.o \
./src.utf8/random.o \
./src.utf8/ring.o \
./src.utf8/room.o \
./src.utf8/save.o \
./src.utf8/score.o \
./src.utf8/spechit.o \
./src.utf8/throw.o \
./src.utf8/trap.o \
./src.utf8/use.o \
./src.utf8/zap.o 

C_DEPS += \
./src.utf8/display.d \
./src.utf8/hit.d \
./src.utf8/init.d \
./src.utf8/invent.d \
./src.utf8/level.d \
./src.utf8/machdep.d \
./src.utf8/main.d \
./src.utf8/message.d \
./src.utf8/monster.d \
./src.utf8/move.d \
./src.utf8/object.d \
./src.utf8/pack.d \
./src.utf8/play.d \
./src.utf8/random.d \
./src.utf8/ring.d \
./src.utf8/room.d \
./src.utf8/save.d \
./src.utf8/score.d \
./src.utf8/spechit.d \
./src.utf8/throw.d \
./src.utf8/trap.d \
./src.utf8/use.d \
./src.utf8/zap.d 


# Each subdirectory must supply rules for building sources it contributes
src.utf8/%.o: ../src.utf8/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



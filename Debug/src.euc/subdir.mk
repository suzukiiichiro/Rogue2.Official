################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src.euc/display.c \
../src.euc/hit.c \
../src.euc/init.c \
../src.euc/invent.c \
../src.euc/level.c \
../src.euc/machdep.c \
../src.euc/main.c \
../src.euc/message.c \
../src.euc/monster.c \
../src.euc/move.c \
../src.euc/object.c \
../src.euc/pack.c \
../src.euc/play.c \
../src.euc/random.c \
../src.euc/ring.c \
../src.euc/room.c \
../src.euc/save.c \
../src.euc/score.c \
../src.euc/spechit.c \
../src.euc/throw.c \
../src.euc/trap.c \
../src.euc/use.c \
../src.euc/zap.c 

OBJS += \
./src.euc/display.o \
./src.euc/hit.o \
./src.euc/init.o \
./src.euc/invent.o \
./src.euc/level.o \
./src.euc/machdep.o \
./src.euc/main.o \
./src.euc/message.o \
./src.euc/monster.o \
./src.euc/move.o \
./src.euc/object.o \
./src.euc/pack.o \
./src.euc/play.o \
./src.euc/random.o \
./src.euc/ring.o \
./src.euc/room.o \
./src.euc/save.o \
./src.euc/score.o \
./src.euc/spechit.o \
./src.euc/throw.o \
./src.euc/trap.o \
./src.euc/use.o \
./src.euc/zap.o 

C_DEPS += \
./src.euc/display.d \
./src.euc/hit.d \
./src.euc/init.d \
./src.euc/invent.d \
./src.euc/level.d \
./src.euc/machdep.d \
./src.euc/main.d \
./src.euc/message.d \
./src.euc/monster.d \
./src.euc/move.d \
./src.euc/object.d \
./src.euc/pack.d \
./src.euc/play.d \
./src.euc/random.d \
./src.euc/ring.d \
./src.euc/room.d \
./src.euc/save.d \
./src.euc/score.d \
./src.euc/spechit.d \
./src.euc/throw.d \
./src.euc/trap.d \
./src.euc/use.d \
./src.euc/zap.d 


# Each subdirectory must supply rules for building sources it contributes
src.euc/%.o: ../src.euc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Control_App.c \
../PWM.c \
../buzzer.c \
../external_eeprom.c \
../gpio.c \
../lcd.c \
../motor.c \
../timer.c \
../twi.c \
../uart.c 

OBJS += \
./Control_App.o \
./PWM.o \
./buzzer.o \
./external_eeprom.o \
./gpio.o \
./lcd.o \
./motor.o \
./timer.o \
./twi.o \
./uart.o 

C_DEPS += \
./Control_App.d \
./PWM.d \
./buzzer.d \
./external_eeprom.d \
./gpio.d \
./lcd.d \
./motor.d \
./timer.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



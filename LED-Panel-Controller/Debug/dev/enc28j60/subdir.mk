################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dev/enc28j60/enc28j60.c 

OBJS += \
./dev/enc28j60/enc28j60.o 

C_DEPS += \
./dev/enc28j60/enc28j60.d 


# Each subdirectory must supply rules for building sources it contributes
dev/enc28j60/%.o: ../dev/enc28j60/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



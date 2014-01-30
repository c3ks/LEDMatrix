################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dev/soft-spi/spi-master.c 

OBJS += \
./dev/soft-spi/spi-master.o 

C_DEPS += \
./dev/soft-spi/spi-master.d 


# Each subdirectory must supply rules for building sources it contributes
dev/soft-spi/%.o: ../dev/soft-spi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



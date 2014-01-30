################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../net/udp/ntp-client/ntp_client.c 

OBJS += \
./net/udp/ntp-client/ntp_client.o 

C_DEPS += \
./net/udp/ntp-client/ntp_client.d 


# Each subdirectory must supply rules for building sources it contributes
net/udp/ntp-client/%.o: ../net/udp/ntp-client/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



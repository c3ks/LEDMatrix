################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../net/udp/dhcp-client/dhcp_client.c 

OBJS += \
./net/udp/dhcp-client/dhcp_client.o 

C_DEPS += \
./net/udp/dhcp-client/dhcp_client.d 


# Each subdirectory must supply rules for building sources it contributes
net/udp/dhcp-client/%.o: ../net/udp/dhcp-client/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



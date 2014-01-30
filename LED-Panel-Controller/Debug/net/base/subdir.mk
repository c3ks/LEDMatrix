################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../net/base/arp.c \
../net/base/icmp.c \
../net/base/ip.c \
../net/base/nic.c 

OBJS += \
./net/base/arp.o \
./net/base/icmp.o \
./net/base/ip.o \
./net/base/nic.o 

C_DEPS += \
./net/base/arp.d \
./net/base/icmp.d \
./net/base/ip.d \
./net/base/nic.d 


# Each subdirectory must supply rules for building sources it contributes
net/base/%.o: ../net/base/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



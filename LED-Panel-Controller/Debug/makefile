################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include net/udp/ntp-client/subdir.mk
-include net/udp/dhcp-client/subdir.mk
-include net/udp/base/subdir.mk
-include net/tcp/base/subdir.mk
-include net/base/subdir.mk
-include dev/soft-spi/subdir.mk
-include dev/enc28j60/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
LED-Panel-Controller.lss \

FLASH_IMAGE += \
LED-Panel-Controller.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: LED-Panel-Controller.elf secondary-outputs

# Tool invocations
LED-Panel-Controller.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,LED-Panel-Controller.map -L/usr/lib/binutils/avr/2.20.1 -mmcu=atmega32 -o"LED-Panel-Controller.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

LED-Panel-Controller.lss: LED-Panel-Controller.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S LED-Panel-Controller.elf  >"LED-Panel-Controller.lss"
	@echo 'Finished building: $@'
	@echo ' '

LED-Panel-Controller.hex: LED-Panel-Controller.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex LED-Panel-Controller.elf  "LED-Panel-Controller.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: LED-Panel-Controller.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t LED-Panel-Controller.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) LED-Panel-Controller.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

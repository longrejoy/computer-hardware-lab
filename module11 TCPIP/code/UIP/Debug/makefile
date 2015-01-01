################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/arm_5.0.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./HARDWARE/led.obj" \
"./HARDWARE/enc28j60.obj" \
"./HARDWARE/buttons.obj" \
"./uIP-1.0/uiplib.obj" \
"./uIP-1.0/uip_arp.obj" \
"./uIP-1.0/uip.obj" \
"./uIP-1.0/uip-split.obj" \
"./uIP-1.0/uip-neighbor.obj" \
"./uIP-1.0/uip-fw.obj" \
"./uIP-1.0/timer.obj" \
"./uIP-1.0/psock.obj" \
"./uIP-APP/tcp_server_demo.obj" \
"./uIP-APP/tcp_demo.obj" \
"./uIP-APP/tcp_client_demo.obj" \
"./uIP-APP/tapdev.obj" \
"./uIP-APP/httpd.obj" \
"./uIP-APP/httpd-fs.obj" \
"./uIP-APP/httpd-cgi.obj" \
"./uIP-APP/http-strings.obj" \
"./uIP-APP/clock-arch.obj" \
"./uartstdio.obj" \
"./startup_ccs.obj" \
"./main.obj" \
-l"libc.a" \
"../driverlib.lib" \
"../tm4c123gh6pm.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include uIP-APP/subdir_vars.mk
-include uIP-1.0/subdir_vars.mk
-include HARDWARE/subdir_vars.mk
-include subdir_rules.mk
-include uIP-APP/subdir_rules.mk
-include uIP-1.0/subdir_rules.mk
-include HARDWARE/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: UIP.out

# Tool invocations
UIP.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.4/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RA1 --define=ccs='ccs' --diag_warning=225 --display_error_number --diag_wrap=off -z --stack_size=512 -m"UIP.map" --heap_size=0 -i"C:/ti/ccsv5/tools/compiler/arm_5.0.4/lib" -i"C:/ti/ccsv5/tools/compiler/arm_5.0.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="UIP_linkInfo.xml" --rom_model -o "UIP.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "UIP.out"
	-$(RM) "main.pp" "startup_ccs.pp" "uartstdio.pp" "uIP-APP\clock-arch.pp" "uIP-APP\http-strings.pp" "uIP-APP\httpd-cgi.pp" "uIP-APP\httpd-fs.pp" "uIP-APP\httpd.pp" "uIP-APP\tapdev.pp" "uIP-APP\tcp_client_demo.pp" "uIP-APP\tcp_demo.pp" "uIP-APP\tcp_server_demo.pp" "uIP-1.0\psock.pp" "uIP-1.0\timer.pp" "uIP-1.0\uip-fw.pp" "uIP-1.0\uip-neighbor.pp" "uIP-1.0\uip-split.pp" "uIP-1.0\uip.pp" "uIP-1.0\uip_arp.pp" "uIP-1.0\uiplib.pp" "HARDWARE\buttons.pp" "HARDWARE\enc28j60.pp" "HARDWARE\led.pp" 
	-$(RM) "main.obj" "startup_ccs.obj" "uartstdio.obj" "uIP-APP\clock-arch.obj" "uIP-APP\http-strings.obj" "uIP-APP\httpd-cgi.obj" "uIP-APP\httpd-fs.obj" "uIP-APP\httpd.obj" "uIP-APP\tapdev.obj" "uIP-APP\tcp_client_demo.obj" "uIP-APP\tcp_demo.obj" "uIP-APP\tcp_server_demo.obj" "uIP-1.0\psock.obj" "uIP-1.0\timer.obj" "uIP-1.0\uip-fw.obj" "uIP-1.0\uip-neighbor.obj" "uIP-1.0\uip-split.obj" "uIP-1.0\uip.obj" "uIP-1.0\uip_arp.obj" "uIP-1.0\uiplib.obj" "HARDWARE\buttons.obj" "HARDWARE\enc28j60.obj" "HARDWARE\led.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

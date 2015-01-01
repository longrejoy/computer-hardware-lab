################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
uIP-1.0/apps/resolv/resolv.obj: ../uIP-1.0/apps/resolv/resolv.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.4/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.4/include" --include_path="D:/TM4C123GH6PM/TCPIP/UIP/HARDWARE" --include_path="D:/TM4C123GH6PM/TCPIP/UIP/uIP-1.0" --include_path="D:/TM4C123GH6PM/TCPIP/UIP/uIP-APP" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RA1 --define=ccs='ccs' --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="uIP-1.0/apps/resolv/resolv.pp" --obj_directory="uIP-1.0/apps/resolv" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



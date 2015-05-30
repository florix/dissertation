################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../main.cpp 

OBJS += \
./main.o 

CPP_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -D__NEWLIB__ -D__CODE_RED -DCPP_USE_HEAP -DTARGET_LPC1768 -DTARGET_M3 -DTARGET_CORTEX_M -DTARGET_NXP -DTARGET_LPC176X -DTARGET_MBED_LPC1768 -DTOOLCHAIN_GCC_CR -DTOOLCHAIN_GCC -D__CORTEX_M3 -DARM_MATH_CM3 -DMBED_BUILD_TIMESTAMP=1432980801.27 -D__MBED__=1 -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/m3pi" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed/TARGET_LPC1768" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_CR" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed/TARGET_LPC1768/TARGET_NXP" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X" -I"/home/andrea/documents/dissertation_source_files/source_LPC1768/sorgenti_definitivi_lpc/lpc_obstacle_avoidance/obstacle_avoidance/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



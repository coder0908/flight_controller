################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/hardware/tranceiver/rtt24/rtt24.c 

OBJS += \
./src/app/hardware/tranceiver/rtt24/rtt24.o 

C_DEPS += \
./src/app/hardware/tranceiver/rtt24/rtt24.d 


# Each subdirectory must supply rules for building sources it contributes
src/app/hardware/tranceiver/rtt24/%.o src/app/hardware/tranceiver/rtt24/%.su src/app/hardware/tranceiver/rtt24/%.cyclo: ../src/app/hardware/tranceiver/rtt24/%.c src/app/hardware/tranceiver/rtt24/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F411xE -c -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Core/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-app-2f-hardware-2f-tranceiver-2f-rtt24

clean-src-2f-app-2f-hardware-2f-tranceiver-2f-rtt24:
	-$(RM) ./src/app/hardware/tranceiver/rtt24/rtt24.cyclo ./src/app/hardware/tranceiver/rtt24/rtt24.d ./src/app/hardware/tranceiver/rtt24/rtt24.o ./src/app/hardware/tranceiver/rtt24/rtt24.su

.PHONY: clean-src-2f-app-2f-hardware-2f-tranceiver-2f-rtt24


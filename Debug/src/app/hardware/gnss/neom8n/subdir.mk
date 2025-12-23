################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/hardware/gnss/neom8n/neom8n.c 

OBJS += \
./src/app/hardware/gnss/neom8n/neom8n.o 

C_DEPS += \
./src/app/hardware/gnss/neom8n/neom8n.d 


# Each subdirectory must supply rules for building sources it contributes
src/app/hardware/gnss/neom8n/%.o src/app/hardware/gnss/neom8n/%.su src/app/hardware/gnss/neom8n/%.cyclo: ../src/app/hardware/gnss/neom8n/%.c src/app/hardware/gnss/neom8n/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F411xE -c -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Core/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-app-2f-hardware-2f-gnss-2f-neom8n

clean-src-2f-app-2f-hardware-2f-gnss-2f-neom8n:
	-$(RM) ./src/app/hardware/gnss/neom8n/neom8n.cyclo ./src/app/hardware/gnss/neom8n/neom8n.d ./src/app/hardware/gnss/neom8n/neom8n.o ./src/app/hardware/gnss/neom8n/neom8n.su

.PHONY: clean-src-2f-app-2f-hardware-2f-gnss-2f-neom8n


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/protocols/ubx/ubx.c 

OBJS += \
./src/lib/protocols/ubx/ubx.o 

C_DEPS += \
./src/lib/protocols/ubx/ubx.d 


# Each subdirectory must supply rules for building sources it contributes
src/lib/protocols/ubx/%.o src/lib/protocols/ubx/%.su src/lib/protocols/ubx/%.cyclo: ../src/lib/protocols/ubx/%.c src/lib/protocols/ubx/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F411xE -c -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Core/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-lib-2f-protocols-2f-ubx

clean-src-2f-lib-2f-protocols-2f-ubx:
	-$(RM) ./src/lib/protocols/ubx/ubx.cyclo ./src/lib/protocols/ubx/ubx.d ./src/lib/protocols/ubx/ubx.o ./src/lib/protocols/ubx/ubx.su

.PHONY: clean-src-2f-lib-2f-protocols-2f-ubx


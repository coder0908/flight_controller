################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/hal/Core/Src/adc.c \
../src/platform/hal/Core/Src/dma.c \
../src/platform/hal/Core/Src/gpio.c \
../src/platform/hal/Core/Src/i2c.c \
../src/platform/hal/Core/Src/spi.c \
../src/platform/hal/Core/Src/stm32f4xx_hal_msp.c \
../src/platform/hal/Core/Src/stm32f4xx_it.c \
../src/platform/hal/Core/Src/syscalls.c \
../src/platform/hal/Core/Src/sysmem.c \
../src/platform/hal/Core/Src/system_stm32f4xx.c \
../src/platform/hal/Core/Src/tim.c \
../src/platform/hal/Core/Src/usart.c 

OBJS += \
./src/platform/hal/Core/Src/adc.o \
./src/platform/hal/Core/Src/dma.o \
./src/platform/hal/Core/Src/gpio.o \
./src/platform/hal/Core/Src/i2c.o \
./src/platform/hal/Core/Src/spi.o \
./src/platform/hal/Core/Src/stm32f4xx_hal_msp.o \
./src/platform/hal/Core/Src/stm32f4xx_it.o \
./src/platform/hal/Core/Src/syscalls.o \
./src/platform/hal/Core/Src/sysmem.o \
./src/platform/hal/Core/Src/system_stm32f4xx.o \
./src/platform/hal/Core/Src/tim.o \
./src/platform/hal/Core/Src/usart.o 

C_DEPS += \
./src/platform/hal/Core/Src/adc.d \
./src/platform/hal/Core/Src/dma.d \
./src/platform/hal/Core/Src/gpio.d \
./src/platform/hal/Core/Src/i2c.d \
./src/platform/hal/Core/Src/spi.d \
./src/platform/hal/Core/Src/stm32f4xx_hal_msp.d \
./src/platform/hal/Core/Src/stm32f4xx_it.d \
./src/platform/hal/Core/Src/syscalls.d \
./src/platform/hal/Core/Src/sysmem.d \
./src/platform/hal/Core/Src/system_stm32f4xx.d \
./src/platform/hal/Core/Src/tim.d \
./src/platform/hal/Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/hal/Core/Src/%.o src/platform/hal/Core/Src/%.su src/platform/hal/Core/Src/%.cyclo: ../src/platform/hal/Core/Src/%.c src/platform/hal/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F411xE -c -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Core/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src/platform/hal/Drivers/CMSIS/Include" -I"C:/Users/unRR/Workspace/STM32CubeIDE/workspace_1.19.0/flight_controller/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-platform-2f-hal-2f-Core-2f-Src

clean-src-2f-platform-2f-hal-2f-Core-2f-Src:
	-$(RM) ./src/platform/hal/Core/Src/adc.cyclo ./src/platform/hal/Core/Src/adc.d ./src/platform/hal/Core/Src/adc.o ./src/platform/hal/Core/Src/adc.su ./src/platform/hal/Core/Src/dma.cyclo ./src/platform/hal/Core/Src/dma.d ./src/platform/hal/Core/Src/dma.o ./src/platform/hal/Core/Src/dma.su ./src/platform/hal/Core/Src/gpio.cyclo ./src/platform/hal/Core/Src/gpio.d ./src/platform/hal/Core/Src/gpio.o ./src/platform/hal/Core/Src/gpio.su ./src/platform/hal/Core/Src/i2c.cyclo ./src/platform/hal/Core/Src/i2c.d ./src/platform/hal/Core/Src/i2c.o ./src/platform/hal/Core/Src/i2c.su ./src/platform/hal/Core/Src/spi.cyclo ./src/platform/hal/Core/Src/spi.d ./src/platform/hal/Core/Src/spi.o ./src/platform/hal/Core/Src/spi.su ./src/platform/hal/Core/Src/stm32f4xx_hal_msp.cyclo ./src/platform/hal/Core/Src/stm32f4xx_hal_msp.d ./src/platform/hal/Core/Src/stm32f4xx_hal_msp.o ./src/platform/hal/Core/Src/stm32f4xx_hal_msp.su ./src/platform/hal/Core/Src/stm32f4xx_it.cyclo ./src/platform/hal/Core/Src/stm32f4xx_it.d ./src/platform/hal/Core/Src/stm32f4xx_it.o ./src/platform/hal/Core/Src/stm32f4xx_it.su ./src/platform/hal/Core/Src/syscalls.cyclo ./src/platform/hal/Core/Src/syscalls.d ./src/platform/hal/Core/Src/syscalls.o ./src/platform/hal/Core/Src/syscalls.su ./src/platform/hal/Core/Src/sysmem.cyclo ./src/platform/hal/Core/Src/sysmem.d ./src/platform/hal/Core/Src/sysmem.o ./src/platform/hal/Core/Src/sysmem.su ./src/platform/hal/Core/Src/system_stm32f4xx.cyclo ./src/platform/hal/Core/Src/system_stm32f4xx.d ./src/platform/hal/Core/Src/system_stm32f4xx.o ./src/platform/hal/Core/Src/system_stm32f4xx.su ./src/platform/hal/Core/Src/tim.cyclo ./src/platform/hal/Core/Src/tim.d ./src/platform/hal/Core/Src/tim.o ./src/platform/hal/Core/Src/tim.su ./src/platform/hal/Core/Src/usart.cyclo ./src/platform/hal/Core/Src/usart.d ./src/platform/hal/Core/Src/usart.o ./src/platform/hal/Core/Src/usart.su

.PHONY: clean-src-2f-platform-2f-hal-2f-Core-2f-Src


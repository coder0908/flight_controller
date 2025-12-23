################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/platform/hal/Core/Startup/startup_stm32f411ceux.s 

OBJS += \
./src/platform/hal/Core/Startup/startup_stm32f411ceux.o 

S_DEPS += \
./src/platform/hal/Core/Startup/startup_stm32f411ceux.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/hal/Core/Startup/%.o: ../src/platform/hal/Core/Startup/%.s src/platform/hal/Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-src-2f-platform-2f-hal-2f-Core-2f-Startup

clean-src-2f-platform-2f-hal-2f-Core-2f-Startup:
	-$(RM) ./src/platform/hal/Core/Startup/startup_stm32f411ceux.d ./src/platform/hal/Core/Startup/startup_stm32f411ceux.o

.PHONY: clean-src-2f-platform-2f-hal-2f-Core-2f-Startup


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.c \
../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.c \
../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.c 

OBJS += \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.o \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.o \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.o 

C_DEPS += \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.d \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.d \
./Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.d 


# Each subdirectory must supply rules for building sources it contributes
Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.o: ../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IUSB_DEVICE/App -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/VL53L0X_1.0.2/Api/core/inc -I../Core/VL53L0X_1.0.2/Api/platform/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_i2c_win_serial_comms.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.o: ../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IUSB_DEVICE/App -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/VL53L0X_1.0.2/Api/core/inc -I../Core/VL53L0X_1.0.2/Api/platform/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.o: ../Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IUSB_DEVICE/App -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/VL53L0X_1.0.2/Api/core/inc -I../Core/VL53L0X_1.0.2/Api/platform/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/VL53L0X_1.0.2/Api/platform/src/vl53l0x_platform_log.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


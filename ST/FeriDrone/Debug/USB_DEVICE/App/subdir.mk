################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_DEVICE/App/usb_device.c \
../USB_DEVICE/App/usbd_cdc_if.c \
../USB_DEVICE/App/usbd_desc.c 

OBJS += \
./USB_DEVICE/App/usb_device.o \
./USB_DEVICE/App/usbd_cdc_if.o \
./USB_DEVICE/App/usbd_desc.o 

C_DEPS += \
./USB_DEVICE/App/usb_device.d \
./USB_DEVICE/App/usbd_cdc_if.d \
./USB_DEVICE/App/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
USB_DEVICE/App/usb_device.o: ../USB_DEVICE/App/usb_device.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../USB_DEVICE/Target -IUSB_DEVICE/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB_DEVICE/App/usb_device.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
USB_DEVICE/App/usbd_cdc_if.o: ../USB_DEVICE/App/usbd_cdc_if.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../USB_DEVICE/Target -IUSB_DEVICE/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB_DEVICE/App/usbd_cdc_if.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
USB_DEVICE/App/usbd_desc.o: ../USB_DEVICE/App/usbd_desc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F411xE -DDEBUG -c -IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../USB_DEVICE/Target -IUSB_DEVICE/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -IDrivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -IDrivers/CMSIS/Device/ST/STM32F4xx/Include -IMiddlewares/Third_Party/FreeRTOS/Source/include -IUSB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -IMiddlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc -IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -ICore/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"USB_DEVICE/App/usbd_desc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


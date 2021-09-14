################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LED.c 

OBJS += \
./src/LED.o 

C_DEPS += \
./src/LED.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L476RGTx -DNUCLEO_L476RG -DDEBUG -I"E:/微處裡機code/Lab7-1/src/stm32l476xx.h" -I"E:/微處裡機code/Lab7-1/src/core_cm4.h" -include"E:/微處裡機code/Lab7-1/src/stm32l476xx.h" -include"E:/微處裡機code/Lab7-1/src/core_cm4.h" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



#include "stm32l476xx.h"
#include "core_cm4.h"

void SysTick_Handler(void) {
	GPIOA->ODR ^= 1 << 5;
}

GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};

void set_moder(int addr, int mode) { // mode: 0 input, 1 output
	int x = addr >> 4, k = addr & 0xF;
	RCC->AHB2ENR |= 1<<(x-10);
	GPIO[x]->MODER &= ~(3 << (2*k));
	GPIO[x]->MODER |= (mode << (2*k));

	if (mode == 0) {
		GPIO[x]->PUPDR &= ~(3 << (2*k));
		GPIO[x]->PUPDR |= (2 << (2*k));
	}
}

void set_clock() {
	// Set system clock as MSI
	RCC->CFGR &= ~3;

	// HPRE -> 1MHz
	RCC->CFGR &= ~(0xF << 4);
	RCC->CFGR |= 11 << 4;

	// enable HSION
	RCC->CR |= 1 << 8;

	// Set system clock as HSI16
	RCC->CFGR |= 1;
}

void systick_config() {
	SysTick->CTRL |= 7;
	SysTick->LOAD = 3000000;
}

void gpio_init() {
	set_moder(0xA5, 1);
}

// freq = freq * N / M / R
// 2MHz = 16MHZ * 8 / 8 / 8

int main() {
	gpio_init();
	set_clock();
	systick_config();
	while (1);
}

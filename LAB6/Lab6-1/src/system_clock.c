#include "stm32l476xx.h"

GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
int PLL_NMR[5][3] = {
	{ 8, 3, 3}, //  1
	{ 9, 0, 2}, //  6
	{10, 0, 1}, // 10
	{ 8, 0, 0}, // 16
	{20, 0, 0}  // 40
};
int freq_idx = 0; // which frequency

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

void set_clock(int N, int M, int R) {
	// Set system clock as MSI
	RCC->CFGR &= ~3;

	// disable PLLON
	RCC->CR &= ~(1 << 24);
	// wait until PLLRDY is cleared
	while (RCC->CR & (1 << 25));

	// Change parameters

	// PLLsrc
	RCC->PLLCFGR &= ~3;
	RCC->PLLCFGR |= 1;

	// PLLN
	RCC->PLLCFGR &= ~(0x7F << 8);
	RCC->PLLCFGR |= N << 8;
	// PLLM
	RCC->PLLCFGR &= ~(0x7 << 4);
	RCC->PLLCFGR |= M << 4;
	// PLLR
	RCC->PLLCFGR &= ~(0x3 << 25);
	RCC->PLLCFGR |= R << 25;

	// Enable PLLON
	RCC->CR |= 1 << 24;
	while (RCC->CR & (1 << 25) == 0);

	// Enable PLLREN (PLLCFGR)
	RCC->PLLCFGR |= 1 << 24;

	// Set system clock as PLL
	RCC->CFGR |= 3;
}

void delay4Mclocks() {
	int n = 95000;
	int cnt = 0;
	while(n--) {
		int button = GPIOC->IDR & (1 << 13);
		if (button == 0) {
			cnt ++;
		} else if (cnt > (1 << 13)) {
			cnt = 0;
			int N = PLL_NMR[freq_idx][0];
			int M = PLL_NMR[freq_idx][1];
			int R = PLL_NMR[freq_idx][2];
			set_clock(N, M, R);
			freq_idx = (freq_idx + 1) % 5;
		} else {
			cnt = 0;
		}
	}
}

void gpio_init() {
	set_moder(0xA5, 1);
	set_moder(0xCD, 0);
}

int main() {
	gpio_init();
	int N = PLL_NMR[freq_idx][0];
	int M = PLL_NMR[freq_idx][1];
	int R = PLL_NMR[freq_idx][2];
	set_clock(N, M, R);
	freq_idx = (freq_idx + 1) % 5;
	while (1) {
		GPIOA->ODR ^= (1 << 5); // switch A5 LED
		delay4Mclocks();
	}
}

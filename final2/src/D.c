#include "stm32l476xx.h"


GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
int X[4] = {0xC7, 0xA9, 0xA8, 0xBA};
int Y[4] = {0xA5, 0xA6, 0xA7, 0xB6};

void set_moder(int addr, int mode) { // mode: 0 input, 1 output, 2 alternate
	int x = addr >> 4, k = addr & 0xF;
	RCC->AHB2ENR |= 1<<(x-10);
	GPIO[x]->MODER &= ~(3 << (2*k));
	GPIO[x]->MODER |= (mode << (2*k));

	if (mode == 0) {
		GPIO[x]->PUPDR &= ~(3 << (2*k));
		GPIO[x]->PUPDR |= (2 << (2*k));
	}
}



void timer_init() {
	RCC->APB1ENR1 |= 1;//timer1 clock enable
	TIM2->PSC = (uint32_t)((4000000 / 50000) / 10 - 1);//Prescalser
	TIM2->ARR = (uint32_t)10;//Reload value
	TIM2->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.

	// timer as output compare
	// cr1 - appe
	TIM2->CR1 |= (1 << 7);
	// ccr
//	TIM2->CCR2 = 50;
	TIM2->CCR2 = 5;
	// ccmr - pwn mode
	TIM2->CCMR1 &= ~(7 << 12);
	TIM2->CCMR1 |= 6 << 12; // pwm mode 1
	TIM2->CCMR1 |= 1 << 11; // oc2pe
	// ccer - enable output compare
	TIM2->CCER |= (1 << 4);
	// dier - cc2ie
	TIM2->DIER |= (1 << 2) + (1 << 10);

	TIM2->CR1 |= 1;//start timer


	// set alternative function for PB3
	GPIOB->AFR[0] &= ~(0xF << 12);
	GPIOB->AFR[0] |= 1 << 12;

}

void change_freq(int freq) {
	TIM2->CR1 &= ~1;//stop timer
	TIM2->PSC = (uint32_t)(4000000 / freq) / 100;//Prescalser
	TIM2->CR1 |= 1;//start timer
}

void silent() {
	set_moder(0xB3, 1);
}

int main() {
	set_moder(0xC0, 0);
	set_moder(0xA5, 1);
	set_moder(0xB3, 2);
	timer_init();
	GPIOA->ODR |= (1<<5);

	while (1) {
		if(GPIOC->IDR &1){
			GPIOA->ODR &= ~(1<<5);
			break;
		}
		else{
			//GPIOA->ODR &= ~(1<<5);
		}
		set_moder(0xB3, 2);
	}
	while(1){

	}
}

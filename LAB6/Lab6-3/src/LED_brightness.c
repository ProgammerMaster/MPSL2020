#include "stm32l476xx.h"


GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
int X[4] = {0xC7, 0xA9, 0xA8, 0xBA};
int Y[4] = {0xA5, 0xA6, 0xA7, 0xB6};
extern void GPIO_init();
const char mapping[16] = {
	1, 2, 3, 10,
	4, 5, 6, 11,
	7, 8, 9, 12,
	15, 0, 14, 13
};

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


void keypad_init(int * X, int * Y) {
	for (int i = 0; i < 4; i++) {
		set_moder(X[i], 1);
	}
	for (int i = 0; i < 4; i++) {
		set_moder(Y[i], 0);
	}
}

int keypad_scan(const char *mapping, int *X, int *Y) {

	int result = 0;
	char nil = 1;

	for (int i=0; i<4; i++){
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR &= ~(1 << k);
		GPIO[x]->ODR |= (1 << k);
		for (int j=0; j<4; j++){
			int y = Y[j] >> 4, l = Y[j] & 0xF;
			if (GPIO[y]->IDR & (1<<l)){
				result += mapping[j*4 + i];
				nil = 0;
			}
		}
		GPIO[x]->ODR ^= (1 << k);
	}
	if (nil) return -1;
	return  result;
}


void timer_init() {
	RCC->APB1ENR1 |= 1;//timer1 clock enable
	TIM2->PSC = (uint32_t)((4000000 / 333.0) / 100 - 1);//Prescalser
	TIM2->ARR = (uint32_t)100;//Reload value
	TIM2->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.

	// timer as output compare
	// cr1 - appe
	TIM2->CR1 |= (1 << 7);
	// ccr
	TIM2->CCR2 = 50;
//	TIM2->CCR2 = 5;
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
	GPIO_init();
	timer_init();
	keypad_init(X, Y);

	double keys[8] = {261.6, 293.7, 329.6, 349.2, 392.0, 440.0, 493.9, 523.3};

	// change_freq(keys[0]);

	//while (1);

	while (1) {
		int key = keypad_scan(mapping, X, Y);

		int t = 50000;
		while (t --);
		/*
while(1){
	set_moder(0xB3, 1);
	GPIOB->ODR |= 0x8;
}*/
		if (key == 15 && TIM2->CCR2 < 90){
					TIM2->CCR2 += 5;
					while(keypad_scan(mapping, X, Y)==15);
		}
		else if (key == 14 && TIM2->CCR2 > 10){
			TIM2->CCR2 -= 5;
			while(keypad_scan(mapping, X, Y)==14);
		}

		if (key < 1 || key > 8) {
		//	silent();
			continue;
		}

		key --;

		change_freq(keys[key]);
		set_moder(0xB3, 2);
	}
}

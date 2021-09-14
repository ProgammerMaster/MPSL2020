#include "stm32l476xx.h"
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char, unsigned char);

GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
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
	TIM2->EGR = 0;//Reinitialize the counter. CNT takes the auto-reload value.

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



//int display(int data) {
//	int num_digs = 1;
//	const int base = 10;
//	for (int i = 0; i < 8; i ++) {
//		int d = data % base;
//		if (num_digs == 3) d = d | (1 << 7);
//		MAX7219Send(num_digs, d);
//		if (data) num_digs ++;
//		data /= base;
//	}
//	int limit = num_digs - 2;
//	if (limit < 2) limit = 2;
//	MAX7219Send(0xb, limit);
//	return 0;
//}
int display(int data) {
	int num_digs = 1;
	const int base = 10;
	for (int i = 0; i < 8; i ++) {
		int d = data % base;
		MAX7219Send(num_digs, d);
		if (data){
			num_digs ++;
			data /= base;}
		else
			break;
	}
	int limit = num_digs - 2;
//	if (limit < 2) limit = 2;
	MAX7219Send(0xb, 7);
	return 0;
}
void timer_init2() {
	RCC->APB1ENR1 |= (1 << 3);//timer1 clock enable
	TIM5->PSC = (uint32_t)4;//Prescalser
	TIM5->ARR = (uint32_t)38000;//Reload value
	TIM5->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.
	TIM5->CR1 |= (1 << 3);
	TIM5->CR1 |= 1;//start timer
}
//void timer_init() {
//	RCC->APB1ENR1 |= (1 << 3);//timer5 clock enable
//	TIM5->PSC = (uint32_t)39999;//Prescalser
//	TIM5->ARR = (uint32_t)1111;//Reload value
//	TIM5->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.
//	TIM5->CR1 |= (1 << 3);
//	TIM5->CR1 |= 1;//start timer
//}

int main() {
	GPIO_init();
	max7219_init();
	set_moder(0xC0, 0);
	set_moder(0xA5, 1);
	set_moder(0xB3, 2);
//	timer_init();
//	while(TIM5->CNT <= 20);
//	RCC->APB1ENR1 &= 0;//timer1 clock disable
	GPIOA->ODR |= (1<<5);
	int cc=1000;
	display(cc);
	while(1){
		timer_init();
		set_moder(0xC0, 0);
		set_moder(0xA5, 1);
		set_moder(0xB3, 2);
		while (1) {
		if(GPIOC->IDR &1){

			timer_init2();
			while(GPIOC->IDR | 0);
			if(TIM5->CNT<38000)
				cc = TIM5->CNT;
			else{
//				cc=38000/0.0346;///0.0343;
				display(38000);
				continue;}
//			RCC->APB1ENR1 |= ~(1 << 3);//timer1 clock disable
			break;
		}

//		set_moder(0xB3, 2);
	}
	display(cc*0.0246);
	display(cc);
	int xx=250000;
	while(1);
	while(xx--);
}
//while(1){		display(cc);}
//	while(1){
//		int timerValue = TIM5->CNT;
//		display(timerValue);
//		if(timerValue==1111){
//			while(1){
//				display(timerValue);
//			}
//		}
//		}

}

#include "stm32l476xx.h"
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char, unsigned char);

GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};





int display(int data) {
	int num_digs = 1;
	const int base = 10;
	for (int i = 0; i < 8; i ++) {
		int d = data % base;
		if (num_digs == 3) d = d | (1 << 7);
		MAX7219Send(num_digs, d);
		if (data) num_digs ++;
		data /= base;
	}
	int limit = num_digs - 2;
	if (limit < 2) limit = 2;
	MAX7219Send(0xb, limit);
	return 0;
}

void timer_init() {
	RCC->APB1ENR1 |= (1 << 3);//timer5 clock enable
	TIM5->PSC = (uint32_t)39999;//Prescalser
	TIM5->ARR = (uint32_t)1270;//Reload value
	TIM5->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.
	TIM5->CR1 |= (1 << 3);
	TIM5->CR1 |= 1;//start timer
}

int main() {
	GPIO_init();
	max7219_init();
	timer_init();
	while(1){
		int timerValue = TIM5->CNT;
		display(timerValue);
		if(timerValue==1270){
			while(1){
				display(timerValue);
			}
		}
		}

}

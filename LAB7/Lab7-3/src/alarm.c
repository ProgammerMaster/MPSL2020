#include "stm32l476xx.h"
#include "core_cm4.h"

/* GPIO */

GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
const unsigned int X[4] = {0xA5, 0xA6, 0xA7, 0xB6};
const unsigned int Y[4] = {0xC7, 0xA9, 0xA8, 0xBA};

extern void max7219_init();
extern void GPIO_init();
extern void MAX7219Send(unsigned char address, unsigned char data);
int gate=0,p=0,value=0;
void read_button() {
	int cnt = 0;
	display(0,1);
	while(1) {
		int button = GPIOC->IDR & (1 << 13);
		if (button == 0) { // 0
			cnt ++;
		} else if (cnt > (1 << 13)) { // 1 trigger
			cnt = 0;
			// TODO
			// end alert
			silent();
			// keypad output 1
			for (int i = 0; i < 4; i ++) {
				int x = X[i] >> 4, k = X[i] & 0xF;
				GPIO[x]->ODR |= (1 << k);
			}
			SysTick->LOAD = 0;
			break;
		} else { // 1
			cnt = 0;
		}
	}
}

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

void gpio_init() {
	set_moder(0xA5, 1);
	set_moder(0xCD, 0);

//	set_moder(0xC0, 1);
//	set_moder(0xC1, 1);
//	set_moder(0xC2, 1);
}

/* SysTick */

void SysTick_Handler(void) {
	// TODO
	gate=0;
	systick_disable();

	ring();
	// read_button
	read_button();
}

void set_clock() {
	// Set system clock as MSI
	RCC->CFGR &= ~3;

	// HPRE -> 1MHz
	RCC->CFGR &= ~(0xF << 4);
	RCC->CFGR |= 11 << 4;

	// enable HSION
	RCC->CR |= 1 << 8;

	// Set system clock as PLL
	RCC->CFGR |= 1;
}
void systick_config(int value1) {
	// load first
	if(value > 0){
		gate = 1;
//	gate=1;
		SysTick->LOAD = value*1000000;
		SysTick->CTRL |= 7;
	}
	else if (value == 0){
		gate = 1;
//	gate=1;
		SysTick->LOAD = 5000;
		SysTick->CTRL |= 7;

	}
}

void systick_disable() {
	SysTick->CTRL &= ~1;
}

/* keypad */

const char mapping[16] = {
	1, 4, 7, 15,
	2, 5, 8, 0,
	3, 6, 9, 14,
	10, 11, 12, 13
};

void keypad_init(int * X, int * Y) {
	for (int i = 0; i < 4; i++) {
		set_moder(X[i], 1);
	}
	for (int i = 0; i < 4; i++) {
		set_moder(Y[i], 0);
	}
}

int keypad_scan() {
	int result = -1;
	char nil = 1;

	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR &= ~(1 << k);
	}

	for (int i=0; i<4; i++){
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR &= ~(1 << k);
		GPIO[x]->ODR |= (1 << k);
		for (int j=0; j<4; j++){
			int y = Y[j] >> 4, l = Y[j] & 0xF;
			if (GPIO[y]->IDR & (1<<l)){
				result = mapping[j*4 + i];
				nil = 0;
			}
			if (result != -1)
				break;
		}
		GPIO[x]->ODR ^= (1 << k);
		if (result != -1)
			break;
	}
	if (nil) return -1;
	return  result;
}

/* ring */
void timer_init() {
	RCC->APB1ENR1 |= 1;//timer1 clock enable
	TIM2->PSC = (uint32_t)(1000000 / 500) / 100;//Prescalser
	TIM2->ARR = (uint32_t)100;//Reload value
	TIM2->EGR = 1;//Reinitialize the counter. CNT takes the auto-reload value.

	// timer as output compare
	// cr1 - appe
	TIM2->CR1 |= (1 << 7);
	// ccr
	TIM2->CCR2 = 50;
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

void silent() {
	GPIOB->ODR &= ~(1 << 3);
	set_moder(0xB3, 1);
}

void ring() {
	gate=0;
	set_moder(0xB3, 2);
}

/* interrupt */

void EXTI_config(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // set GPIO PC7 PA8 PA9 PB10 as external input
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PC;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PA;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB;

    // enable GPIO 7~10 interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM7;
	EXTI->IMR1 |= EXTI_IMR1_IM8;
	EXTI->IMR1 |= EXTI_IMR1_IM9;
	EXTI->IMR1 |= EXTI_IMR1_IM10;

    // set GPIO 7~10 as rising trigger
	EXTI->RTSR1 |= EXTI_RTSR1_RT7;
	EXTI->RTSR1 |= EXTI_RTSR1_RT8;
	EXTI->RTSR1 |= EXTI_RTSR1_RT9;
	EXTI->RTSR1 |= EXTI_RTSR1_RT10;
}

void NVIC_config(){
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI9_5_IRQn, 0);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

void EXTI9_5_IRQHandler(){
	if (EXTI->PR1==0) return;
	// TODO
	// read
	 value = keypad_scan();
	// clear keypad output
	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR &= ~(1 << k);
	}
	// enable systick
	systick_config(value);
//	for(int i=value; i>=0; i++){
//		display(i,1);
//		delay();
//	}

	EXTI->PR1 &= ~EXTI_PR1_PIF7;
	EXTI->PR1 &= ~EXTI_PR1_PIF8;
	EXTI->PR1 &= ~EXTI_PR1_PIF9;
	EXTI->PR1 &= ~EXTI_PR1_PIF10;
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(){
	if (EXTI->PR1==0) return;
	// TODO
	// read
	 value = keypad_scan();
	// clear keypad output
	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR &= ~(1 << k);
	}
	// enable systick
	systick_config(value);
//	for(int i=value; i>=0; i++){
//		display(i,1);
//		delay();
//	}

	EXTI->PR1 &= ~EXTI_PR1_PIF7;
	EXTI->PR1 &= ~EXTI_PR1_PIF8;
	EXTI->PR1 &= ~EXTI_PR1_PIF9;
	EXTI->PR1 &= ~EXTI_PR1_PIF10;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}
void delay(){
	for (int i=0; i<(1<<13); i++);
}
int display(int data, int num_digs){

	int i;
	int divider = 10000000;
	int temp;
	if(num_digs<=8 && num_digs>=0){
		// first set
		for(i = 8; i>num_digs; i--){
			MAX7219Send(i,15);
			divider = divider / 10;
		}

		// Here is for output the digit
		for(i =num_digs; i>=1; i--){
			temp = data / divider;
			MAX7219Send(i,temp);
			data = data % divider;
			divider = divider / 10;
		}
		return 0;
	}
	else{
		return -1;
	}
}


int main() {
	gpio_init();
	GPIO_init();
	max7219_init();


	keypad_init(X, Y);
	NVIC_config();
	EXTI_config();
	set_clock();
	timer_init();

	silent();

	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR |= (1 << k);
	}

	while (1){
		if(gate==1){
				int k=(SysTick->VAL)/1000000;
				k += 1;
				if(k/10 == 0){
					display(k,1);
				}
				else{
					display(k,2);
				}

				//delay();
		}
		else {
			display(0,1);
			SysTick->LOAD =0;
		}
	};
}

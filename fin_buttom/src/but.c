
GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
const unsigned int X[4] = {0xA5, 0xA6, 0xA7, 0xB6};
const unsigned int Y[4] = {0xC7, 0xA9, 0xA8, 0xBA};
const char mapping[16] = {
	1, 4, 7, 15,
	2, 5, 8, 0,
	3, 6, 9, 14,
	10, 11, 12, 13
};
extern void GPIO_init();
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


void gpio_init() {
	set_moder(0xA5, 1);
//	GPIOA->ODR |= (1 << 5);
	set_moder(0xCD, 0);
}

void EXTI_config(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // set GPIO PC13 as external input
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // enable GPIO 13 interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM13;

    // set GPIO 13 as falling trigger
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
//	EXTI->RTSR1 |= EXTI_RTSR1_RT13;
}

void NVIC_config(){
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI9_5_IRQn, 0);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

void delay(){
	for (int i=0; i<(1<<17); i++);
}
void EXTI15_10_IRQHandler(){
	if (EXTI->PR1==0) return;
	GPIOA->ODR ^= (1<<5);
	delay();
	GPIOA->ODR ^= (1<<5);
	delay();
	GPIOA->ODR ^= (1<<5);
	delay();
	GPIOA->ODR ^= (1<<5);
	delay();
	int gate = 0;
	while(1){
		for(int i=0; i<4; i++){
			int x = X[i] >> 4, k = X[i] & 0xF;
			if(GPIO[x]->ODR & (1 << k) == 1){
				gate = 1;
				break;
			}
		}
		if(gate == 1){
			break;
		}
	}

	int value = keypad_scan();
	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR |= (1 << k);
	}

	while (value--){
		GPIOA->ODR ^= (1<<5);
		delay();
		GPIOA->ODR ^= (1<<5);
		delay();
	}

	GPIOA->ODR |= (1 << 5);
	EXTI->PR1 &= ~EXTI_PR1_PIF13;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
//	EXTI->PR1&=0;
}

int main() {
	gpio_init();
	keypad_init(X, Y);
	NVIC_config();
	EXTI_config();
	while(1){
	}
}

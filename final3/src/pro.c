GPIO_TypeDef* GPIO[16] = {[0xA]=GPIOA, [0xB]=GPIOB, [0xC]=GPIOC};
const unsigned int X[4] = {0xA5, 0xA6, 0xA7, 0xB6};
const unsigned int Y[4] = {0xC7, 0xA9, 0xA8, 0xBA};
int distance=0;

extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char, unsigned char);

const char mapping[16] = {
	1, 4, 7, 15,
	2, 5, 8, 0,
	3, 6, 9, 14,
	10, 11, 12, 13
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
	GPIOA->ODR |= (1 << 5);
}

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

void delay(){
	for (int i=0; i<(1<<17); i++);
}

void EXTI9_5_IRQHandler(){
	if (EXTI->PR1==0) return;

	distance = 0;
	DIS();
	EXTI->PR1 &= ~EXTI_PR1_PIF7;
	EXTI->PR1 &= ~EXTI_PR1_PIF8;
	EXTI->PR1 &= ~EXTI_PR1_PIF9;
	EXTI->PR1 &= ~EXTI_PR1_PIF10;
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}
int display(int data) {
	if(data == 0){
		MAX7219Send(1, 0);
		MAX7219Send(0xb,0);
		return 0;

	}
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
	MAX7219Send(0xb,limit);
	return 0;
}
void DIS(){
	int value = keypad_scan();
	int xx = 200000;
	while(xx--);
	for (int i = 0; i < 4; i ++) {
		int x = X[i] >> 4, k = X[i] & 0xF;
		GPIO[x]->ODR |= (1 << k);
	}
	if(value == -1){
		DIS();
	}
	if(value == 15){//enter
		return;
	}
	else if(value == 14){//backspace
		distance/=10;
		display(distance);
		DIS();
	}
	else if(value == 10){//+2
		distance += 2;
		display(distance);
		DIS();
	}
	else if(value == 11){//+1
		distance += 1;
		display(distance);
		DIS();
	}
	else if(value == 12){//-1
		distance -= 1;
		display(distance);
		DIS();
	}
	else if(value == 13){//-2
		distance -= 2;
		display(distance);
		DIS();
	}
	else{
		distance*=10;
		distance += value;
		display(distance);
		DIS();
	}
}

void EXTI15_10_IRQHandler(){
	if (EXTI->PR1==0) return;
	distance = 0;
	DIS();


	EXTI->PR1 &= ~EXTI_PR1_PIF7;
	EXTI->PR1 &= ~EXTI_PR1_PIF8;
	EXTI->PR1 &= ~EXTI_PR1_PIF9;
	EXTI->PR1 &= ~EXTI_PR1_PIF10;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

int main() {
	GPIO_init();
	max7219_init();
	gpio_init();
	keypad_init(X, Y);
	NVIC_config();
	EXTI_config();

	while(1){
		display(0);
	}

}

/*//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char address, unsigned char data);

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

int main(){
GPIO_init();
max7219_init();
while(1){
	GPIOC->BSRR = 0;//set bit as high
	GPIOC->BRR = 1;//set bit as low
	GPIOC->BRR = 2;//set bit as low
	GPIOC->BRR = 3;//set bit as low

	if(GPIOB->IDR& 1)
	display(1,1);
	if(GPIOB->IDR& 2 )
	display(4,1);
	if(GPIOB->IDR& 0x4 )
	display(5,1);
	if(GPIOB->IDR& 0x8 )
	display(15,2);

	GPIOC->BRR = 0;//set bit as low
	GPIOC->BSRR = 1;//set bit as high
	GPIOC->BRR = 2;//set bit as low
	GPIOC->BRR = 3;//set bit as low
	if(GPIOB->IDR& 0x1 )
	display(2,1);
	if(GPIOB->IDR& 0x2 )
	display(5,1);
	if(GPIOB->IDR& 0x4 )
	display(8,1);
	if(GPIOB->IDR & 0x8 )
	display(0,1);

	GPIOC->BRR = 0;//set bit as low
	GPIOC->BRR = 1;//set bit as low
	GPIOC->BSRR = 2;//set bit as high
	GPIOC->BRR = 3;//set bit as low
	if(GPIOB->IDR & 0x1 )
	display(3,1);
	if(GPIOB->IDR & 0x2 )
	display(6,1);
	if(GPIOB->IDR & 0x4 )
	display(9,1);
	if(GPIOB->IDR & 0x8 )
	display(14,2);

	GPIOC->BSRR = 0;//set bit as low
	GPIOC->BRR = 1;//set bit as low
	GPIOC->BRR = 2;//set bit as low
	GPIOC->BRR = 3;//set bit as high
	if(GPIOB->IDR &0x1 )
	display(55,7);
	if(GPIOB->IDR & 0x2 )
	display(11,2);
	if(GPIOB->IDR & 0x4 )
	display(12,2);
	if(GPIOB->IDR & 0x8 )
	display(13,2);

	//display(21,2);
}

}*/


//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char address, unsigned char data);
//TODO: define your gpio pin
/*#define X0 GPIOXX
#define X1
#define X2
#define X3
#define Y0
#define Y1
#define Y2
#define Y3
unsigned int x_pin = {X0, X1, X2, X3};
unsigned int y_pin = {Y0, Y1, Y2, Y3};*/
/* TODO: initial keypad gpio pin, X as output and Y as input
*/
void keypad_init()
{
	// SET keypad gpio OUTPUT //
	RCC->AHB2ENR = RCC->AHB2ENR|0x7;
	//Set PC0 1 2 3 as output mode
	GPIOC->MODER= GPIOC->MODER&0xFFFFFF55;
	//set PC0 1 2 3 is Pull-up output
	GPIOC->PUPDR=GPIOC->PUPDR|0x55;

	//Set PC0 1 2 3 as high
	GPIOC->ODR=GPIOC->ODR|0xF;
	// SET keypad gpio INPUT //
	//Set PB0 1 2 3 as INPUT mode
	GPIOB->MODER=GPIOB->MODER&0xFFFFFF00;
	//set PB0 1 2 3 is Pull-down input
	GPIOB->PUPDR=GPIOB->PUPDR|0xAA;

}
/* TODO: scan keypad value
* return:
* >=0: key pressed value
* -1: no key press
*/
char keypad_scan()
{
	int flag_keypad, flag_debounce, k, position_c, position_r, flag_keypad_r;
	int Table[4][4] = {{1,2,3,10},{4,5,6,11},{7,8,9,12},{15,0,14,13}};
	while(1){
		flag_keypad=GPIOB->IDR&0xF;
		if(flag_keypad!=0){
			k=40000;
			while(k!=0){
				flag_debounce = GPIOB->IDR&0xF;
				k--;
			}
			if(flag_debounce!=0){
				for(int i=0;i<4;i++){ //scan keypad from first column
					position_c=i;
					//set PC0,1,2,3(column) low and set pin high from PA8
					GPIOC->ODR=(GPIOC->ODR&0xFFFFFF00)|1<<position_c;
					for(int j=0;j<4;j++){ //read input from first row
						position_r=j;
						flag_keypad_r=GPIOB->IDR&1<<position_r;
						if(flag_keypad_r!=0){
							if(Table[j][i] > 9)
								display(Table[j][i], 2);
							else
								display(Table[j][i], 1);
							return Table[j][i];
						}
					}
				}
			}
			GPIOC->ODR=GPIOC->ODR|0xF; //set PC0,1,2,3(column) high
		}else {
			display(0, 0);
			return -1;
		}
	}
}
/**
* TODO: Show data on 7-seg via max7219_send
* Input:
* data: decimal value
* num_digs: number of digits will show on 7-seg
* Return:
* 0: success
* -1: illegal data range(out of 8 digits range)
*/
int display(int data, int num_digs)
{
	if (num_digs <= 8 && num_digs >= 0){
		int i;
		int div = 10000000;
		for (i = 8; i > num_digs; i--){
			MAX7219Send(i, 15);
			div = div / 10;
		}
		if (data < 0){
			MAX7219Send(num_digs, 10);
			data = -data;
			num_digs--;
			div = div / 10;
		}
		for (i = num_digs; i > 0; i--){
			int num = data / div;
			MAX7219Send(i, num);
			data = data % div;
			div = div / 10;
		}
		return 0;
	}else
		return -1;
}
int main()
{
	GPIO_init();
	max7219_init();
	keypad_init();
	while(1)
		keypad_scan();
}

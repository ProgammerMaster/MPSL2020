
//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char address, unsigned char data);
int Table[4][4] = {{1,2,3,10},{4,5,6,11},{7,8,9,12},{15,0,14,13}};
int pressed[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
int bit_seq = 1;
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
	GPIOC->MODER= GPIOC->MODER&0xFFFFFF00|0x55;
	//set PC0 1 2 3 is Pull-up output
	GPIOC->PUPDR=GPIOC->PUPDR&0xFFFFFF00|0x55;

	//Set PC0 1 2 3 as high
	GPIOC->ODR=GPIOC->ODR&0xFFFFFFF0|0xF;
	// SET keypad gpio INPUT //
	//Set PB0 1 2 3 as INPUT mode
	GPIOB->MODER=GPIOB->MODER&0xFFFFFF00;
	//set PB0 1 2 3 is Pull-down input
	GPIOB->PUPDR=GPIOB->PUPDR&0xFFFFFF00|0xAA;

}

void keypad_init2(){
	//Set PC0 1 2 3 as input mode
	GPIOC->MODER= GPIOC->MODER&0xFFFFFF00;
	//set PC0 1 2 3 is Pull-down input
	GPIOC->PUPDR=GPIOC->PUPDR&0xFFFFFF00|0xAA;


	// SET keypad gpio outPUT //
	//Set PB0 1 2 3 as INPUT mode
	GPIOB->MODER=GPIOB->MODER&0xFFFFFF00|0x55;
	//set PB0 1 2 3 is Pull-up output
	GPIOB->PUPDR=GPIOB->PUPDR&0xFFFFFF00|0x55;
	//Set PB0 1 2 3 as high
	GPIOB->ODR=GPIOB->ODR&0xFFFFFFF0|0xF;
}
/* TODO: scan keypad value
* return:
* >=0: key pressed value
* -1: no key press
*/
char keypad_scan()
{
	int flag_keypad, flag_debounce, k, position_c, position_r, flag_keypad_r,total1 = 0, total2 = 0, count;
	count=0;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			pressed[j][i]=0;
		}
	}
	while(1){
		//count = 0;
		/*for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				pressed[j][i]=0;
			}
		}*/
		keypad_init();
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
						if(flag_keypad_r!=0 && pressed[j][i]==0){
							//display(Table[j][i], 2);
							//return Table[j][i];
							count += 1;
							pressed[j][i]=count;
							total1 += Table[j][i];
						}
					}
				}
			}

//			GPIOC->ODR=GPIOC->ODR|0xF; //set PC0,1,2,3(column) high
			keypad_init2();

			for(int i=0;i<4;i++){ //scan keypad from first column
				position_c=i;
				//set PC0,1,2,3(column) low and set pin high from PA8
				GPIOB->ODR=(GPIOB->ODR&0xFFFFFF00)|1<<position_c;
				for(int j=0;j<4;j++){ //read input from first row
					position_r=j;
					flag_keypad_r=GPIOC->IDR&1<<position_r;
					if(flag_keypad_r!=0 && pressed[i][j]==0){
						//display(Table[j][i], 2);
						//return Table[j][i];
						count+= 1;
						pressed[i][j]=count;
						total1 += Table[i][j];
					}
				}
			}
			int count_much = 0;
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					if(pressed[j][i]!=0)
						count_much+=1;
				}
			}

			display2(total1,count_much);


			//total1 = 0;
			//total2 = 0;


			//GPIOC->ODR=GPIOB->ODR|0xF; //set PC0,1,2,3(column) high
		}else {
			count=0;
			total1 = 0;
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					pressed[j][i]=0;
				}
			}
			display2(0,-1);
		}

		// change direction

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

int main()
{
	GPIO_init();
	max7219_init();
	keypad_init();
	while(1)
		keypad_scan();
}
int display2(int total, int count_num){
	bit_seq = 1;
	if(count_num>-1){
	display3(total);
	MAX7219Send(bit_seq, 15);
	bit_seq++;
	}
	if(count_num == 2){
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			if(pressed[i][j] == 2){
				display3(Table[i][j]);
				MAX7219Send(bit_seq, 15);
				bit_seq++;
				display3(total - Table[i][j]);
			}
		}
	}
	}
	for(int i=bit_seq; i<=8; i++){
		MAX7219Send(i, 15);
	}
}
void display3(int total){
	if(total / 10 == 0){// 1 bit
		MAX7219Send(bit_seq, total);
		bit_seq++;
}
	else{
		MAX7219Send(bit_seq, total%10);
		bit_seq++;
		MAX7219Send(bit_seq, total/10);
		bit_seq++;

	}
}

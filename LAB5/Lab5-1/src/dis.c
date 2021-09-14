//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void MAX7219Send(unsigned char address, unsigned char data);
/**
* TODO: Show data on 7-seg via max7219_send
* Input:
* data: decimal value
* num_digs: number of digits will show on 7-seg
* Return:
* 0: success
* -1: illegal data range(out of 8 digits range)
*/
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



void main(){
int id = 613304;
GPIO_init();
 max7219_init();
display(id, 7);
}

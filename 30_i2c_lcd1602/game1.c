#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>

#define BtnPin          0

int LCDAddr = 0x27;
int BLEN = 1;
int fd;
int playerY = 0;
int playerLives = 1;
void write_word(int data){
	int temp = data;
	if ( BLEN == 1 )
		temp |= 0x08;
	else
		temp &= 0xF7;
	wiringPiI2CWrite(fd, temp);
}

void send_torben(int comm, int opt)	// opt=0 ==> command, opt=1 ==> data
{
	int buf;
	// Send bit7-4 firstly
	buf = comm & 0xF0;
	buf |= 0x04 | opt;		// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (comm & 0x0F) << 4;
	buf |= 0x04 | opt;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void init(){
	send_torben(0x33,0);	// Must initialize to 8-line mode at first
	delay(5);
	send_torben(0x32,0);	// Then initialize to 4-line mode
	delay(5);
	send_torben(0x28,0);	// 2 Lines & 5*7 dots
	delay(5);
	send_torben(0x0C,0);	// Enable display without cursor
	delay(5);
	send_torben(0x01,0);	// Clear Screen
	wiringPiI2CWrite(fd, 0x08);
}

void clear(){
	send_torben(0x01,0);	//clear Screen
}

void write(int x, int y, char data[]){
	int addr, i;
	int tmp;
	if (x < 0)  x = 0;
	if (x > 15) x = 15;
	if (y < 0)  y = 0;
	if (y > 1)  y = 1;

	// Move cursor
	addr = 0x80 + 0x40 * y + x;
	send_torben(addr,0);
	
	tmp = strlen(data);
	for (i = 0; i < tmp; i++){
		send_torben(data[i],1);
	}
}
bool checkcollision(int ox, int oy,int playerY )
{
	printf("PlayerY: %d Ox:%d Oy:%d\n",playerY,ox,oy);
	if(playerY==oy && ox == 0)
	{
		return true;	
	}
	return false;

}
int shoot(int y, int speed)
{
	int shootdelay = 500/speed+300;
	for(int pos = 16; pos >= 0; pos--)

	{
		playerY = digitalRead(BtnPin);
		printf("Button: %d ",playerY);
		write(pos,y, "*");
		write(0,playerY, "X");
		if(checkcollision(pos,y,playerY))
		{
			printf("Hit");
			playerLives -=1;
			if(playerLives == 0)
			{
				printf("Dead");
				return 1;
								
				
			}
		}
		delay(shootdelay);
		clear();
			
	}
	return 0;
}
void main()
{
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return; 
	}
	pinMode(BtnPin, INPUT);
	fd = wiringPiI2CSetup(LCDAddr);
	init();
//for(;;){ playerY = digitalRead(BtnPin); printf("%d",playerY);}
	srand((unsigned int)time(NULL));
	int x,y;
	int result = 0;
	int move = 0;
	int scount = 0;
	while (result!=1)
	{
		
	//	if(move==38)
	//	{
	//		playerY = 0;
	//	}
	//	if(move==40)
	//	{
	//		playerY = 1;
	//	}
		result = shoot(rand()%2,scount+1);
		if(result)
		{
			char buffer [1000];
			sprintf(buffer,"Score: %d",scount);
			write(0,0,buffer);
			delay(2000);
		}
	}
	


	for ( x = 0; x < 16; ++x )
	{
		clear();
		write(x, 0, "Greetings!");
		write(16-x, 1, "From SunFounder");
		delay(200);
	}
	delay(2000);
	clear();
}

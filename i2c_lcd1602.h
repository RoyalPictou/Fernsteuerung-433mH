#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>

void clear();

void write(int x, int y, char data[]);


void initlcd(char *headline1, char *headline2);

#include "control.h"
#include "rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include <stdbool.h>
#include "i2c_lcd1602.h"
#define PCF 120
#define uchar unsigned char

int AIN0 = PCF + 0;
int AIN1 = PCF + 1;
int AIN2 = PCF + 2;
#define OPTIONLENGTH 8
char *state[ 7 ] = { "home", "up", "down", "left", "right", "pressed" };
int option[ OPTIONLENGTH ] = { 0 };
int codesOn[4] = {5506385,5509457,5510225,5510417};
int codesOff[4] = {5506388,5509460,5510228,5510420};
int position = 0;
bool
setOption()
{
    int x, y, b;
    bool rv = false;
    static bool changePos = true;
    static bool buttonState = false;
    int tmp = 0;
    x = analogRead( AIN1 );
    y = analogRead( AIN0 );
    b = analogRead( AIN2 );
    if ( ( y <= 30 ) && changePos )
    {
        position--; // up
        changePos = false;
        rv = true;
    }
    if ( ( y >= 255 ) && changePos )
    {
        position++; // down
        changePos = false;
        rv = true;
    }
    if ( ( x >= 255 ) && changePos )
    {
        option[ position ] = 0; // right
	sendCode( codesOff[position]);
        changePos = false;
        rv = true;
    }
    if ( ( x <= 30 ) && changePos )
    {
        option[ position ] = 1; // left
	sendCode(codesOn[position]);
        changePos = false;
        rv = true;
    }
    if ( b <= 30 )
    {
        if ( !buttonState )
        {
            option[ position ] = 1 - option[ position ]; // button pressed
            changePos = false;
            rv = true;
            printf( "button pressed" );
        }
        buttonState = true;
    }
    else
    {
        if ( buttonState )
        {
            rv = true;
        }
        buttonState = false;
    }
    if ( ( x - 125 < 15 ) && ( x - 125 > -15 ) && ( y - 125 < 15 ) && ( y - 125 > -15 ) && ( b >= 60 ) )
    {
        changePos = true; // home position
    }
    if ( position >= OPTIONLENGTH )
    {
        position = 0;
    }
    else
    {
        if ( 0 > position )
        {
            position = OPTIONLENGTH - 1;
        }
    }
    return rv;
}

void
sendCode(int code)
{
    printf("sending code[%i]\n", code);
    RCSwitch mySwitch = RCSwitch();
    int PIN = 0;
    int protocol = 0;
    int pulseLength = 0;
    if (protocol != 0) mySwitch.setProtocol(protocol);
    if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
    mySwitch.enableTransmit(PIN);
    mySwitch.send(code, 24);
}

int
main( void )
{
    int tmp = 0;
    int status = 0;
    wiringPiSetup();
    

    char bufline1[ 10 ];
    char bufline2[ 10 ];
    char bufdigit[ 2 ];
    bool initalPrint = true;
    bufdigit[ 1 ] = 0;
    // Setup pcf8591 on base pin 120, and address 0x48
    pcf8591Setup( PCF, 0x48 );
    initlcd( "Jesko", "was here" );
    bufline2[ OPTIONLENGTH ] = 0;
    while ( 1 ) // loop forever
    {
        bufline1[ 0 ] = 0;
        bool rv = setOption();
        if ( rv || initalPrint )
        {
            initalPrint = false;
            printf( "Position: %d\n", position );

            clear();
            for ( int i = 0; i < OPTIONLENGTH; i++ )
            {
                if ( i == position )
                {
                    strcat( bufline1, "*" );
                }
                else
                {
                    bufdigit[ 0 ] = i + '0';

                    strcat( bufline1, bufdigit );
                }
                bufline2[ i ] = option[ i ] + '0';
            }
            write( 0, 0, bufline1 );
            write( 0, 1, bufline2 );
        }
    }
    return 0;
}






















     


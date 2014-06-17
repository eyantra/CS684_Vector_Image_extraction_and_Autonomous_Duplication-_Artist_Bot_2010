/**
 * @file project.c
 * cs684 course project. The project duplicates a black and white image
 * read through the camera and redraws it on a chart paper/surface.
 * This piece of code, is converted to a hex file of ATMEGA2560 standard
 * and burnt onto the FIREBIRD V Robot.
 * @author Group5, Abhirup Ghosh, Anirban Basumallik & Gourab Roy, IIT Bombay
 * @date 07/Nov/2010
 * @version 1.0
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 * This is a property of ERTS Lab, IIT Bombay.
 *
 * @section Compilation
 * Compile using the AVR studio for ATMEGA2560 and F_CPU=11059200
 * Write the .hex file to the FIREBIRD V robot
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

/// PWM supplied to the dc motors
#define RIGHTAVGSPEED 115
#define LEFTAVGSPEED 115 //Should be slower

/// STEP of synchronisation of dc motor shaft counter during translational movement
#define STEP 1

/// STEP of synchronisation of dc motor shaft counter during rotational movement
#define TURNSTEP 3

/// Delay used between acts like penUp, move, turn and pendown
#define DELAY1 1500

/// Switch to enable XBEE wireless controller or the rs 232 serial port
#define XBEE 0

int left_shaft_count = 0; ///< left shaft interrupt counter
int right_shaft_count = 0; ///< right shaft interrupt counter
int dataRcvCount = 0; ///< counts the number of data triplets received
int dataType = 0; ///< keeps track of which part of the data triplet is being received
int dataRcv[100][3]; ///< total of 100 triplets/line segment information can be received
int startExec = 0; ///< a global flag set to 1 after data receiving is finished

void forward(float);
void backward(float);
void stop();
void hard_left_turn(float);
void hard_right_turn(float);

/**
 * XBee wireless trans-receiver initiator
 */
void INIT_USRT_0 ()
{
	UCSR0B = 0x00;
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UBRR0L = 0x47; //use baud rate low
	UBRR0H = 0x00; //use baud rate high
	UCSR0B = 0x98; 
}

/**
 * Serial port RS232D initiator
 */
void INIT_USRT_1 ()
{
	UCSR1B = 0x00;
	UCSR1A = 0x00;
	UCSR1C = 0x06;
	UBRR1L = 0x47; //use baud rate low
	UBRR1H = 0x00; //use baud rate high
	UCSR1B = 0x98; 
}

/**
 * Initiate other ports
 */
void INIT_PORTS()
{
	DDRA = 0x0F;	//set 0-3 pins as output value
	PORTA=0x00;		// Port A init
	DDRL = 0x18;
	PORTL = 0x18;
	DDRE = 0x00; 	// PORT E is set for input
	PORTE = 0x00;

	//Configure PORTB 5 pin for servo motor 1 operation
	DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
	PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1

	//Configure PORTB 6 pin for servo motor 2 operation
	DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
	
	DDRE = DDRE & 0x7F;  //PORTE 7 pin set as input   
	PORTE = PORTE | 0x80; //PORTE7 internal pull-up enabled 
}

/**
 * Initiate PWM
 */
void INIT_PWM ()
{
	// For Servo Motors
	TCCR1B = 0x00; 	//stop
	TCNT1H = 0xFC; 	//Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
	OCR1AH = 0x03;	//Output compare eegister high value for servo 1
	OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
	OCR1BH = 0x03;	//Output compare eegister high value for servo 2
	OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
	OCR1CH = 0x03;	//Output compare eegister high value for servo 3
	OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
	ICR1H  = 0x03;	
	ICR1L  = 0xFF;
	TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
						For Overriding normal port functionalit to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
	TCCR1C = 0x00;
	TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)	

	// For DC Motors
	TCCR5B = 0x00;
	TCCR5A = 0xA1;
	TCCR5B = 0x0B;
	
	OCR5AL = RIGHTAVGSPEED;
	OCR5BL = LEFTAVGSPEED;
}

/**
 * Initiate interrupt system
 */
void INIT_INT ()
{
	cli();
	EICRB = 0x0A; // Enable interrupt at falling edge for both wheels.
	EIMSK = 0x30; // Enable interrupt for both the wheels.
	sei(); // Set global interrupt.
}

/**
 * Interrupt Service Routine for right motor (in the opposite configuration)
 */
ISR(INT4_vect)
{
	right_shaft_count++; // Inverted purposefully in opposite configuration
}

/**
 * Interrupt Service Routine for left motor (in the opposite configuration)
 */
ISR(INT5_vect)
{
	left_shaft_count++; // Inverted purposefully in opposite configuration
}

/**
 * Rotate panning servo by a specified angle in the multiples of 2.25 degrees
 */
void servo_1(unsigned char degrees)
{
	float PositionPanServo = 0;
	PositionPanServo = ((float)degrees / 2.25) + 21.0;
	OCR1AH = 0x00;
	OCR1AL = (unsigned char) PositionPanServo;
}

/**
 * Rotate pen controller servo by a specified angle in the multiples of 2.25 degrees
 */
void servo_2(unsigned char degrees)
{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 2.25) + 21.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

/**
 * Free the panning servo
 */
void servo_1_free (void) //makes servo 1 free rotating
{
	OCR1AH = 0x03; 
	OCR1AL = 0xFF; //Servo 1 off
}

/**
 * Free the pen controller servo
 */
void servo_2_free (void) //makes servo 2 free rotating
{
	OCR1BH = 0x03;
	OCR1BL = 0xFF; //Servo 2 off
}


/**
 * Display shaft count on LCD
 */
void display_shaft_count()
{
	//Display left shaft count
	lcd_cursor(1,1);
	lcd_wr_char('L');
	lcd_print (1,2,left_shaft_count,3);

	//Display right shaft count
	lcd_cursor(1,6);
	lcd_wr_char('R');
	lcd_print (1,7,right_shaft_count,3);
}

/**
 * Move both motor forward
 */
void bothMotorForward()
{
	PORTA = 0x09; //Orginal Config: PORTA = 0x06;
}

/**
 * Move both motor backward
 */
void bothMotorBackward()
{
	PORTA = 0x06; //Opp Config
}

/**
 * Move right motor forward
 */
void rightMotorForward()
{
	PORTA = 0x01; //Orginal Config: PORTA = 0x04;
}

/**
 * Move right motor backward
 */
void rightMotorBackward()
{
	PORTA = 0x02; //Opp Config
}

/**
 * Move left motor forward
 */
void leftMotorForward()
{
	PORTA = 0x08; //Orginal Config: PORTA = 0x02;
}

/**
 * Move left motor backward
 */
void leftMotorBackward()
{
	PORTA = 0x04; //Opp Config
}

/**
 * Move right motor forward and left motor backward
 */
void rightMotorForwardLeftMotorBackward()
{
	PORTA = 0x05; // This is opposite config
}

/**
 * Move right motor backward and left motor forward
 */
void leftMotorForwardRightMotorBackward()
{
	PORTA = 0x0A; // This is opposite config
}

/**
 * Move forward by a certain distance by synchronising the motors STEP wise
 * @param distance distance should be in cm
 */
void forward (float distance) // distance in cm
{
	int interimLeftCountLimit = STEP, interimRightCountLimit = STEP;
	int spokeCount = 1.854 * distance; // 1.854 = 30 / 16.18cm where 16.18cm is the circumference of the wheel
	left_shaft_count = 0;
	right_shaft_count = 0;
	do
	{
		display_shaft_count();

	
		bothMotorForward();
		/*while(left_shaft_count < interimLeftCountLimit && right_shaft_count < interimRightCountLimit)
		{
			bothMotorForward();
		}
		while(left_shaft_count < interimLeftCountLimit)
		{
			leftMotorForward();
		}
		while(right_shaft_count < interimRightCountLimit)
		{
			rightMotorForward();
		}

		// Increase Interim Values
		if(left_shaft_count < spokeCount)
		{
			if(spokeCount - left_shaft_count < STEP)
				interimLeftCountLimit = spokeCount;
			else
				interimLeftCountLimit += STEP;
		}
		if(right_shaft_count < spokeCount)
		{
			if(spokeCount - right_shaft_count < STEP)
				interimRightCountLimit = spokeCount;
			else
				interimRightCountLimit += STEP;
		}*/
		//stop();
	}while(left_shaft_count < spokeCount && right_shaft_count < spokeCount);

	stop();
	display_shaft_count();
}

/**
 * Move backward by a certain distance by synchronising the motors STEP wise
 * @param distance distance should be in cm
 */
void backward (float distance)
{
	int interimLeftCountLimit = STEP, interimRightCountLimit = STEP;
	int spokeCount = 1.854 * distance; // 1.854 = 30 / 16.18cm where 16.18cm is the circumference of the wheel
	left_shaft_count = 0;
	right_shaft_count = 0;
	do
	{
		display_shaft_count();
		
		bothMotorBackward();
		/*
		while(left_shaft_count < interimLeftCountLimit && right_shaft_count < interimRightCountLimit)
		{
			bothMotorBackward();
		}
		while(left_shaft_count < interimLeftCountLimit)
		{
			leftMotorBackward();
		}
		while(right_shaft_count < interimRightCountLimit)
		{
			rightMotorBackward();
		}

		// Increase Interim Values
		if(left_shaft_count < spokeCount)
		{
			if(spokeCount - left_shaft_count < STEP)
				interimLeftCountLimit = spokeCount;
			else
				interimLeftCountLimit += STEP;
		}
		if(right_shaft_count < spokeCount)
		{
			if(spokeCount - right_shaft_count < STEP)
				interimRightCountLimit = spokeCount;
			else
				interimRightCountLimit += STEP;
		}
		stop();*/
	}while(left_shaft_count < spokeCount && right_shaft_count < spokeCount);

	stop();
	display_shaft_count();
}

/**
 * Turn left by a certain distance by synchronising the motors TURNSTEP wise
 * @param angle angle should be in degrees
 */
void hard_left_turn (float angle)
{
	int interimLeftCountLimit, interimRightCountLimit;
	int spokeCount = angle / 3.95; // Experimental
	static int c = 0;
	left_shaft_count = 0;
	right_shaft_count = 0;
	if(spokeCount < TURNSTEP)
	{
		interimLeftCountLimit = interimRightCountLimit = spokeCount;
	}
	else
	{
		interimLeftCountLimit = interimRightCountLimit = TURNSTEP;
	}
	/*if(angle < 35 && angle > 10)
	{
		c++;
		if( c % 2 == 0 )
			spokeCount++; // Experimental adjustments
	}*/
	do
	{
		display_shaft_count();
			//rightMotorForwardLeftMotorBackward();

		while(left_shaft_count < interimLeftCountLimit && right_shaft_count < interimRightCountLimit)
		{
			rightMotorForwardLeftMotorBackward();
		}
		while(left_shaft_count < interimLeftCountLimit)
		{
			leftMotorBackward();
		}
		while(right_shaft_count < interimRightCountLimit)
		{
			rightMotorForward();
		}

		// Increase Interim Values
		if(left_shaft_count < spokeCount)
		{
			if(spokeCount - left_shaft_count < TURNSTEP)
				interimLeftCountLimit = spokeCount;
			else
				interimLeftCountLimit += TURNSTEP;
		}
		if(right_shaft_count < spokeCount)
		{
			if(spokeCount - right_shaft_count < TURNSTEP)
				interimRightCountLimit = spokeCount;
			else
				interimRightCountLimit += TURNSTEP;
		}
		stop();
	}while(left_shaft_count < spokeCount && right_shaft_count < spokeCount);

	stop();
	display_shaft_count();

}

/**
 * Turn right by a certain distance by synchronising the motors TURNSTEP wise
 * @param angle angle should be in degrees
 */
void hard_right_turn (float angle)
{
	int interimLeftCountLimit, interimRightCountLimit;
	int spokeCount = angle / 3.95; // Explain the calculations here
	static int c = 0;
	left_shaft_count = 0;
	right_shaft_count = 0;
	if(spokeCount < TURNSTEP)
	{
		interimLeftCountLimit = interimRightCountLimit = spokeCount;
	}
	else
	{
		interimLeftCountLimit = interimRightCountLimit = TURNSTEP;
	}
	/*if(angle < 35 && angle > 10)
	{
		c++;
		if( c % 2 == 0 )
			spokeCount++; // Experimental adjustments
	}*/
	do
	{
		display_shaft_count();

			//leftMotorForwardRightMotorBackward();

		while(left_shaft_count < interimLeftCountLimit && right_shaft_count < interimRightCountLimit)
		{
			leftMotorForwardRightMotorBackward();
		}
		while(left_shaft_count < interimLeftCountLimit)
		{
			leftMotorForward();
		}
		while(right_shaft_count < interimRightCountLimit)
		{
			rightMotorBackward();
		}

		// Increase Interim Values
		if(left_shaft_count < spokeCount)
		{
			if(spokeCount - left_shaft_count < TURNSTEP)
				interimLeftCountLimit = spokeCount;
			else
				interimLeftCountLimit += TURNSTEP;
		}
		if(right_shaft_count < spokeCount)
		{
			if(spokeCount - right_shaft_count < TURNSTEP)
				interimRightCountLimit = spokeCount;
			else
				interimRightCountLimit += TURNSTEP;
		}
		stop();
	}while(left_shaft_count < spokeCount && right_shaft_count < spokeCount);

	stop();
	display_shaft_count();
}

/**
 * Stop the motors
 */
void stop ()
{
	PORTA = 0x00;
}

/**
 * Lift the pen
 */
void penUp()
{
	servo_2(110);
	_delay_ms(100);
}

/**
 * Put down the pen
 */
void penDown()
{
	servo_2(135);
	_delay_ms(100);
}

/**
 * Send the turning sequence moves to the robot like penup, forward, turn, backward and pendown
 * @param angle in degrees
 */
void turn_left(int angle)
{
	penUp();
	_delay_ms(DELAY1);
	forward(2.0);
	_delay_ms(DELAY1);
	hard_left_turn(angle);
	_delay_ms(DELAY1);
	backward(1.0);
	_delay_ms(DELAY1);
	penDown();
}

/**
 * Send the turning sequence moves to the robot like penup, forward, turn, backward and pendown
 * @param angle in degrees
 */
void turn_right(int angle) // angle in degrees
{
	penUp();
	_delay_ms(DELAY1);
	forward(1.5);
	_delay_ms(DELAY1);
	hard_right_turn(angle);
	_delay_ms(DELAY1);
	backward(1.5);
	_delay_ms(DELAY1);
	penDown();
}

/**
 * Display number of triplets/line segments' information received
 */
void display_data_count()
{
	/*if(dataRcvCount <= 1)
	{
		flashDisplay("Receiving Data..",0);
	}*/
	lcd_cursor(2,1);
	lcd_wr_char('D');
	lcd_print (2,2,dataRcvCount,3);
	lcd_cursor(2,6);
	lcd_wr_char('T');
	lcd_print (2,7,dataType,3);
}

/**
 * Interrupt Service Routine for receiving XBEE data
 */
SIGNAL (SIG_USART0_RECV)
{
	cli();
	unsigned int data = UDR0;
	if(data == 255)
		startExec = 1;
	else if(data == 254)
	{
		dataRcvCount++;
		dataType = 0;
	}
	else
	{
		dataRcv[dataRcvCount][dataType++] = data;
		display_data_count();
	}
	display_data_count();
	sei();
}

/**
 * Interrupt Service Routine for receiving RS232D serial port data
 */
SIGNAL (SIG_USART1_RECV)
{
	cli();
	unsigned int data = UDR1;
	if(data == 255)
		startExec = 1;
	else if(data == 254)
	{
		dataRcvCount++;
		dataType = 0;
	}
	else
	{
		display_data_count();
		dataRcv[dataRcvCount][dataType++] = data;
	}
	display_data_count();
	sei();
}


/**
 * Initialize all the ports and hardwares
 */
void init_all()
{	
	INIT_INT ();
	INIT_PORTS();
	INIT_PWM();
	lcd_port_config();
	lcd_set_4bit();
	lcd_init();

	if(XBEE)
		INIT_USRT_0();
	else
		INIT_USRT_1();
}


/**
 * Flash a line of display on the LCD
 * @param str is the string to display
 * @param time is the time in ms to display the string
 */
void flashDisplay(char *str, int time)
{
	lcd_cursor(1,1);
	lcd_string(str);
	lcd_cursor(2,1);
	lcd_string("                ");
	if(time != 0)
	{
		_delay_ms(time);
		lcd_cursor(1,1);
		lcd_string("                ");
	}
}

/**
 * Display received data
 */
void displayRcvdData(int time)
{
	int i;
	for(i = 0; i < dataRcvCount ; i++)
	{
		lcd_cursor(1,1);
		lcd_wr_char('F');
		lcd_print (1,2,dataRcv[i][0],3);
		
		lcd_cursor(1,6);
		lcd_wr_char('V');
		lcd_print (1,7,dataRcv[i][1],3);
		
		lcd_cursor(1,11);
		lcd_wr_char('T');
		lcd_print (1,12,dataRcv[i][2],1);

		_delay_ms(time);
	}
}

/**
 * Draw the sequence of line segments as received from the PC
 */
void drawTheShape()
{
	int i;

	for(i=0;i<dataRcvCount;i++)
	{
		// Moving forward
		if(dataRcv[i][0] != 0)
		{
			lcd_cursor(2,1);
			lcd_string("Forward         ");
			lcd_print(2,9,dataRcv[i][0],3);
			forward (dataRcv[i][0]);
			_delay_ms(10);
		}
		// Turning
		if(dataRcv[i][2] == 1 && dataRcv[i][1] > 3)
		{// Left Turn if angle of turn is more than 3 degrees
			lcd_cursor(2,1);
			lcd_string("Left            ");
			lcd_print(2,6,dataRcv[i][1],3);
			turn_left(dataRcv[i][1]);
		}
		else if(dataRcv[i][2] == 0 && dataRcv[i][1] > 3)
		{// Right Turn if angle of turn is more than 3 degrees
			lcd_cursor(2,1);
			lcd_string("Right            ");
			lcd_print(2,7,dataRcv[i][1],3);
			turn_right(dataRcv[i][1]);
		}
		else if(dataRcv[i][2] == 2)
		{// Pull up the pen
			penUp();
		}
		_delay_ms(10);	
	}
}

/**
 * The main method
 */
int main ()
{
	init_all();
	// Set servo pan
	servo_1(88);
	_delay_ms(100);
	penUp();
	
	flashDisplay("ERTS Lab Project",500);
	flashDisplay("Waiting for I/P",0);

	// Wait for data receiving to finish
	 while(!startExec);

	//displayRcvdData(100);
	
	flashDisplay("Remove Serial   ",0);
	while(PINE & 0x80);
	flashDisplay("Starting to draw",1000);

	//penDown();
	
	//Draw the shape
	drawTheShape();
	//forward(20);


	flashDisplay("Finished        ",0);

	penUp();
	while(1);
}

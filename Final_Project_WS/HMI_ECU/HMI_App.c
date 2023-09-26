#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>/* For the delay functions */
#include "lcd.h"
#include "gpio.h"
#include "keypad.h"
#include "common_macros.h"
#include "uart.h"
#include "timer.h"



/*******************************************************************************
 *                      MACROS                                   *
 *******************************************************************************/

#define ENTER_KEY 13
#define HMI_Ready 0xAC
#define CONTROL_Ready 0xCA
#define OPEN_DOOR 0x66
#define CHANGE_PASS 0x55



/*******************************************************************************
 *                      global Varibles                                   *
 *******************************************************************************/

typedef enum {Unmatched,Matched}matchig_state;/*defining a new type that has two cases which indicates the system Password state  */
uint8 g_matching_flag;/*to be used to get out the loop when reach to the desired case */
uint8 password_1[5]; /*ARRAY TO SAVE THE FIRST 5-DIGIT-PASSWORD & SENNDING IT BY LOOPING ON THIS ARR */
uint8 password_2[5];/*ARRAY TO SAVE THE SECOND 5-DIGIT-PASSWORD & SENNDING IT BY LOOPING ON THIS ARR */
uint8 i;/*counter*/
uint8 g_option;/*the */
uint8 g_ticks=0;/*to be used in ISR callback function*/
matchig_state password_state;/*used as a flag that indicates the system PASSWORD state*/
uint8 g_trials=0;/*Indicates the number of wrong trials of entering the password*/

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
 * Description :
 * ****** Create System Password *******
 *	1.This function manage the user to make a new password  for the system by getting the passwords from the keypad and saving them in password_1[5] &password_2[5]
 * 	2.then function Sends the two Passwords from The HMI_MUC  TO CONTROL_MUC By UART MODULE
 * 	3. the CONTROL_MUC will check if the two passwords are matched or not and send the state
 * 	4. In Case they are matched  will Put the g_matchingflag=1 and break the do{}while(); loop
 * 	5.   In Case they are not matched it will repeat it self till matching will occur
 */
void passwor_setting(void);

/*
 * Description :
 * It is a function to just display the options of the system
 * "+:Open Door"&"-:Change Pass"
 */


void main_options(void);


/*
 * Description :
 * 1.First it awaits a byte that indicates the CONTROL_MUC is
 * ready to receive the byte that indicates any any function
 * will be called in the switch statement in the WHILE(1)
 *  in this  case OPEN_DOOR:check_door_pass();-->will be called
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked for 3 times if matched
 *   to the saved one in in EEPROM
 *
 *  3.in case the password is matched it will break the loop
*   after display "opening" the door and "closing"it back on the LCD
 *
 * 	4. in case the password is not matched for 3 times
 * 	the ERROR Message will appear for 60 seconds
 */

void open_door(void);

/*
 * Description :
 * It is just a incremented counter
 * used to be called back by the timer
 * which set to make an interrupt every 1 second
 * so the counter's value will increase every 1 second
 */


void TIMER_ISR_COUNTER(void);


/*
 * Description :
 * 1.First it awaits a byte that indicates the CONTROL_MUC is
 * ready to receive the byte that indicates any any function
 * will be called in the switch statement in the WHILE(1)
 *  in this  case CHANGE_PASS:Change_password();-->will be called
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked (by the CONTROL_MUC) for 3 times
 *  if matched to the saved one in in EEPROM
 *   the CONTROL_MUC will send the password state to the HMI-MUC if matched or not
 *   if matched the HMI_MUC will call the function  passwor_setting(); and break the loop
 *  if not matched the ERROR MESSAGE will appear for 60 seconds
 *
 */

void Change_pass(void);

/*******************************************************************************
 *                      MAIN                                   *
 *******************************************************************************/



int main(void)
{

	UART_ConfigType UART_Configs={_8_bit,no_parity,_1_bit,9600};
	Timer1_ConfigType TIMER_Configs={0,1000,F_CPU_1024,CTC_MODE};
	Timer1_init(&TIMER_Configs);
	LCD_init();
	UART_init(&UART_Configs);
	passwor_setting();
	LCD_clearScreen();

	/***______________________________________________________________________***/

	while(1)
	{



		do{
			main_options();
			g_option=KEYPAD_getPressedKey();


		}while(g_option!='-'&&g_option!='+');


		switch (g_option)
		{
		case'+': open_door();


		break;
		case'-':Change_pass();


		break;


		}




	}




}




/*
 * Description :
 * ****** Create System Password *******
 *	1.This function manage the user to make a new password  for the system by getting the passwords from the keypad and saving them in password_1[5] &password_2[5]
 * 	2.then function Sends the two Passwords from The HMI_MUC  TO CONTROL_MUC By UART MODULE
 * 	3. the CONTROL_MUC will check if the two passwords are matched or not and send the state
 * 	4. In Case they are matched  will Put the g_matchingflag=1 and break the do{}while(); loop
 * 	5.   In Case they are not matched it will repeat it self till matching will occur
 */



void passwor_setting(void)
{

	do{
		password_1[0]=0;
		password_2[0]=0;
		g_matching_flag=0;

		LCD_displayStringRowColumn(0, 0, "PLZ Enter the");
		LCD_moveCursor(1, 0);
		LCD_displayStringRowColumn(1, 0, "New Pass:");



		while(KEYPAD_getPressedKey()!=ENTER_KEY){

			for(i=0;i<5;i++)
			{
				password_1[i]=KEYPAD_getPressedKey();
				if(KEYPAD_getPressedKey()<=9 && KEYPAD_getPressedKey()>=0)
				{
					LCD_displayCharacter('*');
					_delay_ms(500);

				}

				else

				{
					i--;
				}


			}



		}

		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "PLZ Re_enter ");
		LCD_displayStringRowColumn(1, 0, "Pass:");



		for(i=0;i<5;i++)
		{
			password_2[i]=KEYPAD_getPressedKey();
			if(KEYPAD_getPressedKey()<=9 && KEYPAD_getPressedKey()>=0){
				LCD_displayCharacter('*');
				_delay_ms(500);

			}

			else
			{
				i--;
			}


		}





		/*send password1  to control*/

		while(UART_recieveByte()!=CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			UART_sendByte(password_1[i]);
		}

		while(UART_recieveByte()!=CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			UART_sendByte(password_2[i]);
		}

		UART_sendByte(HMI_Ready);
		password_state=UART_recieveByte();

		LCD_clearScreen();

		if(password_state == Matched) /*flag that sent by CoNTrOl MC that indicates the state of the password*/
		{
			g_matching_flag=1;
			LCD_displayStringRowColumn(0, 4, "Matched");
			_delay_ms(500);

		}

		else{

			LCD_displayStringRowColumn(0, 4, "Unmatched");
			_delay_ms(500);

		}

	}while(g_matching_flag==0);

}

/*
 * Description :
 * It is a function to just display the options of the system
 * "+:Open Door"&"-:Change Pass"
 */


void main_options(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0,"+:Open Door");
	LCD_displayStringRowColumn(1, 0,"-:Change Pass");

}

/*
 * Description :
 * 1.First it awaits a byte that indicates the CONTROL_MUC is
 * ready to receive the byte that indicates any any function
 * will be called in the switch statement in the WHILE(1)
 *  in this  case OPEN_DOOR:check_door_pass();-->will be called
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked for 3 times if matched
 *   to the saved one in in EEPROM
 *
 *  3.in case the password is matched it will break the loop
 *   after display "opening" the door and "closing"it back on the LCD
 *
 * 	4. in case the password is not matched for 3 times
 * 	the ERROR Message will appear for 60 seconds
 */




void open_door(void)
{

	while(UART_recieveByte()!=CONTROL_Ready);
	UART_sendByte(OPEN_DOOR);

	do{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "PLZ Enter Pass:");
		LCD_moveCursor(1, 0);

		while(KEYPAD_getPressedKey()!=ENTER_KEY){

			for(i=0;i<5;i++)
			{
				password_1[i]=KEYPAD_getPressedKey();
				if(KEYPAD_getPressedKey()<=9 && KEYPAD_getPressedKey()>=0)
				{
					LCD_displayCharacter('*');
					_delay_ms(500);

					//LCD_intgerToString(KEYPAD_getPressedKey());

				}

				else
				{
					i--;
				}

			}

		}


		while(UART_recieveByte()!=CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			UART_sendByte(password_1[i]);


		}

		UART_sendByte(HMI_Ready);
		password_state=UART_recieveByte();
		if(password_state == Matched)
		{
			g_ticks=0;
			LCD_clearScreen();
			LCD_displayString("Opennig..");
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=15);

			g_ticks=0;
			LCD_clearScreen();
			LCD_displayString("WELCOM BACK");
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=3);

			g_ticks=0;
			LCD_clearScreen();
			LCD_displayString("Closing..");
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=15);
			Timer1_deInit();


			break;


		}

		else{
			g_trials++;


		}

		if(g_trials==1){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, "TRIALS=1");
			_delay_ms(500);
		}
		else if(g_trials==2){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, "TRIALS=2");
			_delay_ms(500);
		}


	}while(g_trials<3);

	if(g_trials==3){
		g_ticks=0;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 2, "!!!!ERROR!!!!");
		Timer1_setCallBack(TIMER_ISR_COUNTER);
		while(g_ticks!=60);
		Timer1_deInit();
		g_trials=0;
	}





}

/*
 * Description :
 * It is just a incremented counter
 * used to be called back by the timer
 * which set to make an interrupt every 1 second
 * so the counter's value will increase every 1 second
 */

void TIMER_ISR_COUNTER(void)
{
	g_ticks++;
}


/*
 * Description :
 * 1.First it awaits a byte that indicates the CONTROL_MUC is
 * ready to receive the byte that indicates any any function
 * will be called in the switch statement in the WHILE(1)
 *  in this  case CHANGE_PASS:Change_password();-->will be called
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked (by the CONTROL_MUC) for 3 times
 *  if matched to the saved one in in EEPROM
 *   the CONTROL_MUC will send the password state to the HMI-MUC if matched or not
 *
 *  3.if matched the HMI_MUC will call the function  passwor_setting(); and break the loop
 *
 * 	4. if not matched the ERROR MESSAGE will appear for 60 seconds
 *
 */



void Change_pass(void)
{

	while(UART_recieveByte()!=CONTROL_Ready);
	UART_sendByte(CHANGE_PASS);

	do{

		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "PLZ Enter Pass:");
		LCD_moveCursor(1, 0);



		while(KEYPAD_getPressedKey()!=ENTER_KEY){

			for(i=0;i<5;i++)
			{
				password_1[i]=KEYPAD_getPressedKey();
				if(KEYPAD_getPressedKey()<=9 && KEYPAD_getPressedKey()>=0)
				{
					LCD_displayCharacter('*');
					_delay_ms(500);

				}

				else

				{
					i--;
				}


			}



		}


		while(UART_recieveByte()!=CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			UART_sendByte(password_1[i]);


		}

		UART_sendByte(HMI_Ready);
		password_state=UART_recieveByte();

		if(password_state == Matched)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 4, "Matched");
			_delay_ms(500);

			passwor_setting();
			break;
		}

		else
		{
			g_trials++;
		}
		if(g_trials==1){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, "TRIALS=1");
			_delay_ms(500);
		}
		else if(g_trials==2){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, "TRIALS=2");
			_delay_ms(500);
		}




	}while(g_trials<3);
	if(g_trials==3){
		g_ticks=0;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 2, "!!!!ERROR!!!!");
		Timer1_setCallBack(TIMER_ISR_COUNTER);
		while(g_ticks!=60);
		Timer1_deInit();
		g_trials=0;
	}



}

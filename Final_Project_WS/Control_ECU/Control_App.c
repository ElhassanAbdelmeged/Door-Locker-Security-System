#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>/* For the delay functions */
#include "gpio.h"
#include "common_macros.h"
#include "uart.h"
#include "timer.h"
#include "twi.h"
#include "motor.h"
#include "lcd.h"/*for testing */
#include "buzzer.h"
#include "external_eeprom.h"


/*******************************************************************************
 *                      MACROS                                   *
 *******************************************************************************/
#define CONTROL_Ready 0xCA
#define HMI_Ready 0xAC
#define CONTROL_ADDRESS 0x01
#define OPEN_DOOR 0x66
#define CHANGE_PASS 0x55

/*******************************************************************************
 *                      global Variables                                   *
 *******************************************************************************/

uint8 g_ticks=0; /*to be used in ISR callback function*/
uint8 password_1[5];/*ARRAY TO SAVE THE FIRST 5-DIGIT-PASSWORD BY RECIEVING  IT  FROM THE HMI BY LOOPING ON THIS ARR */
uint8 password_2[5];/*ARRAY TO SAVE THE SECOND 5-DIGIT-PASSWORD BY RECIEVING  IT  FROM THE HMI BY LOOPING ON THIS ARR */
uint8 g_matching_flag=0;/*to be used to get out the loop when reach to the desired case */
uint8 g_trials=0; /*Indicates the number of wrong trials of entering the password*/
typedef enum {Unmatched,Matched}matchig_state;/*defining a new type that has two cases which indicates the system Password state  */
matchig_state password_state=Matched;/*used as a flag that indicates the system PASSWORD state*/
uint8 i;/*counter*/


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/*
 * Description :
 * ****** Create System Password *******
 * 1.this function Receives The two Passwords from The HMI_MUC By UART MODULE
 * 2. Comparing the two passwords and decide that they are matched or not.
 * 3.in the case they are matched the password will be saved in EEPROM
 */
void recive_password(void);

/*
 * Description :

 *  1.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked for 3 times if matched
 *   to the saved one in in EEPROM  and send the password state to the HMI_MUC
 *
 *  2.in case the password is matched it will break the loop
 *   after opening the door and closing it back
 *
 * 	3. in case the password is not matched for 3 times the BUZZER will be ON  for 60s
 */



void check_door_pass(void);


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
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked (by the CONTROL_MUC) for 3 times
 *  if matched to the saved one in in EEPROM
 *  the CONTROL_MUC will send the password state to the HMI-MUC if matched or not
 *
 *  3.if matched the HMI_MUC will call the function  passwor_setting(); and break the loop
 *
 * 	4. if not matched for 3 times the BUZZER will be ON  for 60s
 *
 */


void Change_password(void);

/*******************************************************************************
 *                      MAIN                                   *
 *******************************************************************************/


int main(void)
{
	Timer1_ConfigType TIMER_Configs={0,1000,F_CPU_1024,CTC_MODE};
	UART_ConfigType UART_Configs={_8_bit,no_parity,_1_bit,9600};
	TWI_ConfigType TWI_Configs={CONTROL_ADDRESS,250};
	Timer1_init(&TIMER_Configs);
	UART_init(&UART_Configs);
	TWI_init(&TWI_Configs);
	DcMotor_Init();
	LCD_init();
	Buzzer_init();
	recive_password();


	/*=====================================================================*/

	while(1){


		UART_sendByte(CONTROL_Ready);
		switch (UART_recieveByte())
		{
		case OPEN_DOOR:check_door_pass();
		break;
		case CHANGE_PASS:Change_password();
		break;
		}

	}

}












/*
 * Description :
 * ****** Create System Password *******
 * 1.this function Receives The two Passwords from The HMI_MUC By UART MODULE
 * 2. Comparing the two passwords and decide that they are matched or not.
 * 3.in the case they are matched the password will be saved in EEPROM
 */

void recive_password(void)
{

	do{
		g_matching_flag=0;
		password_1[0]=0;
		password_2[0]=0;

		UART_sendByte(CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			password_1[i]=UART_recieveByte();

		}


		UART_sendByte(CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			password_2[i]=UART_recieveByte();

		}
		/*test*/
		LCD_clearScreen();
		for(i=0;i<5;i++){
			LCD_intgerToString(password_1[i]);
		}

		LCD_moveCursor(1, 0);
		for(i=0;i<5;i++){
			//LCD_displayCharacter('*');
			LCD_intgerToString(password_2[i]);
		}



		for(i=0;i<5;i++){
			if(password_1[i]!=password_2[i])
			{
				//g_matching_flag=0;
				password_state=Unmatched;
				break;

			}

		}

		while(UART_recieveByte()!=HMI_Ready);
		UART_sendByte(password_state);

		if( password_state==Matched)
		{
			g_matching_flag=1;
			for(i=0;i<5;i++)
			{
				EEPROM_writeByte(0x01+i, password_1[i]);
				_delay_ms(10);
			}
		}






	}while(g_matching_flag==0);





}




/*
 * Description :

 *  1.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked for 3 times if matched
 *   to the saved one in in EEPROM  and send the password state to the HMI_MUC
 *
 *  2.in case the password is matched it will break the loop
 *   after opening the door and closing it back
 *
 * 	3. in case the password is not matched for 3 times the BUZZER will be ON  for 60s
 */

void check_door_pass(void)
{

	do{
		password_state=Matched; // to be sure that the flag is matched as if it changed so the pass is not identical

		UART_sendByte(CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			password_1[i]=UART_recieveByte();/*I  will use the same two array to minimize the num of variables*/
		}

		for(i=0;i<5;i++)
		{
			EEPROM_readByte(0x01+i, password_2+i);//the name of the array  is an address the first postion
			_delay_ms(10);
		}


		for(i=0;i<5;i++){
			if(password_1[i]!=password_2[i])
			{

				password_state=Unmatched;
				break;

			}

		}

		while(UART_recieveByte()!=HMI_Ready);
		UART_sendByte(password_state);

		if(password_state==Matched)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 4, "Matched");
			_delay_ms(500);


			g_ticks=0;
			DcMotor_Rotate(CW,100);
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=15);

			g_ticks=0;
			DcMotor_Rotate(STOP,0);
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=3);

			g_ticks=0;
			DcMotor_Rotate(ANTI_CW,100);
			Timer1_setCallBack(TIMER_ISR_COUNTER);
			while(g_ticks!=15);
			DcMotor_Rotate(STOP,0);
			Timer1_deInit();
			break;

		}

		else{

			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 4, "UNMatched");
			_delay_ms(500);

			g_trials++;

		}





	}while(g_trials<3);

	if(g_trials==3){
		g_ticks=0;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 2, "!!!!ERROR!!!!");
		Buzzer_on();
		Timer1_setCallBack(TIMER_ISR_COUNTER);
		while(g_ticks!=60);
		Timer1_deInit();
		Buzzer_off();
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
 *
 *  2.enter a do{}while(g_trials<3);loop to make sure that
 *  the entered password will be checked (by the CONTROL_MUC) for 3 times
 *  if matched to the saved one in in EEPROM
 *  the CONTROL_MUC will send the password state to the HMI-MUC if matched or not
 *
 *  3.if matched the HMI_MUC will call the function  passwor_setting(); and break the loop
 *
 * 	4. if not matched for 3 times the BUZZER will be ON  for 60s
 *
 */


void Change_password(void)
{
	do{

		password_state=Matched; // to be sure that the flag is matched as if it changed so the pass is not identical

		UART_sendByte(CONTROL_Ready);
		for(i=0;i<5;i++)
		{

			password_1[i]=UART_recieveByte();/*I  will use the same two array to minimize the num of variables*/
		}

		for(i=0;i<5;i++)
		{
			EEPROM_readByte(0x01+i, password_2+i);//the name of the array is an address the first postion
			_delay_ms(50);
		}


		for(i=0;i<5;i++){
			if(password_1[i]!=password_2[i])
			{
				//g_matching_flag=0;
				password_state=Unmatched;
				break;

			}

			LCD_clearScreen();
			for(i=0;i<5;i++)
			{LCD_intgerToString(password_2[i]);

			}

		}

		while(UART_recieveByte()!=HMI_Ready);
		UART_sendByte(password_state);

		if(password_state==Matched)
		{

			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 4, "Matched");
			_delay_ms(500);

			recive_password();
			break;

		}
		else
		{


		}

		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 4, "UNMatched");
		_delay_ms(500);
		g_trials++;

	}
	while(g_trials<3);

	if(g_trials==3){
		g_ticks=0;
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 2, "!!!!ERROR!!!!");
		Buzzer_on();
		Timer1_setCallBack(TIMER_ISR_COUNTER);
		while(g_ticks!=60);
		Timer1_deInit();
		Buzzer_off();
		g_trials=0;

	}


}



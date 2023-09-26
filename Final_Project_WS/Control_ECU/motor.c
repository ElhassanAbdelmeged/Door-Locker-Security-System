#include<avr/io.h>
#include"PWM.h"
#include"motor.h"

void DcMotor_Init(void){

	SET_BIT(DDRB,PB0);
	SET_BIT(DDRB,PB1);

	/* Motor is stop at the beginning */
	PORTB &= 0xFC;


}
 void DcMotor_Rotate(DcMotor_State state,uint8 speed)
 {

	 if(state==CW){
		 PORTB &=0XFC ;
		 PORTB|=(1<<PB0);
	 }

	 else if(state==ANTI_CW){
		 PORTB &=0XFC ;
		 PORTB|=(1<<PB1);
	 }

	 else if(state==STOP){
		 PORTB &=0XFC ;
	 }

	 PWM_Timer0_Start(speed);

 }

#include <avr/io.h> /* For the REGSTERS */
#include <avr/interrupt.h>
#include "common_macros.h"
#include "timer.h"


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
/*FOR COMPARE MODE*/
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
	}
}


/*FOR OVERFLOW MODE*/
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)();
	}
}







void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{

	TCNT1 = Config_Ptr->initial_value;
	OCR1A = Config_Ptr->compare_value;
	TCCR1A |= (1<<FOC1A)|(1<<FOC1B); //for NON_PWM mode
	/*
	 * insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0XF8) |(Config_Ptr->prescaler);
	TCCR1B = (TCCR1B& 0XF7)|(Config_Ptr->mode<<WGM12);
	TIMSK |= (1<<OCIE1A) | (1<< TOIE1); ////OCIE1A interrupt enable for a compare Maching.TOIE  OVERFLOW interrupt enable
	sei(); //SET I-BIT
}

void Timer1_deInit(void)
{

	TCNT1 = 0;
	OCR1A = 0;
	TCCR1A =0;
	TCCR1B=0;
	TIMSK =0;
	cli(); //Clear I-BIT


}

void Timer1_setCallBack(void(*a_ptr)(void))
{

	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;

}

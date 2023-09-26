#include "common_macros.h"
#include "gpio.h"
#include "buzzer.h"
#include<util/delay.h>
void Buzzer_init(void)
{

	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_LOW);

}

void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_HIGH);
}

void Buzzer_off(void)
{

	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_LOW);
}
void Buzzer_toggle(void)
{

	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_HIGH);
	_delay_ms(100);

	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_LOW);

}

/*
 * buzzer.h
 *
 *  Created on: Nov 6, 2022
 *      Author: dell
 */

#ifndef BUZZER_H_
#define BUZZER_H_
#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT  PORTB_ID
#define BUZZER_PIN   PIN6_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/



/*Description
 * Setup the direction for the buzzer pin as output pin through the GPIO driver
 */
void Buzzer_init();
/*Description
 *  Function to enable the Buzzer through the GPIO.
 */

void Buzzer_on(void);

/*Description
 * Function to disable the Buzzer through the GPIO.
 */


void Buzzer_off(void);
/*test*/

void Buzzer_toggle(void);

#endif /* BUZZER_H_ */

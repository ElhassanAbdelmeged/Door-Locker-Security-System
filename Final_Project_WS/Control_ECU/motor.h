/*
 * motor.h
 *
 *  Created on: Oct 9, 2022
 *      Author: dell
 */

#ifndef MOTOR_H_
#define MOTOR_H_


#include "std_types.h"
#include "common_macros.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
typedef enum{CW,ANTI_CW,STOP}DcMotor_State;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void DcMotor_Init(void);

void DcMotor_Rotate(DcMotor_State state,uint8 speed);


#endif /* MOTOR_H_ */

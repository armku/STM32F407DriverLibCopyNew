#ifndef _INPUT_CAPTURE_EXIT_H_
#define _INPUT_CAPTURE_EXIT_H_

#include "stm32f4xx.h"

class InputCapture_EXIT
{
protected:
	GPIO_TypeDef *mExitPort;   //GPIO Port of exit pin
	u8 EXTI_PortSource;
	u8 EXTI_PinSource;
	u16 mExitPin;              //GPIO pin for exit
	double mRiseTime;          //rising edge time
	double mFallTime;          //falling edge time
	double mInterval;          //time between rising edge and falling edge
public:
	InputCapture_EXIT(GPIO_TypeDef *exitPort, u16 exitPin, u8 itGroup = 3, u8 prePriority = 5, u8 subPriority = 0);
	void IRQ();
	double Value();
};





#endif

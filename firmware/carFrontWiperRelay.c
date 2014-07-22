/*
 Lifan Smily (320) car front wiper control module
 
 MCU pic12F683, hardware PWM
 Hi-Tech C STD 9.60PL3
 v0.1
 
 Yuriy Gorbachev, 2014
 urvindt@gmail.com
*/

//----------------------------------------------------------------------------//

#include <htc.h>
#include "vartypes.h"

//----------------------------------------------------------------------------//
// CONFIGURATION
//----------------------------------------------------------------------------//

/* Program device configuration word
 * Oscillator = Internal RC No Clock
 * Watchdog Timer = Off
 * Power Up Timer = Off
 * Master Clear Enable = Internal
 * Code Protect = Off
 * Data EE Read Protect = Off
 * Brown Out Detect = BOD and SBOREN disabled
 * Internal External Switch Over Mode = Disabled
 * Monitor Clock Fail-safe = Disabled
 */
__CONFIG(INTIO & WDTDIS & PWRTDIS & MCLRDIS & UNPROTECT & UNPROTECT & BORDIS & IESODIS & FCMDIS);

//----------------------------------------------------------------------------//

#define PIN_MOTOR_OUT    GPIO0
#define PIN_WATER        GPIO1
#define PIN_INTERMITTENT GPIO2

#define MOTOR_ON  1
#define MOTOR_OFF 0
#define WATER_ON  0
#define WATER_OFF 1
#define INTERMITTENT_ON  0
#define INTERMITTENT_OFF 1

#define WIPER_MODE_WAIT 0
#define WIPER_MODE_WORK 1
#define WIPER_TIMER_INFINITY 65534

//----------------------------------------------------------------------------//

// Comment this to disable adjustable intermittent functionality
#define ADJUSTABLE_INTERMITTENT_DELAY_ON


// All times in ticks, ~30 ticks per second

#define SWITCH_DEBOUNCE_TICKS 3

#define WATER_WIPER_WAIT_TIME 15
#define WATER_WIPER_WORK_TIME_BIG 240
#define WATER_WIPER_WORK_TIME_SMALL 30
#define WATER_WIPER_SWITCH_MODE_TIME 30

#define INTERMITTENT_WIPER_WORK_TIME 30
#define INTERMITTENT_WIPER_WAIT_TIME 240	      /* default, 8 seconds */
#define INTERMITTENT_WIPER_WAIT_TIME_MIN 150  /* 5 seconds */
#define INTERMITTENT_WIPER_WAIT_TIME_MAX 3600	/* 2 minutes */

//----------------------------------------------------------------------------//

volatile bit fWaterModeOn;
volatile bit fWiperMode;

volatile uint8 fSwitchIntermittentDebouncer;
volatile uint8 fSwitchWaterDebouncer;
volatile bit fSwitchIntermittentState;
volatile bit fSwitchWaterState;

volatile uint16 fWiperTimer;

volatile uint16 fWiperCurrentWaitTime;
volatile uint16 fWiperCurrentWorkTime;

volatile uint8 fWaterSwitchTimer;

volatile uint16 fWaitTimer;
volatile uint16 fIntermittentWaitTime;

//----------------------------------------------------------------------------//

void initSoftware(void)
{
	PIN_MOTOR_OUT = MOTOR_OFF;
	
	fWaterModeOn = 0;
	
	fSwitchIntermittentDebouncer = 0;
	fSwitchWaterDebouncer = 0;
	fSwitchIntermittentState = INTERMITTENT_OFF;
	fSwitchWaterState = WATER_OFF;

	fWiperTimer = 0;	
	fWiperMode = WIPER_MODE_WAIT;
	
	fWaitTimer = WIPER_TIMER_INFINITY;
	fIntermittentWaitTime = INTERMITTENT_WIPER_WAIT_TIME;
}

void initHardware(void)
{
	// IO Port direction (1 - input)
	TRISIO = 0b00111110;
	GPIO   = 0b00000000;
	
	// Disable watch dog
	WDTCON = 0;
	
	//Internal oscillator set to 8MHz
	OSCCON = 0b01110000;
	
	// Prescale to 1:256 (30 owerflows per second)
	OPTION = 0b10000111;
	//         ||||||||- Prescaler rate
	//         |||||---- Prescaler assighnment (0 - Timer0, 1 - WDT)
	//         ||||----- Timer0 edge (0 - low to high)
	//         |||------ Timer0 source (0 - internal)
	//         ||------- Interrupt edge (0 - falling)
	//         |-------- GPIO Pull up resistors (1 to disable)
	
	// Disable comparators and PWM
	CMCON1 = 0x07;
	CMCON0 = 0x07;	
	CCP1CON = 0x00;
	
	// ADC Configuration 
	ADCON0 = 0x00;
	ANSEL = 0x00;
	
	// Interrupts
	INTCON = 0b01100000;	
	//         |||||- GPIO change
	//         ||||-- GP2/INT
	//         |||--- Timer0
	//         ||---- Peripherial interrupts
	//         |----- Global intterupt (off while initialization)
	
	// Peripherial interrupts
	PIE1 = 0x00;
}

//----------------------------------------------------------------------------//

void setupWaterTimes(void)
{
	fWiperCurrentWaitTime = WATER_WIPER_WAIT_TIME;
	fWiperCurrentWorkTime = WATER_WIPER_WORK_TIME_SMALL;
}

void setupWaterTimeBig(void)
{
	fWiperCurrentWorkTime = WATER_WIPER_WORK_TIME_BIG;
}

void setupIntermittentTimes(void)
{
	#ifdef ADJUSTABLE_INTERMITTENT_DELAY_ON
		fWiperCurrentWaitTime = fIntermittentWaitTime;
	#else
		fWiperCurrentWaitTime = INTERMITTENT_WIPER_WAIT_TIME;
	#endif
	fWiperCurrentWorkTime = INTERMITTENT_WIPER_WORK_TIME;
}

//----------------------------------------------------------------------------//

void nullWaitTimer(void)
{
	fWaitTimer = 0;
}

void processWaitTimer(void)
{
	if(fWiperMode == WIPER_MODE_WAIT)
	{
		if(fWaitTimer < WIPER_TIMER_INFINITY)
			fWaitTimer++;
	}
}

//----------------------------------------------------------------------------//

void processWiper(void)
{
	if(fWiperTimer != WIPER_TIMER_INFINITY)
	{
		
		if(fWiperMode == WIPER_MODE_WAIT)
		{
			if(fWiperTimer >= fWiperCurrentWaitTime)
			{
				fWiperMode = WIPER_MODE_WORK;
				fWiperTimer = 0;
			}
		}
		
		else
		{
			if(fWiperTimer >= fWiperCurrentWaitTime)
			{
				if(fWaterModeOn == 1)
					fWaterModeOn = 0;
				
				if(fSwitchIntermittentState == INTERMITTENT_ON)
				{
					fWiperTimer = 0;
					setupIntermittentTimes();
				}
				else
				{
					fWiperTimer = WIPER_TIMER_INFINITY;
				}
				
				fWiperMode = WIPER_MODE_WAIT;
				nullWaitTimer();
			}
		}
		fWiperTimer++;
	}
	
}
//----------------------------------------------------------------------------//

void processWaterTimer(void)
{
	if(fSwitchWaterState == WATER_ON)
		if(fWaterSwitchTimer < 255)
			fWaterSwitchTimer++;
}

void onWaterSwitchOn(void)
{
	if(fWaterModeOn == 0)
	{
		fWaterModeOn = 1;
		fWiperMode = WIPER_MODE_WAIT;
		fWiperTimer = 0;
	
		fWaterSwitchTimer = 0;
	}
	else
	{
		setupWaterTimeBig();
	}
	
	setupWaterTimes();
}

void onWaterSwitchOff(void)
{
	if(fWaterSwitchTimer > WATER_WIPER_SWITCH_MODE_TIME)
		setupWaterTimeBig();
}

//----------------------------------------------------------------------------//

void onIntermittentSwitchOn(void)
{
	if(fWaitTimer < INTERMITTENT_WIPER_WAIT_TIME_MIN || fWaitTimer > INTERMITTENT_WIPER_WAIT_TIME_MAX)
		fIntermittentWaitTime = INTERMITTENT_WIPER_WAIT_TIME;
	else
		fIntermittentWaitTime = fWaitTimer;
			
	
	if(fWaterModeOn == 0)
	{
		fWiperMode = WIPER_MODE_WORK;
		fWiperTimer = 0;
		
		setupIntermittentTimes();
	}
}

void onIntermittentSwitchOff(void)
{
}

//----------------------------------------------------------------------------//

void processSwitchIntermittent(void)
{
	if(fSwitchIntermittentDebouncer == 0)
	{
		if(PIN_INTERMITTENT != fSwitchIntermittentState)
			fSwitchIntermittentDebouncer = SWITCH_DEBOUNCE_TICKS;
	}
	else
	{
		if(fSwitchIntermittentDebouncer == 1 && PIN_INTERMITTENT != fSwitchIntermittentState)
		{
			fSwitchIntermittentState = PIN_INTERMITTENT;
			
			if(fSwitchIntermittentState == INTERMITTENT_ON)
				onIntermittentSwitchOn();
			else
				onIntermittentSwitchOff();
		}
		
		fSwitchIntermittentDebouncer--;
	}
}


void processSwitchWater(void)
{
	if(fSwitchWaterDebouncer == 0)
	{
		if(PIN_WATER != fSwitchWaterState)
			fSwitchWaterDebouncer = SWITCH_DEBOUNCE_TICKS;
	}
	else
	{
		if(fSwitchWaterDebouncer == 1 && PIN_WATER != fSwitchWaterState)
		{
			fSwitchWaterState = PIN_WATER;
			
			if(fSwitchWaterState == WATER_ON)
				onWaterSwitchOn();
			else
				onWaterSwitchOff();
		}
		
		fSwitchWaterDebouncer--;
	}
}

//----------------------------------------------------------------------------//

void interrupt isr(void)
{
	// Timer0
	if((T0IE) && (T0IF))
	{
		processSwitchIntermittent();
		processSwitchWater();
		processWaterTimer();
		processWiper();
		
		#ifdef ADJUSTABLE_INTERMITTENT_DELAY_ON
			processWaitTimer();
		#endif
		
		T0IF = 0;
	}
}

//----------------------------------------------------------------------------//

void main(void)
{
	initHardware();
	initSoftware();
	ei();	
		
	while (1)
	{
		if(fWiperMode == WIPER_MODE_WORK)
			PIN_MOTOR_OUT = MOTOR_ON;
		else
			PIN_MOTOR_OUT = MOTOR_OFF;
	}
}
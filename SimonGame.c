//---------------------------------------------------------------//
//	SIMON GAME													 //
//	Designed and built to operate on the TI MSP430FG4618 device, //
//  with the use of external LEDs and push buttons.				 //
//  External I/O is expected to be connected to pin access H8	 //
//  on the MSP430 device.										 //
//	Authors: Robert Knapp and Joe Goldbach						 //
//---------------------------------------------------------------//

#include <msp430.h>
#include <SimonGame.h>

#include <stdio.h>

uint8_t simonSequence[SEQUENCE_MAX] = { 0 };
uint8_t sequenceLength = 0;

volatile bool gameOver = false;
volatile uint8_t gameOverLED = 0;
volatile static uint16_t tenthSecondCtr = 0;

volatile uint8_t highScore = 0;
// Joe's high score
//volatile uint8_t highScore = 16;

/*
* The main game loop;
* this will loop infinitely, and pass game control out to specially-designed functions
*/
void main(void) {
	// watchdog timer initialization
	WDTCTL = WDTPW +WDTHOLD;
	// This will set 4 pins (the odd pins, even bits) in Port 6 to output for LEDs
	P6DIR |= LED_0 + LED_1 + LED_2 + LED_3;
	// This will set 3 pins (the even pins, odd bits) in Port 6 to inputs;
    // Please ensure the push buttons are connected to those pins.
	P6DIR &= ~0xA2;
	// And set a pin on Port 7 to output for an LED;
	// There are some onboard connections that make it difficult to tie everything to pins on Port 6.
    P7DIR &= ~0x10;
	
	// timer A initialization
	// upper limit of count for TAR
	// ACLK runs at 32kHz, so a counter value set to 32767 ~ 1s;
	// 3277 is used for ~.1s delay, or about 100ms.
	TACCR0 = 3277;
	// Enable interrupts on Compare 0
	TACCTL0 = CCIE;
	
	// set up and start Timer A
	TACTL = MC_1 | ID_0 | TASSEL_1 | TACLR;
	// Up to CCR0 mode, input divider /1, clock from ACLK, clear timer

	if (DEBUG_MODE)
		DEBUG_functions();
	
	// Initiate GameStart routine to wait for player ready
	GameStart();
	
	// main game loop
	for(;;) {
		CPURound();
		
		PlayerRound();
		
		if (gameOver == true) {
			playGameOverBuzzer();
			
			// Initiate GameStart routine to wait for player ready
			GameStart();
		}
	}
	
}

/*
* The computer picks a new element/LED at random, and adds it to the sequence.
* Then, it plays back the whole sequence, with the new element, so the player may see.
*/
void CPURound(void) {
	// add new element to sequence
	// there will be four valid values, so the value is selected out of 4.
	simonSequence[sequenceLength] = rand() % 4;
	
	uint8_t i;
	// play through entire sequence
	for (i = 0; i <= sequenceLength; i++) {
		// delay for short time between LED pulses
		delay(FIFTH_SECOND);
		
		lightLED(simonSequence[i], HALF_SECOND);
	}
}

/*
* This function handles the player's turn.
* It awaits player input, then verifies whether or not the input is correct.
* If the player's input is correct, it will then wait for the next input, if there is one.
* Otherwise, the function ends, so the computer's turn may start.
*
* If the player's input is incorrect, the game over flag is set, and the player's turn ends.
*/
void PlayerRound(void) {
	// initialize player turn variables
	uint8_t sqcIter = 0;
	
	// main player round loop;
	// exits when whole sequence has been input by player,
	// or when the player inputs an incorrect value.
	do {
		uint8_t buttonPressed = getButtonPress(true);
		
		// we don't care if the player has the correct input, we just move on to the next iteration.
		// if the player presses the wrong input, we set the game over flag to be true.
		// we also set the game over LED to what the correct LED would have been.
		if (simonSequence[sqcIter] != buttonPressed) {
			gameOver = true;
			gameOverLED = simonSequence[sqcIter];
			
			// TODO: for debugging, comparing incorrect player input to correct sequence element
			printf("Player pressed: %d\nCorrect answer: %d\n", buttonPressed, gameOverLED);
		}
		
		sqcIter++;
	} while (gameOver == false && sqcIter < (sequenceLength + 1));
	
	// increment sequence counter;
	// the reason we're doing this at the end of the player round
	// is because this will double as the player score, which 
	// should not increment if the player inputs an incorrect value.
	if (gameOver == false) {
        sequenceLength++;
    } else {
        if (sequenceLength > highScore)
            highScore = sequenceLength;
        printf("Your score is: %d\nThe all-time high score is: %d\n", sequenceLength, highScore);
    }
}

/*
* Wait for a player to indicate they would like to start playing a new game
*/
void GameStart(void) {
	// initialize game variables
	gameOver = false;
	// After the score is displayed to the user,
	// then we'll want to reset the sequence counter
	// for a new game.
	sequenceLength = 0;
	
	// wait for player to press any button,
	// indicating they wish to start a new game
	
	/*
	* For now, we light orange LED to indicate the board is ready.
	* When the player presses corner button on board, green LED will flash,
	* and then the game will begin after a little light show.
	*/
	P2DIR |= (BIT1+BIT2);
	P2OUT |= BIT1;
	getButtonPress(false);
	P2OUT &= ~BIT1;
	P2OUT |= BIT2;
	delay(TENTH_SECOND);
	P2OUT &= ~BIT2;
	
	playGameStartLightPattern();
}

/*
* using odd pins of pin access H8 on MSP430 board for LEDs
*/
void lightLED(uint8_t LED_ID, uint8_t duration) {
	uint8_t LEDPort = 0x0;
	
	switch (LED_ID) {
		case 0 :
			// setup LED0
			LEDPort = LED_0;
			break;
		case 1 :
			// setup LED1
			LEDPort = LED_1;
			break;
		case 2 :
			// setup LED2
			LEDPort = LED_2;
			break;
		case 3 :
			// setup LED3
			LEDPort = LED_3;
			break;
		case 4 :
			// setup LED0 and LED1
			LEDPort = LED_0 + LED_1;
			break;
		case 5 :
			// setup LED0 and LED2
			LEDPort = LED_0 + LED_2;
			break;
		case 6 :
			// setup LED0 and LED3
			LEDPort = LED_0 + LED_3;
			break;
		case 7 :
			// setup LED1 and LED2
			LEDPort = LED_1 + LED_2;
			break;
		case 8 :
			// setup LED1 and LED3
			LEDPort = LED_1 + LED_3;
			break;
		case 9 :
			// setup LED2 and LED3
			LEDPort = LED_2 + LED_3;
			break;
		case 10 :
			// setup LED0 and LED1 and LED2
			LEDPort = LED_0 + LED_1 + LED_2;
			break;
		case 11 :
			// setup LED0 and LED1 and LED3
			LEDPort = LED_0 + LED_1 + LED_3;
			break;
		case 12 :
			// setup LED0 and LED2 and LED3
			LEDPort = LED_0 + LED_2 + LED_3;
			break;
		case 13 :
			// setup LED1 and LED2 and LED3
			LEDPort = LED_1 + LED_2 + LED_3;
			break;
		case 14 :
			// setup all four LEDs
			LEDPort = LED_0 + LED_1 + LED_2 + LED_3;
			break;
		default :
			// This code should never run
			printf("Fatal error occurred in lighting LED.\n");
			// lighting red LED semi-permanently to indicate fault
			P5DIR |= BIT1;
			P5OUT |= BIT1;
	}
	
	// Turn on designated LED
    P6OUT |= LEDPort;
	
	// call delay to hold LED
	delay(duration);
	
	// Turn off designated LED
    P6OUT &= ~LEDPort;

}

/*
* lightMode - flag enable matching LED on button press
* Retrieves a button press input, and converts to usable value
* buttons should be using even pins of pin access H8 on MSP430 board
*/
uint8_t getButtonPress(bool lightMode) {
	// in the case of invalid input value
	//return a junk value.  Game logic will handle as game over.
	uint8_t buttonValue = 255;
	bool buttonDetection = false;
	
	/* this loop will hold the program until a button press is detected
	* from one of the external push buttons, connected to Port 6.
	* when a button is pressed, the buttonValue is set to that button,
	* the associated LED is flashed, and the loop ends,
	* when the button is no longer being held.
	*/
	do {
		if ((PLAY_BUTTON_0) == 0) {
			buttonValue = 0;
			buttonDetection = true;
			while ((PLAY_BUTTON_0) == 0);
		}
		if ((PLAY_BUTTON_1) == 0) {
			buttonValue = 1;
			buttonDetection = true;
			while ((PLAY_BUTTON_1) == 0);
		}
		if ((PLAY_BUTTON_2) == 0) {
			buttonValue = 2;
			buttonDetection = true;
			while ((PLAY_BUTTON_2) == 0);
		}
		if ((PLAY_BUTTON_3) == 0) {
			buttonValue = 3;
			buttonDetection = true;
			while ((PLAY_BUTTON_3) == 0);
		}
		
		if (buttonDetection && lightMode) {
			lightLED(buttonValue, FIFTH_SECOND);
		}
	} while (buttonDetection == false);
	
	// this loop waits for the button to be lifted before proceeding; debounce
	do {
        if ((PLAY_BUTTON_0) && (PLAY_BUTTON_1) && (PLAY_BUTTON_2) && (PLAY_BUTTON_3))
            buttonDetection = false;
    } while (buttonDetection);
	
	return buttonValue;
}

/*
* Played upon game over; incorrect player input
* This could be cleaner, but it works, and in the interest of time, I'll leave it.
* Ideally, it would use established delay function, as well as Pulse Width Modulation to control the buzzer tone.
* There was not time enough to implement that.
*/
void playGameOverBuzzer(void) {
	// setting buzzer port to output
	P3DIR |= BIT5;
	P3OUT |= BIT5;
	
	// Flash red LED on board to indicate game over
	P5DIR |= BIT1;
	P5OUT |= BIT1;
	
	// select what the correct LED would have been, so the player knows which it was.
	uint8_t LEDPort = 0x0;
	switch (gameOverLED) {
		case 0 :
			// setup LED0
			LEDPort = LED_0;
			break;
		case 1 :
			// setup LED1
			LEDPort = LED_1;
			break;
		case 2 :
			// setup LED2
			LEDPort = LED_2;
			break;
		case 3 :
			// setup LED3
			LEDPort = LED_3;
			break;
		default :
			// This code should never run
			printf("Fatal error occurred in lighting game over LED.\n");
	}
	
	// Turn on designated LED
    P6OUT |= LEDPort;
	
	int duty_cycle_high = 0;
	int duty_cycle_low = 0;
	uint16_t total_delay = (FIFTH_SECOND * 1200 - 1);
	uint16_t delay_ctr = 0;
	for (delay_ctr = 0; delay_ctr < total_delay; delay_ctr++) {
	    P3OUT |= BIT5;
        for (duty_cycle_high = 0; duty_cycle_high < 10; duty_cycle_high++);
        P3OUT &= ~BIT5;
        for (duty_cycle_low = 0; duty_cycle_low < 90; duty_cycle_low++);
	}
	
	// Turn off designated LED
    P6OUT &= ~LEDPort;
	
	// turn off buzzer and red LED
	P3OUT &= ~BIT5;
	P5OUT &= ~BIT1;
	
	// blink all the LEDs
	lightLED(14, TENTH_SECOND);
}

/*
* This function exists partially for visual flair.
* However, it is also practical, as it will give the player a clear indication
* that the game is about to begin.
*/
void playGameStartLightPattern(void) {
	// initializing pattern variables
	bool incr = true;
	uint8_t lightValue = 0;
	uint8_t i = 0;
	uint8_t patternLength = 4;
	
	// adding phase zero after the fact
	for (i = 0; i < patternLength; i++) {
		//delay(TENTH_SECOND);
		switch (i) {
			case 0 :
				lightLED(0, FIFTH_SECOND);
				break;
			case 1 :
				lightLED(4, FIFTH_SECOND);
				break;
			case 2 :
				lightLED(10, FIFTH_SECOND);
				break;
			case 3 :
				lightLED(14, FIFTH_SECOND);
				break;
			default :
				// This code should never run
				printf("Error in phase zero for loop of game start light sequence.\n");
		}
	}
	
	delay(FIFTH_SECOND);
	
	patternLength = 13;
	
	// let the light show, commence!
	// NightRider mode!
	for (i = 0; i < patternLength; i++) {
		lightLED(lightValue, FIFTH_SECOND);
		if (incr) {
			lightValue++;
			if (lightValue == 3)
				incr = false;
		} else {
			lightValue--;
			if (lightValue == 0)
				incr = true;
		}
	}
	
	// a short delay between phase one and phase two
	delay(FIFTH_SECOND);
	
	// light show: phase two: alternate blinking
	patternLength = 4;
	for (i = 0; i < patternLength; i++) {
		if (i%2 == 0) {
			lightLED(5, FIFTH_SECOND);
		} else {
			lightLED(8, FIFTH_SECOND);
		}
	}
	
	// more alternate blinking
	for (i = 0; i < patternLength; i++) {
		if (i%2 == 0) {
			lightLED(4, FIFTH_SECOND);
		} else {
			lightLED(9, FIFTH_SECOND);
		}
	}
	
	// for the final phase, blink all the LEDs
	patternLength = 2;
	for (i = 0; i < patternLength; i++) {
		delay(TENTH_SECOND);
		lightLED(14, FIFTH_SECOND);
	}
	
	delay(ONE_AND_HALF_SECOND);
}

/*
* duration - length of delay in units of 100ms
*/
void delay(uint8_t duration) {
	// Enable global interrupts
	__enable_interrupt();
	
	// Reset the counter
	tenthSecondCtr = 0;
	while (tenthSecondCtr < duration) {
		// may need to reduce optimization during compilation, or this loop might be optimized out?
		// This loop is necessary to maintain a delay as close to the desired amount of time as possible.
	}
	
	// Disable global interrupts
	__disable_interrupt();
}

//---------------------------------------------------------------//
// Interrupt service routine for Timer A channel 0				 //
// Processor remains in Active Mode after ISR					 //
//---------------------------------------------------------------//
#pragma vector = TIMERA0_VECTOR
__interrupt void TA0_ISR (void) {
	tenthSecondCtr++; // Increment counter to keep track of 
	if (tenthSecondCtr >= 65535)
		tenthSecondCtr = 0; // To prevent overflow
}

void displayScore(void) {
	// TODO: may not be implemented, depending on time constraints
}

void playLEDTone(void) {
	// TODO: may not be implemented, depending on time constraints
}

/*
* This function will use the corner button on the MSP430 board to cycle through test scenarios.
* If the program is set to debug mode via the header file, this code will run after initialization,
* but before the game officially begins.
*/
void DEBUG_functions(void) {
	// for testing purposes
	
	// INDICATION: turn red LED on MSP430 board on while debugging
	P5DIR |= BIT1;
	P5OUT |= BIT1;
	
	// Turn the game LEDs off for testing
	P6OUT &= ~0x55;

	// set switch to input
	P1DIR &= ~BIT1;
	bool debugButtonPress = false;
	
	// TEST: infinite NightRider mode, lol
	// initializing pattern variables
	bool incr = true;
	uint8_t lightValue = 0;
	
	// let the light show, commence!
	// NightRider mode!
	for(;;) {
		lightLED(lightValue, FIFTH_SECOND);
		if (incr) {
			lightValue++;
			if (lightValue == 3)
				incr = false;
		} else {
			lightValue--;
			if (lightValue == 0)
				incr = true;
		}
	}
	
	// for testing solder outputs
	printf("Entering solder test loop.\n");
	do {
	    P6DIR |= 0xFF;
	    P6OUT |= 0xFF;
	    if (P1IN == BIT1)
	        debugButtonPress = true;
	} while (debugButtonPress == false);
	printf("Leaving solder test loop.\n");

	printf("Release corner button.\n");
    while (P1IN == BIT1);

    // Turn the game LEDs off for testing
    P6OUT &= ~0x55;

	debugButtonPress = false;

	// Wait for user to confirm test execution
	printf("Waiting for user confirmation to begin test execution.\nHold the corner button down for about 2 seconds. Release when you see the green light.\n");
	// preparing port 2 for the green LED
	P2DIR |= BIT2;
	P2OUT &= ~BIT2;
	do {
		if (P1IN == BIT1) {
			debugButtonPress = true;
			// turn on green LED
            P2OUT |= BIT2;
            delay(FIFTH_SECOND);
		}
	} while (debugButtonPress == false);
	// turn off green LED
	P2OUT &= ~BIT2;
	
	printf("Release corner button.\n");
    while (P1IN == BIT1);

	debugButtonPress = false;
	
	// TEST: input button 3 on Port 6.7
	/*printf("Begin test for orange button.\n");
	P6DIR &= ~0xA0;
	do {
		int buttonValue = 255;
		bool buttonDetection = false;
		// gotta figure out why button 3 isn't working;
		// don't use board #2.
		do {
		    if ((PLAY_BUTTON_2) == 0) {
                buttonValue = 2;
                buttonDetection = true;
                lightLED(2, HALF_SECOND);
                printf("Red button pressed.\n");
                while (((PLAY_BUTTON_2) == 0) && (P1IN != BIT1));
            }
			if ((PLAY_BUTTON_3) == 0) {
				buttonValue = 3;
				buttonDetection = true;
				lightLED(3, HALF_SECOND);
				// program hangs here; reading 1.8V on the switch.
				// something is pulling this switch to ground;
				// not likely a software error, more likely a hardware issue.
				printf("Beginning problem loop now.  Use corner button to escape.\n");
				while (((PLAY_BUTTON_3) == 0) && (P1IN != BIT1));
			}
		} while ((buttonDetection == false) && (P1IN != BIT1));
	
		printf("Button value is: %d\n", buttonValue);
	
	    if (P1IN == BIT1)
	        debugButtonPress = true;
	} while (debugButtonPress == false);

	printf("Release corner button.\n");
	while (P1IN == BIT1);*/

	debugButtonPress = false;
	
	// TEST: buzzer
	P3DIR |= BIT5;
	P3OUT |= BIT5;
	printf("Can you hear the buzzer?\n");
	do {
	    int i = 0;
	    int j = 0;
	    P3OUT |= BIT5;
	    for (i = 0; i < 10; i++);
	    P3OUT &= ~BIT5;
	    for (j = 0; j < 90; j++);
		if (P1IN == BIT1)
			debugButtonPress = true;
	} while (debugButtonPress == false);
	// turn off buzzer when done
	P3OUT &= ~BIT5;
	
	printf("Release corner button.\n");
    while (P1IN == BIT1);

	debugButtonPress = false;
	
	// TEST: random number generation
	uint8_t numberOfIterations = 20;
	printf("Creating sequence of %d elements.\n", numberOfIterations);
	// create a test sequence of values
	// there will be four valid values, so the value is selected out of 4.
	sequenceLength = 0;
	uint8_t j = 0;
	for (j = 0; j < numberOfIterations; j++) {
		simonSequence[sequenceLength] = rand() % 4;
	
		//for debugging the sequence
		printf("%d\n", simonSequence[sequenceLength]);
			
		// sequenceLength will be artifically modified for this test;
		// this is fine, since it is reset upon starting a new game, anyway.
		sequenceLength++;
	}
	
	printf("Sequence created.  Beginning LED sequence playback.\n");
		
	uint8_t i;
	// play through entire sequence
	// for statement modified for test case; '<=' changed to '<', to avoid off-by-one error.
	for (i = 0; i < sequenceLength; i++) {
		// delay for short time between LED pulses
		delay(TENTH_SECOND);
		
		lightLED(simonSequence[i], HALF_SECOND);
	}
	// end test case
	
	printf("Release corner button.\n");
    while (P1IN == BIT1);

	debugButtonPress = false;
	
	// TEST: delay function
	printf("Begin delay testing.\n");
	do {
	    P6DIR |= 0xFF;
	    P6OUT |= 0xFF;
		delay(ONE_SECOND);
	    if (P1IN == BIT1)
	        debugButtonPress = true;
		P6OUT &= ~0xFF;
		delay(ONE_SECOND);
		if (P1IN == BIT1)
			debugButtonPress = true;
	} while (debugButtonPress == false);
	printf("Leaving delay test loop.\n");

	printf("Release corner button.\n");
    while (P1IN == BIT1);

    // Turn the game LEDs off for testing
    P6OUT &= ~0x55;

	debugButtonPress = false;
	
	// INDICATION: turn red LED off when done
	P5OUT &= ~BIT1;
}

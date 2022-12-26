#ifndef SIMON_GAME_H
#define SIMON_GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG_MODE 0

/*
* The sequence will also use 100 bytes in memory;
* The reason for this is that the current world record holder for the Simon game (as of 2022/11/04)
* has a world record of 84 (achieved 2020/11/28), so we shouldn't need more slots than that in the immediate future.
*/
#define SEQUENCE_MAX 100

/*
* Defining time constants for use with the delay function.
* These constants represent multiples of 100ms, or one-tenth of a second.
*/
#define TWO_SECOND 20
#define ONE_AND_HALF_SECOND 15
#define ONE_SECOND 10
#define HALF_SECOND 5
#define FIFTH_SECOND 2
#define TENTH_SECOND 1

//----------------------------------------------------------------------//
// BUTTON AND LED MAPPING												//
// Port 6																//
// Pins: 7		6		5		4		3		2		1		0		//
//		 IN		OUT		IN		OUT		P7		OUT		IN		OUT		//
//		orange	orange	red		red		N/A		blue	green	green	//
//		button	LED		button	LED				LED		button	LED		//
//																		//
// P7.4 set to input, tied to blue button								//
//----------------------------------------------------------------------//

#define PLAY_BUTTON_0 P6IN&BIT1
#define PLAY_BUTTON_1 P7IN&BIT4
#define PLAY_BUTTON_2 P6IN&BIT5
#define PLAY_BUTTON_3 P6IN&BIT7

#define LED_0 BIT0
#define LED_1 BIT2
#define LED_2 BIT4
#define LED_3 BIT6

/*
* We're using uint8_t to store the sequence values, 0-3 
* (this being due to using 4 lights and buttons for the Simon game).
* That means we only really need 2 bits of information to store each value.
* However, the MSP430's RAM is byte addressable, and trying to parse 2 bit values 
* in and out of each byte of RAM is out of the scope of this project, 
* as well, this program won't need all of the RAM available on the MSP430,
* so we'll make the sacrifice of a bit of wasted space, to make the code simpler, and easier to follow.
*/
extern uint8_t simonSequence[SEQUENCE_MAX];
extern uint8_t sequenceLength;

// Game Over game state flag and LED tracker
extern volatile bool gameOver;
extern volatile uint8_t gameOverLED;

extern volatile uint8_t highScore;

// main game functions
void CPURound(void);
void PlayerRound(void);
void GameStart(void);
void lightLED(uint8_t LED_ID, uint8_t duration);
uint8_t getButtonPress(bool lightMode);
void playGameOverBuzzer(void);
void playGameStartLightPattern(void);
void delay(uint8_t duration);

// additional feature functions
void displayScore(void);
void playLEDTone(void);

// test cases
void DEBUG_functions(void);

#endif

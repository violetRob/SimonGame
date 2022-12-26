# SimonGame

## A reconstruction of the classic Simon game, programmed to operate on TI's MSP430FG4618 Experimenter's Board

### Author's Note:

The code has not been neatly uploaded to github.  It's mostly comments that are not indented correctly; this will be corrected at a future point.  It is, however, functional.

### Software Setup

The code provided in the SimonGame.c file and SimonGame.h header file need to be flashed to an MSP430 Experimenter's Board.  This can be done with, for example, [Code Composer Studio](https://www.ti.com/design-resources/embedded-development/ccs-development-tools.html), which can be downloaded from the Texas Instruments website.

In the header file, you can find the declaration of a definition for running the program in debug mode.
```
#define DEBUG_MODE 0
```
This will call a special debug function near the beginning of the execution of the game code.  This is primarily used for debugging purposes, and does not run the actual game itself.  In most cases, this constant should be defined as 0, to run the Simon game itself.
To run debug mode, set this constant to 1 in the header file.

### Hardware Setup

Attached in the [Simon Game External Circuitry.PNG](Simon Game External Circuitry.PNG) file which can be located in the root directory is a rough diagram of the circuit layout for setting up the Simon game.  A description of the hardware setup, as well as a visual of the circuit diagram can be consulted below.

#### Important Note!  Do not power on the DC power supply to the circuit until the Experimenter's Board has been turned on, and the code flashed to the microcontroller.  Supplying power early could cause undesired effects with the circuitry, and could even damage the MSP430 board.  Do not ever provide more than 5-volts DC power to the circuit; the MSP430 board is not designed to handle much more than that.  Use only the prescribed 3-volts power when operating the circuit.

#### Hardware Supplies

**Microcontrollers**

- (1) MSP430FG4618 Experimenter's Board

**Equipment**

- (1) DC Power Supply

**Transistors**

- (4) NPN Transistors, with beta >= 20

**Resistors**

- (4) 150 Ω resistors
- (5) 1 kΩ resistors
- (4) 3 kΩ resistors
- (3) 10 kΩ resistors

**Miscellaneous Supplies**

- (4) LEDs, each of different color (preferred, not strictly required, depending on physical setup)
- (4) normally open push buttons
- An assortment of wires
- Soldering tools, if necessary

A few important things to note:  you will have to run common ground from the external circuit to the ground on the MSP430 board.  Consult the MSP430 schematics for more information.

It is best to physically line up the appropriate LEDs with their matching push buttons, to avoid confusion on the player's part.

The game expects to communicate with certain ports on the MSP430 board for lighting LEDs, and reading player input from the push buttons.  Consult the header file ((SimonGame.h)[SimonGame.h]) for information on which ports are tied to which LEDs and buttons.  Consult the schematics for the MSP430 board for more information on where the ports are physically located on the board.

![Simon Game Circuit Diagram](https://github.com/violetRob/SimonGame/blob/main/Simon%20Game%20External%20Circuitry.PNG)

### Running the Simon Game

Once the circuit has been wired up, flash the code to the MSP430 Experimenter's Board.  This can be done, for example, using [Code Composer Studio](https://www.ti.com/design-resources/embedded-development/ccs-development-tools.html) from Texas Instruments.  Once this is done, you may provide power from a 3-volt DC power supply to the external circuit, as shown in the circuit diagram highlighted in the Hardware Setup section.  Once the game starts up, a little light show will play across the LEDs, followed by a one-and-a-half second pause.  After this pause, the game begins.

Sometimes, two LEDs may "stick" when first turning on the board; I'm not entirely sure yet why this is the case.  It may have something to do with the startup process.  It is an issue that will resolve itself, however, after playing the first game.

#### How to Play

Each game starts with a light show, followed by a pause, and then the game begins.  Each turn, the CPU will play a sequence on the LEDs.  Once it is done, it is the player's turn.  As the player, you must input the same sequence that was played by the CPU.  If the player successfully matches the sequence, the next round will begin, and one more element will be added by the CPU to the end of the sequence.  If the player inputs an incorrect input, the game over buzzer will sound, while the correct LED lights up, so the player may view which element was supposed to have come next.  Then, another light show will play, and the game will start over from the beginning.

#### Technical Descriptions

Elements are selected randomly by the CPU, utilizing the rand() function from the C standard library.
```
<stdlib.h>
```

In the current version of the game, the player's score, as well as the current high score, will be displayed in the developer's console.  The high score resets when the board is powered down.

### Credits
This project came about as the final project for a Microcontrollers class taken at Waukesha County Technical College, as a joint effort of two people, Robert Knapp and Joe Goldbach.

Robert was primarily in charge of writing and debugging the software for the Simon game.

Joe was primarily responsible for working out the hardware design and circuitry to run the game from the Experimenter's Board to the buttons and LEDs used for the project.

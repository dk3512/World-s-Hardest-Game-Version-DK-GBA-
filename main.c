#include "myLib.h"
#include <stdlib.h>
#include "title.h"
#include "gameover.h"
#include "dk.h"
#include "mario.h"
#include <stdio.h>
#include "banana.h"
#include "win.h"

//different states of the game
enum GBAState {
	START,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	WIN,
	GAMEOVER
};

//start at the START state
enum GBAState state = START;


int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	while(1) {
		waitForVblank();

		//handle different states
		switch(state) {

		//Enter Title Screen
		case START:
			drawImage3(0, 0, 240, 160, title);
			state = LEVEL1;
			
			break;

		//Enter Level 1
		case LEVEL1:
			if (KEY_DOWN_NOW(BUTTON_START)) {
				drawLevel(state);
				state = play(state);
			}
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;

		//Enter Level 2
		case LEVEL2:
			drawLevel(state);
			state = play(state);
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;

		//Enter Level 3
		case LEVEL3:
			drawLevel(state);
			state = play(state);
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;

		//You lost... gameover.
		case GAMEOVER:
			drawImage3(0, 0, 240, 160, gameover);
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;

		//You actually won...
		case WIN:
			drawImage3(0, 0, 240, 160, win);
			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;
		}
	}
	return 0;
}




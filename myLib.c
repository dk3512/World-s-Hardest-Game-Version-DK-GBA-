#include "myLib.h"
#include "banana.h"
#include "dk.h"
#include "mario.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;
DK donk; //Donkey Kong Struct
MARIO top[5]; // Top Mario Structs
MARIO bot[4]; // Bottom Mario Structs
BANANA b[5]; // Banana Structs
//different states of the game
enum GBAState {
	START,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	WIN,
	GAMEOVER
};

//sets a pixel
void setPixel(int r, int c, unsigned short color)
{
	videoBuffer[OFFSET(r, c, 240)] = color;
}

//draws a rectangle
void drawRect(int row, int col, int height, int width, volatile unsigned short color)
{
	for(int r = 0; r < height; r++)
	{
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(row+r, col, 240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

//waits for Vertical Blank
void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

//fills screen with a color
void fillScreen(volatile u16 color)
{
	DMA[3].src = &color;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

//draw the specific levels
void drawLevel(int state) {
	fillScreen(SKYBLUE);
	drawRect(0, 0, 20, 240, BLACK);

	//State the Level
	if (state == LEVEL1) {
		drawString(7, 180, "LEVEL 1", WHITE);
	} else if (state == LEVEL2) {
		drawString(7, 180, "LEVEL 2", WHITE);
	} else if (state == LEVEL3) {
		drawString(7, 180, "LEVEL 3", WHITE);
	}

	drawRect(65, 5, 50, 20, LIGHTGREEN);
	drawRect(30, 25, 120, 185, YELLOW);
	drawRect(65, 210, 50, 20, LIGHTGREEN);
	drawImage3(82, 5, 15, 15, dk);
	if (state == LEVEL2) {
		b[0].r = 82;
		b[0].c = 110;
	} else if (state == LEVEL3) {
		b[0].r = 82;
		b[0].c = 110;

		b[1].r = 35;
		b[1].c = 30;

		b[2].r = 25 + 120 - 15;
		b[2].c = 30;

		b[3].r = 35;
		b[3].c = 190;

		b[4].r = 25 + 120 - 15;
		b[4].c = 190;
	}
}

/* drawimage3
* A function that will draw an arbitrary sized image
* onto the screen (with DMA).
* @param r row to draw the image
* @param c column to draw the image
* @param width width of the image
* @param height height of the image
* @param image Pointer to the first element of the image.
*/
void drawImage3(int r, int c, int width, int height, const u16* image)
{
	for (int i = 0; i < height; ++i)
	{
		DMA[3].src = &image[OFFSET(i, 0, width)];
		DMA[3].dst = &videoBuffer[OFFSET(r + i, c, 240)];
		DMA[3].cnt = width | DMA_ON;
	}
}

// Draw a character
void drawChar(int row, int col, char ch, u16 color)
{
	for(int r=0; r<8; r++)
	{
		for(int c=0; c<6; c++)
		{
			if(fontdata_6x8[OFFSET(r, c, 6) + ch*48])
			{
				setPixel(row+r, col+c, color);
			}
		}
	}
}

// Draw a string
void drawString(int row, int col, char *str, u16 color)
{
	while(*str)
	{
		drawChar(row, col, *str++, color);
		col += 6;
	}
}

// Game logic
int play(int state) {
	//donk's row, column, and previous row and column
	donk.r = 82;
	donk.c = 5;
	donk.oldr = 82;
	donk.oldc = 10;

	//boolean used for moving marios up and down
	int topb = 1;
	int botb = 1;

	//boolean used to draw bananas
	int b1 = 0;
	int b2 = 0;
	int b3 = 0;
	int b4 = 0;
	int b5 = 0;

	//check to see if you have obtained the banana for level 3
	int gotBanana1 = 0;
	int gotBanana2 = 0;
	int gotBanana3 = 0;
	int gotBanana4 = 0;
	int gotBanana5 = 0;

	// number of bananas to obtain for level 3
	int bananaNum = 5;

	// draw top marios
	for (int i = 0; i < 5; i++) {
		top[i].r = 35;
		top[i].oldr = 35;
		drawImage3(35, 30 + i * 40, 15, 15, mario);
	}

	//draw bottom marios
	for (int i = 0; i < 4; i++) {
		bot[i].r = 25 + 120 - 15;
		bot[i].oldr = 25 + 120 - 15;
		drawImage3(25 + 120 - 15, 50 + i * 40, 15, 15, mario);
	}

	while(1) {
		waitForVblank();

		// if in level 2, draw the banana
		if (state == LEVEL2) {
			if (!b1) {
				drawImage3(b[0].r, b[0].c, 15, 15, banana);
			}
		} else if (state == LEVEL3) { // if in level 3 draw the multiple bananas
			if (!b1) {
				drawImage3(b[0].r, b[0].c, 15, 15, banana);
			}
			if (!b2) {
				drawImage3(b[1].r, b[1].c, 15, 15, banana);
			}
			if (!b3) {
				drawImage3(b[2].r, b[2].c, 15, 15, banana);
			}
			if (!b4) {
				drawImage3(b[3].r, b[3].c, 15, 15, banana);
			}
			if (!b5) {
				drawImage3(b[4].r, b[4].c, 15, 15, banana);
			}
		}

		//move the marios starting at the top down and up
		if (topb) {
			for (int i = 0; i < 5; i++) {
				top[i].oldr = top[i].r++;
				drawRect(top[i].oldr, 30 + i * 40, 15, 15, YELLOW);
				drawImage3(top[i].r, 30 + i * 40, 15, 15, mario);
			}
		} else {
			for (int i = 0; i < 5; i++) {
				top[i].oldr = top[i].r--;
				drawRect(top[i].oldr, 30 + i * 40, 15, 15, YELLOW);
				drawImage3(top[i].r, 30 + i * 40, 15, 15, mario);
			}
		}
		if (top[0].r + 15 == 145) {
				topb = 0;
		} else if (top[0].r == 35) {
			topb = 1;
		}

		//move the marios starting at the bottom up and down
		if (botb) {
			for (int i = 0; i < 4; i++) {
				bot[i].oldr = bot[i].r--;
				drawRect(bot[i].oldr, 50 + i * 40, 15, 15, YELLOW);
				drawImage3(bot[i].r, 50 + i * 40, 15, 15, mario);
			}
		} else {
			for (int i = 0; i < 4; i++) {
				bot[i].oldr = bot[i].r++;
				drawRect(bot[i].oldr, 50 + i * 40, 15, 15, YELLOW);
				drawImage3(bot[i].r, 50 + i * 40, 15, 15, mario);
			}
		}
		if (bot[0].r == 35) {
			botb = 0;
		} else if (bot[0].r + 15 == 145) {
			botb = 1;
		}

		// Go back to title screen
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			return START;
		}

		// Move right
		if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
			//check if donk is not within the rows of the green area
			if (donk.r < 65 || donk.r + 15 > 115) {
				if (donk.c + 15 == 210) {
					donk.oldc = donk.c; // donk does not move
				} else {
					donk.oldc = donk.c++; // donk moves right
					drawRect(donk.r, donk.oldc, 15, 15, YELLOW); // erase previous donk
				}
			} else { //donk is within the rows of the green area
				donk.oldc = donk.c++; //donk moves right
				if (donk.oldc >= 25) {
					drawRect(donk.r, donk.oldc, 15, 15, YELLOW); // erase previous donk in yellow area
				} else {
					drawRect(65, 5, 50, 20, LIGHTGREEN); // erase previous donk in green area
				}
			}
			drawImage3(donk.r, donk.c, 15, 15, dk); // draw the moved donk
			// If donk touches green, proceed to next level.
			// If in level 2, you must get the banana.
			// If in level 3, you must get the 5 bananas.
			// If you touch the green area without these requirements, you lose.
			if (donk.c + 15 > 210) { 
				if (state == LEVEL1) {
					return LEVEL2;
				} else if (state == LEVEL2) { 
					if (b1) {
						return LEVEL3;
					} else {
						return GAMEOVER;
					}
				} else if (state == LEVEL3) {
					if (bananaNum == 0) {
						return WIN;
					} else {
						return GAMEOVER;
					}
				}
			}
		}

		if (KEY_DOWN_NOW(BUTTON_LEFT)) {
			if (donk.r < 65 || donk.r + 15 > 115) { //not within the rows of the green area
				if (donk.c == 25) {
					donk.oldc = donk.c; // donk does not move outside of yellow area
				} else {
					donk.oldc = donk.c--; // donk moves inside yellow area
					drawRect(donk.r, donk.oldc, 15, 15, YELLOW);
				}
			} else {
				//moves only inside the green area
				if (donk.c == 5) {
					donk.oldc = donk.c;
				} else {
					donk.oldc = donk.c--;
				}
				if (donk.c + 15 < 25) {
					drawRect(65, 5, 50, 20, LIGHTGREEN);
				} else {
					drawRect(donk.r, donk.oldc, 15, 15, YELLOW);
				}
			}
			drawImage3(donk.r, donk.c, 15, 15, dk);
		}

		if (KEY_DOWN_NOW(BUTTON_UP)) {
			//if inside the green area, you cannot move above the green area
			if (donk.r >= 65 && donk.r + 15 <= 115  
				&& donk.c >= 5 && donk.c < 25) {
				if (donk.r == 65) {
					donk.oldr = donk.r;
				} else {
					donk.oldr = donk.r--;
					drawRect(donk.oldr, donk.c, 15, 15, YELLOW);
					drawRect(65, 5, 50, 20, LIGHTGREEN);
				}
			} else if (donk.r > 30) { //cannot move above yellow area
				donk.oldr = donk.r--;
				drawRect(donk.oldr, donk.c, 15, 15, YELLOW);
			}
			drawImage3(donk.r, donk.c, 15, 15, dk);
		}

		if (KEY_DOWN_NOW(BUTTON_DOWN)) {
			//if inside the green area, you cannot move below the green area
			if (donk.r >= 65 && donk.r + 15 <= 115  
				&& donk.c >= 5 && donk.c < 25) {
				if (donk.r + 15 == 115) {
					donk.oldr = donk.r;
				} else {
					donk.oldr = donk.r++;
					drawRect(donk.oldr, donk.c, 15, 15, YELLOW);
					drawRect(65, 5, 50, 20, LIGHTGREEN);
				}
			} else if (donk.r + 15 < 150) { //cannot move below yellow area
				donk.oldr = donk.r++;
				drawRect(donk.oldr, donk.c, 15, 15, YELLOW);
			}
			drawImage3(donk.r, donk.c, 15, 15, dk);
		}

		// If donk touches top marios, gameover. Checks to see if the corners are within the mario.
		for (int i = 0; i < 5; i++) { 
			if ((donk.r >= top[i].r) && (donk.r <= top[i].r + 15) && (donk.c >= 30 + i * 40)
				&& (donk.c <= 30 + i * 40 + 15)) {
				return GAMEOVER;
			} else if ((donk.c + 15 <= 30 + i * 40 + 15) && (donk.c + 15 >= 30 + i * 40)
				&& (donk.r >= top[i].r) && (donk.r <= top[i].r + 15)) {
				return GAMEOVER;
			} else if ((donk.r + 15 >= top[i].r) && (donk.r + 15 <= top[i].r + 15)
				&& (donk.c >= 30 + i * 40) && (donk.c <= 30 + i * 40 + 15)) {
				return GAMEOVER;
			} else if ((donk.r + 15 >= top[i].r) && (donk.r + 15 <= top[i].r + 15)
				&& (donk.c + 15 <= 30 + i * 40 + 15) && (donk.c + 15 >= 30 + i * 40)) {
				return GAMEOVER;
			} 
		}

		// If donk touches bot marios, gameover. Checks to see if the corners are within the mario.
		for (int i = 0; i < 4; i++) {
			if ((donk.r >= bot[i].r) && (donk.r <= bot[i].r + 15) && (donk.c >= 50 + i * 40)
				&& (donk.c <= 50 + i * 40 + 15)) {
				return GAMEOVER;
			} else if ((donk.c + 15 <= 50 + i * 40 + 15) && (donk.c + 15 >= 50 + i * 40)
				&& (donk.r >= bot[i].r) && (donk.r <= bot[i].r + 15)) {
				return GAMEOVER;
			} else if ((donk.r + 15 >= bot[i].r) && (donk.r + 15 <= bot[i].r + 15)
				&& (donk.c >= 50 + i * 40) && (donk.c <= 50 + i * 40 + 15)) {
				return GAMEOVER;
			} else if ((donk.r + 15 >= bot[i].r) && (donk.r + 15 <= bot[i].r + 15)
				&& (donk.c + 15 <= 50 + i * 40 + 15) && (donk.c + 15 >= 50 + i * 40)) {
				return GAMEOVER;
			} 
		}

		// If donk touches bananas, stop drawing the bananas for stage 2. Checks to see if corners are within banana.
		if (state == LEVEL2) {
			if ((donk.r >= b[0].r) && (donk.r <= b[0].r + 15) && (donk.c >= b[0].c)
			&& (donk.c <= b[0].c + 15)) {
				b1 = 1;
			} else if ((donk.c + 15 <= b[0].c + 15) && (donk.c + 15 >= b[0].c)
				&& (donk.r >= b[0].r) && (donk.r <= b[0].r + 15)) {
				b1 = 1;
			} else if ((donk.r + 15 >= b[0].r) && (donk.r + 15 <= b[0].r + 15)
				&& (donk.c >= b[0].c) && (donk.c <= b[0].c + 15)) {
				b1 = 1;
			} else if ((donk.r + 15 >= b[0].r) && (donk.r + 15 <= b[0].r + 15)
				&& (donk.c + 15 <= b[0].c + 15) && (donk.c + 15 >= b[0].c)) {
				b1 = 1;
			}
		}

		// If donk touches bananas, stop drawing the bananas for stage 3. Checks to see if corners are within banana.
		if (state == LEVEL3) {
			for (int i = 0; i < 5; i++) {
				if ((donk.r >= b[i].r) && (donk.r <= b[i].r + 15) && (donk.c >= b[i].c)
				&& (donk.c <= b[i].c + 15)) {
					if (i == 0) {
						b1 = 1;
					} else if (i == 1) {
						b2 = 1;
					} else if (i == 2) {
						b3 = 1;
					} else if (i == 3) {
						b4 = 1;
					} else {
						b5 = 1;
					}
				} else if ((donk.c + 15 <= b[i].c + 15) && (donk.c + 15 >= b[i].c)
					&& (donk.r >= b[i].r) && (donk.r <= b[i].r + 15)) {
					if (i == 0) {
						b1 = 1;
					} else if (i == 1) {
						b2 = 1;
					} else if (i == 2) {
						b3 = 1;
					} else if (i == 3) {
						b4 = 1;
					} else {
						b5 = 1;
					}
				} else if ((donk.r + 15 >= b[i].r) && (donk.r + 15 <= b[i].r + 15)
					&& (donk.c >= b[i].c) && (donk.c <= b[i].c + 15)) {
					if (i == 0) {
						b1 = 1;
					} else if (i == 1) {
						b2 = 1;
					} else if (i == 2) {
						b3 = 1;
					} else if (i == 3) {
						b4 = 1;
					} else {
						b5 = 1;
					}
				} else if ((donk.r + 15 >= b[i].r) && (donk.r + 15 <= b[i].r + 15)
					&& (donk.c + 15 <= b[i].c + 15) && (donk.c + 15 >= b[i].c)) {
					if (i == 0) {
						b1 = 1;
					} else if (i == 1) {
						b2 = 1;
					} else if (i == 2) {
						b3 = 1;
					} else if (i == 3) {
						b4 = 1;
					} else {
						b5 = 1;
					}
				} 
			}
		}
		
		// Get rid of the middle banana if touched.
		if (b1 & !gotBanana1) {
			drawRect(b[0].r, b[0].c, 15, 15, YELLOW);
			drawImage3(donk.r, donk.c, 15, 15, dk);
			gotBanana1 = 1;
			bananaNum--;
		}

		// Get rid of the bananas in Level 3 if touched. Decrement bananaNum if touched.
		if (state == LEVEL3) {
			if (b2 & !gotBanana2) {
				drawRect(b[1].r, b[1].c, 15, 15, YELLOW);
				drawImage3(donk.r, donk.c, 15, 15, dk);
				gotBanana2 = 1;
				bananaNum--;
			}
			if (b3 & !gotBanana3) {
				drawRect(b[2].r, b[2].c, 15, 15, YELLOW);
				drawImage3(donk.r, donk.c, 15, 15, dk);
				gotBanana3 = 1;
				bananaNum--;
			}
			if (b4 & !gotBanana4) {
				drawRect(b[3].r, b[3].c, 15, 15, YELLOW);
				drawImage3(donk.r, donk.c, 15, 15, dk);
				gotBanana4 = 1;
				bananaNum--;
			}
			if (b5 & !gotBanana5) {
				drawRect(b[4].r, b[4].c, 15, 15, YELLOW);
				drawImage3(donk.r, donk.c, 15, 15, dk);
				gotBanana5 = 1;
				bananaNum--;
			}
		}
	}
	return 0;
}
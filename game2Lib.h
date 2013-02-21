//Steven Wojcio
//A3
//Brandon Whitehead

#ifndef GAME2LIB_H
#define GAME2LIB_H

#include "LifeGame.h"
#include "play.h"
#include "pause.h"
#include "next.h"
#include "1.h"
#include "2.h"
#include "clear.h"
#include "nextModeButton.h"
#include "normalModeButton.h"

//#include "text.h"
//#include <stdio.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
 
#define MAXR 25
#define MAXC 30
#define UpperLeftRow 31
#define UpperLeftCol 60
#define NORMAL 1
#define NEXT 2

#define REG_DISPCTL (*(u16*)0x4000000)
#define MODE3 3
#define BG2_ENABLE (1<<10)
#define OFFSET(r, c, rowlen)  ((r)*(rowlen)+(c))
#define COLOR(r,g,b) ((r) | (g)<<5 | (b)<<10) 
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define MAGENTA COLOR(31,0,31)
#define CYAN COLOR(0,31,31)
#define YELLOW COLOR(31,31,0)
#define WHITE COLOR(31,31,31)
#define BLACK 0
#define SCREENWIDTH 240
#define SCREENHEIGHT 160
#define LTGREY COLOR(25,25,25)
#define DKGREY COLOR(15, 15, 15)

// Mode 4
#define MODE4 4
#define BUFFER0 (unsigned short *)0x6000000
#define BUFFER1 (unsigned short *)0x600A000
#define BUFFER1FLAG (1<<4)
#define PALETTE ((u16 *)0x5000000)

extern u16 *videoBuffer;

#define SCANLINECOUNTER *(u16 *)0x4000006

/* Buttons */

#define BUTTON_A      (1<<0)
#define BUTTON_B      (1<<1)
#define BUTTON_SELECT (1<<2)
#define BUTTON_START  (1<<3)
#define BUTTON_RIGHT  (1<<4)
#define BUTTON_LEFT   (1<<5)
#define BUTTON_UP     (1<<6)
#define BUTTON_DOWN   (1<<7)
#define BUTTON_R      (1<<8)
#define BUTTON_L      (1<<9)



#define BUTTONS (*( unsigned int *)0x04000130)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

/* DMA ---------------------------------------- */

typedef struct 
{
	const volatile void *src;
	volatile void *dst;
	volatile u32 cnt;
} DMAREC;

#define DMA ((volatile DMAREC *)0x040000B0)

#define REG_DMA0SAD         *(u32*)0x40000B0  // source address
#define REG_DMA0DAD         *(u32*)0x40000B4  // destination address
#define REG_DMA0CNT         *(u32*)0x40000B8  // control register

/* DMA channel 1 register definitions */
#define REG_DMA1SAD         *(u32*)0x40000BC  // source address
#define REG_DMA1DAD         *(u32*)0x40000C0  // destination address
#define REG_DMA1CNT         *(u32*)0x40000C4  // control register

/* DMA channel 2 register definitions */
#define REG_DMA2SAD         *(u32*)0x40000C8  // source address
#define REG_DMA2DAD         *(u32*)0x40000CC  // destination address
#define REG_DMA2CNT         *(u32*)0x40000D0  // control register

/* DMA channel 3 register definitions */
#define REG_DMA3SAD         *(u32*)0x40000D4  // source address
#define REG_DMA3DAD         *(u32*)0x40000D8  // destination address
#define REG_DMA3CNT         *(u32*)0x40000DC  // control register

/* Defines */
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

char a[MAXR][MAXC];
char prevA[MAXR][MAXC];
char oldA[MAXR][MAXC];
int mode;
int playing;
int toReset;
int newBoard;
typedef struct USER{
		int r;
		int c;
	} usr;
	
usr mouse[1];
usr oldmouse[1];



enum{BLACKINDEX,LTGREYINDEX,REDINDEX,GREENINDEX,BLUEINDEX,WHITEINDEX,DKGREYINDEX,YELLOWINDEX};		
// Prototypes
void setPixel(int row, int col, unsigned short color);
void drawRect(int row, int col, int height, int width, u16 color);
void delay(int n);
void waitForVblank();
void setPixel4(int row, int col, u8 index);


void setPixel4(int row, int col, u8 index);
void drawRect4(int row, int col, int height, int width, u8 index);
void FlipPage();
void drawChar4(int row, int col, char ch, u8 color);
void drawString4(int row, int col, char *str, u8 color);
void fillScreen(u8 index);
void drawBoard();
void nextArray();
void clear();
void drawMouse(int row, int col, int height, int width, u8 index);
void mouseLogic();
void menuLogic();
void gameLogic();
void drawImage4();
#endif

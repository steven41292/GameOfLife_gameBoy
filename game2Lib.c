//Steven Wojcio
//A3
//Brandon Whitehead

#include "game2Lib.h"

u16* videoBuffer = (u16*) 0x6000000;


void setPixel4(int r, int c, u8 paletteIndex){
	int col=c;
	if(col%2){
		videoBuffer[((240*r)+c)/2] = (videoBuffer[((240*r)+c)/2] & 0x00ff) |  (paletteIndex<<8) ;
	}
	else
		videoBuffer[((240*r)+c)/2]=(videoBuffer[((240*r)+c)/2] & 0xff00) |  (paletteIndex);
}
	
void drawRect4(int row, int col, int width, int height, u8 index)
{
	volatile u16 color = index<<8 | index;
	int r;
	for(r=0; r<height; r++)
	{
		DMA[3].src = &color;
		DMA[3].dst = videoBuffer + ((row+r)*240+col)/2; 
		DMA[3].cnt = (width/2) | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void drawMouse(int row, int col, int width, int height, u8 index){

	drawRect4(row,col,width,1,index);
	drawRect4(row,col,2,height-1,index);
	drawRect4(row+height-1,col,width+2,1,index);
	drawRect4(row,col+(width-1),2,height-1,index);
	
	
}

void drawImage4(int r, int c, int width, int height, const u16* ptr){

	int row;
	
	for(row=0;row<height;row++){
		DMA[3].src = &ptr[row*width/2];
		DMA[3].dst = videoBuffer + ((row+r)*240+c)/2;
		DMA[3].cnt = (width/2) | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
	}

}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void FlipPage(){
	if(REG_DISPCTL & BUFFER1FLAG){
		REG_DISPCTL &= ~BUFFER1FLAG;  
		videoBuffer = BUFFER1;		  
	}
	else{	
		REG_DISPCTL |= BUFFER1FLAG;   
		videoBuffer = BUFFER0;        
	}
}

void drawBoard(){
	
	if(toReset){
		int i;
		mode=NORMAL;
		playing=0;
		for (i=0;i<2;i++){
			PALETTE[i]=LifeGame_palette[i];
		}
		drawImage4(0,0,240,160,LifeGame);	
		FlipPage();
		while(!(KEY_DOWN_NOW(BUTTON_START))){
		}
		FlipPage();
		u16 colors[]={BLACK,LTGREY,RED,GREEN,BLUE,WHITE,DKGREY,YELLOW};
		int numcolors = sizeof(colors)/sizeof(colors[0]);
		for (i=0;i<numcolors;i++){
			PALETTE[i]=colors[i];
		}
		clear();//Clear array to start game

		mouse->r=UpperLeftRow-1;
		mouse->c=UpperLeftCol-2;
		toReset=0;
	}
	else{
	drawRect4(0,0,240,160,LTGREYINDEX);
	drawImage4(140,58,20,12,clearButton);
	drawImage4(140,80,20,12,One);
	drawImage4(140,110,20,12,Two);
	drawImage4(140,145,20,12,next);
	if(playing)drawImage4(140,30,12,12,pause);
	else drawImage4(140,30,12,12,play);
	if (mode==NORMAL){
		drawImage4(2,5,50,12,normalModeButton);
		drawImage4(140,177,50,12,nextModeButton);
	}
	else{
		drawImage4(2,5,50,12,nextModeButton);
		drawImage4(140,177,50,12,normalModeButton);
		}
		drawMouse(mouse->r,mouse->c,5,5,BLUEINDEX);
	
		int i;
		int j;
		if (mode==NORMAL){
			for (i=0;i<MAXR;i++){
				for (j=0;j<MAXC;j++){
					if (a[i][j])drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,GREENINDEX); 
					else drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,BLACKINDEX);
				}
			}
		}
		else{ 
			int n;
			int m;
			for(n=0;n<MAXR;n++){
				for(m=0;m<MAXC;m++){
					oldA[n][m]=prevA[n][m];
				}
			}
			nextArray();
			
			for (i=0;i<MAXR;i++){
				for (j=0;j<MAXC;j++){
					if (a[i][j]==prevA[i][j]){
						if(a[i][j])
							drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,GREENINDEX);

						else drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,BLACKINDEX); 
					}
					else{
						if(a[i][j])drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,YELLOWINDEX);
						else drawRect4(i*4+UpperLeftRow,j*4+UpperLeftCol,3,3,REDINDEX);
					}
				}
			}
			
			for(n=0;n<MAXR;n++){
				for(m=0;m<MAXC;m++){
					a[n][m]=prevA[n][m];
				}
			}
			for(n=0;n<MAXR;n++){
				for(m=0;m<MAXC;m++){
					prevA[n][m]=oldA[n][m];
				}
			}
				
			
		}
	}
	FlipPage();
	waitForVblank();
	
}

void nextArray(){
	int m;
	int n;
	
	for(m=0;m<MAXR;m++){
		for(n=0;n<MAXC;n++){
			prevA[m][n]=a[m][n];
		}
	}
	
	for(m=0;m<MAXR;m++){
		for(n=0;n<MAXC;n++){
			int down=m+1;
			int up=m-1;
			int left=n-1;
			int right=n+1;

			if (m==MAXR-1){
				down=0;
			}
			if (m==0){
				up=MAXR-1;
			}
			if (n==MAXC-1){
				right=0;
			}
			if (n==0){
				left=MAXC-1;
			}
			int counter=0;
			if(prevA[up][left])counter++;
			if(prevA[up][n])counter++;
			if(prevA[up][right])counter++;
			if(prevA[m][left])counter++;
			if(prevA[m][right])counter++;
			if(prevA[down][left])counter++;
			if(prevA[down][n])counter++;
			if(prevA[down][right])counter++;

			if((!a[m][n])&&(counter==3))a[m][n]=1;
			if((a[m][n])&&((counter==2)||(counter==3)))a[m][n]=1;
			else a[m][n]=0;
		}
	}
	
}


void delay(int n){
	volatile int i;
	for (i=0;i<(n*1000);i++){
		if(KEY_DOWN_NOW(BUTTON_A)){
			return;
		}
	}
}

void clear(){
	int i;
	int j;
	for (i=0;i<MAXR;i++){
		for (j=0;j<MAXC;j++){
			a[i][j]=0;
		}
	}
	if(mode==NEXT)nextArray();
}

void mouseLogic(){
	while(1){
		if(KEY_DOWN_NOW(BUTTON_SELECT)){
			while(KEY_DOWN_NOW(BUTTON_SELECT));
			toReset=1;
		}
		if(KEY_DOWN_NOW(BUTTON_DOWN)){
			delay(10);
			if (mouse->r>(UpperLeftRow+MAXR*4-6)){
				while(KEY_DOWN_NOW(BUTTON_DOWN));
				menuLogic();
				
			}
			else{mouse->r+=4;}
			
		}
		if(KEY_DOWN_NOW(BUTTON_UP)){
			delay(10);
			if (mouse->r<=(UpperLeftRow)){
				while(KEY_DOWN_NOW(BUTTON_UP));
				menuLogic();
			}
			else{mouse->r-=4;}
			
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT)){
			//while(KEY_DOWN_NOW(BUTTON_RIGHT));
			delay(10);
			if (mouse->c>=(UpperLeftCol+(4*MAXC)-6)){
				mouse->c=UpperLeftCol-2;
			}
			else{mouse->c+=4;}
			
		}
		if(KEY_DOWN_NOW(BUTTON_LEFT)){
			delay(10);
			//while(KEY_DOWN_NOW(BUTTON_LEFT));
			if (mouse->c<=UpperLeftCol){
				mouse->c=(UpperLeftCol+(4*MAXC))-6;
			}
			else{mouse->c-=4;}
			
		}
		if(KEY_DOWN_NOW(BUTTON_A)){
			while(KEY_DOWN_NOW(BUTTON_A));
			
			int rowConversion=((mouse->r-UpperLeftRow+1)/4);
			int colConversion=((mouse->c-UpperLeftCol+2)/4);
			if (a[rowConversion][colConversion]) a[rowConversion][colConversion]=0;
			else a[rowConversion][colConversion]=1;
			newBoard=1;
		}
		
		drawBoard();
	}
}

void menuLogic(){
	int stillHere=1;
	usr temp[1];
	temp->r=mouse->r;
	temp->c=mouse->c;
	

	mouse->r=140;
	mouse->c=20;
	drawBoard();
	
	while(stillHere){
		
		if(KEY_DOWN_NOW(BUTTON_SELECT)){
			while(KEY_DOWN_NOW(BUTTON_SELECT));
			toReset=1;
		}
		if(KEY_DOWN_NOW(BUTTON_UP)){
			delay(10);
			//while(KEY_DOWN_NOW(BUTTON_UP))
				if(temp->r>50){
					mouse->r=temp->r;
					mouse->c=temp->c;
					
				}
				else{
					mouse->r=(UpperLeftRow+MAXR*4-5);
					mouse->c=UpperLeftCol-2;
				}
			drawBoard();		
			
			stillHere=0;
			continue;
				
		
		}
		if(KEY_DOWN_NOW(BUTTON_DOWN)){
			delay(10);
			//while(KEY_DOWN_NOW(BUTTON_DOWN))
				if(temp->r<50){
					mouse->r=temp->r;
					mouse->c=temp->c;
				}
				else{
					mouse->r=UpperLeftRow-1;
					mouse->c=UpperLeftCol-2;
				}
			drawBoard();
		
			stillHere=0;
			continue;
		
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT)){
			delay(10);
			//while(KEY_DOWN_NOW(BUTTON_RIGHT));
			if (mouse->c>=170){
				;
			}
			else{mouse->c+=30;}
			drawBoard();
			
		}
		if(KEY_DOWN_NOW(BUTTON_LEFT)){
			delay(10);
			//while(KEY_DOWN_NOW(BUTTON_LEFT));
			if (mouse->c<39){
				;
			}
			else{mouse->c-=30;}
			drawBoard();
			
		}
		if(KEY_DOWN_NOW(BUTTON_A)){
			while(KEY_DOWN_NOW(BUTTON_A));
			if (mouse->c==20){
				
				int check=1;
				playing=1;
				while(check){
					delay(20);
					nextArray();
					drawBoard();
					if(KEY_DOWN_NOW(BUTTON_A)){
						while(KEY_DOWN_NOW(BUTTON_A));
						check=0;
					}
				}
				playing=0;
				
			}
			if (mouse->c==50){		
				clear();
				drawBoard();
			}		
					
			if (mouse->c==80){
					
				
				clear();
				a[10][15]=1;
				a[10][25]=1;
				a[17][12]=1;
				a[18][13]=1;
				a[19][14]=1;
				a[20][15]=1;
				a[21][16]=1;
				a[22][17]=1;
				a[22][18]=1;
				a[22][19]=1;
				a[22][20]=1;
				a[22][21]=1;
				a[22][22]=1;
				a[22][23]=1;
				a[21][24]=1;
				a[20][25]=1;
				a[19][26]=1;
				a[18][27]=1;
				a[17][28]=1;

				drawBoard();
				
					
				}
			if (mouse->c==110){
				
				clear();
				a[15][20]=1;
				a[12][20]=1;
				a[13][20]=1;
				a[13][19]=1;
				a[13][21]=1;
				a[17][20]=1;
				a[17][19]=1;
				a[17][21]=1;
				a[18][20]=1;
				drawBoard();
					
					
				
			}
			if (mouse->c==140){
				nextArray();
				drawBoard();
			}
			if (mouse->c==170){
				if (mode==NORMAL){
					clear();
					int m;
					int n;
					for(m=0;m<MAXR;m++){
						for(n=0;n<MAXC;n++){
							prevA[m][n]=0;
						}
					}
					mode=NEXT;
					drawImage4(2,5,50,12,nextModeButton);
					drawImage4(140,177,50,12,normalModeButton);
				}
				else {
					mode=NORMAL;
					drawImage4(2,5,50,12,normalModeButton);
					drawImage4(140,177,50,12,nextModeButton);
				}
			}
		}
		
		drawBoard();
	}
	
}

void gameLogic(){
	int i;
	mode=NORMAL;
	playing=0;

	for (i=0;i<2;i++){
		PALETTE[i]=LifeGame_palette[i];
		}
	drawImage4(0,0,240,160,LifeGame);
	
	while(!(KEY_DOWN_NOW(BUTTON_START))){
	}
	
	u16 colors[]={BLACK,LTGREY,RED,GREEN,BLUE,WHITE,DKGREY,YELLOW};
	int numcolors = sizeof(colors)/sizeof(colors[0]);
	for (i=0;i<numcolors;i++){
		PALETTE[i]=colors[i];
	}
	
	clear();//Clear array to start game
	
	toReset=1;
	drawBoard();
	mouseLogic();
}

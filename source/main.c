#include <stdlib.h>
#include "framebuffer.h"
#include "image.h"	//Data related to Iamges are in this file
#include "elements.h"	//Some struct of game elements, for example enemies
#include "control.h"	//functions read input from SNES 
#include <stdio.h>
/* Definition for Pixel stucture used to draw to the screen*/
typedef struct {
	int color;
	int x, y;
} Pixel;
struct fbs framebufferstruct;

/**
 * Declare functions before using them throughout the program
 * */
void drawPixel(Pixel *pixel);
void drawimage(int x, int y, image pic);
int mainmenu();
void drawblack(int x, int y, int width, int height);
void update();
void draw();
void enemyinitialize();	//Initialize the enemies. We will assign some default values to enemies.
void detection();	//Detect if some laser hit ship, or called tank, 
void drawbeams();	//Draw laser, or called beam
void updateBeams();	//Update the beams on the screen
void blockinitialize();	// Initialize values of blocks(obstacles)
void drawBlock(int x, int y, int width, int height);	// Draws the obstacles/blocks
void updateCounters();		// Updates the counters that are used to keep track of game state
void enemyfire();			// Checks which and if enemies will fire
void trackIndexOfLowest();	// We trace the bottom of enemies. When the lowest enemies touch the red line, the gamer loses.
void clearBeams();	//Remove beams from screen
void drawTop();		//Draw Score and Lives Words on top
void updateLives(); // Update the lives
int pauseMenu();	// Display Pause/Game Menu
void resetElements(); // Reset elements of the game
void updateScore(int type); //Update the Score (type is the type of enemy killed)
void drawScore(); 	// Draw the Score number values
void checkEnemyHeight();	// Check if enemies reached red line, if so then user loses
void displayWin();		// Display win screen if user wins
void displayLoss();		// Display lose screen if user loses(run out of lives/ enemies reach red line)
void drawBarrierLine();	//A red line, if enemies touch this line, the gamer will lose.

/**
 * Variables used to keep track of the game and help limit interactions such as
 * shooting of beams and 
 * */
int beamdelay = 0;
int status, pauseCheck;
int cyclecounter = 0;	//Game Counter, used to measure the status of the game
// Column Delays used to prevent same column from shooting too quickly
// Can't fire if != 0;
int column1delay = 0;
int column2delay = 0;
int column3delay = 0;
int column4delay = 0;
int column5delay = 0;
// Delay used to limit enemy firing, they can't fire if != 0
int allEnemyDelay = 0;
// Int used 
int enemyfired = 0;
int indexOfLowest[5];
int score = 0;

/**
 * Array of structs used;
 * */
block blocks[27];
enemy enemies[20];
pbeam ebullets[3]; // Array of enemy beams/bullets (can have 3 at most)
pbeam pbullets[3]; // Array of user/player beams/bullets(can have 3 at most)
tank player;


/**
 * Main function that runs the game
 * */
int main(){
	//Initialize GPIO and framebuffer
	gpio = gpioPtr();
    Init_GPIO(10, 0);
    Init_GPIO(9, 1);
    Init_GPIO(11, 1);
	framebufferstruct = initFbInfo();
	// Running int used to check if game is running, if ==1 then it is running
	int running = 1;
	// Variable used to check if user paused the game mid-way
	pauseCheck = 0;
	// Variable used to check which screen game is in (main menu, playing game, lose/win screen)
	status = 0;
	//GAME START!
	while(running == 1){
		/**
		 * If game is running and status  == 0 then go to main menu
		 * */
		if (status == 0){
			int menuChoice = mainmenu();
			// If user chose start in main menu, start the game
			if(menuChoice == 0){
				//Initialize/Reset Variables for game.
				resetElements();
				status = 1;
			}
			// If user chose quit in main menu, close the game
			if(menuChoice == 1){
				running = 0;
			}
		}
		// Start the game
		if (status == 1){
			drawblack(0,0,700,900);
			resetElements();// Initialize /Reset(if replaying) the elements
			// draw Top score and life and then display score
			drawTop();
			drawScore();
			//Game loop, while game is playing update elements, draw, then update variables/counters
			while(status == 1){
				update();
				draw();
				updateCounters();
				delayMicroseconds(33333);
				// If game is paused mid-way through the game, run the pause/game menu
				if (pauseCheck == 1){
					int pauseChoice = pauseMenu();
					// If user chose to resume game, clear game menu and then resume
					if(pauseChoice == 0){
						pauseCheck = 0;
						drawblack(0,0,700,900);
						trackIndexOfLowest();
						drawTop();
						updateLives();
						drawScore();
					}
					//If user chose to restart, clear game menu, reset elements then continue
					if(pauseChoice == 1){
						drawblack(0,0,700,900);
						drawTop();
						resetElements();
						drawScore();
						pauseCheck = 0;
					}
					// If user chose to quit change running to 0 to exit loop and terminate program
					if(pauseChoice == 2){
						status = 0;
						running = 0;
						pauseCheck = 0;
					}
				}
			}
		}
		// If user won the game then display win screen
		if(status == 3){
			displayWin();
			}
		// If user lost the game then display lose screen
		if(status == 4){
			displayLoss();
			}
	}
	// If running == 0 clear whole screen and return 0
	drawblack(0,0,700,900);
	//GAME END!
	return 0;
}
/**
 * Function used to draw red line barrier
 * If enemy reaches the barrier then user loses
 * 
 * Is based off of the drawPixel from tutorials, but is looped to
 * draw a 700,3 line
 * */
void drawBarrierLine(){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	pixel->color = 0xFF0000;
	pixel->x = 0;
	pixel->y = 717;
	for(int i=0;i<3; i++){
		for(int j=0;j<700; j++){
			drawPixel(pixel);
			pixel->x++;
		}
		pixel->x=0;
		pixel->y++;
	}
	free(pixel);
}

/**
 * Function to display win screen if the user reached the max score.
 * If user presses any button on controller then replay the game.
 * */
void displayWin(){
	image winToDraw= {533 ,600 ,winScreen};
	drawimage(50, 150, winToDraw);
	delayMicroseconds(250000);
	int buttonPress = 1;
	// While button isn't pressed then loop
	while(buttonPress == 1){
		Read_SNES();
		// If user presses and button on controller restart the game
		for (int i = 0; i<12; i++){
			if(value[i]==0){
					drawblack(0,0,700,900);
					drawTop();
					resetElements();
					drawScore();
					status = 1;
					buttonPress = 0;
			}
		}
		delayMicroseconds(33333);
	}
}

/**
 * Function to display lose screen if the user loses all 3 lives
 * If user presses any button on controller then replay the game.
 * */
void displayLoss(){
	image loseToDraw= {533 ,600 ,loseScreen};
		drawimage(50, 150, loseToDraw);
		delayMicroseconds(250000);
		int buttonPress = 1;
		// Read from controller, and stay in loop until user presses a button onto the screen
		while(buttonPress == 1){
			Read_SNES();
			for (int i = 0; i<12; i++){
				if(value[i]==0){
						drawblack(0,0,700,900);
						drawTop();
						resetElements();
						drawScore();
						status = 1;
						buttonPress = 0;
			}
		}
		delayMicroseconds(33333);
	}
}

/**
 * Function used to reset the game elements, used for when restarting/ replaying the game
 * */
void resetElements(){
	cyclecounter = 0;
	enemyinitialize();
	player.x = 330;
	player.y = 850;
	player.life = 3;
	score = 0;
	blockinitialize();
	beamdelay = 400;
	column1delay = 0;
	column2delay = 0;
	column3delay = 0;
	column4delay = 0;
	column5delay = 0;
	allEnemyDelay = 0;
	trackIndexOfLowest();
	clearBeams();
}

/**
 * Pause menu used when user presses start when playing the game
 * */
int pauseMenu(){
	image pauseMenuToDraw = {533,600,gameMenuPic};
	drawimage(50, 150, pauseMenuToDraw);
	image arr = {50,50,arrowpic}; // Image of arrow that will be used to help select
	drawimage(125, 415, arr);
	int choice = 0;
	// Loop until user selects an option
	while(1){
		Read_SNES();
		// Move arrow up
		if(value[4]==0 && choice == 1){
				choice = 0;
				drawblack(125, 495, 50, 50);
				drawimage(125, 415, arr);
		}
		else if(value[4]==0 && choice == 2){
				choice = 1;
				drawblack(125, 560, 50, 50);
				drawimage(125, 495, arr);
		}
		// Move arrow down
		else if(value[5]==0 && choice == 0){
				choice = 1;
				drawblack(125, 415, 50, 50);
				drawimage(125, 495, arr);
		}
		
		else if(value[5]==0 && choice == 1){
				choice = 2;
				drawblack(125, 495, 50, 50);
				drawimage(125, 560, arr);
		}
		// If user presses a button to select an option
		if(value[8]==0 && choice == 0){
				return 0;
		}
		else if(value[8]==0 && choice == 1){
			return 1;
		}
		else if(value[8]==0 && choice == 2){
			return 2;
		}
		delayMicroseconds(100000);
	}
}

/**
 * Update the display for user lives
 * */
void updateLives(){
	if (player.life == 2){
		drawblack(610,0,75,40);
	}
	if (player.life == 1){
		drawblack(520,0,175,40);
	}
	if (player.life == 0){
		drawblack(430,0,275,40);
	}
}

/**
 * Draws top image that contains the lives and score words
 * */
void drawTop(){
	image topToDraw = {700 ,50, top};
	image tankToDraw = {75, 40, tankImg};
	
	// Draw tank images that represent lives in top right
	drawimage(0,0,topToDraw);
	drawimage(430,0,tankToDraw);
	drawimage(520,0,tankToDraw);
	drawimage(610,0,tankToDraw);
}

/**
 * Function used to draw a pixel onto the screen (FROM TUTORIAL)
 * */
void drawPixel(Pixel *pixel){
	int offset = ((pixel->y * framebufferstruct.x) + pixel->x);
	framebufferstruct.fptr[offset] = pixel->color;
}

/**
 * Function used to remove all beams, used for when restarting/replaying
 * */
void clearBeams(){
	for(int i = 0; i<3; i++){
		pbullets[i].beamstatus = 0;
		ebullets[i].beamstatus = 0;
	}
	// Reset delays as beams have been reset/removed
	column1delay = 0;
	column2delay = 0;
	column3delay = 0;
	column4delay = 0;
	column5delay = 0;
	allEnemyDelay = 0;
	enemyfired = 0;
	beamdelay =0;
	
}
/**
 * Function that uses draw pixel over and over again to display an image onto the screen
 * x, and y are the top left coordinates of the image, image is which image to draw
 * */
void drawimage(int x, int y, image pic){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	pixel->color = 0xFFFFFFFF;
	pixel->x = x;
	pixel->y = y;
	int counter=0;
	int *coloradress;
	coloradress = (int*)pic.image_pixels;
	for(int i=0;i<pic.height;i++){
		for(int j=0;j<pic.width;j++){
			pixel->color = coloradress[counter++];
			drawPixel(pixel);
			pixel->x++;
		}
		pixel->x=x;
		pixel->y++;
	}
	free(pixel);
	pixel = NULL;
}

/**
 * Draws the score(numbers) onto the top left
 * */
void drawScore(){
	// Images for numbers 0 to 9
	image zeroToDraw ={32,32,Img0};
	image oneToDraw ={32,32,Img1};
	image twoToDraw ={32,32,Img2};
	image threeToDraw ={32,32,Img3};
	image fourToDraw ={32,32,Img4};
	image fiveToDraw ={32,32,Img5};
	image sixToDraw ={32,32,Img6};
	image sevenToDraw ={32,32,Img7};
	image eightToDraw ={32,32,Img8};
	image nineToDraw ={32,32,Img9};
	
	
	int numToPrint;	// Number that will be drawn to the screen
	int rest = score;// Rest set to score
	for(int i = 0; i < 3; i++){
		// first iteration of loop, set numToPrint as hundreds place of rest using integer division
		if (i == 0){
		numToPrint = rest/100;
		rest = rest % 100; // remove hundreds place in score in rest variable
		}
		// second iteration of loop, numToPrint is the tens place of rest using integer division
		else if (i == 1){
		numToPrint = rest/10;
		rest = rest % 10; // remove tens place in score in rest variable
		}
		else{				// 3rd iteration of loop, rest should contain only the ones places of score
		numToPrint = rest;	// set numToPrint to rest
		
		}
		
	switch(numToPrint){ // use switch statement to draw score, draw first digit at x-coordinate 130, y-coordinate 7. 
						// Draw every digit after 32 pixels to the right. 
		case 0:
			drawimage((i*32)+130,7, zeroToDraw);
			break;
		case 1:
			drawimage((i*32)+130,7, oneToDraw);
			break;
		case 2:
			drawimage((i*32)+130,7, twoToDraw);
			break;
		case 3:
			drawimage((i*32)+130,7, threeToDraw);
			break;
		case 4:
			drawimage((i*32)+130,7, fourToDraw);
			break;
		case 5:
			drawimage((i*32)+130,7, fiveToDraw);
			break;
		case 6:
			drawimage((i*32)+130,7, sixToDraw);
			break;
		case 7:
			drawimage((i*32)+130,7, sevenToDraw);
			break;
		case 8:
			drawimage((i*32)+130,7, eightToDraw);
			break;
		case 9:
			drawimage((i*32)+130,7, nineToDraw);
			break;
		}
	}
	// if the max score is reached then status is set to 3, which means the player has won. 
	if (score ==315){
		status = 3;
	}
}

/**
 * Displays the main menu until user chooses an option (presses a)
 * */
int mainmenu(int *gameStatus){
	image mainmenubackground = {700 ,900 ,mainmenubkg};
	drawimage(0, 0, mainmenubackground);
	image arr = {50,50,arrowpic};
	drawimage(200, 460, arr);
	int choice = 0;
	// Loop until user presses a to select an option
	while(1){
		Read_SNES();
		if(value[4]==0 && choice == 1){
				choice--;
				drawblack(200, 540, 50, 50);
				drawimage(200, 460, arr);
		}
		if(value[5]==0 && choice == 0){
				choice++;
				drawblack(200, 460, 50, 50);
				drawimage(200, 540, arr);
		}
		if(value[8]==0 && choice == 0){
				return 0;
		}
		else if(value[8]==0 && choice == 1){
			return 1;
		}
		delayMicroseconds(33333);
	}
	
}

/**
 * Similar to draw image but will draw a widthxheight block with specific color
 * */
void drawBlock(int x, int y, int width, int height){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	pixel->color = 0x836b6b;
	pixel->x = x;
	pixel->y = y;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			drawPixel(pixel);
			pixel->x++;
		}
		pixel->x=x;
		pixel->y++;
	}
	free(pixel);
}

/**
 * Similar to draw image but will draw a widthxheight black box
 * */
void drawblack(int x, int y, int width, int height){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	pixel->color = 0x0;
	pixel->x = x;
	pixel->y = y;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			drawPixel(pixel);
			pixel->x++;
		}
		pixel->x=x;
		pixel->y++;
	}
	free(pixel);
}

/**
 * Function that updates all the elements of the game while playing.
 * Reads NES Controller everytime and checks the user input
 * */
void update(){
	Read_SNES();
	//Enemies moves regularly
	trackIndexOfLowest();
	checkEnemyHeight();
	
	// If cycle counter < 120 then enemies move right
	if(cyclecounter<120){
		for(int i=0;i<20;i++){
			enemies[i].x++;
		}
	}
	// If cycle counter == 120 then enemies move down
	else if(cyclecounter==120){
		for(int i=0;i<20;i++){
			enemies[i].y+=30;
		}
	}
	// If 120<cyclecounter<360 then enemies move left 
	else if(cyclecounter<360){
		for(int i=0;i<20;i++){
			enemies[i].x--;
		}
	}
	// If cyclecounter == 360 then enemies move down
	else if(cyclecounter==360){
		for(int i=0;i<20;i++){
			enemies[i].y+=30;
		}
	}
	// If 360 < cyclecounter <=480 then enemies move right
	else if(cyclecounter<=480){
		for(int i=0;i<20;i++){
			enemies[i].x++;
		}
	}

	//If user pressed left then move the user to the left
	if(value[6]==0){
		if(player.x>1){
			if(value[7]==0) ;
			else{
				drawblack(player.x+70, player.y, 5, 50);
				player.x-=5;
			}
		}
	}
	// If user pressed right then move the user to the right
	else if(value[7]==0){
		if(player.x<625){
			drawblack(player.x, player.y, 5, 50);
			player.x+=5;
		}
	}
	
	// If the user presses a then see if you can fire a beam(enemy can have at most 3 beams at once)
	// There is a delay between each beam that user can fire, user cannot fire if delay>0.
	if(value[8]==0 && (pbullets[0].beamstatus==0) && beamdelay == 0){
		pbullets[0].x=player.x+(75/2);
		pbullets[0].y=player.y - 30;
		pbullets[0].beamstatus=1;
		beamdelay = 50;
	}
	// 2nd beam that the user can fire
	else if(value[8]==0 && (pbullets[1].beamstatus==0) && beamdelay == 0){
		pbullets[1].x=player.x+(75/2);
		pbullets[1].y=player.y - 30;
		pbullets[1].beamstatus=1;
		beamdelay = 50;
	}
	// 3rd beam that the user can fire
	else if(value[8]==0 && (pbullets[2].beamstatus==0) && beamdelay == 0){
		pbullets[2].x=player.x+(75/2);
		pbullets[2].y=player.y - 30;
		pbullets[2].beamstatus=1;
		beamdelay = 50;
	}
	// If user presses start then pause the game
	if(value[3]==0){
		pauseCheck = 1;
	}
	//Check if enemies fire and then move beams
	enemyfire();
	updateBeams();
	//Check if beams hit something (ENEMIES CANNOT BE KILLED BY OTHER ENEMIES)
	detection();
}

/**
 * Function used to update the score
 * Takes the type of enemy killed as parameter
 * */
void updateScore(int type){
	// If pawn was killed increase score by 
	if(type == 1){
		score += 5;
	// If knight was killed increase score by 10
	}
	else if (type == 2){
		score += 10;
	}
	// If queen was killed increase score by 100
	else if (type == 3){
	score += 100;
	}
	// Draw the updated score onto screen
	drawScore();	
}

/**
 * Function to update counters, called after gamestate is drawn
 * */
void updateCounters(){
	// If beam delay > 0 , decrement by 1
	if(beamdelay > 0){
		beamdelay--;
	}
	//If column delays > 0, decrement the delays by 1
	if(column1delay>0){
		column1delay--;
	}
	if(column2delay>0){
		column2delay--;
	}
	if(column3delay>0){
		column3delay--;
	}
	if(column4delay>0){
		column4delay--;
	}
	if(column5delay>0){
		column5delay--;
	}
	if(allEnemyDelay>0){
		allEnemyDelay--;
	}
	// If cycle counter == 480, reset it to 0
	if (cyclecounter == 480){
		cyclecounter = 0;
	}
	// Otherwise increment it by one
	else{
		cyclecounter++;
	}
}

/**
 * Update the arrays of beams, if they are on the screen (beamstatus = 1) then update the
 * y values and draw black on parts where they were for animation
 * */
void updateBeams(){
	// Check array of user beams/bullets
	for(int i =0; i<3;i++){
		if(pbullets[i].beamstatus == 1){
			pbullets[i].y -= 6;
		}
		if((pbullets[i].y) < 40){
			drawblack(pbullets[i].x,(pbullets[i].y+6),3,30);
			pbullets[i].beamstatus=0;
			pbullets[i].y= (player.y-30);
		}	
	}
	// Check arrat of enemy beams/ bullets
	for(int j =0; j<3;j++){
		if(ebullets[j].beamstatus == 1){
			ebullets[j].y += 6;
		}
		if((ebullets[j].y) > 880){
			drawblack(ebullets[j].x,(ebullets[j].y-36),3,30);
			ebullets[j].beamstatus=0;
			ebullets[j].y= enemies[5].y;
		}	
	}
}

/**
 * Function that draws the beams onto the screen
 * */
void drawbeams(){
	// User beams/bullets
	for (int i =0; i<3; i++){
		// Check that beam should be printed onto the screen
		if(pbullets[i].beamstatus == 1){
			drawblack(pbullets[i].x,(pbullets[i].y+30),3,6);
			Pixel *pixel;
			pixel = malloc(sizeof(Pixel));
			pixel->color = 0xFFFFFFFF;
			pixel->x = (pbullets[i].x);
			pixel->y = (pbullets[i].y);
			for(int l=0;l<30;l++){
				for(int j=0;j<3;j++){
					drawPixel(pixel);
					pixel->x++;
				}
				pixel->x=pbullets[i].x;
				pixel->y++;
			}
			free(pixel);
		}
	}
	// Enemy beams/bullets
	for (int i =0; i<3; i++){
		// Check that beam should be printed onto the screen
		if(ebullets[i].beamstatus == 1){
			drawblack(ebullets[i].x,(ebullets[i].y-36),3,6);
			Pixel *pixel;
			pixel = malloc(sizeof(Pixel));
			pixel->color = 0xFFFFFFFF;
			pixel->x = (ebullets[i].x);
			pixel->y = (ebullets[i].y-30);
			for(int l=0;l<30;l++){
				for(int j=0;j<3;j++){
					drawPixel(pixel);
					pixel->x++;
				}
				pixel->x=ebullets[i].x;
				pixel->y++;
			}
			free(pixel);
		}
	}
}
/**
 * Method used to draw the game onto the screen
 * */
void draw(){
	image pawnToDraw = {50 ,50 ,pawnImg};
	image knightToDraw = {50, 50, knightImg};
	image queenToDraw = {50, 50, queenImg};
	image tankToDraw = {75, 40, tankImg};
	//-------draw enemies
	for(int i=0;i<20;i++){
		// If enemy is not alive then don't draw it
		if(enemies[i].life < 1){
			continue;
		}
		/* If enemies are moving to the right, draw black on the left most	
		 * column that they were on*/
		if(cyclecounter<120){
			drawblack(enemies[i].x-1, enemies[i].y, 1, 50);
		}
		/* If enemies moving down  draw above them, where they were before */
		else if(cyclecounter==120){
			drawblack(enemies[i].x, enemies[i].y-30, 50, 30);
		}
		/* If enemies moving left, draw black on the right most	
		 * column that they were on*/
		else if(cyclecounter<360){
			drawblack(enemies[i].x+50, enemies[i].y, 1, 50);
		}
		/* If enemies moving down  draw above them, where they were before */
		else if(cyclecounter==360){
				drawblack(enemies[i].x, enemies[i].y-30, 50, 30);
		}
		/* If enemies are moving to the right, draw black on the left most	
		 * column that they were on*/
		else if(cyclecounter<480){
				drawblack(enemies[i].x-1, enemies[i].y, 1, 50);
		}
		/* If enemies moving down  draw above them, where they were before */
		else if(cyclecounter==480){
			drawblack(enemies[i].x, enemies[i].y-30, 1, 50);
		}
		// If enemy is a pawn
		if(enemies[i].type==1){
			drawimage(enemies[i].x, enemies[i].y, pawnToDraw);
		}
		// If enemy is a knight
		else if(enemies[i].type==2)
			drawimage(enemies[i].x, enemies[i].y, knightToDraw);
		// If enemy is a queen
		else
			drawimage(enemies[i].x, enemies[i].y, queenToDraw);
	}
	//------draw tank
	drawimage(player.x, player.y, tankToDraw);
	//------draw beam
	drawbeams();
	//------draw obstacles
	for(int k=0;k<27;k++){
		if(blocks[k].life == 1){
			drawBlock(blocks[k].x, blocks[k].y, 15, 15);
		}
	}
	// Draw red barrier line
	drawBarrierLine();
}

/**
 * Function used to initialize the enemies
 * */
void enemyinitialize(){
	int tempx = 125, tempy = 50;
	int counter = 0;
	for(int i=0;i<4;i++){
		for(int j=0; j<5;j++){
			enemies[counter].x = tempx;
			enemies[counter].y = tempy;
			enemies[counter].type = 1;
			enemies[counter].life = 1;
			tempx+=100;
			counter++;
		}
		tempx = 125;
		tempy+=100;
	}
	// The queens
	enemies[1].type = 3;
	enemies[1].life = 3;
	enemies[3].type = 3;
	enemies[3].life = 3;
	// The knights
	for(int i=5;i<10;i++){
		enemies[i].type = 2;
		enemies[i].life = 2;
	}
}

void blockinitialize(){	//We initialize blocks or called obstacles here.
	int xd = 50, yd = 750;
	int count = 0;
	for(int i=0; i < 3; i++){
		for(int k=0; k<9; k++){
			if(k < 5){
				xd += 15;
			}
			else if (k == 5){
				yd -= 15;
				xd -= 15;}
			else if (k < 8){
				xd -= 15;
			}
			else{
				yd -= 15;
				xd += 15;
			}
				
				
			blocks[count].x = xd;
			blocks[count].y = yd;
			//yd = 750;
			blocks[count].life = 1;
			//xd += 15;
			count++;
		}
		yd = 750;
		xd += 200;
	}
	
}

/**
 * Function used to detect if something related to game happens.
 * */ 
void detection(){
	for(int i=0; i<3; i++){
		/**
		 * Check if user shot an enemy.
		 * */
		if(pbullets[i].beamstatus == 1){
			for (int j =0; j<20; j++){
				// If enemy is dead, skip forward
				if(enemies[j].life<1){
					continue;
				}
				// If enemy not within the beam x boundaries
				if((pbullets[i].x > enemies[j].x + 50) || (pbullets[i].x+3 < enemies[j].x)){
					continue;
				}
				// If enemy not within the beam y boundaries
				if((pbullets[i].y > enemies[j].y+50) ||(pbullets[i].y+30 < enemies[j].y) ){
					continue;
				}
				// the above fails then a beam is inside the boundaries of an enemy
				pbullets[i].beamstatus = 0;						// Remove beam
				drawblack(pbullets[i].x,pbullets[i].y,3,36);
				pbullets[i].y = player.y-30;					// Reset beam y value
				enemies[j].life-=1;								// Decrase life of enemy by 1
				// If enemy is dead
				if(enemies[j].life<1){
					// Check if they were killed just as they moved down(clear larger section if yes)
					if(cyclecounter==480||cyclecounter==120||cyclecounter==360){
					drawblack(enemies[j].x-1,enemies[j].y-31,52,52);
					}
					// Otherwise clear the sot where enemy was normally
					drawblack(enemies[j].x-1,enemies[j].y-1,52,52);
					updateScore(enemies[j].type);
				}
			}
			/**
			 * Check if the user shot an obstacle
			 * */
			for (int k =0; k<27; k++){
				if(blocks[k].life<1){
					continue;
				}
				// Check x positions
				if((pbullets[i].x > blocks[k].x + 15) || (pbullets[i].x+3 < blocks[k].x)){
					continue;
				}
				// Check y positions
				if((pbullets[i].y > blocks[k].y+15) ||(pbullets[i].y+30 < blocks[k].y) ){
					continue;
				}
				// If beam is in block boundaries
				pbullets[i].beamstatus = 0;
				drawblack(pbullets[i].x,pbullets[i].y,3,36);
				pbullets[i].y = player.y-30;
				blocks[k].life-=1;
				// If block is dead then remove from screen
				if(blocks[k].life<1){
					drawblack(blocks[k].x,blocks[k].y,15,15);
				}
			}
		}
	}
	for(int i=0; i<3; i++){
		/**
		 * Check if enemy shot the player.
		 * */
		if(ebullets[i].beamstatus == 1){
			// Check bullet x values
				if((ebullets[i].x > player.x + 75) || (ebullets[i].x+3 < player.x)){
					continue;
				}
				// Check bullet y valuese
				if((ebullets[i].y-30 > player.y+40) ||(ebullets[i].y < player.y) ){
					continue;
				}
				// If bullet x and y values are overlapping with the player
				ebullets[i].beamstatus = 0;
				drawblack(ebullets[i].x,ebullets[i].y-36,3,36);
				ebullets[i].y = player.y-30;
				player.life-=1; // Decrease life by one
				updateLives(); // Update lives
				// If player loses all lives
				if(player.life < 1){
					drawblack(player.x,player.y,75,40);
					status = 4;
				}
			/**
			 * Check if the enemy shot an obstacle
			 * */
		}
	}
	for(int i =0; i<3; i++){
		for (int k =0; k<27; k++){
					if(blocks[k].life<1){
						continue;
					}
					// Check bullet x values
					if((ebullets[i].x > blocks[k].x + 15) || (ebullets[i].x+3 < blocks[k].x)){
						continue;
					}
					// Check bullet y values
					if((ebullets[i].y-30 > blocks[k].y+15) ||(ebullets[i].y < blocks[k].y) ){
						continue;
					}
					// If bullet overlaps with the boundaries of a block
					ebullets[i].beamstatus = 0;
					drawblack(ebullets[i].x,ebullets[i].y-36,3,36);
					ebullets[i].y = player.y-30;
					blocks[k].life-=1;
					// If block is hit then remove from screen.
					if(blocks[k].life<1){
						drawblack(blocks[k].x,blocks[k].y,15,15);
					}
				}
			}
}

/**
 * Function used to update indexOfLowest
 * Puts index of enemy that is the lowest in each coulmn
 * E.g. if the enemy[19] is lowest in column 5 then indexOfLowest[4] is set to 19
 * */
void trackIndexOfLowest(){
	for(int i =0; i<5;i++){
		indexOfLowest[i]=0;
}
	for(int i =0; i<5;i++){
		// Check enemy on 4th  row
		if(enemies[19-i].life > 0){
			indexOfLowest[4-i] = 19-i;
		}
		// Check enemy on 3rd row
		else if(enemies[19-i-5].life > 0){
			indexOfLowest[4-i] = 19-i-5;
		}
		// Check enemy on 2nd row
		else if(enemies[19-i-10].life > 0){
			indexOfLowest[4-i] = 19-i-10;
		}
		// Check enemy on top row
		else if(enemies[19-i-15].life > 0){
			indexOfLowest[4-i] = 19-i-15;
		}
		// Set to 50 as all enemies are dead in the column
		else{
			indexOfLowest[4-i] = 50;
		}
	}
}

/**
 * Check height of the lowest row of enemies and check if they are at or below the red beam
 * and if they are then changes status of game to lose.
 * */
void checkEnemyHeight(){
	for(int i=0; i<5; i++){
		if(indexOfLowest[i] != 50){
			// Check if the lowest in each column is at or below the beam
			if(enemies[indexOfLowest[i]].y + 50 >= 720){
				status = 4; 
			}
		}
	}
	
}

/**
 * Checks to see if enemy will fire, and if yes, which column will fire
 * */
void enemyfire(){
	enemyfired = 0;
	trackIndexOfLowest();
	int columnToFire = ((cyclecounter*3) % 5);  // Column that will fire a beam/bullet
	int enemyNum = indexOfLowest[columnToFire]; // Lowest enemy in the column
	// If enemy is alive (IndexOfLowest[columnToFire] is 50 if all enemies in column is dead
	if(enemyNum <20 && enemyNum >= 0){
		// Double check that the enemy in that index is alive
		if(enemies[enemyNum].life > 0){
			// Check each beam, if it is able to be fired
			// Check column and all enemy delay to see if that column can fire
			for (int j = 0; j<3; j++){
				if(columnToFire == 0 && column1delay != 0){
					continue;
				}
				if(columnToFire == 1 && column2delay != 0){
					continue;
				}
				if(columnToFire == 2 && column3delay != 0){
					continue;
				}
				if(columnToFire == 3 && column4delay != 0){
					continue;
				}
				if(columnToFire == 4 && column5delay != 0){
					continue;
				}
				// Check if beam is able to be fired, it it can then change the 
				// beam status to 1 and then set enemyfired to 1 so it only shoots 1
				// Then set delays
				if(ebullets[j].beamstatus== 0 && enemyfired == 0 && allEnemyDelay == 0){
					ebullets[j].x=enemies[enemyNum].x+(25);
					ebullets[j].y=enemies[enemyNum].y +80;
					ebullets[j].beamstatus=1;
					allEnemyDelay = 20;
					enemyfired = 1;
					if(columnToFire == 0){
						column1delay = 30;
					}
					if(columnToFire == 1){
						column2delay = 30;
					}
					if(columnToFire == 2){
						column3delay = 30;
					}
					if(columnToFire == 3){
						column4delay = 30;
					}
					if(columnToFire == 4){
						column5delay = 30;
				}
			}
		}
	}
}
}


#include <mmbackground.h>   // h file for main menu background image
#include <arrow.h>          // h file for arrow image used in main menu
#include <pawn.h>           // h file for pawn image
#include <knight.h>         // h file for knight image
#include <queen.h>          // h file for queen image
#include <tank.h>           // h file for tank/player image
#include <top.h>            // h file for in-game image showing score and lives text
#include <gameMenu.h>       // h file for pause menu image
#include <0.h>              // h files for images of numbers 0 to 9 used to calculate the score
#include <1.h>              
#include <2.h>              
#include <3.h>              
#include <4.h>
#include <5.h>
#include <6.h>
#include <7.h>
#include <8.h>
#include <9.h>
#include <winScreen.h>      // h file for screen shown when user wins
#include <loseScreen.h>     // h file for screen show when user loses 

// contains references to h files for all images used
// image struct contains width and height of image in pixels, and bitmap array. 
typedef struct{
  unsigned int		width;
  unsigned int		height;
  unsigned char*	image_pixels;
}image;



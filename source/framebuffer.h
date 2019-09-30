#ifndef FRAMEBUFF
#define FRAMEBUFF

/* definitions */
struct fbs {
	unsigned int *fptr;		// framebuffer pointer
	int x; 					// screen width
	int y;					// screen height
};
	
struct fbs initFbInfo(void);


#endif

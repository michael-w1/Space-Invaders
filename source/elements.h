// This is the struct for enemy, x and y are its screen coordinates. Pawn has 1 life, Knight has 1 life, and Queen has 3 life.
// Type = 1 is Pawn, Type = 2 is Knight, Type = 3 is Queen. 
typedef struct {
	int x;
	int y;
	int life;
	int type;
}enemy;

// struct for beam, x and y are screen coordinates. Beam status - is 1 if beam is on screen, otherwise it is 0
typedef struct{
	int x;
	int y;
	int beamstatus;
}pbeam;

// struct for tank. x and y are screem coordinates. The tank has 3 life.  
typedef struct{
	int x;
	int y;
	int life;
}tank;

// struct for block. x and y are screen coordinates. All blocks have 1 life.
typedef struct{
	int x;
	int y;
	int life;
}block; 

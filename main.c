/*
	12 Player Pong Clone
  	        by
          Slinga

  A to join\leave game
  Up, Down, Left, Right to move
  P1 Start to pause\unpause ball
  P1 Left\Right Triggers to slowdown\speedup game


*/

#include "sgl.h" // Required for basic sgl functions

struct ball
{
  char shape[2]; // The shape of the ball
  int dx;
  int dy;
  int x;
  int y;
  int active; // Is the ball moving or not
};

struct paddle
{
  int ID;
  char shape[2]; // the shape of the paddle
  int dx;
  int dy;
  int x;
  int y;
  int active; // Is this player playing or not
};

// Controller assignments
static Uint16 pad_asign[]=
{
	PER_DGT_KU,
	PER_DGT_KD,
	PER_DGT_KR,
	PER_DGT_KL,
	PER_DGT_TA,
	PER_DGT_TB,
	PER_DGT_TC,
	PER_DGT_ST,
	PER_DGT_TX,
	PER_DGT_TY,
	PER_DGT_TZ,
	PER_DGT_TR,
	PER_DGT_TL,
};

void displayText(); // Displays the heading information
void initializePlayer(struct paddle* somePlayer, int controllerNum);
void initializeBall(struct ball* someBall, char ballShape);
void drawGrid(); // Draws the playing field
void drawBall(struct ball* someBall, struct paddle* players, Uint16* teamOne, Uint16* teamTwo); // the ball, erases old ball
void drawPaddle(struct paddle* somePlayer); // Draws the paddle, erases old paddle
void pressStart();

void ss_main(void)
{

	int i;
	Uint16 data, scoreA, scoreB;
	Sint16 slowdown;
	struct ball theBall;
	struct paddle players[12];

	// Initializing functions
	slInitSystem(TV_320x224, NULL, 1); // Initializes screen
	initializeBall(&theBall, '*');
	displayText();
	drawGrid(); // Draws the playing field box

	scoreA = scoreB = 0;
	slowdown = 3;

	do
	{
		// Did player 1 pause\unpause ball
		data = Smpc_Peripheral[0].data;
		if((data & pad_asign[7]) == 0)
		{
			if(theBall.active==0)
			{
				theBall.active = 1;
			}
			else
			{
				theBall.active = 0;
			}
		}

		// Did player decrease game speed
		if((data & pad_asign[12]) == 0)
		{
			slowdown++;
		}

		// Did player increase game speed
		if((data & pad_asign[11]) == 0)
		{
			slowdown--;
			if(slowdown<0)
			{
				slowdown = 0;
			}
		}



		// Check if players on tap one joined\left
		for(i = 0; i < 6; i++)
		{
		    data = Smpc_Peripheral[i].data;

		    if((data & pad_asign[4]) == 0)
		    {
				initializePlayer(&players[i], i);
		    }

			if(players[i].active==1)
			{
			    drawPaddle(&players[i]);
			}
		}

		// Check if players on tap two joined\left
		for(i = 6; i < 12; i++)
				{
				    data = Smpc_Peripheral[i+9].data;

				    if((data & pad_asign[4]) == 0)
				    {
						initializePlayer(&players[i], i+9);
				    }

					if(players[i].active==1)
					{
					    drawPaddle(&players[i]);

					}
		}

		if(theBall.active==1)
		{
			drawBall(&theBall, players, &scoreA, &scoreB);
		}

		slSynch(); // You won't see anything without this!!

		for(i = 0; i < slowdown; i++)
		{
			slSynch(); // Slow down
		}

	}while(1);


	return;
}

void initializePlayer(struct paddle* somePlayer, int controllerNum)
{


   // Create player
   if(somePlayer->active!=1)
   {
	somePlayer->shape[0] = (char*)14;
	somePlayer->shape[1] = '\0';

     if(controllerNum > 5)
     {
	//Player is in second port
	somePlayer->x = (3*(controllerNum-9))+3;
	somePlayer->y = controllerNum+1;
	somePlayer->ID = controllerNum;
	somePlayer->active = 1;
     }
     else
     {
	// Player is in first port
	somePlayer->x = (3*controllerNum)+3;
	somePlayer->y = controllerNum+10;
	somePlayer->ID = controllerNum;
	somePlayer->active = 1;
     }
   }
   else // destroy player
   {
	// erase old paddle position
	slPrint(" ", slLocate(somePlayer->x, somePlayer->y));
	slPrint(" ", slLocate(somePlayer->x, somePlayer->y+1));
	slPrint(" ", slLocate(somePlayer->x, somePlayer->y+2));

	somePlayer->active = 0;
	somePlayer->x = -100;
	somePlayer->y = -100;
   }
}

void initializeBall(struct ball* someBall, char ballShape)
{
	someBall->shape[0] = ballShape;
	someBall->shape[1] = '\0';
	someBall->dx = 1;
	someBall->dy = 1;
	someBall->x = 20;
	someBall->y = 15;
	someBall->active = 0;

	// Draw the ball
	slPrint(someBall->shape, slLocate(someBall->x, someBall->y));
}

void drawGrid()
{
	int j;

	char temp[10];
	char top[40]; // The top of the playing field
	char bottom[40]; // The bottom of the playing field

	// Fill the arrays with '-'
	sprintf(temp, "%c", 21);
	for(j = 0; j<38; j++)
	{
		top[j] = temp[0];
		bottom[j] = temp[0];
	}

	// Corner pieces
	sprintf(temp, "%c", 23);
	top[0] = temp[0];

	sprintf(temp, "%c", 24);
	top[37] = temp[0];

	sprintf(temp, "%c", 25);
	bottom[0] = temp[0];

	sprintf(temp, "%c", 26);
	bottom[37] = temp[0];

	top[38] = '\0';
	bottom[38] = '\0';

	// Draw the top and bottom borders
	slPrint(top, slLocate(1, 5));
	slPrint(bottom, slLocate(1, 25));

	// Draw the sides
	sprintf(temp, "%c", 22);
	for(j = 6; j<25; j++)
	{
		slPrint(temp, slLocate(1, j));
		slPrint(temp, slLocate(38, j));

	}

}

void drawBall(struct ball* someBall, struct paddle* players, Uint16* teamOne, Uint16* teamTwo)
{
	int j;
	char temp[5];

	// erase old ball position
	slPrint(" ", slLocate(someBall->x, someBall->y));

	// Check if you hit the side
	if(someBall->x + someBall->dx <= 1 || someBall->x + someBall->dx >= 38)
	{
		// Someone scored a goal!!
		if(someBall->dx > 0)
		{
			*teamOne += 1;
		}
		else
		{
			*teamTwo += 1;
		}

		slPrint("     ", slLocate(9,4));
		sprintf(temp, "%d", *teamOne);
		slPrint(temp, slLocate(9, 4));

		slPrint("     ", slLocate(36,4));
		sprintf(temp, "%d", *teamTwo);
		slPrint(temp, slLocate(36, 4));

		// Teleport ball to center
		someBall->dx *=-1;
		someBall->dy *=-1;
		someBall->x =20;
		someBall->y = 15;

	}
	else
	{
		someBall->x += someBall->dx;
	}

	// Check if you hit the ceiling
	if(someBall->y + someBall->dy <= 5)
	{
		someBall->dy *=-1;
		someBall->y = 7;
	}
	else if(someBall->y + someBall->dy >= 25)
	{
		someBall->dy *=-1;
		someBall->y = 23;
	}
	else
	{
		someBall->y += someBall->dy;
	}

	for(j = 0; j < 12; j++)
	{
		if(players[j].active==1)
		{
			// Check if you hit an active player
			if((someBall->y + someBall->dy >= players[j].y && someBall->y + someBall->dy <= players[j].y+2) && someBall->x + someBall->dx == players[j].x)
			{
				someBall->dx *=-1;
			}
		}
	}

	// draw new ball position
	slPrint(someBall->shape, slLocate(someBall->x, someBall->y));
}

void drawPaddle(struct paddle* somePaddle)
{
	Uint16 data;

	// Checks the controller for input
	data = Smpc_Peripheral[somePaddle->ID].data;

	// Check vertical movement
	if((data & pad_asign[1])== 0)
     	{
  		somePaddle->dy = 1;
	}
	else if((data & pad_asign[0])== 0)
	{
		somePaddle->dy = -1;
	}

	// Check horizontal movement
	if((data & pad_asign[2])== 0)
     	{
  		somePaddle->dx = 1;
	}
	else if((data & pad_asign[3])== 0)
	{
		somePaddle->dx = -1;
	}

	// erase old paddle position
	slPrint(" ", slLocate(somePaddle->x, somePaddle->y));
	slPrint(" ", slLocate(somePaddle->x, somePaddle->y+1));
	slPrint(" ", slLocate(somePaddle->x, somePaddle->y+2));

	// Check if you hit the side
	if(somePaddle->x + somePaddle->dx <= 1 || somePaddle->x + somePaddle->dx >= 38)
	{
		// don't move in x axis, you'll hit the border!!
	}
	else
	{
		somePaddle->x += somePaddle->dx;
	}
	// Check if you hit the ceiling
	if(somePaddle->y + somePaddle->dy <= 5 || somePaddle->y + somePaddle->dy >= 23)
	{
		// don't move in y axis, you'll hit the border!!
	}
	else
	{
		somePaddle->y += somePaddle->dy;
	}

	// draw new paddle position
	slPrint(somePaddle->shape, slLocate(somePaddle->x, somePaddle->y));
	slPrint(somePaddle->shape, slLocate(somePaddle->x, somePaddle->y+1));
	slPrint(somePaddle->shape, slLocate(somePaddle->x, somePaddle->y+2));

	somePaddle->dx = 0;
	somePaddle->dy = 0;

	return;
}

void displayText()
{
  slPrint("12 Player Pong Clone by Slinga", slLocate(1,1));
  slPrint("Press A to join or leave game.", slLocate(1,2));
  slPrint("Team A: 0", slLocate(1, 4));
  slPrint("Team B: 0", slLocate(28,4));
}

// Displays the text "Press Start" and waits for the user to hit start
void pressStart()
{
   Uint16 start;
   do{
		start = Smpc_Peripheral[0].data; // Checks if start button has been pressed
		slPrint("Press Start", slLocate(15, 25));
		slSynch();
		slSynch();
		slSynch();

    }while((start & pad_asign[7])!= 0);

}

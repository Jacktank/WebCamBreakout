#include <iostream>
#include <stdio.h>

using namespace std;

class GameLogic{
public:
	static const int BALL_RADIUS = 25;
	static const int WIN_WIDTH = 640, WIN_HEIGHT = 480;
	static const int PADDLE_WIDTH = 100, PADDLE_HEIGHT = 15;
	static const int BRICK_WIDTH = 64, BRICK_HEIGHT = 32;

	//Boolean array for bricks where true -  not destroyed
	bool bricks[40];
	//GameState	
	enum GameStates {RUNNING, PAUSE, GAMEOVER, WIN};
	GameStates GameState;
	// Auxiliar variables score, number of lives and number of bricks on screen
	int score, lives, noBricks;
	//Position and Speed variables for the game elements
	float ballX, ballY, lastBX, lastBY;
	float ballSpeedX, ballSpeedY;
	float paddleX, paddleY;
			
	GameLogic(){
		restart();	
	}

	//Resets game variables
	void restart(){
		paddleX = WIN_WIDTH/2;
		paddleY = WIN_HEIGHT - (2*PADDLE_HEIGHT); 
		ballY = paddleY - 40;
		ballX = 320;
		lastBX=ballX;
		lastBY=ballY;
		ballSpeedX = 10;
		ballSpeedY = -10;
		for(int i=0; i < 40; i++){
			bricks[i] = true;
		}
		score=0;
		lives=3;
		noBricks = 40; 
		GameState = RUNNING;
	}

	void update(float x, float y){
		//Check for win condition
		if(noBricks<=0){GameState=WIN;}

		if(GameState == RUNNING){
			//Save last ball position
			lastBX=ballX;
			lastBY=ballY;

			// Set paddleX to the projected equivalent of players hand X   			
			if(x >=0 && x <= WIN_WIDTH){
				paddleX = x;
			}

			// Increment Ball Coordinates
			ballX += ballSpeedX;
			ballY += ballSpeedY;

			//Check Ball Collisions with the walls 
			if(ballX <= 0 || ballX >= WIN_WIDTH - BALL_RADIUS){
				ballSpeedX *= -1;
				rbBallPos();
			}
			//Check Ball Collissions with the ceiling
			if(ballY <= 0){
				ballSpeedY *= -1;
				rbBallPos();
			}
			//Check Ball Collisions with the floor
			else if(ballY > WIN_HEIGHT){
				lives--;
				// Reset ball or gameover based on the remaining number of lives
				if(lives > 0){
					ballY = paddleY - 40;
					ballX = 320 ;
					ballSpeedX = 10;
					ballSpeedY = -10;
				}
				else{GameState = GAMEOVER;}	
			}
			
			//Check for collsions with visible bricks
			for(int i=0; i < 40;i ++){
				int c = checkBrickCollision(i);	
				if(c!=0){
					bricks[i]=false;
					if(c>=1){ballSpeedY *= -1;}
					else if(c==-1 || c==2){ballSpeedX *= -1;}	
					score+=100;
					noBricks--;
					rbBallPos();
					break;	
				}											
			}		

			//Test for collision between ball and the paddle
			int col = rectangleCollision(paddleX - (PADDLE_WIDTH/2), paddleY - (PADDLE_HEIGHT/2), PADDLE_WIDTH, PADDLE_HEIGHT,
				ballX, ballY, BALL_RADIUS, BALL_RADIUS);
			if(col==1){ballSpeedY*=-1;rbBallPos();}
			else if(col==-1){ballSpeedX*=-1;rbBallPos();}
			else if(col==2){
				ballSpeedY*=-1;
				ballSpeedX*=-1;
				rbBallPos();
			}	

		}
	}

	int checkBrickCollision(int idx){
		if(bricks[idx]){
			// Coordinates of the top left coordinate of brick		
			float brickX = (idx%10) * BRICK_WIDTH;
			float brickY = (idx/10) * BRICK_HEIGHT;

			//return ballRectangleIntersect(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT);
			return rectangleCollision(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT,
			 			ballX, ballY, BALL_RADIUS, BALL_RADIUS);	
		}
		else{
			return 0;	
		}  
	}

	//Roll back ball position and update based on speed
	void rbBallPos(){
		ballX = lastBX;
		ballY = lastBY;
		ballX += ballSpeedX;
		ballY += ballSpeedY;	 
	}

	//Check for collision between 2 rectangles
	int rectangleCollision(float AX, float AY, float AW, float AH,
			 float BX, float BY, float BW, float BH){
		
		float AX2 = AX+AW;
		float AY2 = AY+AH;
		float BX2 = BX+BW;
		float BY2 = BY+BH; 

		if(AX <= BX2 && BX <= AX2 && AY <= BY2 && BY <= AY2){
			if(AX <= BX2 && BX <= AX2){
				return 1;
			}
			else if( AY <= BY2 && BY <= AY2 ){
				return -1;
			}
			else return 2;
		}

		return 0;
	}

};
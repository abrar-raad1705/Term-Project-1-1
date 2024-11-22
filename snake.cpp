# include "iGraphics.h"
#include <stdlib.h>
#include <time.h>

#define window_width 800
#define window_height 680
#define game_width 800
#define game_height 600
#define snake_sr 10 // snake_sr -> snake segment radius
#define step snake_sr * 2

#define up 0
#define down 1
#define left 2
#define right 3

int direction = right;
int snake_init_x = game_width/2 + snake_sr, snake_init_y = game_height/2 + snake_sr;
int food_x, food_y;

const int max_len = 100000; // Maximum length of the snake
int snakeX[max_len];
int snakeY[max_len];
int curren_len = 3; // Starting length of the snake

int score = 0;
int is_bonus_active = 0;
int is_gameover = 0;
int bonus_start_time;
const int bonus_duration = 10000; // 5 seconds in milliseconds
int bonus_x, bonus_y;

int snake_speed = 200; // Initial speed (milliseconds)
const int speed_increment = 5;  //Decrease in milsec for normal food

void init_snake(); // Snake initialization
void gen_food();
void snake_move();
void snake_len_inc(); // Increase the length of the snake

void iDraw() {
    iClear();
    
	//Draw the background
    iSetColor(161,185,102);
	iFilledRectangle(0, 0, game_width, game_height);

	//Draw the score board
	iSetColor(125, 99, 63);
	iFilledRectangle(0, game_height, game_width, window_height-game_height);

	//Display the score
	iSetColor(255, 255, 255);
	char score_text[20];
	sprintf(score_text, "Score: %d", score);
	iText(10, game_height+10, score_text, GLUT_BITMAP_TIMES_ROMAN_24);
	
	//Draw the grid
	iSetColor(200, 200, 200);
    // Draw vertical lines
    for(int x = 0; x <= game_width; x += step){
        iLine(x, 0, x, game_height);
    }
    // Draw horizontal lines
    for(int y = 0; y <= game_height; y += step){
        iLine(0, y, game_width, y);
    }
	
	if(is_gameover){
		iSetColor(0, 0, 0); // Black background
        iFilledRectangle(0, 0, game_width, game_height);
		iSetColor(255, 0, 0);
		iText(game_width/2-50, game_height/2, "Game Over", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else{
		//Draw the snake
		iSetColor(44, 51, 29);
		for(int i=0; i<curren_len; i++){
			iFilledCircle(snakeX[i], snakeY[i], snake_sr, 100);
		}

		//Draw the regular food
		iSetColor(191, 63, 63);
		iFilledCircle(food_x, food_y, snake_sr, 1000);
		iSetColor(255, 204, 51);
		iCircle(food_x, food_y, snake_sr+1, 1000);

		//Draw the bonus food
		if(is_bonus_active){
			iSetColor(0, 0, 255);
			iFilledCircle(bonus_x, bonus_y, snake_sr, 1000);
			iSetColor(255, 204, 51);
			iCircle(bonus_x, bonus_y, snake_sr+1, 1000);
		}
	}
}

void iMouseMove(int mx, int my) {

}

void iMouse(int button, int state, int mx, int my) {
	
}

int is_paused = 0; // Global variable to track pause state

void iKeyboard(unsigned char key) {
    if (key == 'q' || key == 'Q') {
        exit(0);
    }
    if (key == ' ') {
        if (is_paused) {
            iResumeTimer(0);
            is_paused = 0;
        } else {
            iPauseTimer(0);
            is_paused = 1;
        }
    }
}

void iSpecialKeyboard(unsigned char key) {
	if(key == GLUT_KEY_UP && direction != down){
		direction = up;
	}
	else if(key == GLUT_KEY_DOWN && direction != up){
		direction = down;
	}
	else if(key == GLUT_KEY_LEFT && direction != right){
		direction = left;
	}
	else if(key == GLUT_KEY_RIGHT && direction != left){
		direction = right;
	}
}


int main() {
	srand(time(0));
	init_snake();
	gen_food();
	iSetTimer(snake_speed, snake_move);
	iInitialize(window_width, window_height, "Snake Xenzia V1.0");
	return 0;
}

void gen_food(){ // generate food for the snake
	int is_overlap = 1;
	while(is_overlap){
		is_overlap = 0;
		food_x = (rand() % (game_width / snake_sr));
		if(food_x % 2 == 0) food_x += 1;
		food_x *= snake_sr;
		food_y = (rand() % (game_height / snake_sr));
		if(food_y % 2 == 0) food_y += 1;
		food_y *= snake_sr;

		for(int i = 0; i < curren_len; i++){
			if(snakeX[i] == food_x && snakeY[i] == food_y){
				is_overlap = 1;
				break;
			}
		}
	}
}

void snake_move(){
		if(is_gameover == 0){
			for(int i = curren_len; i > 0; i--){
			snakeX[i] = snakeX[i-1];
			snakeY[i] = snakeY[i-1];
		}

		if(direction == up){
			snakeY[0] += step;
			if(snakeY[0] > game_height-snake_sr){
				snakeY[0] = 0+snake_sr;
			}
		}
		else if(direction == down){
			snakeY[0] -= step;
			if(snakeY[0] < 0+snake_sr){
				snakeY[0] = game_height-snake_sr;
			}
		}
		else if(direction == left){
			snakeX[0] -= step;
			if(snakeX[0] < 0+snake_sr){
				snakeX[0] = game_width-snake_sr;
			}
		}
		else if(direction == right){
			snakeX[0] += step;
			if(snakeX[0] > game_width-snake_sr){
				snakeX[0] = 0+snake_sr;
			}
		}
		snake_len_inc();
	}
	//Handles bonus food disappearance
	if(is_bonus_active){
		if((clock() - bonus_start_time) > bonus_duration){
			is_bonus_active = 0;
		}
	}

	// Check for self-collision
    for(int i=1; i<curren_len; i++){
		if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]){
			is_gameover = 1;
			break;
			}
		}
	}

void init_snake(){
	for(int i = 0; i < curren_len; i++){
		snakeX[i] = snake_init_x - i * step;
		snakeY[i] = snake_init_y;
	}
}

void snake_len_inc(){
	if(snakeX[0] == food_x && snakeY[0] == food_y){
		score += 10;
		curren_len++;
		gen_food();

		if(score % 50 == 0 && is_bonus_active == 0){
			is_bonus_active = 1;
			bonus_start_time = clock();
			bonus_x = (rand() % (game_width / snake_sr));
			if(bonus_x % 2 == 0) bonus_x += 1;
			bonus_x *= snake_sr;
			bonus_y = (rand() % (game_height / snake_sr));
			if(bonus_y % 2 == 0) bonus_y += 1;
			bonus_y *= snake_sr;
		}
	}
	if(is_bonus_active){
		if(snakeX[0] == bonus_x && snakeY[0] == bonus_y){
			score += 30;
			curren_len += 3;
			is_bonus_active = 0;
		}
	}
}
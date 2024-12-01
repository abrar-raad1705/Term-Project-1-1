# include "iGraphics.h"
#include <stdlib.h>
#include <time.h>

#define window_width 800
#define window_height 680
#define game_width 800
#define game_height 600
#define snake_sr 10 // snake_sr -> snake segment radius
#define step snake_sr * 2

int is_gameover = 0;
int is_paused = 0;
int snake_speed = 200; // Initial speed
int snake_speed_inc_k = 200;
int score = 0;

#define menu 0
#define start_game 1
#define high_score 2
#define game_type 3
#define controls 4

#define classic 0
#define classic_revisited 1
#define box 2
#define campaign 3

int game_state = 0;
int play_state = 0;

#define up 0
#define down 1
#define left 2
#define right 3
int direction = right;

int food_x, food_y;
int bonus_x, bonus_y;
int signX, signY;

const int max_len = (game_width/step)*(game_height/step); // Maximum length of the snake
int snakeX[max_len];
int snakeY[max_len];
int current_len = 3; // Starting length of the snake

int is_auto_snake_active = 0;
int auto_snakeX[max_len];
int auto_snakeY[max_len];
int auto_snake_current_len = current_len;
int border;

int food_eat_cnt = 0;
int is_bonus_active = 0;
int bonus_start_time;
int bonus_duration = 7000;
int bonus_activation_k = 5;

void game_start(); // Resets the game
void init_snake(); // Initializes snake position

void snake_move(); // Moves the snake
int s = iSetTimer(snake_speed, snake_move);

void gen_food(); // Generates regular food
void gen_bonus_food(); // Generates bonus food
void snake_len_inc(); // Increase the length of the snake
void collision(); // Handles collision detection
void init_auto_snake(); // Initializes auto snake

void auto_snake_move(); // Moves the auto snake
int as = iSetTimer(snake_speed, auto_snake_move);

void iDraw() {
    iClear();

	if(game_state == menu){
        // Draw the menu background
        iSetColor(44, 51, 29);
        iFilledRectangle(0, 0, window_width, window_height);
		iShowBMP(window_width/2 - 127, window_height/2, "game_logo.bmp");

        // Draw the title
        iSetColor(245, 245, 220);
        iText(window_width / 2 - 130, window_height/2 - 30, "Old Snake Game Revisited", GLUT_BITMAP_TIMES_ROMAN_24);

        // Draw menu options
        iSetColor(245, 245, 220);

        // Start Game
        iRectangle(window_width / 2 - 100, window_height/2 - 125, 200, 40);
        iText(window_width / 2 - 45, window_height/2 - 110, "Start Game", GLUT_BITMAP_HELVETICA_18);

        // High Score
        iRectangle(window_width / 2 - 100, window_height/2 - 185, 200, 40);
        iText(window_width / 2 - 45, window_height/2 - 170, "High Score", GLUT_BITMAP_HELVETICA_18);

        // Game Type
        iRectangle(window_width / 2 - 100, window_height/2 - 245, 200, 40);
        iText(window_width / 2 - 45, window_height/2 - 230, "Game Type", GLUT_BITMAP_HELVETICA_18);

        // Controls
        iRectangle(window_width / 2 - 100, window_height/2 - 305, 200, 40);
        iText(window_width / 2 - 37, window_height/2 - 290, "Controls", GLUT_BITMAP_HELVETICA_18);

		iText(game_width/2+250, 10, "Press 'Q' to exit", GLUT_BITMAP_8_BY_13);
    }

	else if(game_state == game_type){
        // Draw the game type selection menu
        iSetColor(44, 51, 29);
        iFilledRectangle(0, 0, window_width, window_height);

        // Draw the title
        iSetColor(245, 245, 220);
        int title_y = window_height / 2 + 150;
        iText(window_width / 2 - 60, title_y, "Game Type", GLUT_BITMAP_TIMES_ROMAN_24);

        // Game Types
        iSetColor(245, 245, 220);

        // Classic
        int classic_y = title_y - 100;
        iRectangle(window_width / 2 - 100, classic_y, 200, 40);
        iText(window_width / 2 - 30, classic_y + 15, "Classic", GLUT_BITMAP_HELVETICA_18);

        // Classic Revisited
        int classicRevisited_y = classic_y - 60;
        iRectangle(window_width / 2 - 100, classicRevisited_y, 200, 40);
        iText(window_width / 2 - 67, classicRevisited_y + 15, "Classic Revisited", GLUT_BITMAP_HELVETICA_18);

        // Box
        int box_y = classicRevisited_y - 60;
        iRectangle(window_width / 2 - 100, box_y, 200, 40);
        iText(window_width / 2 - 17, box_y + 15, "Box", GLUT_BITMAP_HELVETICA_18);

        // Campaign
        int campaign_y = box_y - 60;
        iRectangle(window_width / 2 - 100, campaign_y, 200, 40);
        iText(window_width / 2 - 40, campaign_y + 15, "Campaign", GLUT_BITMAP_HELVETICA_18);
    }

	else if(game_state == controls){
		// Draw the controls menu
		iSetColor(44, 51, 29);
		iFilledRectangle(0, 0, window_width, window_height);

		// Draw the title
		iSetColor(245, 245, 220);
		int title_y = window_height / 2 + 150;
		iText(window_width / 2 - 45, title_y, "Controls", GLUT_BITMAP_TIMES_ROMAN_24);

		// Controls
		iSetColor(245, 245, 220);

		// Up
		int upY = title_y - 100;
		iShowBMP(window_width / 2 - 80, upY+5, "up.bmp");
		iText(window_width / 2 - 30, upY + 15, "Move Up", GLUT_BITMAP_HELVETICA_18);

		// Down
		int downY = upY - 60;
		iShowBMP(window_width / 2 - 80, downY+5, "down.bmp");
		iText(window_width / 2 - 30, downY + 15, "Move Down", GLUT_BITMAP_HELVETICA_18);

		// Left
		int leftY = downY - 60;
		iShowBMP(window_width / 2 - 80, leftY+5, "left.bmp");
		iText(window_width / 2 - 30, leftY + 15, "Move Left", GLUT_BITMAP_HELVETICA_18);

		// Right
		int rightY = leftY - 60;
		iShowBMP(window_width / 2 - 80, rightY+5, "right.bmp");
		iText(window_width / 2 - 30, rightY + 15, "Move Right", GLUT_BITMAP_HELVETICA_18);

		iText(game_width/2+130, 10, "Press 'M' to go back to Menu", GLUT_BITMAP_9_BY_15);
	}

	else if(game_state == start_game){
		if(is_gameover == 0){
			//Draw the background
			iSetColor(161,185,102);
			iFilledRectangle(0, 0, game_width, game_height);

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
			
			//Draw the snake
			iSetColor(44, 51, 29);
			for(int i=0; i<current_len; i++){
				iFilledCircle(snakeX[i], snakeY[i], snake_sr, 100);
			}

			//Draw the regular food
			iSetColor(191, 63, 63);
			iFilledCircle(food_x, food_y, snake_sr, 1000);
			iSetColor(255, 204, 51);
			iCircle(food_x, food_y, snake_sr+1, 1000);

			// Draw the bonus food
			if(is_bonus_active == 1){
				iSetColor(0, 0, 255);
				iFilledCircle(bonus_x, bonus_y, snake_sr, 1000);
				iSetColor(255, 204, 51);
				iCircle(bonus_x, bonus_y, snake_sr+1, 1000);
			}

			//Draw the score board
			iSetColor(125, 99, 63);
			iFilledRectangle(0, game_height, game_width, window_height-game_height);

			iSetColor(255, 255, 255);
			char scoreText[100];
			sprintf(scoreText, "Score: %d", score);
			iText(10, game_height + 30, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

			if(play_state == classic_revisited){
				//Draw the auto snake
				if(is_auto_snake_active == 1){
					iSetColor(128, 0, 0);
					for(int i=0; i<auto_snake_current_len; i++){
						iFilledCircle(auto_snakeX[i], auto_snakeY[i], snake_sr, 100);
					}
				}

				//Draw the score board
				iSetColor(125, 99, 63);
				iFilledRectangle(0, game_height, game_width, window_height-game_height);

				iSetColor(255, 255, 255);
				char scoreText[100];
				sprintf(scoreText, "Score: %d", score);
				iText(10, game_height + 30, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
			}

			else if(play_state == box){
				//Draw the border
				iSetColor(160, 82, 45);
				// Draw top and bottom borders
				iFilledRectangle(0, 0, game_width, step);
				iFilledRectangle(0, game_height-step, game_width, step);
				// Draw left and right borders
				iFilledRectangle(0, 0, step, game_height);
				iFilledRectangle(game_width-step, 0, step, game_height);
			}

			else if(play_state == campaign){
				// Draw the border with gaps
				iSetColor(160, 82, 45);
				// Draw top and bottom borders
				iFilledRectangle(0, 0, 240, step);
				iFilledRectangle(560, 0, game_width, step);
				iFilledRectangle(0, game_height-step, 240, step);
				iFilledRectangle(560, game_height-step, game_width, step);
				// Draw left and right borders
				iFilledRectangle(0, 0, step, 160);
				iFilledRectangle(0, 440, step, game_height-440);
				iFilledRectangle(game_width-step, 0, step, 160);			
				iFilledRectangle(game_width-step, 440, step, game_height-440);

				//Draw the middle obstacles
				iFilledRectangle(100, 180, 600 ,step);
				iFilledRectangle(100, 400, 600, step);	
			}		
		}

		else{
			iSetColor(0, 0, 0); // Black background
			iFilledRectangle(0, 0, game_width, game_height);
			iSetColor(255, 0, 0);
			iText(game_width/2-60, game_height/2+50, "Game Over :(", GLUT_BITMAP_TIMES_ROMAN_24);
			char scoreText[100];
			sprintf(scoreText, "Score: %d", score);
			iText(game_width/2-40, game_height/2+10, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
			iSetColor(255, 255, 255);
			iText(game_width/2+130, 40, "Press 'M' to go back to Menu", GLUT_BITMAP_9_BY_15);
			iText(game_width/2+230, 10, "Press 'Q' to exit", GLUT_BITMAP_9_BY_15);
		}
	}
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouseMove(int mx, int my) {

}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(game_state == menu){
            // Start Game Button
            if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 125 && my <= window_height / 2 - 85){
                game_state = start_game;
				game_start();
            }
            // High Score Button
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 185 && my <= window_height / 2 - 145){
                game_state = high_score;
            }
            // Game Type Button
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 245 && my <= window_height / 2 - 205){
                game_state = game_type;
            }
            // Controls Button
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 305 && my <= window_height / 2 - 265){
                game_state = controls;
            }
        }
        // Game Type Selection Interaction
        else if (game_state == game_type) {
            int title_y = window_height / 2 + 150;
            int classic_y = title_y - 100;
            int classicRevisited_y = classic_y - 60;
            int box_y = classicRevisited_y - 60;
            int campaign_y = box_y - 60;

            // Classic Game Type
            if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= classic_y && my <= classic_y + 40){
                play_state = classic;
                game_state = menu;  // Return to main menu
            }
            // Classic Revisited Game Type
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= classicRevisited_y && my <= classicRevisited_y + 40){
                play_state = classic_revisited;
                game_state = menu;  // Return to main menu
            }
            // Box Game Type
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= box_y && my <= box_y + 40){
                play_state = box;
                game_state = menu;  // Return to main menu
            }
            // Campaign Game Type
            else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= campaign_y && my <= campaign_y + 40){
                play_state = campaign;
                game_state = menu;  // Return to main menu
            }
        }
    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key) {
	if (key == 'q' || key == 'Q') {
		if(game_state == start_game){
			game_state = menu;
		}
		else exit(0);
    }
	if(key == 'm' || key == 'M'){
		game_state = menu;
	}
    if(key == ' ' && is_paused == 0){
        is_paused = 1;
		iPauseTimer(s);
		iPauseTimer(as);
    }
    else if(key == ' ' && is_paused == 1){
        is_paused = 0;
		iResumeTimer(s);
		if(play_state == classic_revisited)iResumeTimer(as);
    }
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
	*/
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
	iInitialize(window_width, window_height, "Snake Game V1.1");
	return 0;
}

void init_snake(){
	snakeX[0] = game_width/2 + snake_sr, snakeY[0] = game_height/2 + snake_sr;
	for(int i = 1; i < current_len; i++){
		snakeX[i] = snakeX[0] * step;
		snakeY[i] = snakeY[0];
	}
}

void snake_move(){
    for(int i = current_len; i > 0; i--){
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
	collision();
}

void gen_food(){
    int is_overlap = 1;
    while(is_overlap == 1){
        is_overlap = 0;
        food_x = (rand() % (game_width / snake_sr));
        if(food_x % 2 == 0) food_x += 1;
        food_x *= snake_sr;

        food_y = (rand() % (game_height / snake_sr));
        if(food_y % 2 == 0) food_y += 1;
        food_y *= snake_sr;

        for(int i = 0; i < current_len; i++){
            if(snakeX[i] == food_x && snakeY[i] == food_y){
                is_overlap = 1;
                break;
            }
        }
		if(play_state == box){
			if(food_x == snake_sr || food_x == game_width - snake_sr || food_y == snake_sr || food_y == game_height - snake_sr){
				is_overlap = 1;
			}
		}

		if(play_state == campaign){
			if(food_x>=snake_sr && food_x<=240-snake_sr && (food_y == snake_sr || food_y == game_height-snake_sr)){
				is_overlap = 1;
			}
			else if(food_x>=560+snake_sr && food_x<=game_width-snake_sr && (food_y == snake_sr || food_y == game_height-snake_sr)){
				is_overlap = 1;
			}
			else if((food_x == snake_sr || food_x == game_width-snake_sr) && food_y>=snake_sr && food_y<=160-snake_sr){
				is_overlap = 1;
			}
			else if((food_x == snake_sr || food_x == game_width-snake_sr) && food_y>=440+snake_sr && food_y<=game_height-snake_sr){
				is_overlap = 1;
			}
			else if(food_x >= 100+snake_sr && food_x <= 700-snake_sr && food_y == 180+snake_sr){
				is_overlap = 1;
			}
			else if(food_x >= 100+snake_sr && food_x <= 700-snake_sr && food_y == 400+snake_sr){
				is_overlap = 1;
			}
		}
    }
}

void gen_bonus_food(){
	int is_overlap = 1;
    while(is_overlap == 1){
        is_overlap = 0;
        bonus_x = (rand() % (game_width / snake_sr));
        if(bonus_x % 2 == 0) bonus_x += 1;
        bonus_x *= snake_sr;

        bonus_y = (rand() % (game_height / snake_sr));
        if(bonus_y % 2 == 0) bonus_y += 1;
        bonus_y *= snake_sr;

        for(int i = 0; i < current_len; i++){
            if(snakeX[i] == bonus_x && snakeY[i] == bonus_y){
                is_overlap = 1;
                break;
            }
        }
		if(bonus_x == food_x && bonus_y == food_y){
			is_overlap = 1;
			break;
		}
		if(play_state == box){
			if(bonus_x == snake_sr || bonus_x == game_width - snake_sr || bonus_y == snake_sr || bonus_y == game_height - snake_sr){
				is_overlap = 1;
			}
		}

		if(play_state == campaign){
			if(bonus_x>=snake_sr && bonus_x<=240-snake_sr && (bonus_y == snake_sr || bonus_y == game_height-snake_sr)){
				is_overlap = 1;
			}
			else if(bonus_x>=560+snake_sr && bonus_x<=game_width-snake_sr && (bonus_y == snake_sr || bonus_y == game_height-snake_sr)){
				is_overlap = 1;
			}
			else if((bonus_x == snake_sr || bonus_x == game_width-snake_sr) && bonus_y>=snake_sr && bonus_y<=160-snake_sr){
				is_overlap = 1;
			}
			else if((bonus_x == snake_sr || bonus_x == game_width-snake_sr) && bonus_y>=440+snake_sr && bonus_y<=game_height-snake_sr){
				is_overlap = 1;
			}
			else if(bonus_x >= 100+snake_sr && bonus_x <= 700-snake_sr && bonus_y == 180+snake_sr){
				is_overlap = 1;
			}
			else if(bonus_x >= 100+snake_sr && bonus_x <= 700-snake_sr && bonus_y == 400+snake_sr){
				is_overlap = 1;
			}
		}
    }
}

int checker = 0;
void snake_len_inc(){
    if(snakeX[0] == food_x && snakeY[0] == food_y){
		score += 10;
        gen_food();
		food_eat_cnt++;
		current_len++;
		if(food_eat_cnt % bonus_activation_k == 0){
			gen_bonus_food();
			is_bonus_active = 1;
			bonus_start_time = clock();
			if(play_state == classic_revisited){
				init_auto_snake();
				is_auto_snake_active = 1;
				auto_snake_current_len = current_len;
			}
		}
    }
	if(is_bonus_active == 1){
		if(play_state != classic_revisited && clock() - bonus_start_time > bonus_duration){
			is_bonus_active = 0;
		}
		else if(snakeX[0] == bonus_x && snakeY[0] == bonus_y){
			score += 30;
			current_len += 3;
			is_bonus_active = 0;
		}
	}
	// if(score >= 20 && checker == 0){
	// 	snake_speed -= 25;
	// 	s = iSetTimer(snake_speed, snake_move);
	// 	printf("Called\n");
	// 	as = iSetTimer(snake_speed, auto_snake_move);
	// 	printf("Called\n");
	// 	checker = 1;
	// }
	// if(score >= 40 && checker == 1){
	// 	snake_speed -= 25;
	// 	s = iSetTimer(snake_speed, snake_move);
	// 	as = iSetTimer(snake_speed, auto_snake_move);
	// 	checker = 0;
	// }
	// if(score >= 600 && checker == 0){
	// 	snake_speed -= 25;
	// 	s = iSetTimer(snake_speed, snake_move);
	// 	as = iSetTimer(snake_speed, auto_snake_move);
	// 	checker = 1;
	// }
	// if(score >= 800 && checker == 1){
	// 	snake_speed -= 25;
	// 	s = iSetTimer(snake_speed, snake_move);
	// 	as = iSetTimer(snake_speed, auto_snake_move);
	// 	checker = 0;
	// }
	// if(score >= 1000 && checker == 0){
	// 	snake_speed -= 25;
	// 	s = iSetTimer(snake_speed, snake_move);
	// 	as = iSetTimer(snake_speed, auto_snake_move);
	// 	checker = 1;
	// }
}

void init_auto_snake(){
	border = rand() % 4; // top -> 0, bottom -> 1, left -> 2, right -> 3
	int is_overlap = 1;
	while(is_overlap == 1){
		is_overlap = 0;
		if(border == 0){
			auto_snakeY[0] = game_height-snake_sr;
			auto_snakeX[0] = (rand() % (game_width / snake_sr));
			if(auto_snakeX[0] % 2 == 0) auto_snakeX[0] += 1;
			auto_snakeX[0] *= snake_sr;
		}
		else if(border == 1){
			auto_snakeY[0] = snake_sr;
			auto_snakeX[0] = (rand() % (game_width / snake_sr));
			if(auto_snakeX[0] % 2 == 0) auto_snakeX[0] += 1;
			auto_snakeX[0] *= snake_sr;
		}
		else if(border == 2){
			auto_snakeX[0] = snake_sr;
			auto_snakeY[0] = (rand() % (game_height / snake_sr));
			if(auto_snakeY[0] % 2 == 0) auto_snakeY[0] += 1;
			auto_snakeY[0] *= snake_sr;
		}
		else{
			auto_snakeX[0] = game_width - snake_sr;
			auto_snakeY[0] = (rand() % (game_height / snake_sr));
			if(auto_snakeY[0] % 2 == 0) auto_snakeY[0] += 1;
			auto_snakeY[0] *= snake_sr;
		}
		for(int i = 0; i < current_len; i++){
			if(auto_snakeX[0] == snakeX[i] && auto_snakeY[0] == snakeY[i]){
				is_overlap = 1;
				break;
			}
		}
		if(auto_snakeX[0] == food_x && auto_snakeY[0] == food_y){
			is_overlap = 1;
		}
		if(auto_snakeX[0] == bonus_x && auto_snakeY[0] == bonus_y){
			is_overlap = 1;
		}
	}
	
	signX = bonus_x - auto_snakeX[0];
	signY = bonus_y - auto_snakeY[0];

	for(int i = 1; i < auto_snake_current_len; i++){
        auto_snakeX[i] = auto_snakeX[0];
        auto_snakeY[i] = auto_snakeY[0];
    }
}

void auto_snake_move(){ // top -> 0, bottom -> 1, left -> 2, right -> 3
	if(is_auto_snake_active == 1){
		for(int i = auto_snake_current_len - 1; i > 0; i--){
			auto_snakeX[i] = auto_snakeX[i-1];
			auto_snakeY[i] = auto_snakeY[i-1];
		}
		if(border == 2 || border == 3){
			if(bonus_x != auto_snakeX[0]){
				auto_snakeX[0] += (signX > 0) ? step : -step;
			}
			else{
				auto_snakeY[0] += (signY > 0) ? step : -step;
			}
			
			if(auto_snakeX[0] == bonus_x && auto_snakeY[0] == bonus_y){
				is_bonus_active = 0;
			}
		}
		else if(border == 0 || border == 1){
			if(bonus_y != auto_snakeY[0]){
				auto_snakeY[0] += (signY > 0) ? step : -step;
			}
			else{
				auto_snakeX[0] += (signX > 0) ? step : -step;
			}

			if(auto_snakeX[0] == bonus_x && auto_snakeY[0] == bonus_y){
				is_bonus_active = 0;
			}
		}
		for(int i=0; i<current_len; i++){
			if(auto_snakeX[0] == snakeX[i] && auto_snakeY[0] == snakeY[i]){
				is_gameover = 1;
				break;
			}
		}
	}
}

void collision(){
	for(int i=1; i<current_len; i++){ // Check for self-collision
		if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]){
			is_gameover = 1;
			break;
		}
	}
	if(is_auto_snake_active == 1){
		for(int i=0; i<auto_snake_current_len; i++){
			if(snakeX[0] == auto_snakeX[i] && snakeY[0] == auto_snakeY[i]){
				is_gameover = 1;
				break;
			}
		}
	}
	if(play_state == box){
		if(snakeX[0] == snake_sr || snakeX[0] == game_width - snake_sr || snakeY[0] == snake_sr || snakeY[0] == game_height - snake_sr){
			is_gameover = 1;
		}
	}

	if(play_state == campaign){
		if(snakeX[0]>=snake_sr && snakeX[0]<=240-snake_sr && (snakeY[0] == snake_sr || snakeY[0] == game_height-snake_sr)){
			is_gameover = 1;
		}
		else if(snakeX[0]>=560+snake_sr && snakeX[0]<=game_width-snake_sr && (snakeY[0] == snake_sr || snakeY[0] == game_height-snake_sr)){
			is_gameover = 1;
		}
		else if((snakeX[0] == snake_sr || snakeX[0] == game_width-snake_sr) && snakeY[0]>=snake_sr && snakeY[0]<=160-snake_sr){
			is_gameover = 1;
		}
		else if((snakeX[0] == snake_sr || snakeX[0] == game_width-snake_sr) && snakeY[0]>=440+snake_sr && snakeY[0]<=game_height-snake_sr){
			is_gameover = 1;
		}
		else if(snakeX[0] >= 100+snake_sr && snakeX[0] <= 700-snake_sr && snakeY[0] == 180+snake_sr){
			is_gameover = 1;
		}
		else if(snakeX[0] >= 100+snake_sr && snakeX[0] <= 700-snake_sr && snakeY[0] == 400+snake_sr){
			is_gameover = 1;
		}
	}
}

void game_start(){
    is_gameover = 0;
    score = 0;
    current_len = 3; 
    direction = right;
	food_eat_cnt = 0;
	is_bonus_active = 0;

	gen_food();
	init_snake();

    iResumeTimer(s);
	if(play_state == classic_revisited){
		auto_snake_current_len = current_len;
		iResumeTimer(as);
	}
	else{
		iPauseTimer(as);
	}
}
# include "iGraphics.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#define window_width 800
#define window_height 680
#define game_width 800
#define game_height 600
#define snake_sr 10 // snake_sr -> snake segment radius
#define step snake_sr * 2

int is_gameover = 0;
int is_paused = 0;
int snake_speed = 200; // Default speed
int snake_speed_inc_k = 200;
int score = 0;
bool musicOn = true;

#define menu 0
#define start_game 1
#define score_board 2
#define game_type 3
#define controls 4
#define speed_level 5

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

char player_name[100];
int name_len = 0;
int name_input_mode = 0;
int score_cnt = 5;
int is_score_updated = 0;

typedef struct{
	char name[100];
	int score;
}str_score;

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

int load_score(str_score topScores[]);
void save_score(str_score topScores[], int score_cnt);
void update_score(char player_name[], int score);

void iDraw() {
    iClear();

	if(game_state == menu){
		iShowBMP(0, 0, "bg.bmp");
		iShowBMP2(window_width/2 - 180, window_height/2, "logo.bmp", 0);

        // Draw menu options
        iSetColor(245, 245, 220);

		int y_pos = window_height / 2 - 65;
        // Start Game
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 45, y_pos + 15, "Start Game", GLUT_BITMAP_HELVETICA_18);
		iRectangle(window_width / 2 - 100 , y_pos, 200, 40);

        // Score Board
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos - 60, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 48, y_pos - 60 + 15, "Scoreboard", GLUT_BITMAP_HELVETICA_18);
        iRectangle(window_width / 2 - 100, y_pos - 60, 200, 40);

        // Game Type
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos - 120, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 48, y_pos - 120 + 15, "Game Type", GLUT_BITMAP_HELVETICA_18);
        iRectangle(window_width / 2 - 100, y_pos - 120, 200, 40);

		// Speed Level
		iSetColor(255, 250, 205);
		iFilledRectangle(window_width / 2 - 100, y_pos - 180, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 50, y_pos - 180 + 15, "Speed Level", GLUT_BITMAP_HELVETICA_18);
		iRectangle(window_width / 2 - 100, y_pos - 180, 200, 40);

        // Controls
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos - 240, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 50, y_pos - 240 + 15, "Instructions", GLUT_BITMAP_HELVETICA_18);
        iRectangle(window_width / 2 - 100, y_pos - 240, 200, 40);

		iSetColor(255, 255, 255);
		iText(game_width/2+250, 10, "Press 'Q' to exit", GLUT_BITMAP_8_BY_13);

		if (name_input_mode == 1) {
			iSetColor(0, 0, 0);
			iFilledRectangle(0, 0, window_width, window_height);

			// Draw textbox
			iSetColor(255, 255, 255);
			iRectangle(window_width / 2 - 125, window_height / 2 - 20, 250, 40);
			iText(window_width / 2 - 70, window_height / 2 + 30, "Enter Your Name:", GLUT_BITMAP_HELVETICA_18);

			// Display the name
			iSetColor(255, 255, 255);
			iText(window_width / 2 - 115, window_height / 2 - 5, player_name, GLUT_BITMAP_HELVETICA_18);

			iText(window_width / 2 - 85, window_height / 2 - 50, "Press 'Enter' to start", GLUT_BITMAP_8_BY_13);

			//Game state
			char state[100];
			if(play_state == classic){
				strcpy(state, " Classic");
			}
			else if(play_state == classic_revisited){
				strcpy(state, " Classic Revisited");
			}
			else if(play_state == box){
				strcpy(state, " Box");
			}
			else if(play_state == campaign){
				strcpy(state, " Campaign");
			}
			char level[100];
			if(snake_speed == 200){
				strcpy(level, " Low");
			}
			else if(snake_speed == 150){
				strcpy(level, " Medium");
			}
			else if(snake_speed == 100){
				strcpy(level, " High");
			}
			iSetColor(255, 255, 255);
			iText(window_width / 2 - 70, window_height / 2 - 120, "Your Game:", GLUT_BITMAP_8_BY_13);
			iText(window_width / 2 - 70, window_height / 2 - 140, "Speed Level:", GLUT_BITMAP_8_BY_13);
			iSetColor(255, 0, 0);
			iText(window_width / 2 + 5, window_height / 2 - 121, state, GLUT_BITMAP_8_BY_13);
			iText(window_width / 2 + 22, window_height / 2 - 141, level, GLUT_BITMAP_8_BY_13);

			iSetColor(255, 51, 51);
			iFilledRectangle(window_width / 2 + 250, window_height / 2 - 320, 100, 40);
			iSetColor(255, 255, 255);
			iText(window_width / 2 + 278, window_height / 2 - 305, "Menu", GLUT_BITMAP_HELVETICA_18);
		}
    }

	else if(game_state == game_type){
		iShowBMP(0, 0, "bg.bmp");
        // Draw the title
        iSetColor(245, 245, 220);
        int title_y = window_height / 2 + 150;
        iText(window_width / 2 - 60, title_y, "Game Type", GLUT_BITMAP_TIMES_ROMAN_24);

        // Game Types
        iSetColor(245, 245, 220);

        // Classic
        int classic_y = title_y - 100;
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, classic_y, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 30, classic_y + 15, "Classic", GLUT_BITMAP_HELVETICA_18);

        // Classic Revisited
        int classicRevisited_y = classic_y - 60;
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, classicRevisited_y, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 67, classicRevisited_y + 15, "Classic Revisited", GLUT_BITMAP_HELVETICA_18);

        // Box
        int box_y = classicRevisited_y - 60;
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, box_y, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 17, box_y + 15, "Box", GLUT_BITMAP_HELVETICA_18);

        // Campaign
        int campaign_y = box_y - 60;
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, campaign_y, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 40, campaign_y + 15, "Campaign", GLUT_BITMAP_HELVETICA_18);

		iSetColor(230, 230, 230);
		iFilledRectangle(window_width / 2 + 250, window_height / 2 - 320, 100, 40);
		iSetColor(255, 51, 51);
		iText(window_width / 2 + 278, window_height / 2 - 305, "Menu", GLUT_BITMAP_HELVETICA_18);
    }

	else if(game_state == speed_level){
        iShowBMP(0, 0, "bg.bmp");

        // Draw the title
        iSetColor(245, 245, 220);
        iText(window_width / 2 - 60, window_height / 2 + 150, "Speed Level", GLUT_BITMAP_TIMES_ROMAN_24);

        // Low Speed
        int y_pos = window_height / 2 + 50;
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 20, y_pos + 15, "Low", GLUT_BITMAP_HELVETICA_18);

        // Medium Speed
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos - 60, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 30, y_pos - 60 + 15, "Medium", GLUT_BITMAP_HELVETICA_18);

        // High Speed
		iSetColor(255, 250, 205);
        iFilledRectangle(window_width / 2 - 100, y_pos - 120, 200, 40);
		iSetColor(44, 44, 44);
        iText(window_width / 2 - 20, y_pos - 120 + 15, "High", GLUT_BITMAP_HELVETICA_18);

		iSetColor(230, 230, 230);
		iFilledRectangle(window_width / 2 + 250, window_height / 2 - 320, 100, 40);
		iSetColor(255, 51, 51);
		iText(window_width / 2 + 278, window_height / 2 - 305, "Menu", GLUT_BITMAP_HELVETICA_18);
    }

	else if(game_state == controls){
		iShowBMP(0, 0, "instr.bmp");

		iSetColor(230, 230, 230);
		iFilledRectangle(window_width / 2 + 250, window_height / 2 - 320, 100, 40);
		iSetColor(255, 51, 51);
		iText(window_width / 2 + 278, window_height / 2 - 305, "Menu", GLUT_BITMAP_HELVETICA_18);
	}

	else if(game_state == score_board){
		// Draw background
		iShowBMP(0, 0, "score.bmp");

		// Draw the title
    	iSetColor(245, 245, 220);
    	iText(window_width / 2 - 60, window_height/2 + 150, "Scoreboard", GLUT_BITMAP_TIMES_ROMAN_24);

		char state[100];
			if(play_state == classic){
				strcpy(state, "(Classic)");
			}
			else if(play_state == classic_revisited){
				strcpy(state, "(Classic Revisited)");
			}
			else if(play_state == box){
				strcpy(state, "(Box)");
			}
			else if(play_state == campaign){
				strcpy(state, "(Campaign)");
			}

    	iText(window_width / 2 + 50, window_height/2 + 150, state, GLUT_BITMAP_9_BY_15);

		// Draw table headers
		iSetColor(255, 215, 0);
		iText(window_width / 2 - 150, window_height/2 + 100, "Rank", GLUT_BITMAP_HELVETICA_18);
		iText(window_width / 2 - 30, window_height/2 + 100, "Name", GLUT_BITMAP_HELVETICA_18);
		iText(window_width / 2 + 100, window_height/2 + 100, "Score", GLUT_BITMAP_HELVETICA_18);

    	iSetColor(255, 215, 0);
    	iLine(window_width / 2 - 160, window_height/2 + 95, window_width / 2 + 160, window_height/2 + 95);

		str_score topScores[5];
		int cnt = load_score(topScores);

		for(int i = 0; i < cnt; i++){
			int y_coord = window_height / 2 + 60 - i * 50;

			char rank[10];
			sprintf(rank, "%d.", i + 1);
			char name[100];
			strcpy(name, topScores[i].name);
			char score[10];
			sprintf(score, "%d", topScores[i].score);

			iSetColor(255, 255, 255);

			// Display Rank, Name, Score
			iText(window_width / 2 - 140, y_coord, rank, GLUT_BITMAP_HELVETICA_18);
			iText(window_width / 2 - 30, y_coord, name, GLUT_BITMAP_HELVETICA_18);
			iText(window_width / 2 + 110, y_coord, score, GLUT_BITMAP_HELVETICA_18);
		}

		iSetColor(230, 230, 230);
		iFilledRectangle(window_width / 2 + 250, window_height / 2 - 320, 100, 40);
		iSetColor(255, 51, 51);
		iText(window_width / 2 + 278, window_height / 2 - 305, "Menu", GLUT_BITMAP_HELVETICA_18);
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
			iShowBMP(0, game_height, "score.bmp");

			iSetColor(255, 255, 255);
			iText(670, game_height + 30, "Score:", GLUT_BITMAP_TIMES_ROMAN_24);
			iSetColor(255, 215, 0);
			char scoreText[100];
			sprintf(scoreText, " %d", score);
			iText(727, game_height + 30, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
			
			iSetColor(255, 255, 255);
			iText(10, game_height + 30, "Player: ", GLUT_BITMAP_TIMES_ROMAN_24);
			iSetColor(255, 215, 0);
			iText(80, game_height + 30, player_name, GLUT_BITMAP_TIMES_ROMAN_24);

			//Display bonus timer
			if(is_bonus_active == 1){
                int remaining_time = (bonus_duration - clock() + bonus_start_time) / 1000; // in seconds
                if(remaining_time < 0) remaining_time = 0; // Ensure it doesn't go negative

                iText(280, game_height + 30, "Time for BONUS left:", GLUT_BITMAP_9_BY_15);
				iSetColor(255, 0, 0);
                char bonus_timer[100];
                sprintf(bonus_timer, " %d s", remaining_time);
                iText(460, game_height + 30, bonus_timer, GLUT_BITMAP_9_BY_15);
            }

			if(play_state == classic_revisited){
				//Draw the auto snake
				if(is_auto_snake_active == 1){
					iSetColor(128, 0, 0);
					for(int i=0; i<auto_snake_current_len; i++){
						iFilledCircle(auto_snakeX[i], auto_snakeY[i], snake_sr, 100);
					}
				}

				//Draw the score board
				iShowBMP(0, game_height, "score.bmp");

				iSetColor(255, 255, 255);
				iText(670, game_height + 30, "Score:", GLUT_BITMAP_TIMES_ROMAN_24);
				iSetColor(255, 215, 0);
				char scoreText[100];
				sprintf(scoreText, " %d", score);
				iText(727, game_height + 30, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
				
				iSetColor(255, 255, 255);
				iText(10, game_height + 30, "Player: ", GLUT_BITMAP_TIMES_ROMAN_24);
				iSetColor(255, 215, 0);
				iText(80, game_height + 30, player_name, GLUT_BITMAP_TIMES_ROMAN_24);
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

		else if(is_gameover == 1){
			iSetColor(0, 0, 0);
			iFilledRectangle(0, 0, window_width, window_height);
			
			iSetColor(255, 0, 0);
			iText(window_width/2 - 70, window_height/2 + 50, "GAME OVER :'(", GLUT_BITMAP_TIMES_ROMAN_24);
			
			iSetColor(255, 215, 0);
			char score_text[40];
			sprintf(score_text, "Score: %d", score);
			iText(window_width/2 - 35, window_height/2, score_text, GLUT_BITMAP_TIMES_ROMAN_24);
			
			iSetColor(255, 51, 51);
			iFilledRectangle(window_width/2 - 125, window_height/2 - 75, 120, 30);
			iSetColor(255, 255, 255);
			iText(window_width/2 - 120, window_height/2 - 65, "Play Again :D", GLUT_BITMAP_HELVETICA_18);
			
			iSetColor(255, 51, 51);
			iFilledRectangle(window_width/2 + 45, window_height/2 - 75, 120, 30);
			iSetColor(255, 255, 255);
			iText(window_width/2 + 60, window_height/2 - 65, "Main Menu", GLUT_BITMAP_HELVETICA_18);
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
			if(name_input_mode == 1){
				if(mx >= window_width / 2 + 250 && mx <= window_width / 2 + 350 &&
                   my >= window_height / 2 - 320 && my <= window_height / 2 - 280){
                    name_input_mode = 0;
                    game_state = menu;
                }
			}
			else{
				//Start Game Button
				if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 65 && my <= window_height / 2 - 25){
					name_input_mode = 1;
					name_len = 0;
					player_name[0] = '\0';
					if(mx >= window_width / 2 + 250 && mx <= window_width / 2 + 350 && my >= window_height / 2 - 320 && my <= window_height / 2 - 280){
						name_input_mode = 0;
						game_state = menu;
					}
				}
				// Score Board Button
				else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 125 && my <= window_height / 2 - 85){
					game_state = score_board;
				}
				// Game Type Button
				else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 185 && my <= window_height / 2 - 145){
					game_state = game_type;
				}
				// Speed Level Button
				else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 245 && my <= window_height / 2 - 205){
					game_state = speed_level;
				}
				// Controls Button
				else if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= window_height / 2 - 305 && my <= window_height / 2 - 265){
					game_state = controls;
				}
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

		else if(game_state == speed_level){
			// Low Speed
			int y_pos = window_height / 2 + 50;
			if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= y_pos && my <= y_pos + 40){
				snake_speed = 200;
				iPauseTimer(s);
				s = iSetTimer(snake_speed, snake_move);
				game_state = menu;
				// printf("Speed: %d\n", snake_speed);
			}
			// Medium Speed
			if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= y_pos - 60 && my <= y_pos - 60 + 40){
				snake_speed = 150;
				game_state = menu;
				iPauseTimer(s);
				s = iSetTimer(snake_speed, snake_move);
				// printf("Speed: %d\n", snake_speed);
			}
			// High Speed
			if(mx >= window_width / 2 - 100 && mx <= window_width / 2 + 100 && my >= y_pos - 120 && my <= y_pos - 120 + 40){
				snake_speed = 100;
				game_state = menu;
				iPauseTimer(s);
				s = iSetTimer(snake_speed, snake_move);
				// printf("Speed: %d\n", snake_speed);
			}
		}

		if(is_gameover == 1){
			// Play Again Button
			if(mx >= window_width/2 - 125 && mx <= window_width/2 - 5 && my >= window_height/2 - 75 && my <= window_height/2 - 45){
				game_start();
				game_state = start_game;
			}
			// Main Menu Button
			else if(mx >= window_width/2 + 45 && mx <= window_width/2 + 165 && my >= window_height/2 - 75 && my <= window_height/2 - 45){
				game_state = menu;
			}
		}
		if(mx >= window_width / 2 + 250 && mx <= window_width / 2 + 350 &&
            my >= window_height / 2 - 320 && my <= window_height / 2 - 280){
            game_state = menu;
        }
    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
	*/
void iKeyboard(unsigned char key) {
	if (name_input_mode == 1) {
        if (key == '\r') {
            name_input_mode = 0;
            printf("Player Name: %s\n", player_name);
            game_start();
			game_state = start_game;
        }
        else if (key == '\b') {
            if (name_len > 0) {
                player_name[name_len - 1] = '\0';
                name_len--;
            }
        }
        else {
            player_name[name_len] = key;
            name_len++;
            player_name[name_len] = '\0';
        }
    }
	else{
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
	iInitialize(window_width, window_height, "Snake Game");
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

	if(is_gameover == 1 && is_score_updated == 0){
		update_score(player_name, score);
		is_score_updated = 1;
		iPauseTimer(s);
		iPauseTimer(as);
	}
}

void game_start(){
    is_gameover = 0;
    score = 0;
    current_len = 3; 
    direction = right;
	food_eat_cnt = 0;
	is_bonus_active = 0;
	is_score_updated = 0;

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

int load_score(str_score topScores[]){
	FILE *fp = NULL;
	if(play_state == 0) fp = fopen("score0.txt", "r");
	else if(play_state == 1) fp = fopen("score1.txt", "r");
	else if(play_state == 2) fp = fopen("score2.txt", "r");
	else if(play_state == 3) fp = fopen("score3.txt", "r");
	
	int cnt = 0;
	if(fp != NULL){
		while(fscanf(fp, "%s %d", topScores[cnt].name, &topScores[cnt].score) == 2){
			cnt++;
		}
		fclose(fp);
	}
	return cnt;
}

void save_score(str_score topScores[], int score_cnt){
	FILE *fp = NULL;
	if(play_state == 0) fp = fopen("score0.txt", "w");
	else if(play_state == 1) fp = fopen("score1.txt", "w");
	else if(play_state == 2) fp = fopen("score2.txt", "w");
	else if(play_state == 3) fp = fopen("score3.txt", "w");

	for(int i=0; i<score_cnt; i++){
		fprintf(fp, "%s %d\n", topScores[i].name, topScores[i].score);
	}
	fclose(fp);
}

void update_score(char player_name[], int score){
	str_score topScores[score_cnt+1];
	int scr_cnt = load_score(topScores);
	
	strcpy(topScores[scr_cnt].name, player_name);
	topScores[scr_cnt].score = score;
	scr_cnt++;

	for(int i=0; i<scr_cnt-1; i++){
		for(int j=i+1; j<scr_cnt; j++){
			if(topScores[i].score < topScores[j].score){
				str_score temp = topScores[i];
				topScores[i] = topScores[j];
				topScores[j] = temp;
			}
		}
	}
	int score_save = (scr_cnt < score_cnt) ? scr_cnt : score_cnt;
	save_score(topScores, score_save);
}
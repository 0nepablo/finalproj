/************************************************************
 * By pablo
 * ncurses snake pit
 * with visible border.
 ************************************************************/

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>   // for rand()
#include <time.h>     // for srand()

#define MAX_LENGTH 100
#define DIRUP 0
#define DIRDOWN 1
#define DIRLEFT 2
#define DIRRIGHT 3

int direction = DIRRIGHT; // starting direc
int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int snakeLength = 3;

//food pos
int foodX, foodY;

//game over flag
int gameOver = 0;

//food spawn
void spawnFood(int startX, int startY, int pitW, int pitH){
    foodX = startX + 1 + rand() % (pitW - 1);
    foodY = startY + 1 + rand() % (pitH - 1);
}

int main() {

    // init ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // getch
    srand(time(NULL));     // random seed

    // fullscreen pit size (must be AFTER initscr)
    int pitW = COLS - 2;
    int pitH = LINES - 2;

    // top-left corner of fullscreen pit
    int startY = 1;
    int startX = 1;

    //set start of snake pos(center of fullscreen pit)
    int centerX = startX + pitW / 2;
    int centerY = startY + pitH / 2;

    //snake head
    snakeX[0] = centerX;
    snakeY[0] = centerY;

    //body
    snakeX[1] = centerX - 1;
    snakeY[1] = centerY;

    snakeX[2] = centerX - 2;
    snakeY[2] = centerY;

    //spawn first food
    spawnFood(startX, startY, pitW, pitH);

    refresh();      // update screen

    //basic game loop
    while(1){

        // INPUT HANDLING
        int ch = getch();   // read key

        if(ch == 'q'){      // temp exit
            break;
        }

        // arrow keys
        if (ch == KEY_UP    && direction != DIRDOWN)  direction = DIRUP;
        if (ch == KEY_DOWN  && direction != DIRUP)    direction = DIRDOWN;
        if (ch == KEY_LEFT  && direction != DIRRIGHT) direction = DIRLEFT;
        if (ch == KEY_RIGHT && direction != DIRLEFT)  direction = DIRRIGHT;

        // wasd keys
        if (ch == 'w' && direction != DIRDOWN) direction = DIRUP;
        if (ch == 's' && direction != DIRUP)   direction = DIRDOWN;
        if (ch == 'a' && direction != DIRRIGHT) direction = DIRLEFT;
        if (ch == 'd' && direction != DIRLEFT)  direction = DIRRIGHT;

        // MOVE SNAKE HEAD
        int newHeadX = snakeX[0];
        int newHeadY = snakeY[0];

        if(direction == DIRUP)    newHeadY--;
        if(direction == DIRDOWN)  newHeadY++;
        if(direction == DIRLEFT)  newHeadX--;
        if(direction == DIRRIGHT) newHeadX++; 

        // shift body
        for(int i = snakeLength - 1; i > 0; i--){
            snakeX[i] = snakeX[i-1];
            snakeY[i] = snakeY[i-1];
        }

        // head position
        snakeX[0] = newHeadX;
        snakeY[0] = newHeadY;

        //wall hit
        if(snakeX[0] <= startX || snakeX[0] >= startX + pitW || snakeY[0] <= startY || snakeY[0] >= startY + pitH){
            gameOver = 1;
        }
        //self hit
        for(int i = 1; i < snakeLength; i++){
            if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]){
                gameOver = 1;
            }
        }
        //game over handle
        if(gameOver){
            //flash animation
        for(int flash = 0; flash < 6; flash++){
            clear();
            if(flash % 2 == 0){
                mvprintw(LINES/2, (COLS/2)-5, "GAME OVER");
            }
            refresh();
            usleep(200000);
        }   
            //final game over screen
            clear();
            mvprintw(LINES/2, (COLS/2)-5, "GAME OVER");
            mvprintw((LINES/2)+1, (COLS/2)-12, "R = restart, Q = quit");
            refresh();
            //input check
            while(1){
                int ch2 = getch();
                if(ch2 == 'q'){
                    endwin();
                    return 0;
                }
                if(ch2 == 'r'){
                    //reset snake
                    snakeLength = 3;
                    snakeX[0] = centerX;
                    snakeY[0] = centerY;
                    snakeX[1] = centerX -1;
                    snakeY[1] = centerY;
                    snakeX[2] = centerX -2;
                    snakeY[2] = centerY;

                    direction = DIRRIGHT;
                    spawnFood(startX, startY, pitW, pitH);
                    gameOver = 0;
                    break;
                }
                usleep(10000);
            }
            continue;
        }
        //check if snake eats food
        if(snakeX[0] == foodX && snakeY[0] == foodY){
            if(snakeLength < MAX_LENGTH) {
                snakeLength++;
            }
            spawnFood(startX, startY, pitW, pitH);
        }

        //draw frame
        clear();   //clear prev frame

        //background screen
        for(int y = 0; y < LINES; y++){
            for(int x = 0; x < COLS; x++){
                mvprintw(y, x, ".");
            }
        }

        //update borders (fullscreen pit)
        for(int x = startX; x <= pitW + startX; x++){
            mvprintw(startY, x, "#");
            mvprintw(startY + pitH, x, "#");
        }
    
        for(int y = startY; y <= pitH + startY; y++){
            mvprintw(y, startX, "#");
            mvprintw(y, startX + pitW, "#");
        }

        //draw food
        mvprintw(foodY, foodX, "@");

        //draw snake
        for(int i = 0; i < snakeLength; i++){
            mvprintw(snakeY[i], snakeX[i], "O");
        }

        refresh();          // update screen
        usleep(100000);     // delay
    }

    endwin();
    return 0;
}

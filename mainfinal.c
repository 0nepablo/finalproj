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

//score
int score = 0;

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

    // init colors
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN,  COLOR_BLACK); // snake
        init_pair(2, COLOR_RED,    COLOR_BLACK); // food
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // border
        init_pair(4, COLOR_WHITE,  COLOR_BLACK); // text
    }

    // fullscreen pit size (must be AFTER initscr)
    int pitW = COLS - 2;
    int pitH = LINES - 3; // leave top row for score

    // top-left corner of fullscreen pit
    int startY = 2; // start below score line
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

        // WALL COLLISION
        if (snakeX[0] <= startX || snakeX[0] >= startX + pitW ||
            snakeY[0] <= startY || snakeY[0] >= startY + pitH) {
            gameOver = 1;
        }

        // SELF COLLISION
        for (int i = 1; i < snakeLength; i++) {
            if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
                gameOver = 1;
            }
        }

        // GAME OVER HANDLING
        if (gameOver) {

            // FLASHING ANIMATION
            for (int flash = 0; flash < 6; flash++) {
                clear();
                if (flash % 2 == 0) {
                    if (has_colors()) attron(COLOR_PAIR(4));
                    mvprintw(LINES/2, (COLS/2)-5, "GAME OVER");
                    if (has_colors()) attroff(COLOR_PAIR(4));
                }
                refresh();
                usleep(200000); // 0.2 seconds
            }

            // FINAL GAME OVER SCREEN
            clear();
            if (has_colors()) attron(COLOR_PAIR(4));
            mvprintw(LINES/2, (COLS/2)-5, "GAME OVER");
            mvprintw((LINES/2)+1, (COLS/2)-12, "Press R to restart or Q to quit");
            mvprintw((LINES/2)+3, (COLS/2)-6, "Final score: %d", score);
            if (has_colors()) attroff(COLOR_PAIR(4));
            refresh();

            // WAIT FOR INPUT
            while (1) {
                int ch2 = getch();
                if (ch2 == 'q') {
                    endwin();
                    return 0;
                }
                if (ch2 == 'r') {
                    // RESET SNAKE
                    snakeLength = 3;
                    snakeX[0] = centerX;
                    snakeY[0] = centerY;
                    snakeX[1] = centerX - 1;
                    snakeY[1] = centerY;
                    snakeX[2] = centerX - 2;
                    snakeY[2] = centerY;

                    direction = DIRRIGHT;
                    spawnFood(startX, startY, pitW, pitH);
                    score = 0;
                    gameOver = 0;
                    break;
                }
                usleep(10000);
            }

            continue; // skip drawing frame while dead
        }

        //check if snake eats food
        if(snakeX[0] == foodX && snakeY[0] == foodY){
            if(snakeLength < MAX_LENGTH) {
                snakeLength++;
            }
            score += 10;
            spawnFood(startX, startY, pitW, pitH);
        }

        //draw frame
        clear();   //clear prev frame

        // SCORE HUD
        if (has_colors()) attron(COLOR_PAIR(4));
        mvprintw(0, 2, "Score: %d", score);
        if (has_colors()) attroff(COLOR_PAIR(4));

        //background screen
        for(int y = 1; y < LINES; y++){
            for(int x = 0; x < COLS; x++){
                mvprintw(y, x, ".");
            }
        }

        //update borders (fullscreen pit)
        if (has_colors()) attron(COLOR_PAIR(3));
        for(int x = startX; x <= pitW + startX; x++){
            mvprintw(startY, x, "#");
            mvprintw(startY + pitH, x, "#");
        }
    
        for(int y = startY; y <= pitH + startY; y++){
            mvprintw(y, startX, "#");
            mvprintw(y, startX + pitW, "#");
        }
        if (has_colors()) attroff(COLOR_PAIR(3));

        //draw food
        if (has_colors()) attron(COLOR_PAIR(2));
        mvprintw(foodY, foodX, "@");
        if (has_colors()) attroff(COLOR_PAIR(2));

        //draw snake
        if (has_colors()) attron(COLOR_PAIR(1));
        for(int i = 0; i < snakeLength; i++){
            mvprintw(snakeY[i], snakeX[i], "O");
        }
        if (has_colors()) attroff(COLOR_PAIR(1));

        refresh();          // update screen
        usleep(100000);     // delay
    }

    endwin();
    return 0;
}

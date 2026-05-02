/************************************************************
 * By pablo
 * ncurses snake pit
 * with visible border.
 ************************************************************/

#include <ncurses.h>
#include <unistd.h>

#define MAX_LENGTH 100
#define DIRUP 0
#define DIRDOWN 1
#define DIRLEFT 2
#define DIRRIGHT 3

int direction = DIRRIGHT; // starting direc
int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int snakeLength = 3;

int main() {

    // init ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // getch

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

    //snake on screen
    for(int i = 0; i < snakeLength; i++){
        mvprintw(snakeY[i], snakeX[i], "O");
    }

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

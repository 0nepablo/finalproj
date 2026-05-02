/************************************************************
 * By pablo
 * ncurses snake pit
 * with visible border.
 ************************************************************/

#include <ncurses.h>
#include <unistd.h>

#define PIT_WIDTH  20
#define PIT_HEIGHT 20
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

    // start pos
    int startY = 1;
    int startX = 1;

    // top and bottom borders
    for (int x = startX; x <= PIT_WIDTH + startX; x++) {
        mvprintw(startY, x, "#");                     // top border
        mvprintw(startY + PIT_HEIGHT, x, "#");        // bottom border
    }

    // left and right borders
    for (int y = startY; y <= PIT_HEIGHT + startY; y++) {
        mvprintw(y, startX, "#");                     // left border
        mvprintw(y, startX + PIT_WIDTH, "#");         // right border
    }
    //set start of snake pos(center)
    int centerX = startX + PIT_WIDTH / 2;
    int centerY = startY + PIT_HEIGHT / 2;
    //snake head
    snakeX[0] = centerX;
    snakeY[0] = centerY;
    //body
    snakeX[1] = centerX -1;
    snakeY[1] = centerY;

    snakeX[2] = centerX -2;
    snakeY[2] = centerY;

    //snake on screen
    for(int i = 0; i < snakeLength; i++){
        mvprintw(snakeY[i], snakeX[i], "O");
    }

    refresh();      // update screen
   //basic game loop
   nodelay(stdscr, TRUE); // getch

    while(1){
            // game loop
    nodelay(stdscr, TRUE); // getch

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

        //update borders
        for(int x = startX; x <= PIT_WIDTH + startX; x++){
            mvprintw(startY, x, "#");
            mvprintw(startY + PIT_HEIGHT, x, "#");
        }
    
        for(int y = startY; y <= PIT_HEIGHT + startY; y++){
            mvprintw(y, startX, "#");
            mvprintw(y, startX + PIT_WIDTH, "#");
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
}

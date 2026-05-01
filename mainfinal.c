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
   nodelay(stdscr, TRUE); // getch()becomes non blocking

    while(1){
        //clear prev frame
        clear();
        //update borders
        for(int x = startX; x <= PIT_WIDTH + startX; x++){
            mvprintw(startY, x, "#");
            mvprintw(startY + PIT_HEIGHT, x, "#");
        }
    
        for(int y = startY; y < PIT_HEIGHT + startY; y++){
            mvprintw(y, startX, "#");
            mvprintw(y, startX + PIT_WIDTH, "#");
        }
        //draw snake
        for(int i = 0; i < snakeLength; i++){
            mvprintw(snakeY[i], snakeX[i], "O");
        }
        refresh();
        //temp exit
        int ch = getch();
        if(ch == 'q'){
            break;
        }
        usleep(100000); //delay
    }
    endwin();
    return 0;

}

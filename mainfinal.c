/************************************************************
 * By pablo
 * Get ncurses and draw the snake pit
 * with a visible border.
 ************************************************************/

#include <ncurses.h>
#include <unistd.h>

#define PIT_WIDTH  20
#define PIT_HEIGHT 20

int main() {

    // init ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    // start pos
    int start_y = 1;
    int start_x = 1;

    // top and bottom borders
    for (int x = start_x; x <= PIT_WIDTH + start_x; x++) {
        mvprintw(start_y, x, "#");                     // top border
        mvprintw(start_y + PIT_HEIGHT, x, "#");        // bottom border
    }

    // left and right borders
    for (int y = start_y; y <= PIT_HEIGHT + start_y; y++) {
        mvprintw(y, start_x, "#");                     // left border
        mvprintw(y, start_x + PIT_WIDTH, "#");         // right border
    }

    refresh();      // update screen
    getch();        //key press so the border stays
    endwin();       // exot ncurses
    return 0;
}

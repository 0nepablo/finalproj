/************************************************************
 * By pablo
 ************************************************************/
#include <ncurses.h>      // ncurses library for terminal graphics
#include <unistd.h>       // for usleep() timing
#include <stdlib.h>       // for rand(), srand(), exit()
#include <time.h>         // for time() to seed RNG

#define MAX_LENGTH 100    // maximum snake length
#define DIRUP 0           // direction code for up
#define DIRDOWN 1         // direction code for down
#define DIRLEFT 2         // direction code for left
#define DIRRIGHT 3        // direction code for right

#define BONUS_FOOD 5      // number of bonus foods during power-up

int direction = DIRRIGHT; // starting direction of the snake
int snakeX[MAX_LENGTH];   // X positions of snake segments
int snakeY[MAX_LENGTH];   // Y positions of snake segments
int snakeLength = 3;      // initial snake length

int foodX, foodY;         // main food coordinates
int gameOver = 0;         // game-over flag
int score = 0;            // player score

// food visuals
int foodBlink = 0;        // blinking animation counter
int foodCount = 0;        // number of foods eaten
int foodColor = 2;        // color pair for main food

// power-up mode
int powerUpActive = 0;                // whether power-up is active
int powerUpTimer = 0;                 // countdown timer for power-up
int bonusFoodX[BONUS_FOOD];           // X positions of bonus foods
int bonusFoodY[BONUS_FOOD];           // Y positions of bonus foods
int bonusFoodAlive[BONUS_FOOD];       // whether each bonus food is active
int bonusFoodColor[BONUS_FOOD];       // color of each bonus food

// spawn food inside pit
void spawnFood(int startX, int startY, int pitW, int pitH){
    foodX = startX + 1 + rand() % (pitW - 1);   // random X inside pit
    foodY = startY + 1 + rand() % (pitH - 1);   // random Y inside pit
    foodColor = 1 + rand() % 4;                 // random color pair 1–4
}

void showTitleScreen() {
    clear();                                    // clear screen

    if (has_colors()) {                         // check color support
        start_color();                          // enable colors
        init_pair(6, COLOR_GREEN, COLOR_BLACK); // title color
        init_pair(7, COLOR_GREEN, COLOR_BLACK); // menu text color
    }
    // ASCII title
    attron(COLOR_PAIR(6));                      // enable title color
    mvprintw(5, (COLS/2)-20, " .d88888b  888888ba   .d888888   dP     dP  88888888b ");
    mvprintw(6, (COLS/2)-20, "88.       88    `8b   d8'    88  88   .d8'  88        ");
    mvprintw(7, (COLS/2)-20, "`Y88888b. 88     88   88aaaaa88a 88aaa8P'   a88aaaa   ");
    mvprintw(8, (COLS/2)-20, "     `8b  88     88   88     88  88   `8b.  88        ");
    mvprintw(9, (COLS/2)-20, "8'   .8P  88     88   88     88  88     88  88        ");
    mvprintw(10,(COLS/2)-20, "Y88888P   dP     dP   88     88  dP     dP  88888888P ");
    attroff(COLOR_PAIR(6));                     // disable title color
    // menu text
    attron(COLOR_PAIR(7));                      // enable menu color
    mvprintw(13, (COLS/2)-12, "Enter to start"); // start prompt
    mvprintw(14, (COLS/2)-12, "Q to quit");      // quit prompt
    attroff(COLOR_PAIR(7));                     // disable menu color

    refresh();                                  // draw everything
    // wait for user input
    while (1) {
        int ch = getch();                       // read key
        if (ch == 'q' || ch == 'Q') {           // quit
            endwin();                           // end ncurses
            exit(0);                            // exit program
        }
        if (ch == '\n' || ch == ' ') {          // start game
            break;
        }
        usleep(10000);                          // small delay
    }
}
int main() {

    initscr();                     // start ncurses mode
    noecho();                      // disable key echoing
    curs_set(FALSE);               // hide the blinking cursor
    keypad(stdscr, TRUE);          // enable arrow keys
    nodelay(stdscr, TRUE);         // make getch() non-blocking
    srand(time(NULL));             // seed RNG for random food placement
    showTitleScreen();             // display title screen before game starts

    if (has_colors()) {            // check if terminal supports color
        start_color();             // enable color mode
        init_pair(1, COLOR_GREEN,  COLOR_BLACK); // snake color
        init_pair(2, COLOR_RED,    COLOR_BLACK); // main food color
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // border + bonus food color
        init_pair(4, COLOR_WHITE,  COLOR_BLACK); // HUD text color
    }
    int pitW = COLS - 2;           // width of play area (leave room for border)
    int pitH = LINES - 3;          // height of play area (leave room for HUD)

    int startY = 2;                // top border Y position
    int startX = 1;                // left border X position

    int centerX = startX + pitW / 2; // center X of pit
    int centerY = startY + pitH / 2; // center Y of pit

    snakeX[0] = centerX;           // head X position
    snakeY[0] = centerY;           // head Y position

    snakeX[1] = centerX - 1;       // 2nd segment to the left of head
    snakeY[1] = centerY;           // same Y as head

    snakeX[2] = centerX - 2;       // 3rd segment further left
    snakeY[2] = centerY;           // same Y as head

    spawnFood(startX, startY, pitW, pitH); // place first food randomly

    for (int i = 0; i < BONUS_FOOD; i++) {
        bonusFoodAlive[i] = 0;     // no bonus foods active at start
    }

    refresh();                     // draw initial screen state
    while (1) {                               // main game loop (runs until quit or game over)
        int ch = getch();                     // read keyboard input (non-blocking)

        if (ch == 'q') {                      // quit if user presses q
            break;
        }
        // arrow key controls
        if (ch == KEY_UP    && direction != DIRDOWN)  direction = DIRUP;    // move up
        if (ch == KEY_DOWN  && direction != DIRUP)    direction = DIRDOWN;  // move down
        if (ch == KEY_LEFT  && direction != DIRRIGHT) direction = DIRLEFT;  // move left
        if (ch == KEY_RIGHT && direction != DIRLEFT)  direction = DIRRIGHT; // move right

        // WASD controls
        if (ch == 'w' && direction != DIRDOWN)  direction = DIRUP;          // move up
        if (ch == 's' && direction != DIRUP)    direction = DIRDOWN;        // move down
        if (ch == 'a' && direction != DIRRIGHT) direction = DIRLEFT;        // move left
        if (ch == 'd' && direction != DIRLEFT)  direction = DIRRIGHT;       // move right

        int newHeadX = snakeX[0];              // start with current head X
        int newHeadY = snakeY[0];              // start with current head Y

        if (direction == DIRUP)    newHeadY--; // move head up
        if (direction == DIRDOWN)  newHeadY++; // move head down
        if (direction == DIRLEFT)  newHeadX--; // move head left
        if (direction == DIRRIGHT) newHeadX++; // move head right

        for (int i = snakeLength - 1; i > 0; i--) {
            snakeX[i] = snakeX[i - 1];         // shift body X forward
            snakeY[i] = snakeY[i - 1];         // shift body Y forward
        }
        snakeX[0] = newHeadX;                  // update head X
        snakeY[0] = newHeadY;                  // update head Y
        // wall collision
        if (snakeX[0] <= startX || snakeX[0] >= startX + pitW ||
            snakeY[0] <= startY || snakeY[0] >= startY + pitH) {
            gameOver = 1;                      // hit wall = game over
        }
        // self collision
        for (int i = 1; i < snakeLength; i++) {
            if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
                gameOver = 1;                  // hit own body = game over
            }
        }
        // power-up countdown
        if (powerUpActive) {
            powerUpTimer--;                    // decrease timer
            if (powerUpTimer <= 0) {           // time expired?
                powerUpActive = 0;             // disable power-up
                for (int i = 0; i < BONUS_FOOD; i++) {
                    bonusFoodAlive[i] = 0;     // remove all bonus foods
                }
            }
        }
        if (gameOver) {                        // handle game over state

            for (int fade = 0; fade < 5; fade++) {
                clear();                       // clear screen for fade effect
                refresh();                     // update screen
                usleep(120000);                // delay for fade animation
            }

            clear();                           // final clear
            if (has_colors()) attron(COLOR_PAIR(4)); // HUD color
            mvprintw(LINES/2, (COLS/2)-5, "GAME OVER"); // game over text
            mvprintw((LINES/2)+1, (COLS/2)-12, "Press R to restart or Q to quit"); // restart prompt
            mvprintw((LINES/2)+3, (COLS/2)-6, "Final score: %d", score); // show score
            if (has_colors()) attroff(COLOR_PAIR(4));
            refresh();                         // draw game over screen

            while (1) {                        // wait for restart or quit
                int ch2 = getch();             // read key
                if (ch2 == 'q') {              // quit
                    endwin();                  // end ncurses
                    return 0;                  // exit program
                }
                if (ch2 == 'r') {              // restart game
                    snakeLength = 3;           // reset snake length
                    snakeX[0] = centerX;       // reset head X
                    snakeY[0] = centerY;       // reset head Y
                    snakeX[1] = centerX - 1;   // reset body segment 1
                    snakeY[1] = centerY;
                    snakeX[2] = centerX - 2;   // reset body segment 2
                    snakeY[2] = centerY;

                    direction = DIRRIGHT;      // reset direction
                    spawnFood(startX, startY, pitW, pitH); // respawn food
                    score = 0;                 // reset score
                    gameOver = 0;              // clear game-over flag
                    foodCount = 0;             // reset food counter
                    foodBlink = 0;             // reset blink animation
                    powerUpActive = 0;         // disable power-up
                    powerUpTimer = 0;          // reset timer
                    for (int i = 0; i < BONUS_FOOD; i++) {
                        bonusFoodAlive[i] = 0; // remove bonus foods
                    }
                    break;                     // exit restart loop
                }
                usleep(10000);                 // small delay
            }
            continue;                          // skip drawing frame during game over
        }
        // EAT MAIN FOOD
        if (snakeX[0] == foodX && snakeY[0] == foodY) {   // head touches main food
            if (snakeLength < MAX_LENGTH) {
                snakeLength++;                            // grow snake by 1
            }
            score += 10;                                  // add points
            foodCount++;                                  // count eaten foods

            beep();                                       // terminal beep sound

            // every 5th food triggers power-up
            if (foodCount % 5 == 0) {
                powerUpActive = 1;                        // enable power-up
                powerUpTimer = 50;                        // lasts ~5 seconds

                for (int i = 0; i < BONUS_FOOD; i++) {
                    bonusFoodX[i] = startX + 1 + rand() % (pitW - 1); // random X
                    bonusFoodY[i] = startY + 1 + rand() % (pitH - 1); // random Y
                    bonusFoodAlive[i] = 1;                // activate bonus food
                    bonusFoodColor[i] = 1 + rand() % 4;   // random color
                }
            }
            spawnFood(startX, startY, pitW, pitH);        // respawn main food
        }
        // EAT BONUS FOODS
        for (int i = 0; i < BONUS_FOOD; i++) {
            if (bonusFoodAlive[i] &&
                snakeX[0] == bonusFoodX[i] &&
                snakeY[0] == bonusFoodY[i]) {

                score += 5;                               // bonus food points
                if (snakeLength < MAX_LENGTH) {
                    snakeLength++;                        // grow snake
                }
                bonusFoodAlive[i] = 0;                    // remove eaten bonus food
            }
        }
        foodBlink = (foodBlink + 1) % 10;                 // update blink animation
        clear();                                          // clear screen for redraw

        if (has_colors()) attron(COLOR_PAIR(4));          // HUD color
        mvprintw(0, 2, "Score: %d", score);               // draw score
        if (has_colors()) attroff(COLOR_PAIR(4));
        // DRAW PIT BORDER
        if (has_colors()) attron(COLOR_PAIR(3));          // border color
        for (int x = startX; x <= pitW + startX; x++) {
            mvprintw(startY, x, "-");                     // top border
            mvprintw(startY + pitH, x, "-");              // bottom border
        }
        for (int y = startY; y <= pitH + startY; y++) {
            mvprintw(y, startX, "|");                     // left border
            mvprintw(y, startX + pitW, "|");              // right border
        }
        mvprintw(startY, startX, "+");                    // top-left corner
        mvprintw(startY, startX + pitW, "+");             // top-right corner
        mvprintw(startY + pitH, startX, "+");             // bottom-left corner
        mvprintw(startY + pitH, startX + pitW, "+");      // bottom-right corner
        if (has_colors()) attroff(COLOR_PAIR(3));
        // DRAW MAIN FOOD (blinking)
        if (foodBlink < 5) {                              // visible half the time
            if (has_colors()) attron(COLOR_PAIR(foodColor)); // random color
            mvprintw(foodY, foodX, "*");                  // draw main food
            if (has_colors()) attroff(COLOR_PAIR(foodColor));
        }
        // DRAW BONUS FOODS
        for (int i = 0; i < BONUS_FOOD; i++) {
            if (bonusFoodAlive[i]) {                      // only draw active ones
                if (has_colors()) attron(COLOR_PAIR(bonusFoodColor[i])); // random color
                mvprintw(bonusFoodY[i], bonusFoodX[i], "+"); // bonus food symbol
                if (has_colors()) attroff(COLOR_PAIR(bonusFoodColor[i]));
            }
        }
        // DRAW SNAKE
        if (has_colors()) attron(COLOR_PAIR(1));          // snake color
        for (int i = 0; i < snakeLength; i++) {
            if (i == 0) {                                 // head
                char headChar = '>';                      // default head
                if (direction == DIRUP)    headChar = '^';
                if (direction == DIRDOWN)  headChar = 'v';
                if (direction == DIRLEFT)  headChar = '<';
                if (direction == DIRRIGHT) headChar = '>';
                mvprintw(snakeY[i], snakeX[i], "%c", headChar); // draw head
            } else {
                mvprintw(snakeY[i], snakeX[i], "O");      // draw body
            }
        }
        if (has_colors()) attroff(COLOR_PAIR(1));
        refresh();                                        // update screen
        if (powerUpActive)
            usleep(60000);                                // faster speed
        else
            usleep(100000);                               // normal speed
    }
    endwin();                 // exit ncurses mode and restore terminal
    return 0;                 // end program successfully
}

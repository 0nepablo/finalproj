/************************************************************
 * By pablo
 * ncurses snake pit
 ************************************************************/
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LENGTH 100
#define DIRUP 0
#define DIRDOWN 1
#define DIRLEFT 2
#define DIRRIGHT 3

#define BONUS_FOOD 5

int direction = DIRRIGHT;
int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int snakeLength = 3;

int foodX, foodY;
int gameOver = 0;
int score = 0;

// food visuals
int foodBlink = 0;     // controls blink animation
int foodCount = 0;     // how many foods eaten
int foodColor = 2;     // current color pair for food

// power-up mode
int powerUpActive = 0;
int powerUpTimer = 0;

int bonusFoodX[BONUS_FOOD];
int bonusFoodY[BONUS_FOOD];
int bonusFoodAlive[BONUS_FOOD];
int bonusFoodColor[BONUS_FOOD];

// spawn food inside pit
void spawnFood(int startX, int startY, int pitW, int pitH){
    foodX = startX + 1 + rand() % (pitW - 1);
    foodY = startY + 1 + rand() % (pitH - 1);
    // random color 1–4 each time
    foodColor = 1 + rand() % 4;
}

void showTitleScreen() {
    clear();
    if (has_colors()) {
        start_color();
        init_pair(6, COLOR_GREEN, COLOR_BLACK); // title color
        init_pair(7, COLOR_GREEN, COLOR_BLACK); // menu color
    }

    attron(COLOR_PAIR(6));
    mvprintw(5, (COLS/2)-20, " .d88888b  888888ba   .d888888   dP     dP  88888888b ");
    mvprintw(6, (COLS/2)-20, "88.       88    `8b   d8'    88  88   .d8'  88        ");
    mvprintw(7, (COLS/2)-20, "`Y88888b. 88     88   88aaaaa88a 88aaa8P'   a88aaaa   ");
    mvprintw(8, (COLS/2)-20, "     `8b  88     88   88     88  88   `8b.  88        ");
    mvprintw(9, (COLS/2)-20, "8'   .8P  88     88   88     88  88     88  88        ");
    mvprintw(10,(COLS/2)-20, "Y88888P   dP     dP   88     88  dP     dP  88888888P ");
    attroff(COLOR_PAIR(6));

    attron(COLOR_PAIR(7));
    mvprintw(13, (COLS/2)-12, "Enter to start");
    mvprintw(14, (COLS/2)-12, "Q to quit");
    attroff(COLOR_PAIR(7));

    refresh();

    while (1) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            endwin();
            exit(0);
        }
        if (ch == '\n' || ch == ' ') {
            break;
        }
        usleep(10000);
    }
}

int main() {

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    srand(time(NULL));
    showTitleScreen();

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN,  COLOR_BLACK); // snake
        init_pair(2, COLOR_RED,    COLOR_BLACK); // food
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // border / bonus
        init_pair(4, COLOR_WHITE,  COLOR_BLACK); // text / HUD
    }

    int pitW = COLS - 2;
    int pitH = LINES - 3;

    int startY = 2;
    int startX = 1;

    int centerX = startX + pitW / 2;
    int centerY = startY + pitH / 2;

    snakeX[0] = centerX;
    snakeY[0] = centerY;

    snakeX[1] = centerX - 1;
    snakeY[1] = centerY;

    snakeX[2] = centerX - 2;
    snakeY[2] = centerY;

    spawnFood(startX, startY, pitW, pitH);

    // init bonus food state
    for (int i = 0; i < BONUS_FOOD; i++) {
        bonusFoodAlive[i] = 0;
    }

    refresh();

    while (1) {

        int ch = getch();

        if (ch == 'q') {
            break;
        }

        if (ch == KEY_UP    && direction != DIRDOWN)  direction = DIRUP;
        if (ch == KEY_DOWN  && direction != DIRUP)    direction = DIRDOWN;
        if (ch == KEY_LEFT  && direction != DIRRIGHT) direction = DIRLEFT;
        if (ch == KEY_RIGHT && direction != DIRLEFT)  direction = DIRRIGHT;

        if (ch == 'w' && direction != DIRDOWN)  direction = DIRUP;
        if (ch == 's' && direction != DIRUP)    direction = DIRDOWN;
        if (ch == 'a' && direction != DIRRIGHT) direction = DIRLEFT;
        if (ch == 'd' && direction != DIRLEFT)  direction = DIRRIGHT;

        int newHeadX = snakeX[0];
        int newHeadY = snakeY[0];

        if (direction == DIRUP)    newHeadY--;
        if (direction == DIRDOWN)  newHeadY++;
        if (direction == DIRLEFT)  newHeadX--;
        if (direction == DIRRIGHT) newHeadX++;

        for (int i = snakeLength - 1; i > 0; i--) {
            snakeX[i] = snakeX[i - 1];
            snakeY[i] = snakeY[i - 1];
        }

        snakeX[0] = newHeadX;
        snakeY[0] = newHeadY;

        if (snakeX[0] <= startX || snakeX[0] >= startX + pitW ||
            snakeY[0] <= startY || snakeY[0] >= startY + pitH) {
            gameOver = 1;
        }

        for (int i = 1; i < snakeLength; i++) {
            if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
                gameOver = 1;
            }
        }

        // power-up timer
        if (powerUpActive) {
            powerUpTimer--;
            if (powerUpTimer <= 0) {
                powerUpActive = 0;
                for (int i = 0; i < BONUS_FOOD; i++) {
                    bonusFoodAlive[i] = 0;
                }
            }
        }

        if (gameOver) {

            for (int fade = 0; fade < 5; fade++) {
                clear();
                refresh();
                usleep(120000);
            }

            clear();
            if (has_colors()) attron(COLOR_PAIR(4));
            mvprintw(LINES/2, (COLS/2)-5, "GAME OVER");
            mvprintw((LINES/2)+1, (COLS/2)-12, "Press R to restart or Q to quit");
            mvprintw((LINES/2)+3, (COLS/2)-6, "Final score: %d", score);
            if (has_colors()) attroff(COLOR_PAIR(4));
            refresh();

            while (1) {
                int ch2 = getch();
                if (ch2 == 'q') {
                    endwin();
                    return 0;
                }
                if (ch2 == 'r') {
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
                    foodCount = 0;
                    foodBlink = 0;
                    powerUpActive = 0;
                    powerUpTimer = 0;
                    for (int i = 0; i < BONUS_FOOD; i++) {
                        bonusFoodAlive[i] = 0;
                    }
                    break;
                }
                usleep(10000);
            }

            continue;
        }

        // EAT MAIN FOOD
        if (snakeX[0] == foodX && snakeY[0] == foodY) {
            if (snakeLength < MAX_LENGTH) {
                snakeLength++;
            }
            score += 10;
            foodCount++;

            beep();

            // every 5th food: trigger power-up
            if (foodCount % 5 == 0) {
                powerUpActive = 1;
                powerUpTimer = 100; // ~5 seconds at 100ms/frame

                for (int i = 0; i < BONUS_FOOD; i++) {
                    bonusFoodX[i] = startX + 1 + rand() % (pitW - 1);
                    bonusFoodY[i] = startY + 1 + rand() % (pitH - 1);
                    bonusFoodAlive[i] = 1;
                    bonusFoodColor[i] = 1 + rand() % 4; // random color
                }
            }

            spawnFood(startX, startY, pitW, pitH);
        }

        // EAT BONUS FOODS
        for (int i = 0; i < BONUS_FOOD; i++) {
            if (bonusFoodAlive[i] &&
                snakeX[0] == bonusFoodX[i] &&
                snakeY[0] == bonusFoodY[i]) {

                score += 5;
                if (snakeLength < MAX_LENGTH) {
                    snakeLength++;
                }
                bonusFoodAlive[i] = 0;
            }
        }

        foodBlink = (foodBlink + 1) % 10;

        clear();

        if (has_colors()) attron(COLOR_PAIR(4));
        mvprintw(0, 2, "Score: %d", score);
        if (has_colors()) attroff(COLOR_PAIR(4));

        if (has_colors()) attron(COLOR_PAIR(3));
        for (int x = startX; x <= pitW + startX; x++) {
            mvprintw(startY, x, "-");
            mvprintw(startY + pitH, x, "-");
        }
        for (int y = startY; y <= pitH + startY; y++) {
            mvprintw(y, startX, "|");
            mvprintw(y, startX + pitW, "|");
        }
        mvprintw(startY, startX, "+");
        mvprintw(startY, startX + pitW, "+");
        mvprintw(startY + pitH, startX, "+");
        mvprintw(startY + pitH, startX + pitW, "+");
        if (has_colors()) attroff(COLOR_PAIR(3));

        // MAIN FOOD (blinking, random color each spawn)
        if (foodBlink < 5) {
            if (has_colors()) attron(COLOR_PAIR(foodColor));
            mvprintw(foodY, foodX, "*");
            if (has_colors()) attroff(COLOR_PAIR(foodColor));
        }

        // BONUS FOODS
        for (int i = 0; i < BONUS_FOOD; i++) {
            if (bonusFoodAlive[i]) {
                if (has_colors()) attron(COLOR_PAIR(bonusFoodColor[i]));
                mvprintw(bonusFoodY[i], bonusFoodX[i], "+");
                if (has_colors()) attroff(COLOR_PAIR(bonusFoodColor[i]));
            }
        }

        if (has_colors()) attron(COLOR_PAIR(1));
        for (int i = 0; i < snakeLength; i++) {
            if (i == 0) {
                char headChar = '>';
                if (direction == DIRUP)    headChar = '^';
                if (direction == DIRDOWN)  headChar = 'v';
                if (direction == DIRLEFT)  headChar = '<';
                if (direction == DIRRIGHT) headChar = '>';
                mvprintw(snakeY[i], snakeX[i], "%c", headChar);
            } else {
                mvprintw(snakeY[i], snakeX[i], "O");
            }
        }
        if (has_colors()) attroff(COLOR_PAIR(1));

        refresh();

        if (powerUpActive)
            usleep(60000);   // faster during power-up
        else
            usleep(100000);  // normal speed
    }

    endwin();
    return 0;
}

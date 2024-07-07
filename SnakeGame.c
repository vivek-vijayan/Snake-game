/*
NOKIA SNAKE GAME

Developer: Vivek Vijayan
Date: 7 Jul 2024

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WINDOWSIZE 30
typedef long long int llt;

#ifdef _WIN32
#include <windows.h>
void delay(int milliseconds) {
    Sleep(milliseconds);
}
void clearScreen() {
    system("cls");
}
char get_arrow_key() {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            switch (_getch()) {
                case 72: return 'U'; // Up
                case 80: return 'D'; // Down
                case 75: return 'L'; // Left
                case 77: return 'R'; // Right
            }
        }
    }
    return 0;
}
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
void delay(int milliseconds) {
    usleep(milliseconds * 1000);
}
void clearScreen() {
    system("clear");
}
void set_nonblocking() {
    struct termios old, new;
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}
void restore_terminal() {
    struct termios old;
    tcgetattr(STDIN_FILENO, &old);
    old.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}
char get_arrow_key() {
    char buf[3];
    if (read(STDIN_FILENO, buf, 3) < 0) return 0;
    if (buf[0] == '\033' && buf[1] == '[') {
        switch (buf[2]) {
            case 'A': return 'U'; // Up
            case 'B': return 'D'; // Down
            case 'C': return 'R'; // Right
            case 'D': return 'L'; // Left
        }
    }
    return 0;
}
#endif

// GLOBAL VARIABLES
int snake_size = 7;
char placeholder[WINDOWSIZE][WINDOWSIZE];
llt snakeX[WINDOWSIZE * WINDOWSIZE];
llt snakeY[WINDOWSIZE * WINDOWSIZE];
int direction = 1;
int foodX = 0, foodY = 0;
int front = -1, rear = -1;

// FUNCTION DECLARATIONS
void add_recent_visit_pos(int x, int y);
void remove_last_visit_pos();
void initialize_game();
void print_game_state(int points);
void generate_food();
bool is_collision(int x, int y);
void handle_input();

// MAIN FUNCTION
int main() {
    #ifndef _WIN32
    set_nonblocking();
    #endif

    srand(time(NULL));
    initialize_game();

    bool gamemode = true;
    int runnerX = 1, runnerY = 1;
    int points = 0;

    while (gamemode) {
        print_game_state(points);
        delay(100);

        if (is_collision(runnerX, runnerY)) {
            gamemode = false;
            printf("GAME OVER!!! Thanks for playing Snake game. Developed by Vivek Vijayan 2024\n");
            break;
        } else {
            clearScreen();
        }

        if (runnerX == foodX && runnerY == foodY) {
            points += 5;
            generate_food();
            snake_size += 2;
        }

        move_snake(&runnerX, &runnerY);
        handle_input();
    }

    #ifndef _WIN32
    restore_terminal();
    #endif

    return 0;
}

// FUNCTION DEFINITIONS
void add_recent_visit_pos(int x, int y) {
    if (front == -1) front++;
    snakeX[++rear] = x;
    snakeY[rear] = y;
}

void remove_last_visit_pos() {
    if ((rear - front) > snake_size) {
        int px = snakeX[front];
        int py = snakeY[front];
        placeholder[px][py] = ' ';
        front++;
    }
}

void initialize_game() {
    generate_food();
    for (int row = 0; row < WINDOWSIZE; row++) {
        for (int col = 0; col < WINDOWSIZE; col++) {
            if (row == 0 || row == WINDOWSIZE - 1 || col == 0 || col == WINDOWSIZE - 1)
                placeholder[row][col] = '*'; // outer frame
            else
                placeholder[row][col] = ' '; // inner space
        }
    }
    placeholder[foodX][foodY] = '#';
}

void print_game_state(int points) {
    printf("SNAKE GAME \n\nDeveloped by Vivek Vijayan 2024 \n\n");
    printf("Rules: Never hit the wall & Never hit snake body\n\n");
    printf("POINTS: %d\n", points);
    for (int row = 0; row < WINDOWSIZE; row++) {
        for (int col = 0; col < WINDOWSIZE; col++) {
            printf(" %c ", placeholder[row][col]);
        }
        printf("\n");
    }
}

void generate_food() {
    foodX = rand() % (WINDOWSIZE - 2) + 1;
    foodY = rand() % (WINDOWSIZE - 2) + 1;
    placeholder[foodX][foodY] = '#';
}

bool is_collision(int x, int y) {
    return placeholder[x][y] == '^' || placeholder[x][y] == '=';
}

void move_snake(int* runnerX, int* runnerY) {
    switch (direction) {
        case 4:
            placeholder[(*runnerX)++][*runnerY] = '^';
            break;
        case 3:
            placeholder[(*runnerX)--][*runnerY] = '^';
            break;
        case 1:
            placeholder[*runnerX][(*runnerY)++] = '=';
            break;
        case 2:
            placeholder[*runnerX][(*runnerY)--] = '=';
            break;
    }
    add_recent_visit_pos(*runnerX, *runnerY);
    remove_last_visit_pos();
}

void handle_input() {
    char c = get_arrow_key();
    if (c) {
        switch (c) {
            case 'U': direction = 3; break;
            case 'D': direction = 4; break;
            case 'R': direction = 1; break;
            case 'L': direction = 2; break;
        }
    }
}
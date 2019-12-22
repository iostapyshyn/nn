#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define GRID_MARGIN 10
#define GRID_CELL 20

#define GRID_COLS 32
#define GRID_ROWS 24

#define WINDOW_WIDTH (GRID_COLS*GRID_CELL+GRID_MARGIN*2)
#define WINDOW_HEIGHT (GRID_ROWS*GRID_CELL+GRID_MARGIN*2)

enum direction {
    NORTH, SOUTH, WEST, EAST
} direction = SOUTH, prevdirection = NORTH;

Vector2 food = { 5, 5 };

int snake_len = 5;
Vector2 snake[100] = { {0, 0}, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }};

void randomfood() {
    food.x = rand() % GRID_COLS;
    food.y = rand() % GRID_ROWS;
}

int possibledirection(enum direction dir) {
    Vector2 new;
    switch (dir) {
    case NORTH:
        new = (Vector2) { snake[0].x, snake[0].y - 1 }; break;
    case SOUTH:
        new = (Vector2) { snake[0].x, snake[0].y + 1 }; break;
    case WEST:
        new = (Vector2) { snake[0].x + 1, snake[0].y }; break;
    case EAST:
        new = (Vector2) { snake[0].x - 1, snake[0].y }; break;
    }

    for (int i = 0; i < snake_len; i++) {
        if (new.x == snake[i].x && new.y == snake[i].y) {
            switch(i) {
            case 1: return 0;
            }
        }
    }

    return 1;
}

void snake_move() {
    Vector2 prev = snake[0];
    switch (direction) {
    case NORTH:
        snake[0] = (Vector2) { snake[0].x, snake[0].y - 1 }; break;
    case SOUTH:
        snake[0] = (Vector2) { snake[0].x, snake[0].y + 1 }; break;
    case WEST:
        snake[0] = (Vector2) { snake[0].x + 1, snake[0].y }; break;
    case EAST:
        snake[0] = (Vector2) { snake[0].x - 1, snake[0].y }; break;
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_len++;
        randomfood();
    }

    for (int i = 1; i < snake_len; i++) {
        Vector2 tmp = snake[i];
        snake[i] = prev;
        prev = tmp;
    }

}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dla debili");

    srand(time(NULL));

    double timeLast = GetTime();
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT) && possibledirection(WEST)) {
            direction = WEST;
        } if (IsKeyDown(KEY_LEFT) && possibledirection(EAST)) {
            direction = EAST;
        } if (IsKeyDown(KEY_UP) && possibledirection(NORTH)) {
            direction = NORTH;
        } if (IsKeyDown(KEY_DOWN) && possibledirection(SOUTH)) {
            direction = SOUTH;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("Die Python wird deinen Arsch penetrieren!", GRID_MARGIN, 0, GRID_MARGIN, LIGHTGRAY);

        /* Draw the grid */
        for (int i = 0; i <= GRID_ROWS; i++) {
            DrawLine(GRID_MARGIN, i*GRID_CELL+GRID_MARGIN, WINDOW_WIDTH-GRID_MARGIN, i*GRID_CELL+GRID_MARGIN, LIGHTGRAY);
        }
        for (int i = 0; i <= GRID_COLS; i++) {
            DrawLine(i*GRID_CELL+GRID_MARGIN, GRID_MARGIN, i*GRID_CELL+GRID_MARGIN, WINDOW_HEIGHT-GRID_MARGIN, LIGHTGRAY);
        }

        DrawRectangle(food.x*GRID_CELL+GRID_MARGIN, food.y*GRID_CELL+GRID_MARGIN, GRID_CELL, GRID_CELL, GREEN);

        for (int i = 0; i < snake_len; i++) {
            DrawRectangle(snake[i].x*GRID_CELL+GRID_MARGIN, snake[i].y*GRID_CELL+GRID_MARGIN, GRID_CELL, GRID_CELL, RED);
        }

        EndDrawing();

        double timeNow = GetTime();
        if (timeNow - timeLast > 0.1) {
            snake_move();
            timeLast = timeNow;
        }
    }


    CloseWindow();

    return 0;
}

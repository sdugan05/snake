#ifndef SNAKE_H
#define SNAKE_H

#include "util.h"

#include <raylib.h>
#include <stdio.h>
#include <time.h>

#define ROWS 20
#define COLS 20
#define CELL_SIZE 40

typedef struct {
    u32 rows;
    u32 cols;
    u32 size;
    b8 game_over;
    u32 score;
    u32 high_score;
} game_config;

typedef enum { SNAKE, APPLE, EMPTY } cell_type;

typedef struct {
    u32 size;
    i8 x_dir;
    i8 y_dir;
    u32* values;
} Snake;

typedef struct {
    u32 rows;
    u32 cols;
    i32 apple;
    cell_type* values;
} Board;

game_config* init_config(mem_arena* arena);

Board* init_board(mem_arena* arena, game_config* config);

Snake* init_snake(mem_arena* arena, game_config* config);

void update_snake_dir(Snake* snake);

void update(Board* board, Snake* snake, game_config* config);

void draw(Board* board, Snake* snake, game_config* config);

void snake_run(void);

#endif

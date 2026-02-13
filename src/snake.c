#include "snake.h"
#include "util.h"
#include <raylib.h>
#include <stdio.h>

#define ROWS 10
#define COLS 10
#define CELL_SIZE 40

typedef struct {
    u32 rows;
    u32 cols;
    u32 size;
    u32 snake_init_pos;
    b8 game_over;
} game_config;

typedef enum {
    SNAKE,
    APPLE,
    EMPTY
} cell_type;

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

void snake_run(void) {
    printf("Hello from snake.c!\n");

                                                                                
    mem_arena* game_arena = arena_create(GiB(1), MiB(1));
    game_config* config = init_config(game_arena);
    Board* board = init_board(game_arena, config);
    Snake* snake = init_snake(game_arena, config);


                                                                                

    InitWindow(400, 400, "Snake");
    SetTargetFPS(4);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        {
            update(board, snake, config);
            draw(board, snake, config);
            if (config->game_over) {
                printf("Game Over!\n");
                return;
            }
        }
        
        EndDrawing();
    }
    CloseWindow();

                                                                                

    arena_destroy(game_arena);
    return;
}

game_config* init_config(mem_arena* arena) {
    game_config* config = PUSH_STRUCT(arena, game_config);
    config->cols = COLS;
    config->rows = ROWS;
    config->size = ROWS * COLS;
    config->game_over = false;

    return config;
}

Board* init_board(mem_arena* arena, game_config* config) {
    Board* board = PUSH_STRUCT(arena, Board);
    board->rows = config->rows;
    board->cols = config->cols;
    board->values = PUSH_ARRAY(arena, cell_type, config->size);
    for (u32 i = 0; i < config->size; i++) {
        board->values[i] = EMPTY;
    }

    board->apple = 50;

    return board;
}

Snake* init_snake(mem_arena* arena, game_config* config) {
    Snake* snake = PUSH_STRUCT(arena, Snake);
    snake->values = PUSH_ARRAY(arena, u32, config->size);

    snake->values[0] = 32;
    snake->values[1] = 31;
    snake->values[2] = 30;

    snake->size = 3;

    snake->x_dir = 1;
    snake->y_dir = 0;

    return snake;
}

void update_snake_dir(Snake* snake) {
    if (IsKeyPressed(KEY_W)) {
            snake->y_dir = -1;
            snake->x_dir = 0;
        }
        if (IsKeyPressed(KEY_A)) {
            snake->y_dir = 0;
            snake->x_dir = -1;
        }
        if (IsKeyPressed(KEY_S)) {
            snake->y_dir = 1;
            snake->x_dir = 0;
        }
        if (IsKeyPressed(KEY_D)) {
            snake->y_dir = 0;
            snake->x_dir = 1;
        }
}


void update(Board* board, Snake* snake, game_config* config) {
    update_snake_dir(snake);

    u32 new_idx = snake->x_dir + snake->values[0] + (snake->y_dir * config->cols);
    if (new_idx < 0 || new_idx > config->size) {
        printf("Game over because new_idx (%d) out of bounds\n", new_idx);
        config->game_over = true;
        return;
    }
    if ((new_idx / config->cols != snake->values[0] / config->cols) && snake->y_dir == 0) {
        printf("Game over because snake doesnt wrap. new_idx: %d\n", new_idx);
        printf("nix (%d) / cols (%d) : %d, val0 / cols: %d", new_idx, config->cols, new_idx / config->cols, snake->values[0] / config->cols);
        config->game_over = true;
        return;
    }
    for (u32 j = 1; j < snake->size; j++) {
        if (new_idx == snake->values[j]) {
            printf("Game over because new_idx (%d) equals snake->values[j]\n", new_idx);
            config->game_over = true;
            return;
        }
    }
    if (new_idx == board->apple) {
        snake->size++;
        board->apple = -1;
    }
    for (u32 i = snake->size - 1; i > 0; i--) {
        snake->values[i] = snake->values[i-1];

    }
    snake->values[0] = new_idx;

    for (u32 i = 0; i < config->size; i++) {
        board->values[i] = EMPTY;
    }

    for (u32 i = 0; i < snake->size; i++) {
        board->values[snake->values[i]] = SNAKE;
    }

    if (board->apple == -1) {
        b8 done = false;
        while (!done) {
            u32 new_apple_idx = prng_rand() & config->size;
            if (board->values[new_apple_idx] != EMPTY) { break; }
            board->apple = new_apple_idx;
        }
    }
    board->values[board->apple] = APPLE;
}

void draw(Board* board, Snake* snake, game_config* config) {
    for (u32 i = 0; i < config->size; i++) {
        u32 row = i / config->cols;
        u32 col = i % config->cols;
        switch (board->values[i]) {
            case EMPTY: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){0,0,0,255}); break;
            case SNAKE: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){0,220,0,255}); break;
            case APPLE: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){220,0,0,255}); break;
        }
    }
}



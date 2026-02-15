#include "snake.h"

void snake_run(void) {
    printf("Hello from snake.c!\n");

                                                                                

    mem_arena* game_arena = arena_create(GiB(1), MiB(1));
    game_config* config = init_config(game_arena);
    Board* board = init_board(game_arena, config);
    Snake* snake = init_snake(game_arena, config);


                                                                                

    InitWindow(800, 900, "Snake");
    SetTargetFPS(6);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        {
            if (!config->game_over) {
                update(board, snake, config);
                DrawText(TextFormat("Score: %d", snake->size - 3), 20, 10, 40, BLACK);
                DrawText(TextFormat("High Score: %d", config->high_score), 20, 50, 40, BLACK);
            }
            else {
                printf("Game Over!\n");
                config->high_score = MAX(config->score, config->high_score);
                DrawText(TextFormat("Score: %d", snake->size - 3), 300, 10, 40, BLACK);
                DrawText(TextFormat("High Score: %d", config->high_score), 300, 50, 40, BLACK);
                DrawText("Game Over!", 20, 20, 40, RED);
                DrawText("Press R to restart.", 400, 500, 60, RED);
                if (IsKeyPressed(KEY_R)) {
                    u32 high_score = config->high_score;
                    arena_clear(game_arena);
                    config = init_config(game_arena);
                    board = init_board(game_arena, config);
                    snake = init_snake(game_arena, config);
                    config->high_score = high_score;
                }
            }
            draw(board, snake, config);
            if (config->game_over) {
                DrawText("Press R to restart.", 20, 500, 60, RED);
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
    config->score = 0;
    config->high_score = 0;

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

    prng_seed((u32)time(0), 0);
    board->apple = 50;

    return board;
}

Snake* init_snake(mem_arena* arena, game_config* config) {
    Snake* snake = PUSH_STRUCT(arena, Snake);
    snake->values = PUSH_ARRAY(arena, u32, config->size);

    snake->values[0] = 207;
    snake->values[1] = 206;
    snake->values[2] = 205;

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
            u32 new_apple_idx = prng_rand() % config->size;
            if (board->values[new_apple_idx] != EMPTY) { break; }
            board->apple = new_apple_idx;
        }
    }
    board->values[board->apple] = APPLE;

    config->score = snake->size - 3;
}

void draw(Board* board, Snake* snake, game_config* config) {
    for (u32 i = 0; i < config->size; i++) {
        u32 row = i / config->cols;
        u32 col = i % config->cols;
        switch (board->values[i]) {
            case EMPTY: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE + 100, CELL_SIZE, CELL_SIZE, (Color){0,0,0,255}); break;
            case SNAKE: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE + 100, CELL_SIZE, CELL_SIZE, (Color){0,220,0,255}); break;
            case APPLE: DrawRectangle(col * CELL_SIZE, row * CELL_SIZE + 100, CELL_SIZE, CELL_SIZE, (Color){220,0,0,255}); break;
        }
    }
}



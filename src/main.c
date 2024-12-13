#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define GAME_TICK 100 // ms
#define SNAKE_SIZE 24 // px
#define GRID_WIDTH 32 // units
#define GRID_HEIGHT 24 // units
#define SCREEN_WIDTH (GRID_WIDTH * SNAKE_SIZE)
#define SCREEN_HEIGHT (GRID_HEIGHT * SNAKE_SIZE)

// Enums
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Structs
typedef struct {
    int x, y;
} Position;

typedef struct {
    Position positions[GRID_WIDTH * GRID_HEIGHT];
    int length;
    Direction direction;
    Direction next_direction;
    bool hit_wall;
    bool hit_body;
} Snake;

typedef struct {
    Position position;
} Food;

void init_snake(Snake *snake) {
    snake->hit_wall = false;
    snake->hit_body = false;
    snake->positions[0].x = GRID_WIDTH / 5;
    snake->positions[0].y = GRID_HEIGHT / 2;
    snake->direction = RIGHT;
    snake->next_direction = RIGHT;
    snake->length = 2;
}

bool check_collision(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

bool is_position_on_snake(Position position, Snake *snake) {
    for (int i = 0; i < snake->length; i++) {
        if (check_collision(position, snake->positions[i])) {
            return true;
        }
    }
    return false;
}

void update_food_position(Food *food, Snake *snake) {
    Position new_food_position;

    do {
        //new_food_position.x = rand() % GRID_WIDTH;
        //new_food_position.y = rand() % GRID_HEIGHT;

        // Generate a new food position, keeping a 1-cell space from the edges
        new_food_position.x = 2 + rand() % (GRID_WIDTH - 3);
        new_food_position.y = 2 + rand() % (GRID_HEIGHT - 3);
    } while (is_position_on_snake(new_food_position, snake));

    food->position = new_food_position;
}

bool draw_pause_screen(SDL_Renderer *renderer, TTF_Font *font, Snake *snake) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    SDL_Color text_color = {255, 255, 255, 255};

    SDL_Surface *title_surface;
    SDL_Texture *title_texture;
    SDL_Rect title_rect;

    SDL_Surface *score_surface;
    SDL_Texture *score_texture;
    SDL_Rect score_rect;

    SDL_Surface *text1_surface;
    SDL_Texture *text1_texture;
    SDL_Rect text1_rect;

    SDL_Surface *text2_surface;
    SDL_Texture *text2_texture;
    SDL_Rect text2_rect;

    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", snake->length - 2);

    title_surface = TTF_RenderText_Solid(font, "- PAUSED -", text_color);
    score_surface = TTF_RenderText_Solid(font, score_text, text_color);
    text1_surface = TTF_RenderText_Solid(font, "Press 'Esc' to continue,", text_color);
    text2_surface = TTF_RenderText_Solid(font, "'R' to restart, or 'Q' to quit.", text_color);

    title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    text1_texture = SDL_CreateTextureFromSurface(renderer, text1_surface);
    text2_texture = SDL_CreateTextureFromSurface(renderer, text2_surface);

    title_rect.x = (SCREEN_WIDTH - title_surface->w) / 2;
    title_rect.y = (SCREEN_HEIGHT - title_surface->h) / 2 - title_surface->h * 3;
    title_rect.w = title_surface->w;
    title_rect.h = title_surface->h;

    score_rect.x = (SCREEN_WIDTH - score_surface->w) / 2;
    score_rect.y = (SCREEN_HEIGHT - score_surface->h) / 2 - score_surface->h;
    score_rect.w = score_surface->w;
    score_rect.h = score_surface->h;

    text1_rect.x = (SCREEN_WIDTH - text1_surface->w) / 2;
    text1_rect.y = (SCREEN_HEIGHT - text1_surface->h) / 2 + text1_surface->h;
    text1_rect.w = text1_surface->w;
    text1_rect.h = text1_surface->h;

    text2_rect.x = (SCREEN_WIDTH - text2_surface->w) / 2;
    text2_rect.y = (SCREEN_HEIGHT - text2_surface->h) / 2 + text2_surface->h * 2;
    text2_rect.w = text2_surface->w;
    text2_rect.h = text2_surface->h;

    SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    SDL_RenderCopy(renderer, text1_texture, NULL, &text1_rect);
    SDL_RenderCopy(renderer, text2_texture, NULL, &text2_rect);

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(title_texture);
    SDL_DestroyTexture(score_texture);
    SDL_DestroyTexture(text1_texture);
    SDL_DestroyTexture(text2_texture);

    SDL_FreeSurface(title_surface);
    SDL_FreeSurface(score_surface);
    SDL_FreeSurface(text1_surface);
    SDL_FreeSurface(text2_surface);

    SDL_Event event;

    while (true) {
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            exit(0);
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_RETURN:
                case SDLK_ESCAPE:
                    return false;
                case SDLK_r:
                    return true;
                    break;
                case SDLK_q:
                    exit(0);
                    break;
                default:
                    break;
            }
        }
    }
}

bool draw_game_over_screen(SDL_Renderer *renderer, TTF_Font *font, Snake *snake) {
    SDL_Color text_color = {255, 255, 255, 255};

    SDL_Surface *title_surface;
    SDL_Texture *title_texture;
    SDL_Rect title_rect;

    SDL_Surface *score_surface;
    SDL_Texture *score_texture;
    SDL_Rect score_rect;

    SDL_Surface *text1_surface;
    SDL_Texture *text1_texture;
    SDL_Rect text1_rect;

    SDL_Surface *text2_surface;
    SDL_Texture *text2_texture;
    SDL_Rect text2_rect;

    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", snake->length - 2);

    title_surface = TTF_RenderText_Solid(font, "- GAME OVER -", text_color);
    score_surface = TTF_RenderText_Solid(font, score_text, text_color);
    text1_surface = TTF_RenderText_Solid(font, "Press 'Enter' to restart", text_color);
    text2_surface = TTF_RenderText_Solid(font, "or 'Esc' to quit.", text_color);

    title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    text1_texture = SDL_CreateTextureFromSurface(renderer, text1_surface);
    text2_texture = SDL_CreateTextureFromSurface(renderer, text2_surface);

    title_rect.x = (SCREEN_WIDTH - title_surface->w) / 2;
    title_rect.y = (SCREEN_HEIGHT - title_surface->h) / 2 - title_surface->h * 3;
    title_rect.w = title_surface->w;
    title_rect.h = title_surface->h;

    score_rect.x = (SCREEN_WIDTH - score_surface->w) / 2;
    score_rect.y = (SCREEN_HEIGHT - score_surface->h) / 2 - score_surface->h;
    score_rect.w = score_surface->w;
    score_rect.h = score_surface->h;

    text1_rect.x = (SCREEN_WIDTH - text1_surface->w) / 2;
    text1_rect.y = (SCREEN_HEIGHT - text1_surface->h) / 2 + text1_surface->h;
    text1_rect.w = text1_surface->w;
    text1_rect.h = text1_surface->h;

    text2_rect.x = (SCREEN_WIDTH - text2_surface->w) / 2;
    text2_rect.y = (SCREEN_HEIGHT - text2_surface->h) / 2 + text2_surface->h * 2;
    text2_rect.w = text2_surface->w;
    text2_rect.h = text2_surface->h;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the draw color to black
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, title_texture, NULL, &title_rect);
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    SDL_RenderCopy(renderer, text1_texture, NULL, &text1_rect);
    SDL_RenderCopy(renderer, text2_texture, NULL, &text2_rect);

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(title_texture);
    SDL_DestroyTexture(score_texture);
    SDL_DestroyTexture(text1_texture);
    SDL_DestroyTexture(text2_texture);

    SDL_FreeSurface(title_surface);
    SDL_FreeSurface(score_surface);
    SDL_FreeSurface(text1_surface);
    SDL_FreeSurface(text2_surface);

    SDL_Event event;

    while (true) {
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            exit(0);
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                case SDLK_SPACE:
                    return true;
                case SDLK_ESCAPE:
                case SDLK_q:
                    return false;
                default:
                    break;
            }
        }
    }
}

void initialize(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    //SDL_SetRenderDrawColor(*renderer, 0x1E, 0x1E, 0x1E, 0xFF);
    SDL_SetRenderDrawColor(*renderer, 0x00, 0x00, 0x00, 0xFF);

    // Initialize TTF
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void draw_score(SDL_Renderer *renderer, int score, TTF_Font *font) {
    SDL_Color text_color = {0xFF, 0xFF, 0xFF, 0xFF};
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "%d", score);

    SDL_Surface *score_surface = TTF_RenderText_Solid(font, score_text, text_color);
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    SDL_Rect score_rect = {SCREEN_WIDTH - score_surface->w - SNAKE_SIZE, SNAKE_SIZE, score_surface->w, score_surface->h};

    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);

    // Clean up
    SDL_DestroyTexture(score_texture);
    SDL_FreeSurface(score_surface);
}

void draw(SDL_Renderer *renderer, Snake *snake, Food *food, int score, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // Draw background grid
    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 0xFF);

    // Grid lines
    /*for (int y = SNAKE_SIZE; y < SCREEN_HEIGHT; y += SNAKE_SIZE) {
        SDL_Rect rect = {0, y-1, SCREEN_WIDTH, 2};
        SDL_RenderFillRect(renderer, &rect);
    }
    for (int x = SNAKE_SIZE; x < SCREEN_WIDTH; x += SNAKE_SIZE) {
        SDL_Rect rect = {x-1, 0, 2, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);
    }*/

    // Dotted grid
    for (int y = SNAKE_SIZE; y < SCREEN_HEIGHT; y += SNAKE_SIZE) {
        for (int x = SNAKE_SIZE; x < SCREEN_WIDTH; x += SNAKE_SIZE) {
            SDL_Rect rect = {x-1, y-1, 2, 2};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Draw the snake (white color)
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = {snake->positions[i].x * SNAKE_SIZE, snake->positions[i].y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw the food (red color)
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect food_rect = {food->position.x * SNAKE_SIZE, food->position.y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE};
    SDL_RenderFillRect(renderer, &food_rect);

    draw_score(renderer, score, font);
}

void update_snake(Snake *snake, Food *food, bool *game_over) {
    snake->direction = snake->next_direction;

    Position next_position = snake->positions[0];

    switch (snake->direction) {
        case UP:
            next_position.y--;
            break;
        case DOWN:
            next_position.y++;
            break;
        case LEFT:
            next_position.x--;
            break;
        case RIGHT:
            next_position.x++;
            break;
    }

    // Hitting wall
    if (next_position.x < 0 || next_position.x >= GRID_WIDTH ||
        next_position.y < 0 || next_position.y >= GRID_HEIGHT) {
        if (snake->hit_wall) {
            *game_over = true;
            return;
        } else {
            snake->hit_wall = true;
            return;
        }
    }

    snake->hit_wall = false;

    // Hitting body
    for (int i = 1; i < snake->length; i++) {
        if (check_collision(next_position, snake->positions[i])) {
            if (snake->hit_body) {
                *game_over = true;
                return;
            } else {
                snake->hit_body = true;
                return;
            }
        }
    }

    snake->hit_body = false;

    for (int i = snake->length - 1; i > 0; i--) {
        snake->positions[i] = snake->positions[i - 1];
    }

    snake->positions[0] = next_position;

    if (check_collision(next_position, food->position)) {
        snake->length++;

        // Add a new segment to the snake's tail
        snake->positions[snake->length - 1] = snake->positions[snake->length - 2];

        // Update food position
        update_food_position(food, snake);
    }
}

void handle_input(Snake *snake, bool *paused) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_w:
                case SDLK_i:
                    if (snake->direction == DOWN)
                        break;
                    snake->next_direction = UP;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                case SDLK_k:
                    if (snake->direction == UP)
                        break;
                    snake->next_direction = DOWN;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                case SDLK_j:
                    if (snake->direction == RIGHT)
                        break;
                    snake->next_direction = LEFT;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                case SDLK_l:
                    if (snake->direction == LEFT)
                        break;
                    snake->next_direction = RIGHT;
                    break;
                case SDLK_ESCAPE:
                    *paused = true;
                    break;
                default:
                    break;
            }
        }
    }
}

void game_loop(SDL_Renderer *renderer, TTF_Font *font, Snake *snake, bool *game_over) {
    bool paused = false;

    Food food;

    srand(time(NULL));
    update_food_position(&food, snake);

    Uint32 previous_time = SDL_GetTicks();
    Uint32 elapsed_time = 0;

    while (!*game_over) {

        handle_input(snake, &paused);

        elapsed_time = SDL_GetTicks() - previous_time;
        if (elapsed_time < GAME_TICK)
            continue;
        previous_time = SDL_GetTicks();

        if (paused) {
            draw_pause_screen(renderer, font, snake);
            paused = false;
            continue;
        }

        update_snake(snake, &food, game_over);

        if (*game_over)
            break;

        draw(renderer, snake, &food, snake->length - 2, font);
        SDL_RenderPresent(renderer);
    }
}

int main(/*int argc, char *argv[]*/) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Snake snake;
    bool restart = false;
    bool game_over = false;

    initialize(&window, &renderer);

    TTF_Font *font = TTF_OpenFont("retro.ttf", 24);

    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    do {
        init_snake(&snake);
        game_loop(renderer, font, &snake, &game_over);

        if (game_over) {
            restart = draw_game_over_screen(renderer, font, &snake);
            game_over = false;
        }
    } while (restart);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
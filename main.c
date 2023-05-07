#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SNAKE_SIZE 20
#define SNAKE_SPEED 100
#define GRID_WIDTH (SCREEN_WIDTH / SNAKE_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / SNAKE_SIZE)

// Enums
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

// Structs
typedef struct {
    int x, y;
} Position;

typedef struct {
    Position positions[GRID_WIDTH * GRID_HEIGHT];
    int length;
    Direction direction;
    Direction nextDirection;
    bool hitWall;
    bool hitBody;
} Snake;

typedef struct {
    Position position;
} Food;

void init_snake(Snake *snake) {
    snake->length = 2;
    snake->direction = RIGHT;
    snake->nextDirection = RIGHT;
    snake->positions[0].x = GRID_WIDTH / 2;
    snake->positions[0].y = GRID_HEIGHT / 2;
    snake->hitWall = false;
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
    Position newFoodPosition;

    do {
        //newFoodPosition.x = rand() % GRID_WIDTH;
        //newFoodPosition.y = rand() % GRID_HEIGHT;

        // Generate a new food position, keeping a 1-cell margin from the edges
        newFoodPosition.x = 2 + rand() % (GRID_WIDTH - 3);
        newFoodPosition.y = 2 + rand() % (GRID_HEIGHT - 3);
    } while (is_position_on_snake(newFoodPosition, snake));

    food->position = newFoodPosition;
}

bool show_game_over_screen(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255};

    SDL_Surface *textSurface1;
    SDL_Texture *textTexture1;
    SDL_Rect textRect1;
    SDL_Surface *textSurface2;
    SDL_Texture *textTexture2;
    SDL_Rect textRect2;

    textSurface1 = TTF_RenderText_Solid(font, "Game Over!", textColor);
    textSurface2 = TTF_RenderText_Solid(font, "Press Enter to restart, or Esc to quit.", textColor);

    textTexture1 = SDL_CreateTextureFromSurface(renderer, textSurface1);
    textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);

    textRect1.x = (SCREEN_WIDTH - textSurface1->w) / 2;
    textRect1.y = (SCREEN_HEIGHT - textSurface1->h) / 2 - textSurface1->h;
    textRect1.w = textSurface1->w;
    textRect1.h = textSurface1->h;

    textRect2.x = (SCREEN_WIDTH - textSurface2->w) / 2;
    textRect2.y = (SCREEN_HEIGHT - textSurface2->h) / 2 + textSurface1->h;
    textRect2.w = textSurface2->w;
    textRect2.h = textSurface2->h;

    SDL_Event e;
    bool quit = false;
    bool restart = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                    quit = true;
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    quit = true;
                    restart = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the draw color to black
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture1, NULL, &textRect1);
        SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(textTexture1);
    SDL_DestroyTexture(textTexture2);
    SDL_FreeSurface(textSurface1);
    SDL_FreeSurface(textSurface2);
    TTF_CloseFont(font);

    return restart;
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

    SDL_SetRenderDrawColor(*renderer, 0x1E, 0x1E, 0x1E, 0xFF);

    // Initialize TTF
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void draw_score(SDL_Renderer *renderer, int score, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255}; // White color
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "%d", score);

    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {SCREEN_WIDTH - scoreSurface->w - 10, 10, scoreSurface->w, scoreSurface->h};

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    // Clean up
    SDL_DestroyTexture(scoreTexture);
    SDL_FreeSurface(scoreSurface);
}

void draw(SDL_Renderer *renderer, Snake *snake, Food *food, int score, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    // Draw the snake (white color)
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = {snake->positions[i].x * SNAKE_SIZE, snake->positions[i].y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw the food (red color)
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect foodRect = {food->position.x * SNAKE_SIZE, food->position.y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    draw_score(renderer, score, font);
}

void update_snake(Snake *snake, Food *food, bool *gameOver) {
    snake->direction = snake->nextDirection;

    Position nextPosition = snake->positions[0];

    switch (snake->direction) {
        case UP:
            nextPosition.y--;
            break;
        case DOWN:
            nextPosition.y++;
            break;
        case LEFT:
            nextPosition.x--;
            break;
        case RIGHT:
            nextPosition.x++;
            break;
    }

    // Hitting wall
    if (nextPosition.x < 0 || nextPosition.x >= GRID_WIDTH ||
        nextPosition.y < 0 || nextPosition.y >= GRID_HEIGHT) {
        if (snake->hitWall) {
            *gameOver = true;
            return;
        } else {
            snake->hitWall = true;
            return;
        }
    }

    snake->hitWall = false;

    // Hitting body
    for (int i = 1; i < snake->length; i++) {
        if (check_collision(nextPosition, snake->positions[i])) {
            if (snake->hitBody) {
                *gameOver = true;
                return;
            } else {
                snake->hitBody = true;
                return;
            }
        }
    }

    snake->hitBody = false;

    for (int i = snake->length - 1; i > 0; i--) {
        snake->positions[i] = snake->positions[i - 1];
    }

    snake->positions[0] = nextPosition;

    if (check_collision(nextPosition, food->position)) {
        snake->length++;

        // Add a new segment to the snake's tail
        snake->positions[snake->length - 1] = snake->positions[snake->length - 2];

        // Update food position
        update_food_position(food, snake);
    }
}

void handle_input(Snake *snake) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_w:
                case SDLK_i:
                    if (snake->direction != DOWN) {
                        snake->nextDirection = UP;
                    }
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                case SDLK_k:
                    if (snake->direction != UP) {
                        snake->nextDirection = DOWN;
                    }
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                case SDLK_j:
                    if (snake->direction != RIGHT) {
                        snake->nextDirection = LEFT;
                    }
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                case SDLK_l:
                    if (snake->direction != LEFT) {
                        snake->nextDirection = RIGHT;
                    }
                    break;
                case SDLK_ESCAPE:
                case SDLK_q:
                    exit(0);
                    break;
                default:
                    break;
            }
        }
    }
}

void game_loop(SDL_Renderer *renderer, TTF_Font *font) {
    bool running = true;
    bool gameOver = false;

    Snake snake;
    Food food;

    init_snake(&snake);

    srand(time(NULL));
    update_food_position(&food, &snake);

    Uint32 previousTime = SDL_GetTicks();
    Uint32 elapsedTime = 0;

    while (running) {
        handle_input(&snake);

        elapsedTime = SDL_GetTicks() - previousTime;
        if (elapsedTime >= SNAKE_SPEED) {
            update_snake(&snake, &food, &gameOver);
            previousTime = SDL_GetTicks();
        }

        if (gameOver) {
            running = false;
        }

        SDL_RenderClear(renderer);
        draw(renderer, &snake, &food, snake.length - 2, font);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    initialize(&window, &renderer);

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 32);

    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    bool restart;
    do {
        game_loop(renderer, font);
        restart = show_game_over_screen(renderer, font);
    } while (restart);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

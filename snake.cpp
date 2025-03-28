#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <cstdlib>

#define HEIGHT 300
#define WIDTH 300
#define SEGMENT_SIZE 5

typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
}App;

struct Snake_Segment{
    int x;
    int y;
    Snake_Segment(int a, int b) : x(a), y(b) {}
};

typedef struct{
    int x;
    int y;
}Velocity;

typedef struct{
    int x;
    int y;
}Position;

App app;

void draw_snake(std::vector<Snake_Segment> snake){
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);

    for (int i = 0; i < snake.size(); i++) {
        SDL_Rect rect = {snake[i].x, snake[i].y, SEGMENT_SIZE, SEGMENT_SIZE};
        SDL_RenderFillRect(app.renderer, &rect);
    }
}

void draw_apple(Position apple_position){
    SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);
    SDL_Rect rect = {apple_position.x, apple_position.y, SEGMENT_SIZE, SEGMENT_SIZE};
    SDL_RenderFillRect(app.renderer, &rect);
}

bool can_i_place_an_apple(std::vector<Snake_Segment> snake, Position apple_position){
    for(int i = 0; i<snake.size(); i++){
        if(snake[i].x == apple_position.x && snake[i].y == apple_position.y){
            return false;
        }
    }
    return true;
}

int main(){

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    if (SDL_CreateWindowAndRenderer(HEIGHT, WIDTH, 0, &app.window, &app.renderer) != 0) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    //initialize a white window
    bool quit = false;
    SDL_Event event;
    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
    SDL_RenderClear(app.renderer);
    SDL_RenderPresent(app.renderer);

    //initializing a 3 segment snake
    std::vector<Snake_Segment> snake;
    snake.emplace_back(WIDTH/2,HEIGHT/2);
    snake.emplace_back(snake.back().x-SEGMENT_SIZE,snake.back().y);
    snake.emplace_back(snake.back().x-SEGMENT_SIZE,snake.back().y);
    
    Velocity snake_vel, prev_snake_vel;
    snake_vel.x = 0;
    snake_vel.y = 0;
    Position apple;
    do{
        apple.x = (rand()%(WIDTH/SEGMENT_SIZE+1))*SEGMENT_SIZE;
        apple.y = (rand()%(HEIGHT/SEGMENT_SIZE+1))*SEGMENT_SIZE;
    }while(!can_i_place_an_apple(snake, apple));

    Uint32 last_time = SDL_GetTicks();
    const int move_interval = 100;
    
    while(!quit){
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_w){
                    std::cout<< "up"<< std::endl;
                    snake_vel.x = 0;
                    snake_vel.y = -SEGMENT_SIZE;
                }
                if(event.key.keysym.sym == SDLK_a){
                    std::cout<< "left"<< std::endl;
                    snake_vel.x = -SEGMENT_SIZE;
                    snake_vel.y = 0;
                }
                if(event.key.keysym.sym == SDLK_s){
                    std::cout<< "down"<< std::endl;
                    snake_vel.x = 0;
                    snake_vel.y = SEGMENT_SIZE;
                }
                if(event.key.keysym.sym == SDLK_d){
                    std::cout<< "right"<< std::endl;
                    snake_vel.x = SEGMENT_SIZE;
                    snake_vel.y = 0;
                }
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    quit = true;
                }
            }
            
        }
        if(snake_vel.y == 0){
            if(prev_snake_vel.x == -snake_vel.x){
                snake_vel = prev_snake_vel;
            }
        }
        if(snake_vel.x == 0){
            if(prev_snake_vel.y == -snake_vel.y){
                snake_vel = prev_snake_vel;
            }
        }
        Uint32 current_time = SDL_GetTicks();
        if (current_time - last_time > move_interval){
            for (int i = snake.size() - 1; i > 0; i--){
                snake[i] = snake[i - 1];
            }

            snake[0].x = (snake[0].x + snake_vel.x + WIDTH) % WIDTH;
            snake[0].y = (snake[0].y + snake_vel.y + HEIGHT) % HEIGHT;

            last_time = current_time;
        }
        
        for(int i = 3; i<snake.size(); i++){
            if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
                quit = true;
            }
        }
        

        if(snake[0].x == apple.x && snake[0].y == apple.y){
            do{
                apple.x = (rand()%(WIDTH/SEGMENT_SIZE+1))*SEGMENT_SIZE;
                apple.y = (rand()%(HEIGHT/SEGMENT_SIZE+1))*SEGMENT_SIZE;
            }while(!can_i_place_an_apple(snake, apple));
            snake.emplace_back(snake.back().x-SEGMENT_SIZE,snake.back().y);
        }

        prev_snake_vel = snake_vel;
        SDL_RenderClear(app.renderer); 
        
        SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);  
        SDL_RenderFillRect(app.renderer, NULL);  
        //std::cout<< apple.x<< " "<< apple.y<< std::endl;
        draw_snake(snake);
        draw_apple(apple);

        SDL_RenderPresent(app.renderer);
    }


    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();

    return 0;
}
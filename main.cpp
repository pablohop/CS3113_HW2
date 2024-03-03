#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameOver = false;

float playerWidth = 0.02f, playerHeight = 0.4f;
float player1Position = 0.0f, player2Position = 0.0f;
float ballSize = 0.03f;
float ballPosX = 0.0f, ballPosY = 0.0f;
float ballDirX = 0.6f, ballDirY = 0.6f;
bool player2AI = false;

int player1Direction = 0;
int player2Direction = 0;

float paddleSpeed = 0.8f;

void drawRect(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2);
    glVertex2f(x + width / 2, y - height / 2);
    glVertex2f(x + width / 2, y + height / 2);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
}

bool checkCollision(float box1X, float box1Y, float box1Width, float box1Height, float box2X, float box2Y, float box2Width, float box2Height) {
    if (box1X + box1Width / 2 > box2X - box2Width / 2 &&
        box1X - box1Width / 2 < box2X + box2Width / 2 &&
        box1Y + box1Height / 2 > box2Y - box2Height / 2 &&
        box1Y - box1Height / 2 < box2Y + box2Height / 2) {
        return true;
    }
    return false;
}

void render(float deltaTime) {
    if (gameOver) {
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT);

    player1Position += player1Direction * paddleSpeed * deltaTime;
    player2Position += player2Direction * paddleSpeed * deltaTime;

    drawRect(-0.9f, player1Position, playerWidth, playerHeight);
    drawRect(0.9f, player2Position, playerWidth, playerHeight);

    drawRect(ballPosX, ballPosY, ballSize, ballSize);

    ballPosX += ballDirX * deltaTime;
    ballPosY += ballDirY * deltaTime;

    if (ballPosY >= 1.0f - ballSize / 2 || ballPosY <= -1.0f + ballSize / 2) {
        ballDirY = -ballDirY;
    }

    if (checkCollision(ballPosX, ballPosY, ballSize, ballSize, -0.9f, player1Position, playerWidth, playerHeight) ||
        checkCollision(ballPosX, ballPosY, ballSize, ballSize, 0.9f, player2Position, playerWidth, playerHeight)) {
        ballDirX = -ballDirX;
    }

    if (ballPosX <= -1.0f || ballPosX >= 1.0f) {
        gameOver = true;
    }

    if (player2AI) {
        if (ballPosY > player2Position) {
            player2Position += paddleSpeed * deltaTime;
        } else if (ballPosY < player2Position) {
            player2Position -= paddleSpeed * deltaTime;
        }
    }
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Homework 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    Uint32 lastTicks = SDL_GetTicks();
    
    SDL_Event event;
    while (gameIsRunning) {
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                gameIsRunning = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        player1Direction = 1;
                        break;
                    case SDLK_s:
                        player1Direction = -1;
                        break;
                    case SDLK_UP:
                        if (!player2AI) player2Direction = 1;
                        break;
                    case SDLK_DOWN:
                        if (!player2AI) player2Direction = -1;
                        break;
                    case SDLK_t:
                        player2AI = !player2AI;
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_s:
                        player1Direction = 0;
                        break;
                    case SDLK_UP:
                    case SDLK_DOWN:
                        if (!player2AI) player2Direction = 0;
                        break;
                }
            }
        }
        
        render(deltaTime);

        if (ballPosX <= -1.0f || ballPosX >= 1.0f) {
            gameIsRunning = false;
        }

        SDL_GL_SwapWindow(displayWindow);
    }

    SDL_Quit();
    return 0;
}

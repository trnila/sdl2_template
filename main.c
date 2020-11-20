#include <SDL2/SDL_keycode.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "SDL2/SDL_mixer.h"
#include "sdl.h"

int main(int argc, char **argv) {
  int width = 800;
  int height = 600;

  if(argc >= 2) {
    if(sscanf(argv[1], "%dx%d", &width, &height) != 2) {
      fprintf(stderr, "Usage: %s WxH\n", argv[0]);
      exit(1);
    }
  }

  SDL_Context ctx = sdl_context_init("game", width, height);

  Mix_Music *shotSound = Mix_LoadMUS("resources/shot.ogg");
  if(shotSound == NULL) {
    fprintf(stderr, "Could not load sound: %s\n", Mix_GetError());
    exit(1);
  }

  SDL_Texture *image = IMG_LoadTexture(ctx.renderer, "resources/ball.svg");
  if (image == NULL) {
    fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Font *bigFont = TTF_OpenFont("resources/DejaVuSansMono.ttf", 64);
  if (bigFont == NULL) {
    fprintf(stderr, "Failed to load font: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Font *smallFont = TTF_OpenFont("resources/DejaVuSansMono.ttf", 32);
  if (bigFont == NULL) {
    fprintf(stderr, "Failed to load font: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Rect r;
  r.w = 100;
  r.h = 100;
  r.x = ctx.width / 2 - r.w / 2;
  r.y = ctx.height / 2;

  bool quit = false;
  int i = 0;
  Uint64 prevCounter = 0;
  while (!quit) {
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsed_ms = (now - prevCounter) * 1000 / (double) SDL_GetPerformanceFrequency();
    prevCounter = now;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
        break;
      }
      else if (e.type == SDL_KEYDOWN) {
        // key is pressed DOWN
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
          case SDLK_q:
            quit = true;
            break;
          case SDLK_DOWN:
            r.y += 20;
            break;
          case SDLK_UP:
            r.y -= 20;
            break;
          case SDLK_SPACE:
            Mix_PlayMusic(shotSound, 1);
            break;
        }
      }
    }

    // set the current drawing color to black
    SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
    // clear the screen with the current color black
    SDL_RenderClear(ctx.renderer);

    // draw texts
    draw_text(ctx.renderer, bigFont, "The Game", ctx.width / 2, ctx.height / 4, (SDL_Color) {255, 0, 0, 200}, 1);
    draw_text(ctx.renderer, smallFont, "Press up/down or space", ctx.width / 2, ctx.height / 4 + 70, (SDL_Color) {255, 0, 0, 200}, 1);

    // format the FPS string
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d ms / frame, %d FPS", (int) elapsed_ms, (int) (1000.0 / elapsed_ms));
    // draw FPS counter on the screen
    draw_text(ctx.renderer, smallFont, buffer, 0, 0, (SDL_Color) {255, 0, 0, 200}, 0);

    // set the current color to blue with 50% transparency (128 / 255)
    SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 255, 128);
    // draw the rect
    SDL_RenderFillRect(ctx.renderer, &r);

    // render the image, i is rotation - see documentation
    SDL_RenderCopyEx(ctx.renderer, image, NULL, &r, i++, NULL, SDL_FLIP_NONE);

    // update the screen
    SDL_RenderPresent(ctx.renderer);
  }
  return 0;
}

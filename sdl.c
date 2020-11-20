#include <SDL2/SDL_image.h>
#include "SDL2/SDL_mixer.h"
#include "sdl.h"

SDL_Context sdl_context_init(const char* window_name, int width, int height) {
    // Init SDL with video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
      fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
      exit(1);
    }

    // Init TrueType fonts
    if(TTF_Init() != 0) {
      fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
      exit(1);
    }

    // Init audio with support for .ogg, .opus and .mp3
    int flags = MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_MP3;
    if (flags != (Mix_Init(flags))) {
      fprintf(stderr, "Mix_Init Error: %s\n", Mix_GetError());
      exit(1);
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 640) != 0) {
      fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
      exit(1);
    }

    bool enableFullscreen = width <= 0 || height <= 0;
    SDL_Window *window = SDL_CreateWindow(
      window_name,
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height,
      SDL_WINDOW_SHOWN | (enableFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
    );
    if (window == NULL) {
      fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
      exit(1);
    }

    // create GPU accelerated renderer and use vertical sync
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
      fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
      exit(1);
    }

    // enable transparency
    if(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0) {
      fprintf(stderr, "SDL_SetRenderDrawBlendMode Error: %s\n", SDL_GetError());
      exit(1);
    }

    if(enableFullscreen) {
      if(SDL_GetRendererOutputSize(renderer, &width, &height) != 0) {
        fprintf(stderr, "SDL_GetRendererOutputSize Error: %s\n", SDL_GetError());
        exit(1);
      }
    }

    return (SDL_Context) {
      .width = width,
      .height = height,
      .renderer = renderer,
      .window = window,
    };
}

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *str, int x, int y, SDL_Color color, bool center_anchor) {
  // Render the text with a font into the RAM memory
  SDL_Surface *surface = TTF_RenderText_Solid(font, str, color);
  if(surface == NULL) {
    fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Convert surface from the RAM memory to the GPU memory
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(texture == NULL) {
    fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    exit(1);
  }

  // we have the texture in the GPU, surface is no longer needed
  SDL_FreeSurface(surface);

  // render the texture on x, y with width and height
  SDL_Rect rect;
  rect.x = x - surface->w / 2 * center_anchor;
  rect.y = y;
  rect.w = surface->w;
  rect.h = surface->h;
  SDL_RenderCopy(renderer, texture, NULL, &rect);

  // Texture needs to be freed from the GPU
  SDL_DestroyTexture(texture);
}

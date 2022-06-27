// SPDX-FileCopyrightText: 2021 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sdl/SDLUtils.hh"

std::atomic<int> vivictpp::sdl::SDLInitializer::instanceCount(0);

vivictpp::sdl::SDLInitializer::SDLInitializer() {
  if (instanceCount++ == 0) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
      throw SDLException("Failed to initialize SDL");
    }
  }
}

vivictpp::sdl::SDLInitializer::~SDLInitializer() {
  if (--instanceCount == 0) {
    SDL_Quit();
  }
}

std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>
vivictpp::sdl::createWindow(int width, int height) {
  auto window =  std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>
    (SDL_CreateWindow("Vivict++", SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED, width, height,
                      SDL_WINDOW_RESIZABLE),
     SDL_DestroyWindow);
  if (!window) {
    throw SDLException("Failed to create window");
  }
  return window;
}

std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>>
vivictpp::sdl::createRenderer(SDL_Window* window) {
  auto renderer = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>>
    (SDL_CreateRenderer(window, -1, 0),
     SDL_DestroyRenderer);
  if (!renderer) {
    throw SDLException("Failed to create renderer");
  }
  return renderer;
}

std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>>
vivictpp::sdl::createTexture(SDL_Renderer* renderer, int w, int h) {
  auto texture = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>>
    (SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
                       SDL_TEXTUREACCESS_STREAMING, w, h),
     SDL_DestroyTexture);
  if (!texture) {
    throw SDLException("Failed to create texture");
  }
  return texture;
}

std::unique_ptr<SDL_Cursor, std::function<void(SDL_Cursor *)>>
vivictpp::sdl::createHandCursor() {
  auto cursor = std::unique_ptr<SDL_Cursor, std::function<void(SDL_Cursor *)>>
    (SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL), SDL_FreeCursor);
  if (!cursor) {
    throw SDLException("Failed to create cursor");
  }
  return cursor;
}


void vivictpp::sdl::updateYuvTextureFromFrame(std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture *)>> &texture,
                                              const vivictpp::libav::Frame &frame) {
  if (frame.empty()) {
    return;
  }
  AVFrame *f = frame.avFrame();
  SDL_UpdateYUVTexture(
                       texture.get(), nullptr,
                       f->data[0], f->linesize[0],
                       f->data[1], f->linesize[1],
                       f->data[2], f->linesize[2]);
}

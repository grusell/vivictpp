// SPDX-FileCopyrightText: 2021 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ui/TextBox.hh"
#include "ui/Fonts.hh"
#include "ui/TextTexture.hh"

vivictpp::ui::TextBox::TextBox(std::string text, std::string font, int fontSize,
                               TextBoxPosition position, int x, int y, std::string title,
                               Margin margin)
    : texture(nullptr), text(text), font(font), fontSize(fontSize),
      position(position), x(x), y(y), title(title), margin(margin) {
  if (!title.empty()) {
    this->margin.top += fontSize + 4;
  }
}

vivictpp::ui::TextBox::~TextBox() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
}

void vivictpp::ui::TextBox::setText(std::string newText) {
  if (text == newText) {
    return;
  }
  text = newText;
  changed = true;
  //  if (texture != nullptr) {
  //    //SDL_DestroyTexture(texture);
  //  }
  //  texture = nullptr;
}

std::vector<std::string> lines(const std::string &str) {
  std::stringstream ss(str);
  std::string line;
  std::vector<std::string> lines;
  while (std::getline(ss, line)) {
    lines.push_back(line);
  }
  return lines;
}

void vivictpp::ui::TextBox::initTexture(SDL_Renderer *renderer) {
  std::vector<TextTexture> textures;
  textureW = 0;
  textureH = 0;
  for (const std::string line : lines(text)) {
    TextTexture tt(renderer, line, fontSize, fg, font);
    textureW = std::max(textureW, tt.width);
    textureH = textureH + tt.height;
    textures.push_back(tt);
  }
  textureW += margin.left + margin.right;
  textureH += margin.top + margin.bottom;

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, textureW, textureH);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, texture);

  SDL_Rect rect = {0, 0, textureW, textureH};
  SDL_RenderSetClipRect(renderer, &rect);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(renderer);

  int x = margin.left;
  int y = margin.top;
  for (const TextTexture tt : textures) {
    tt.render(renderer, x, y);
    y += tt.height;
  }

  if (!title.empty() ) {
    TextTexture tt(renderer, title, fontSize - 2, fg, font);
    tt.render(renderer, margin.left + 5, 2);
  }
  if (border) {
    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, 128);
    SDL_RenderDrawRect(renderer, nullptr);
  }

  SDL_SetRenderTarget(renderer, nullptr);
}

void vivictpp::ui::TextBox::render(SDL_Renderer *renderer) {
  int rendererWidth;
  int rendererHeight;
  SDL_GetRendererOutputSize(renderer, &rendererWidth, &rendererHeight);
  SDL_Texture *oldTexture(nullptr);
  if (changed || texture == nullptr) {
    oldTexture = texture;
    initTexture(renderer);
  }

  int x = 0 + this->x;
  int y = 5 + this->y;
  switch (position) {
  case TextBoxPosition::ABSOLUTE:
    x = this->x;
    y = this->y;
    break;
  case TextBoxPosition::TOP_LEFT:
    x = 5 + this->x;
    break;
  case TextBoxPosition::TOP_CENTER:
    x = this->x + (rendererWidth - textureW) / 2;
    break;
  case TextBoxPosition::TOP_RIGHT:
    x = this->x + rendererWidth - textureW - 5;
    break;
  case TextBoxPosition::CENTER:
    x = this->x + (rendererWidth - textureW) / 2;
    y = this->y + (rendererHeight - textureH) / 2;
  }
  SDL_Rect rect = {x, y, textureW, textureH};

  SDL_RenderCopy(renderer, texture, nullptr, &rect);
  if (oldTexture) {
    SDL_DestroyTexture(oldTexture);
  }
  changed = false;
}

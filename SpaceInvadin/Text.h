#pragma once

#include <SDL_ttf.h>
#include <SDL.h>
#include <string>

class Text {
public:
    Text(const std::string& font_path, int font_size, const std::string& message_text, const SDL_Color& color);
    ~Text();

    void display(SDL_Renderer* renderer, int x, int y) const;

    static SDL_Texture* loadFont(const std::string& font_path, int font_size, const std::string& message_text, const SDL_Color& color, SDL_Renderer* renderer);

private:
    SDL_Texture* _text_texture = nullptr;
    SDL_Rect _text_rect;
};
#pragma once

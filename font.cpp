#include <iostream>

#include <SDL2/SDL.h>

#include "font.h"


Font::Font()
: mTexture(nullptr), mRenderer(nullptr)
{ }

Font::~Font() {
    if (mTexture) SDL_DestroyTexture(mTexture);
}

bool Font::load(SDL_Renderer *renderer, const std::string &fontName) {
    mRenderer = renderer;
    SDL_Surface *source = SDL_LoadBMP(fontName.c_str());
    if (!source) {
        std::cerr << "Could not load font file " << fontName << ".\n";
        return false;
    }
    mTexture = SDL_CreateTextureFromSurface(renderer, source);
    SDL_FreeSurface(source);
    if (!mTexture) {
        std::cerr << "Failed to create font texture.\n";
        return false;
    }
    return true;
}

void Font::text(int startX, int startY, const std::string &text) {
    int x = startX;
    int y = startY;

    for (char c : text) {
        if (c > ' ') {
            int position = c - 33;
            SDL_Rect source = { position * WIDTH, 0, WIDTH, HEIGHT };
            SDL_Rect dest = { x, y, WIDTH, HEIGHT };
            SDL_RenderCopy(mRenderer, mTexture, &source, &dest);
        }
        if (c == '\n') {
            y += LINESPACE;
            x = startX;
        } else {
            x += 8;
        }
    }
}

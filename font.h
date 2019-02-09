#ifndef FONT_H
#define FONT_H

#include <string>

struct SDL_Renderer;
struct SDL_Texture;

class Font {
public:
    static const int WIDTH = 8;
    static const int HEIGHT = 8;
    static const int LINESPACE = 10;

    Font();
    ~Font();

    bool load(SDL_Renderer *renderer, const std::string &fontName);
    void text(int x, int y, const std::string &text);
private:
    SDL_Texture *mTexture;
    SDL_Renderer *mRenderer;
};




#endif

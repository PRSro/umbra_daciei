#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>

struct SDL_RWops;

namespace core {

struct Color {
    uint8_t r, g, b, a;

    Color(uint8_t rr = 0, uint8_t gg = 0, uint8_t bb = 0, uint8_t aa = 255)
        : r(rr), g(gg), b(bb), a(aa) {}

    template<typename T>
    Color(const T& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

    static Color fromHex(uint32_t hex) {
        return Color(
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            static_cast<uint8_t>((hex >> 24) & 0xFF)
        );
    }

    SDL_Color toSDL() const {
        return {r, g, b, a};
    }
};

class Texture {
public:
    Texture() = default;
    Texture(SDL_Texture* t, int w, int h);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    int width() const { return w_; }
    int height() const { return h_; }
    SDL_Texture* get() const { return tex_; }

    void setAlphaMod(uint8_t alpha);
    void setBlendMode(SDL_BlendMode mode);

private:
    SDL_Texture* tex_ = nullptr;
    int w_ = 0;
    int h_ = 0;
};

class Font {
public:
    Font() = default;
    Font(TTF_Font* f);
    ~Font();

    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    int height() const;
    int getTextWidth(const std::string& text) const;
    TTF_Font* get() const { return font_; }

private:
    TTF_Font* font_ = nullptr;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(SDL_Window* window);
    void shutdown();

    void clear();
    void present();
    void setDrawColor(Color c);
    void setDrawBlendMode(SDL_BlendMode mode);

    void drawPoint(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawRect(const SDL_Rect& rect, bool filled = false);
    void drawRect(int x, int y, int w, int h, bool filled = false);

    void drawCircle(int cx, int cy, int radius, bool filled = false);
    void drawFilledCircle(int cx, int cy, int radius);

    void drawPolygon(const std::vector<SDL_Point>& points, bool filled = false);

    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool filled = false);

    void drawThickLine(int x1, int y1, int x2, int y2, int thickness);

    Texture createTexture(int w, int h);
    Texture loadTexture(const std::string& path);
    Texture createTextTexture(Font& font, const std::string& text, Color fg, Color bg = Color{});

    void copy(Texture& tex, const SDL_Rect* src = nullptr, const SDL_Rect* dst = nullptr);
    void copyEx(Texture& tex, const SDL_Rect* src, const SDL_Rect* dst,
                double angle, const SDL_Point* center, SDL_RendererFlip flip);

    int width() const { return w_; }
    int height() const { return h_; }
    SDL_Renderer* get() const { return renderer_; }

    void setTime(float t) { time_ = t; }
    void addTime(float dt) { time_ += dt; }
    float getTime() const { return time_; }

    void setViewport(const SDL_Rect& rect);
    void resetViewport();

    void drawLSystemTree(int x, int y, const std::string& program, int length, int angle,
                        std::function<void(Color&, int&)> ruleGenerator);

    void drawText(const std::string& text, int x, int y, Color color, int scale = 1);
    void drawDacianSymbol(int cx, int cy, float radius, float rotation, Color col);

private:
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* currentFont_ = nullptr;
    int w_ = 0;
    int h_ = 0;
    float time_ = 0.0f;
};

}

#endif
#include "Renderer.h"
#include <cmath>
#include <algorithm>
#include <SDL2/SDL_image.h>

namespace core {

Texture::Texture(SDL_Texture* t, int w, int h) : tex_(t), w_(w), h_(h) {}

Texture::~Texture() {
    if (tex_) SDL_DestroyTexture(tex_);
}

Texture::Texture(Texture&& other) noexcept : tex_(other.tex_), w_(other.w_), h_(other.h_) {
    other.tex_ = nullptr;
    other.w_ = 0;
    other.h_ = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (tex_) SDL_DestroyTexture(tex_);
        tex_ = other.tex_;
        w_ = other.w_;
        h_ = other.h_;
        other.tex_ = nullptr;
        other.w_ = 0;
        other.h_ = 0;
    }
    return *this;
}

void Texture::setAlphaMod(uint8_t alpha) {
    if (tex_) SDL_SetTextureAlphaMod(tex_, alpha);
}

void Texture::setBlendMode(SDL_BlendMode mode) {
    if (tex_) SDL_SetTextureBlendMode(tex_, mode);
}

Font::Font(TTF_Font* f) : font_(f) {}

Font::~Font() {
    if (font_) TTF_CloseFont(font_);
}

Font::Font(Font&& other) noexcept : font_(other.font_) {
    other.font_ = nullptr;
}

Font& Font::operator=(Font&& other) noexcept {
    if (this != &other) {
        if (font_) TTF_CloseFont(font_);
        font_ = other.font_;
        other.font_ = nullptr;
    }
    return *this;
}

int Font::height() const {
    return font_ ? TTF_FontHeight(font_) : 0;
}

int Font::getTextWidth(const std::string& text) const {
    int w = 0;
    if (font_) TTF_SizeText(font_, text.c_str(), &w, nullptr);
    return w;
}

Renderer::Renderer() = default;

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(SDL_Window* window) {
    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_RenderSetIntegerScale(renderer_, SDL_FALSE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    SDL_RenderGetLogicalSize(renderer_, &w_, &h_);
    if (w_ == 0 || h_ == 0) {
        SDL_GetRendererOutputSize(renderer_, &w_, &h_);
    }
    return true;
}

void Renderer::shutdown() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (currentFont_) {
        TTF_CloseFont(currentFont_);
        currentFont_ = nullptr;
    }
}

void Renderer::clear() {
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::setDrawColor(Color c) {
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
}

void Renderer::setDrawBlendMode(SDL_BlendMode mode) {
    SDL_SetRenderDrawBlendMode(renderer_, mode);
}

void Renderer::drawPoint(int x, int y) {
    SDL_RenderDrawPoint(renderer_, x, y);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void Renderer::drawRect(const SDL_Rect& rect, bool filled) {
    if (filled) {
        SDL_RenderFillRect(renderer_, &rect);
    } else {
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawRect(int x, int y, int w, int h, bool filled) {
    SDL_Rect rect{x, y, w, h};
    if (filled) {
        SDL_RenderFillRect(renderer_, &rect);
    } else {
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawCircle(int cx, int cy, int radius, bool filled) {
    if (radius <= 0) return;

    if (filled) {
        drawFilledCircle(cx, cy, radius);
        return;
    }

    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        drawPoint(cx + x, cy + y);
        drawPoint(cx + y, cy + x);
        drawPoint(cx - y, cy + x);
        drawPoint(cx - x, cy + y);
        drawPoint(cx - x, cy - y);
        drawPoint(cx - y, cy - x);
        drawPoint(cx + y, cy - x);
        drawPoint(cx + x, cy - y);

        y += 1;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x -= 1;
            err += 2 * (x - y) + 1;
        }
    }
}

void Renderer::drawFilledCircle(int cx, int cy, int radius) {
    if (radius <= 0) return;

    for (int y = -radius; y <= radius; y++) {
        int width = static_cast<int>(std::sqrt(radius * radius - y * y));
        if (width > 0) {
            drawLine(cx - width, cy + y, cx + width, cy + y);
        }
    }
}

void Renderer::drawPolygon(const std::vector<SDL_Point>& points, bool filled) {
    if (points.size() < 2) return;

    if (filled) {
        uint8_t r, g, b, a;
        SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
        std::vector<SDL_Vertex> vertices;
        vertices.reserve(points.size());
        for (const auto& p : points) {
            vertices.push_back({static_cast<float>(p.x), static_cast<float>(p.y), {r, g, b, a}, {0.0f, 0.0f}});
        }
        SDL_RenderGeometry(renderer_, nullptr, vertices.data(), vertices.size(), nullptr, 0);
    } else {
        for (size_t i = 0; i < points.size(); ++i) {
            const auto& p1 = points[i];
            const auto& p2 = points[(i + 1) % points.size()];
            drawLine(p1.x, p1.y, p2.x, p2.y);
        }
    }
}

void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool filled) {
    if (filled) {
        uint8_t r, g, b, a;
        SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
        SDL_Vertex vertices[] = {
            {static_cast<float>(x1), static_cast<float>(y1), {r, g, b, a}, {0.0f, 0.0f}},
            {static_cast<float>(x2), static_cast<float>(y2), {r, g, b, a}, {0.0f, 0.0f}},
            {static_cast<float>(x3), static_cast<float>(y3), {r, g, b, a}, {0.0f, 0.0f}}
        };
        int indices[] = {0, 1, 2};
        SDL_RenderGeometry(renderer_, nullptr, vertices, 3, indices, 3);
    } else {
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x3, y3);
        drawLine(x3, y3, x1, y1);
    }
}

void Renderer::drawThickLine(int x1, int y1, int x2, int y2, int thickness) {
    if (thickness <= 1) {
        drawLine(x1, y1, x2, y2);
        return;
    }

    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001) return;

    double nx = -dy / len * thickness / 2.0;
    double ny = dx / len * thickness / 2.0;

    uint8_t r, g, b, a;
    SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
    SDL_Vertex vertices[] = {
        {static_cast<float>(x1 + nx), static_cast<float>(y1 + ny), {r, g, b, a}, {0.0f, 0.0f}},
        {static_cast<float>(x2 + nx), static_cast<float>(y2 + ny), {r, g, b, a}, {0.0f, 0.0f}},
        {static_cast<float>(x2 - nx), static_cast<float>(y2 - ny), {r, g, b, a}, {0.0f, 0.0f}},
        {static_cast<float>(x1 - nx), static_cast<float>(y1 - ny), {r, g, b, a}, {0.0f, 0.0f}}
    };
    int indices[] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer_, nullptr, vertices, 4, indices, 6);
}

Texture Renderer::createTexture(int w, int h) {
    SDL_Texture* tex = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, w, h);
    if (tex) {
        return Texture(tex, w, h);
    }
    return Texture{};
}

Texture Renderer::loadTexture(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer_, path.c_str());
    if (!tex) return Texture{};

    int w, h;
    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
    return Texture(tex, w, h);
}

Texture Renderer::createTextTexture(Font& font, const std::string& text, Color fg, Color bg) {
    SDL_Surface* surf = TTF_RenderText_Blended(font.get(), text.c_str(), fg.toSDL());
    if (!surf) return Texture{};

    if (bg.a > 0) {
        SDL_Surface* filled = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
        SDL_FillRect(filled, nullptr, SDL_MapRGBA(filled->format, bg.r, bg.g, bg.b, bg.a));
        SDL_Rect dstRect = {0, 0, surf->w, surf->h};
        SDL_BlitSurface(surf, nullptr, filled, &dstRect);
        SDL_FreeSurface(surf);
        surf = filled;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_FreeSurface(surf);

    if (tex) {
        return Texture(tex, surf ? surf->w : 0, surf ? surf->h : 0);
    }
    return Texture{};
}

void Renderer::copy(Texture& tex, const SDL_Rect* src, const SDL_Rect* dst) {
    SDL_RenderCopy(renderer_, tex.get(), src, dst);
}

void Renderer::copyEx(Texture& tex, const SDL_Rect* src, const SDL_Rect* dst,
                    double angle, const SDL_Point* center, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer_, tex.get(), src, dst, angle, center, flip);
}

void Renderer::setViewport(const SDL_Rect& rect) {
    SDL_RenderSetViewport(renderer_, &rect);
}

void Renderer::resetViewport() {
    SDL_RenderSetViewport(renderer_, nullptr);
}

void Renderer::drawLSystemTree(int x, int y, const std::string& program, int length, int angleBase,
                               std::function<void(Color&, int&)> ruleGenerator) {
    struct State {
        float x, y;
        int angle;
    };
    std::vector<State> stack;
    State current = {static_cast<float>(x), static_cast<float>(y), -90}; // Grow up by default

    // Get current draw color
    uint8_t r_sdl, g_sdl, b_sdl, a_sdl;
    SDL_GetRenderDrawColor(renderer_, &r_sdl, &g_sdl, &b_sdl, &a_sdl);
    Color baseColor(r_sdl, g_sdl, b_sdl, a_sdl);

    for (char c : program) {
        if (c == 'F') {
            int currentAngle = current.angle;
            Color drawCol = baseColor;
            if (ruleGenerator) {
                ruleGenerator(drawCol, currentAngle);
            }
            setDrawColor(drawCol);
            
            float rad = currentAngle * 3.14159f / 180.0f;
            float x2 = current.x + length * std::cos(rad);
            float y2 = current.y + length * std::sin(rad);
            drawLine(static_cast<int>(current.x), static_cast<int>(current.y), static_cast<int>(x2), static_cast<int>(y2));
            current.x = x2;
            current.y = y2;
        } else if (c == '+') {
            current.angle += angleBase;
        } else if (c == '-') {
            current.angle -= angleBase;
        } else if (c == '[') {
            stack.push_back(current);
        } else if (c == ']') {
            if (!stack.empty()) {
                current = stack.back();
                stack.pop_back();
            }
        }
    }
    setDrawColor(baseColor);
}

// 5x7 bitmap font: each glyph is 5 bytes (columns), each byte has 7 bits (rows, LSB=top)
static const uint8_t FONT_5x7[][5] = {
    // ASCII 32 (space)
    {0x00,0x00,0x00,0x00,0x00},
    // ASCII 33 !
    {0x00,0x00,0x5F,0x00,0x00},
    // ASCII 34 "
    {0x00,0x07,0x00,0x07,0x00},
    // ASCII 35 #
    {0x14,0x7F,0x14,0x7F,0x14},
    // ASCII 36 $
    {0x24,0x2A,0x7F,0x2A,0x12},
    // ASCII 37 %
    {0x23,0x13,0x08,0x64,0x62},
    // ASCII 38 &
    {0x36,0x49,0x55,0x22,0x50},
    // ASCII 39 '
    {0x00,0x05,0x03,0x00,0x00},
    // ASCII 40 (
    {0x00,0x1C,0x22,0x41,0x00},
    // ASCII 41 )
    {0x00,0x41,0x22,0x1C,0x00},
    // ASCII 42 *
    {0x14,0x08,0x3E,0x08,0x14},
    // ASCII 43 +
    {0x08,0x08,0x3E,0x08,0x08},
    // ASCII 44 ,
    {0x00,0x50,0x30,0x00,0x00},
    // ASCII 45 -
    {0x08,0x08,0x08,0x08,0x08},
    // ASCII 46 .
    {0x00,0x60,0x60,0x00,0x00},
    // ASCII 47 /
    {0x20,0x10,0x08,0x04,0x02},
    // ASCII 48 0
    {0x3E,0x51,0x49,0x45,0x3E},
    // ASCII 49 1
    {0x00,0x42,0x7F,0x40,0x00},
    // ASCII 50 2
    {0x42,0x61,0x51,0x49,0x46},
    // ASCII 51 3
    {0x21,0x41,0x45,0x4B,0x31},
    // ASCII 52 4
    {0x18,0x14,0x12,0x7F,0x10},
    // ASCII 53 5
    {0x27,0x45,0x45,0x45,0x39},
    // ASCII 54 6
    {0x3C,0x4A,0x49,0x49,0x30},
    // ASCII 55 7
    {0x01,0x71,0x09,0x05,0x03},
    // ASCII 56 8
    {0x36,0x49,0x49,0x49,0x36},
    // ASCII 57 9
    {0x06,0x49,0x49,0x29,0x1E},
    // ASCII 58 :
    {0x00,0x36,0x36,0x00,0x00},
    // ASCII 59 ;
    {0x00,0x56,0x36,0x00,0x00},
    // ASCII 60 <
    {0x08,0x14,0x22,0x41,0x00},
    // ASCII 61 =
    {0x14,0x14,0x14,0x14,0x14},
    // ASCII 62 >
    {0x00,0x41,0x22,0x14,0x08},
    // ASCII 63 ?
    {0x02,0x01,0x51,0x09,0x06},
    // ASCII 64 @
    {0x32,0x49,0x79,0x41,0x3E},
    // ASCII 65 A
    {0x7E,0x11,0x11,0x11,0x7E},
    // ASCII 66 B
    {0x7F,0x49,0x49,0x49,0x36},
    // ASCII 67 C
    {0x3E,0x41,0x41,0x41,0x22},
    // ASCII 68 D
    {0x7F,0x41,0x41,0x22,0x1C},
    // ASCII 69 E
    {0x7F,0x49,0x49,0x49,0x41},
    // ASCII 70 F
    {0x7F,0x09,0x09,0x09,0x01},
    // ASCII 71 G
    {0x3E,0x41,0x49,0x49,0x7A},
    // ASCII 72 H
    {0x7F,0x08,0x08,0x08,0x7F},
    // ASCII 73 I
    {0x00,0x41,0x7F,0x41,0x00},
    // ASCII 74 J
    {0x20,0x40,0x41,0x3F,0x01},
    // ASCII 75 K
    {0x7F,0x08,0x14,0x22,0x41},
    // ASCII 76 L
    {0x7F,0x40,0x40,0x40,0x40},
    // ASCII 77 M
    {0x7F,0x02,0x0C,0x02,0x7F},
    // ASCII 78 N
    {0x7F,0x04,0x08,0x10,0x7F},
    // ASCII 79 O
    {0x3E,0x41,0x41,0x41,0x3E},
    // ASCII 80 P
    {0x7F,0x09,0x09,0x09,0x06},
    // ASCII 81 Q
    {0x3E,0x41,0x51,0x21,0x5E},
    // ASCII 82 R
    {0x7F,0x09,0x19,0x29,0x46},
    // ASCII 83 S
    {0x46,0x49,0x49,0x49,0x31},
    // ASCII 84 T
    {0x01,0x01,0x7F,0x01,0x01},
    // ASCII 85 U
    {0x3F,0x40,0x40,0x40,0x3F},
    // ASCII 86 V
    {0x1F,0x20,0x40,0x20,0x1F},
    // ASCII 87 W
    {0x3F,0x40,0x38,0x40,0x3F},
    // ASCII 88 X
    {0x63,0x14,0x08,0x14,0x63},
    // ASCII 89 Y
    {0x07,0x08,0x70,0x08,0x07},
    // ASCII 90 Z
    {0x61,0x51,0x49,0x45,0x43},
    // ASCII 91 [
    {0x00,0x7F,0x41,0x41,0x00},
    // ASCII 92 backslash
    {0x02,0x04,0x08,0x10,0x20},
    // ASCII 93 ]
    {0x00,0x41,0x41,0x7F,0x00},
    // ASCII 94 ^
    {0x04,0x02,0x01,0x02,0x04},
    // ASCII 95 _
    {0x40,0x40,0x40,0x40,0x40},
    // ASCII 96 `
    {0x00,0x01,0x02,0x04,0x00},
    // ASCII 97 a
    {0x20,0x54,0x54,0x54,0x78},
    // ASCII 98 b
    {0x7F,0x48,0x44,0x44,0x38},
    // ASCII 99 c
    {0x38,0x44,0x44,0x44,0x20},
    // ASCII 100 d
    {0x38,0x44,0x44,0x48,0x7F},
    // ASCII 101 e
    {0x38,0x54,0x54,0x54,0x18},
    // ASCII 102 f
    {0x08,0x7E,0x09,0x01,0x02},
    // ASCII 103 g
    {0x0C,0x52,0x52,0x52,0x3E},
    // ASCII 104 h
    {0x7F,0x08,0x04,0x04,0x78},
    // ASCII 105 i
    {0x00,0x44,0x7D,0x40,0x00},
    // ASCII 106 j
    {0x20,0x40,0x44,0x3D,0x00},
    // ASCII 107 k
    {0x7F,0x10,0x28,0x44,0x00},
    // ASCII 108 l
    {0x00,0x41,0x7F,0x40,0x00},
    // ASCII 109 m
    {0x7C,0x04,0x18,0x04,0x78},
    // ASCII 110 n
    {0x7C,0x08,0x04,0x04,0x78},
    // ASCII 111 o
    {0x38,0x44,0x44,0x44,0x38},
    // ASCII 112 p
    {0x7C,0x14,0x14,0x14,0x08},
    // ASCII 113 q
    {0x08,0x14,0x14,0x18,0x7C},
    // ASCII 114 r
    {0x7C,0x08,0x04,0x04,0x08},
    // ASCII 115 s
    {0x48,0x54,0x54,0x54,0x20},
    // ASCII 116 t
    {0x04,0x3F,0x44,0x40,0x20},
    // ASCII 117 u
    {0x3C,0x40,0x40,0x20,0x7C},
    // ASCII 118 v
    {0x1C,0x20,0x40,0x20,0x1C},
    // ASCII 119 w
    {0x3C,0x40,0x30,0x40,0x3C},
    // ASCII 120 x
    {0x44,0x28,0x10,0x28,0x44},
    // ASCII 121 y
    {0x0C,0x50,0x50,0x50,0x3C},
    // ASCII 122 z
    {0x44,0x64,0x54,0x4C,0x44},
    // ASCII 123 {
    {0x00,0x08,0x36,0x41,0x00},
    // ASCII 124 |
    {0x00,0x00,0x7F,0x00,0x00},
    // ASCII 125 }
    {0x00,0x41,0x36,0x08,0x00},
    // ASCII 126 ~
    {0x10,0x08,0x08,0x10,0x10},
};

void Renderer::drawDacianSymbol(int cx, int cy, float radius, float rotation, Color col) {
    setDrawColor(col);
    // Outer ring
    drawCircle(cx, cy, static_cast<int>(radius));

    // 8-pointed star using two overlapping drawPolygon() squares rotated 45° apart
    float radRotation = rotation * 3.14159f / 180.0f;
    for (int s = 0; s < 2; s++) {
        std::vector<SDL_Point> pts;
        float offset = s * (3.14159f / 4.0f); // 45 degrees
        for (int i = 0; i < 4; i++) {
            float ang = radRotation + offset + i * (3.14159f / 2.0f);
            pts.push_back({cx + static_cast<int>(cos(ang) * radius * 0.8f), cy + static_cast<int>(sin(ang) * radius * 0.8f)});
        }
        drawPolygon(pts, false);
        // Small filled circles at points
        for (const auto& p : pts) {
            drawFilledCircle(p.x, p.y, 3);
        }
    }

    // Inner circle
    drawCircle(cx, cy, static_cast<int>(radius * 0.3f));

    // 4 lines from center at 45° intervals
    for (int i = 0; i < 4; i++) {
        float ang = radRotation + i * (3.14159f / 4.0f);
        drawLine(cx, cy, cx + static_cast<int>(cos(ang) * radius), cy + static_cast<int>(sin(ang) * radius));
    }
}

void Renderer::drawText(const std::string& text, int x, int y, Color color, int scale) {
    if (scale < 1) scale = 1;
    setDrawColor(color);
    int cursorX = x;
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(text[i]);
        if (ch < 32 || ch > 126) {
            // For non-ASCII (UTF-8 continuation bytes), draw a placeholder or skip
            if (ch >= 0xC0 && ch <= 0xFD) {
                // UTF-8 lead byte — draw '?' and skip continuation bytes
                ch = '?';
                // Skip continuation bytes
                size_t next = i + 1;
                while (next < text.size() && (static_cast<unsigned char>(text[next]) & 0xC0) == 0x80) {
                    ++next;
                }
                // Draw the '?' glyph
                int idx = ch - 32;
                for (int col = 0; col < 5; ++col) {
                    uint8_t colBits = FONT_5x7[idx][col];
                    for (int row = 0; row < 7; ++row) {
                        if (colBits & (1 << row)) {
                            for (int sy = 0; sy < scale; ++sy) {
                                for (int sx = 0; sx < scale; ++sx) {
                                    drawPoint(cursorX + col * scale + sx, y + row * scale + sy);
                                }
                            }
                        }
                    }
                }
                cursorX += 6 * scale;
                i = next - 1; // -1 because loop will increment
                continue;
            }
            if ((ch & 0xC0) == 0x80) {
                // UTF-8 continuation byte — skip
                continue;
            }
            cursorX += 6 * scale;
            continue;
        }
        int idx = ch - 32;
        for (int col = 0; col < 5; ++col) {
            uint8_t colBits = FONT_5x7[idx][col];
            for (int row = 0; row < 7; ++row) {
                if (colBits & (1 << row)) {
                    for (int sy = 0; sy < scale; ++sy) {
                        for (int sx = 0; sx < scale; ++sx) {
                            drawPoint(cursorX + col * scale + sx, y + row * scale + sy);
                        }
                    }
                }
            }
        }
        cursorX += 6 * scale;
    }
}

}
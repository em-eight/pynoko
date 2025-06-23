#pragma once

namespace bolt {
namespace gfx {

struct Color {
    float r;
    float g;
    float b;
    float a;

    Color() = default;
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

Color randomBrightColor(float minBrightness);

extern const Color COLOR_WHITE;
extern const Color COLOR_BLACK;
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;

} // gfx
} // bolt
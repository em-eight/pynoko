#include "gfx/Color.hpp"

#include "util/common.h"

#include <cstdlib>

namespace bolt {
namespace gfx {

const Color COLOR_WHITE(1, 1, 1, 1);
const Color COLOR_BLACK(0, 0, 0, 1);
const Color COLOR_RED(1, 0, 0, 1);
const Color COLOR_GREEN(0, 1, 0, 1);
const Color COLOR_BLUE(0, 0, 1, 1);

Color randomBrightColor(float minBrightness) {
    RUNTIME_ASSERT(minBrightness < 2.0f, "minBrightness must be between 0 and 2");

    Color ret;
    ret.a = 1.0f;
    do {
        ret.r = static_cast<float>(rand()) / RAND_MAX;
        ret.g = static_cast<float>(rand()) / RAND_MAX;
        ret.b = static_cast<float>(rand()) / RAND_MAX;
    } while (ret.r + ret.g + ret.b < minBrightness);

    return ret;
}

} // gfx
} // bolt

#pragma once

#include <filesystem>

namespace bolt {
namespace util {

class Filesystem {
public:
    static void* loadFile(const std::filesystem::path& path, unsigned int& size);
};

} // util
} // bolt

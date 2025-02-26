#pragma once

#include <filesystem>

namespace util {
void* loadFile(const std::filesystem::path& fullpath, unsigned int& size);
void* loadResource(const char* path, unsigned int& size);
}

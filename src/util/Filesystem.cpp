#include <filesystem>
#include <fstream>
#include <cstdio>

#include "Filesystem.hpp"
#include "common.hpp"

using namespace std;

namespace util {
void* loadFile(const filesystem::path& fullpath, unsigned int& size) {
    std::ifstream file(fullpath, std::ios::binary | std::ios::ate);
    RUNTIME_ASSERT(!!file, ("Could not open file " + fullpath.string()).c_str());

    // Get file size
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate buffer and read the file
    void* buffer = malloc(size);
    file.read((char*)buffer, size);
    RUNTIME_ASSERT(!!file, ("Error reading file " + fullpath.string()).c_str());

    return buffer;
}

void* loadResource(const char* path, unsigned int& size) {
    filesystem::path root = "./res";
    filesystem::path fullpath = root / path;

    return loadFile(fullpath, size);
}
}

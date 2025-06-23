#include "util/Filesystem.hpp"

#include "util/common.h"

#include <fstream>

using namespace std;

namespace bolt {
namespace util {

void* Filesystem::loadFile(const filesystem::path& fullpath, unsigned int& size) {
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

} // util
} // bolt
